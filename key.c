#include "key.h"
#include "main.h"

void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = KEY1_PIN | KEY2_PIN | KEY3_PIN | KEY4_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static uint8_t Key_Read(uint8_t key) {
    switch(key) {
        case 1: return GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN);
        case 2: return GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN);
        case 3: return GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN);
        case 4: return GPIO_ReadInputDataBit(KEY4_PORT, KEY4_PIN);
        default: return 1;
    }
}

KeyEvent_t Key_Scan(uint8_t key) {
    static uint32_t press_tick[5] = {0};
    static uint8_t  pressed[5]    = {0};

    if(Key_Read(key) == Bit_RESET) {
        Delay_ms(10);
        if(Key_Read(key) == Bit_RESET) {
            if(!pressed[key]) {
                pressed[key]    = 1;
                press_tick[key] = 0;
            } else {
                press_tick[key]++;
                if(press_tick[key] >= 200) {  // ~2000ms
                    pressed[key]    = 0;
                    press_tick[key] = 0;
                    while(Key_Read(key) == Bit_RESET) Delay_ms(10);
                    return KEY_LONG;
                }
            }
        }
    } else {
        if(pressed[key]) {
            pressed[key] = 0;
            if(press_tick[key] > 2) {
                press_tick[key] = 0;
                return KEY_SHORT;
            }
            press_tick[key] = 0;
        }
    }
    return KEY_NONE;
}

KeyEvent_t Key_ScanAll(uint8_t *key_num) {
    uint8_t i;
    KeyEvent_t evt;
    for(i = 1; i <= 4; i++) {
        evt = Key_Scan(i);
        if(evt != KEY_NONE) {
            *key_num = i;
            return evt;
        }
    }
    *key_num = 0;
    return KEY_NONE;
}
