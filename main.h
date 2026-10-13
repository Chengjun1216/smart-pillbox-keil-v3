#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

#define DHT11_PIN           GPIO_Pin_5
#define DHT11_PORT          GPIOB

#define OLED_SCL_PIN        GPIO_Pin_11
#define OLED_SCL_PORT       GPIOB
#define OLED_SDA_PIN        GPIO_Pin_10
#define OLED_SDA_PORT       GPIOB

#define DS1302_CE_PIN       GPIO_Pin_5
#define DS1302_CE_PORT      GPIOA
#define DS1302_DATA_PIN     GPIO_Pin_6
#define DS1302_DATA_PORT    GPIOA
#define DS1302_SCLK_PIN     GPIO_Pin_7
#define DS1302_SCLK_PORT    GPIOA

#define WS2812_PIN          GPIO_Pin_0
#define WS2812_PORT         GPIOA

#define KEY1_PIN            GPIO_Pin_1
#define KEY1_PORT           GPIOA
#define KEY2_PIN            GPIO_Pin_2
#define KEY2_PORT           GPIOA
#define KEY3_PIN            GPIO_Pin_3
#define KEY3_PORT           GPIOA
#define KEY4_PIN            GPIO_Pin_4
#define KEY4_PORT           GPIOA

typedef enum {
    STATE_IDLE    = 0,
    STATE_ALERT   = 1,
    STATE_SETTING = 2,
} SysState_t;

#define ALARM_GROUP_COUNT 3

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t enabled;
} AlarmTime_t;

extern SysState_t  g_state;
extern AlarmTime_t g_alarms[ALARM_GROUP_COUNT];
extern uint8_t     g_temp;
extern uint8_t     g_humi;
extern uint8_t     g_set_group;
extern uint8_t     g_set_hour;
extern uint8_t     g_set_minute;
extern uint32_t    g_alert_tick;
extern uint8_t     g_alert_timeout;

void Delay_ms(uint32_t ms);
void Delay_us(uint32_t us);

#endif
