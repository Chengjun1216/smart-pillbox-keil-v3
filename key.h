#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

typedef enum {
    KEY_NONE  = 0,
    KEY_SHORT = 1,
    KEY_LONG  = 2,
} KeyEvent_t;

void       Key_Init(void);
KeyEvent_t Key_Scan(uint8_t key);
KeyEvent_t Key_ScanAll(uint8_t *key_num);

#endif
