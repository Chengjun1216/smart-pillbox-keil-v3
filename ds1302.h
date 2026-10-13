#ifndef __DS1302_H
#define __DS1302_H

#include "stm32f10x.h"

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} DS1302_Time_t;

void    DS1302_Init(void);
void    DS1302_WriteByte(uint8_t addr, uint8_t data);
uint8_t DS1302_ReadByte(uint8_t addr);
void    DS1302_ReadTime(DS1302_Time_t *t);
void    DS1302_SetTime(DS1302_Time_t *t);

#endif
