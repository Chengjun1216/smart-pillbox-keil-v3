#include "ds1302.h"
#include "main.h"

static void DS1302_Delay(uint16_t us) {
    uint16_t i;
    for(i = 0; i < us * 8; i++);
}

static void DS1302_DATA_Output(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = DS1302_DATA_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS1302_DATA_PORT, &GPIO_InitStructure);
}

static void DS1302_DATA_Input(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = DS1302_DATA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(DS1302_DATA_PORT, &GPIO_InitStructure);
}

void DS1302_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = DS1302_CE_PIN | DS1302_SCLK_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DS1302_DATA_Output();

    GPIO_ResetBits(DS1302_CE_PORT,   DS1302_CE_PIN);
    GPIO_ResetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);

    DS1302_WriteByte(0x8E, 0x00);
}

void DS1302_WriteByte(uint8_t addr, uint8_t data) {
    uint8_t i;
    DS1302_DATA_Output();

    GPIO_ResetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);
    GPIO_SetBits(DS1302_CE_PORT, DS1302_CE_PIN);
    DS1302_Delay(4);

    for(i = 0; i < 8; i++) {
        if(addr & 0x01) GPIO_SetBits(DS1302_DATA_PORT, DS1302_DATA_PIN);
        else            GPIO_ResetBits(DS1302_DATA_PORT, DS1302_DATA_PIN);
        addr >>= 1;
        GPIO_SetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);
        DS1302_Delay(2);
        GPIO_ResetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);
        DS1302_Delay(2);
    }
    for(i = 0; i < 8; i++) {
        if(data & 0x01) GPIO_SetBits(DS1302_DATA_PORT, DS1302_DATA_PIN);
        else            GPIO_ResetBits(DS1302_DATA_PORT, DS1302_DATA_PIN);
        data >>= 1;
        GPIO_SetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);
        DS1302_Delay(2);
        GPIO_ResetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);
        DS1302_Delay(2);
    }
    GPIO_ResetBits(DS1302_CE_PORT, DS1302_CE_PIN);
}

uint8_t DS1302_ReadByte(uint8_t addr) {
    uint8_t i, data = 0;
    DS1302_DATA_Output();

    GPIO_ResetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);
    GPIO_SetBits(DS1302_CE_PORT, DS1302_CE_PIN);
    DS1302_Delay(4);

    addr |= 0x01;
    for(i = 0; i < 8; i++) {
        if(addr & 0x01) GPIO_SetBits(DS1302_DATA_PORT, DS1302_DATA_PIN);
        else            GPIO_ResetBits(DS1302_DATA_PORT, DS1302_DATA_PIN);
        addr >>= 1;
        GPIO_SetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);
        DS1302_Delay(2);
        GPIO_ResetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);
        DS1302_Delay(2);
    }

    DS1302_DATA_Input();
    for(i = 0; i < 8; i++) {
        data >>= 1;
        if(GPIO_ReadInputDataBit(DS1302_DATA_PORT, DS1302_DATA_PIN))
            data |= 0x80;
        GPIO_SetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);
        DS1302_Delay(2);
        GPIO_ResetBits(DS1302_SCLK_PORT, DS1302_SCLK_PIN);
        DS1302_Delay(2);
    }
    GPIO_ResetBits(DS1302_CE_PORT, DS1302_CE_PIN);
    return data;
}

static uint8_t BCD2DEC(uint8_t bcd) { return (bcd >> 4) * 10 + (bcd & 0x0F); }
static uint8_t DEC2BCD(uint8_t dec) { return ((dec / 10) << 4) | (dec % 10); }

void DS1302_ReadTime(DS1302_Time_t *t) {
    t->second = BCD2DEC(DS1302_ReadByte(0x80) & 0x7F);
    t->minute = BCD2DEC(DS1302_ReadByte(0x82) & 0x7F);
    t->hour   = BCD2DEC(DS1302_ReadByte(0x84) & 0x3F);
    t->day    = BCD2DEC(DS1302_ReadByte(0x86) & 0x3F);
    t->month  = BCD2DEC(DS1302_ReadByte(0x88) & 0x1F);
    t->year   = BCD2DEC(DS1302_ReadByte(0x8C));
}

void DS1302_SetTime(DS1302_Time_t *t) {
    DS1302_WriteByte(0x8E, 0x00);
    DS1302_WriteByte(0x80, DEC2BCD(t->second) & 0x7F);
    DS1302_WriteByte(0x82, DEC2BCD(t->minute));
    DS1302_WriteByte(0x84, DEC2BCD(t->hour));
    DS1302_WriteByte(0x86, DEC2BCD(t->day));
    DS1302_WriteByte(0x88, DEC2BCD(t->month));
    DS1302_WriteByte(0x8C, DEC2BCD(t->year));
    DS1302_WriteByte(0x8E, 0x80);
}
