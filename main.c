/*
 * ============================================================
 *  智能药盒主程序 V3.0
 *  MCU  : STM32F103C8T6 (72MHz)
 *  IDE  : Keil MDK (ARM-CM3, AC6编译器)
 * ============================================================
 */

#include "main.h"
#include "ds1302.h"
#include "oled.h"
#include "dht11.h"
#include "jr6001.h"
#include "ws2812.h"
#include "key.h"

SysState_t  g_state         = STATE_IDLE;
AlarmTime_t g_alarms[ALARM_GROUP_COUNT] = {
    {8,  0, 1},
    {12, 0, 1},
    {18, 0, 1},
};

uint8_t     g_temp          = 0;
uint8_t     g_humi          = 0;
uint8_t     g_set_group     = 0;
uint8_t     g_set_hour      = 8;
uint8_t     g_set_minute    = 0;
uint32_t    g_alert_tick    = 0;
uint8_t     g_alert_timeout = 0;

static DS1302_Time_t s_time;
static uint32_t s_sys_tick     = 0;
static uint32_t s_last_disp    = 0;
static uint32_t s_dht_tick     = 0;
static uint32_t s_led_tick     = 0;
static uint8_t  s_last_min     = 0xFF;
static uint8_t  s_led_state    = 0;
static uint8_t  s_alert_group  = 0;

void SysTick_Handler(void) { s_sys_tick++; }
uint32_t GetTick(void) { return s_sys_tick; }
void Delay_ms(uint32_t ms) { uint32_t start = GetTick(); while((GetTick() - start) < ms); }
void Delay_us(uint32_t us) { uint32_t i = us * 72 / 8; while(i--) __NOP(); }

static void Enter_Alert(uint8_t group) {
    g_state        = STATE_ALERT;
    s_alert_group  = group;
    g_alert_tick   = GetTick();
    g_alert_timeout = 0;

    WS2812_AlertRed();
    JR6001_SayTakeMedicine();
    OLED_ShowAlert(s_time.hour, s_time.minute, s_time.second, group);
}

static void Stop_Alert_Confirmed(void) {
    g_state = STATE_IDLE;
    WS2812_ConfirmGreen();
    JR6001_SayTaken();
    Delay_ms(600);
    WS2812_Off();
}

static void Stop_Alert_Timeout(void) {
    g_state = STATE_IDLE;
    g_alert_timeout = 1;
    WS2812_BuzzerBeep(3);
    WS2812_Off();
    OLED_ShowTimeout();
    Delay_ms(3000);
}

static void Check_Alarm(void) {
    uint8_t i;
    if(g_state != STATE_IDLE) return;
    if(s_time.minute == s_last_min) return;
    s_last_min = s_time.minute;

    for(i = 0; i < ALARM_GROUP_COUNT; i++) {
        if(g_alarms[i].enabled &&
           s_time.hour   == g_alarms[i].hour &&
           s_time.minute == g_alarms[i].minute) {
            Enter_Alert(i);
            return;
        }
    }
}

static void Handle_Idle(void) {
    uint8_t key_num = 0;
    KeyEvent_t evt  = Key_ScanAll(&key_num);
    uint32_t now    = GetTick();

    if(key_num == 1 && evt == KEY_SHORT) {
        Enter_Alert(0);
        return;
    }

    if(key_num == 1 && evt == KEY_LONG) {
        g_state      = STATE_SETTING;
        g_set_group  = 0;
        g_set_hour   = g_alarms[0].hour;
        g_set_minute = g_alarms[0].minute;
        WS2812_SettingBlue();
        OLED_ShowSetting(g_set_group, g_set_hour, g_set_minute);
        return;
    }

    if(now - s_last_disp > 500) {
        s_last_disp = now;
        OLED_ShowIdle(s_time.hour, s_time.minute, s_time.second,
                      g_temp, g_humi);
    }
}

static void Handle_Alert(void) {
    uint32_t now    = GetTick();
    uint8_t  key_num = 0;
    KeyEvent_t evt   = Key_ScanAll(&key_num);

    if(key_num == 2 && evt == KEY_SHORT) {
        Stop_Alert_Confirmed();
        return;
    }

    if(now - g_alert_tick > 600000) {
        Stop_Alert_Timeout();
        return;
    }

    if(now - s_led_tick > 500) {
        s_led_tick  = now;
        s_led_state = !s_led_state;
        if(s_led_state) WS2812_AlertRed();
        else            WS2812_Off();
    }

    if(now - s_last_disp > 1000) {
        s_last_disp = now;
        OLED_ShowAlert(s_time.hour, s_time.minute, s_time.second, s_alert_group);
    }
}

static void Handle_Setting(void) {
    uint8_t    key_num = 0;
    KeyEvent_t evt     = Key_ScanAll(&key_num);
    uint32_t   now     = GetTick();

    if(key_num == 2 && evt == KEY_SHORT) {
        g_set_hour = (g_set_hour + 1) % 24;
        OLED_ShowSetting(g_set_group, g_set_hour, g_set_minute);
        return;
    }

    if(key_num == 3 && evt == KEY_SHORT) {
        g_alarms[g_set_group].hour   = g_set_hour;
        g_alarms[g_set_group].minute = g_set_minute;
        g_set_group = (g_set_group + 1) % ALARM_GROUP_COUNT;
        g_set_hour   = g_alarms[g_set_group].hour;
        g_set_minute = g_alarms[g_set_group].minute;
        OLED_ShowSetting(g_set_group, g_set_hour, g_set_minute);
        return;
    }

    if(key_num == 4 && evt == KEY_SHORT) {
        g_set_minute = (g_set_minute + 1) % 60;
        OLED_ShowSetting(g_set_group, g_set_hour, g_set_minute);
        return;
    }

    if(key_num == 1 && evt == KEY_SHORT) {
        g_alarms[g_set_group].hour    = g_set_hour;
        g_alarms[g_set_group].minute  = g_set_minute;
        g_alarms[g_set_group].enabled = 1;

        g_state = STATE_IDLE;
        WS2812_Off();
        JR6001_SayAlarmSet();
        OLED_ShowSaved(g_set_group, g_set_hour, g_set_minute);
        Delay_ms(2000);
        return;
    }

    if(now - s_last_disp > 500) {
        s_last_disp = now;
        OLED_ShowSetting(g_set_group, g_set_hour, g_set_minute);
    }
}

static void System_Init(void) {
    SystemInit();
    SysTick_Config(SystemCoreClock / 1000);

    DS1302_Init();
    DHT11_Init();
    OLED_Init();
    JR6001_Init();
    WS2812_Init();
    Key_Init();

    WS2812_ConfirmGreen();
    Delay_ms(300);
    WS2812_Off();
}

int main(void) {
    System_Init();

    while(1) {
        uint32_t now = GetTick();

        DS1302_ReadTime(&s_time);

        if(now - s_dht_tick > 5000) {
            s_dht_tick = now;
            DHT11_Read(&g_temp, &g_humi);
        }

        Check_Alarm();

        switch(g_state) {
            case STATE_IDLE:    Handle_Idle();    break;
            case STATE_ALERT:   Handle_Alert();   break;
            case STATE_SETTING: Handle_Setting(); break;
            default: g_state = STATE_IDLE;        break;
        }
    }
}
