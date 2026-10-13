#ifndef __WS2812_H
#define __WS2812_H

#include "stm32f10x.h"

void WS2812_Init(void);
void WS2812_SetColor(uint8_t r, uint8_t g, uint8_t b);
void WS2812_Off(void);
void WS2812_AlertRed(void);
void WS2812_ConfirmGreen(void);
void WS2812_SettingBlue(void);
void WS2812_BuzzerBeep(uint8_t times);

#endif
