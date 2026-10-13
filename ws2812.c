#include "ws2812.h"
#include "main.h"

#define WS2812_H()   GPIO_SetBits(WS2812_PORT,   WS2812_PIN)
#define WS2812_L()   GPIO_ResetBits(WS2812_PORT, WS2812_PIN)

static void delay_us_ws(uint32_t us) {
    uint32_t i = us * 72 / 5;
    while(i--) __NOP();
}

void WS2812_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = WS2812_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WS2812_PORT, &GPIO_InitStructure);

    WS2812_L();
}

static void WS2812_SendBit(uint8_t bit) {
    if(bit) {
        WS2812_H();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        WS2812_L();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
    } else {
        WS2812_H();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        WS2812_L();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
    }
}

static void WS2812_SendByte(uint8_t byte) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        WS2812_SendBit((byte >> (7 - i)) & 0x01);
    }
}

static void WS2812_Reset(void) {
    WS2812_L();
    delay_us_ws(60);
}

void WS2812_SetColor(uint8_t r, uint8_t g, uint8_t b) {
    WS2812_Reset();
    WS2812_SendByte(g);
    WS2812_SendByte(r);
    WS2812_SendByte(b);
    WS2812_Reset();
}

void WS2812_Off(void) {
    WS2812_SetColor(0, 0, 0);
}

void WS2812_AlertRed(void) {
    WS2812_SetColor(40, 0, 0);
}

void WS2812_ConfirmGreen(void) {
    WS2812_SetColor(0, 40, 0);
}

void WS2812_SettingBlue(void) {
    WS2812_SetColor(0, 0, 40);
}

void WS2812_BuzzerBeep(uint8_t times) {
    uint8_t i;
    for(i = 0; i < times; i++) {
        WS2812_AlertRed();
        Delay_ms(300);
        WS2812_Off();
        Delay_ms(200);
    }
}
