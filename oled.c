#include "oled.h"
#include "oledfont.h"
#include "main.h"

#define OLED_SCL_H()  GPIO_SetBits(OLED_SCL_PORT,   OLED_SCL_PIN)
#define OLED_SCL_L()  GPIO_ResetBits(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SDA_H()  GPIO_SetBits(OLED_SDA_PORT,   OLED_SDA_PIN)
#define OLED_SDA_L()  GPIO_ResetBits(OLED_SDA_PORT, OLED_SDA_PIN)

static void IIC_Start(void) {
    OLED_SDA_H(); OLED_SCL_H();
    OLED_SDA_L(); OLED_SCL_L();
}

static void IIC_Stop(void) {
    OLED_SDA_L(); OLED_SCL_H();
    OLED_SDA_H();
}

static void IIC_WriteByte(uint8_t data) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        if(data & 0x80) OLED_SDA_H();
        else            OLED_SDA_L();
        data <<= 1;
        OLED_SCL_H(); OLED_SCL_L();
    }
    OLED_SCL_H(); OLED_SCL_L();
}

static void OLED_WriteCmd(uint8_t cmd) {
    IIC_Start();
    IIC_WriteByte(0x78);
    IIC_WriteByte(0x00);
    IIC_WriteByte(cmd);
    IIC_Stop();
}

static void OLED_WriteData(uint8_t data) {
    IIC_Start();
    IIC_WriteByte(0x78);
    IIC_WriteByte(0x40);
    IIC_WriteByte(data);
    IIC_Stop();
}

void OLED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = OLED_SCL_PIN | OLED_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    OLED_WriteCmd(0xAE);
    OLED_WriteCmd(0x20); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xB0);
    OLED_WriteCmd(0xC8);
    OLED_WriteCmd(0x00); OLED_WriteCmd(0x10);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x81); OLED_WriteCmd(0xFF);
    OLED_WriteCmd(0xA1);
    OLED_WriteCmd(0xA6);
    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xA4);
    OLED_WriteCmd(0xD3); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xD5); OLED_WriteCmd(0xF0);
    OLED_WriteCmd(0xD9); OLED_WriteCmd(0x22);
    OLED_WriteCmd(0xDA); OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xDB); OLED_WriteCmd(0x20);
    OLED_WriteCmd(0x8D); OLED_WriteCmd(0x14);
    OLED_WriteCmd(0xAF);

    OLED_Clear();
}

void OLED_Clear(void) {
    uint8_t i, j;
    for(i = 0; i < 8; i++) {
        OLED_WriteCmd(0xB0 + i);
        OLED_WriteCmd(0x00); OLED_WriteCmd(0x10);
        for(j = 0; j < 128; j++) OLED_WriteData(0x00);
    }
}

static void OLED_SetPos(uint8_t x, uint8_t y) {
    OLED_WriteCmd(0xB0 + y);
    OLED_WriteCmd(((x & 0xF0) >> 4) | 0x10);
    OLED_WriteCmd(x & 0x0F);
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch) {
    uint8_t i;
    ch -= ' ';
    OLED_SetPos(x, y);
    for(i = 0; i < 8; i++) OLED_WriteData(F8X16[ch * 16 + i]);
    OLED_SetPos(x, y + 1);
    for(i = 0; i < 8; i++) OLED_WriteData(F8X16[ch * 16 + 8 + i]);
}

void OLED_ShowString(uint8_t x, uint8_t y, const char *str) {
    while(*str) {
        OLED_ShowChar(x, y, *str++);
        x += 8;
        if(x > 120) { x = 0; y += 2; }
    }
}

void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len) {
    char buf[6];
    uint8_t i;
    for(i = len; i > 0; i--) {
        buf[i-1] = '0' + num % 10;
        num /= 10;
    }
    buf[len] = '\0';
    OLED_ShowString(x, y, buf);
}

static const char* GroupName(uint8_t group) {
    switch(group) {
        case 0: return "AM";
        case 1: return "NN";
        case 2: return "PM";
        default: return "??";
    }
}

void OLED_ShowIdle(uint8_t hour, uint8_t min, uint8_t sec,
                    uint8_t temp, uint8_t humi) {
    char buf[20];
    OLED_Clear();
    sprintf(buf, "Time:%02d:%02d:%02d", hour, min, sec);
    OLED_ShowString(0, 0, buf);
    sprintf(buf, "T:%dC  H:%d%%", temp, humi);
    OLED_ShowString(0, 2, buf);
    OLED_ShowString(0, 4, "AM/NN/PM Set:K1");
    OLED_ShowString(0, 6, "Status:  OK     ");
}

void OLED_ShowAlert(uint8_t hour, uint8_t min, uint8_t sec, uint8_t group) {
    char buf[20];
    OLED_Clear();
    sprintf(buf, "Time:%02d:%02d:%02d", hour, min, sec);
    OLED_ShowString(0, 0, buf);
    sprintf(buf, "!!TAKE MED[%s]!!", GroupName(group));
    OLED_ShowString(0, 2, buf);
    OLED_ShowString(0, 4, "K1:Manual trigger");
    OLED_ShowString(0, 6, "K2:Confirm taken ");
}

void OLED_ShowSetting(uint8_t group, uint8_t set_h, uint8_t set_m) {
    char buf[20];
    OLED_Clear();
    sprintf(buf, "--SET %s ALARM--", GroupName(group));
    OLED_ShowString(0, 0, buf);
    sprintf(buf, "Hour  : %02d", set_h);
    OLED_ShowString(0, 2, buf);
    sprintf(buf, "Minute: %02d", set_m);
    OLED_ShowString(0, 4, buf);
    OLED_ShowString(0, 6, "K3:Grp K4:Save  ");
}

void OLED_ShowSaved(uint8_t group, uint8_t h, uint8_t m) {
    char buf[20];
    OLED_Clear();
    sprintf(buf, " [%s] Saved!    ", GroupName(group));
    OLED_ShowString(0, 0, buf);
    sprintf(buf, "  Set: %02d:%02d   ", h, m);
    OLED_ShowString(0, 3, buf);
}

void OLED_ShowTimeout(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "!! WARNING !!   ");
    OLED_ShowString(0, 2, "Medicine NOT    ");
    OLED_ShowString(0, 4, "taken timeout!  ");
}
