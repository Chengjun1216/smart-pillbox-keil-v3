#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch);
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);

void OLED_ShowIdle(uint8_t hour, uint8_t min, uint8_t sec,
                    uint8_t temp, uint8_t humi);
void OLED_ShowAlert(uint8_t hour, uint8_t min, uint8_t sec, uint8_t group);
void OLED_ShowSetting(uint8_t group, uint8_t set_h, uint8_t set_m);
void OLED_ShowSaved(uint8_t group, uint8_t h, uint8_t m);
void OLED_ShowTimeout(void);

#endif
