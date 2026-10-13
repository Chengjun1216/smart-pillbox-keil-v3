#ifndef __JR6001_H
#define __JR6001_H

#include "stm32f10x.h"

#define JR6001_TRACK_TAKE   1   // "该吃药啦"
#define JR6001_TRACK_TAKEN  2   // "已记录服药"
#define JR6001_TRACK_SET    3   // "定时设置成功"

void JR6001_Init(void);
void JR6001_Play(uint8_t track);
void JR6001_SayTakeMedicine(void);
void JR6001_SayTaken(void);
void JR6001_SayAlarmSet(void);

#endif
