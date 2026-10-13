#include "jr6001.h"
#include "stm32f10x.h"

/*
 * JR6001 语音模块
 * STM32 USART1: PA9(TX) -> JR6001 RX
 *               PA10(RX) -> JR6001 TX
 * 波特率: 9600
 *
 * 语音曲目约定 (需提前烧录到JR6001):
 *   001.wav — "该吃药啦"
 *   002.wav — "已记录服药"
 *   003.wav — "定时设置成功"
 */

void JR6001_Init(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = 9600;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

static void JR6001_SendByte(uint8_t data) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC)  == RESET);
}

void JR6001_Play(uint8_t track) {
    JR6001_SendByte(0xAA);
    JR6001_SendByte(track);
}

void JR6001_SayTakeMedicine(void) { JR6001_Play(JR6001_TRACK_TAKE);  }
void JR6001_SayTaken(void)        { JR6001_Play(JR6001_TRACK_TAKEN); }
void JR6001_SayAlarmSet(void)     { JR6001_Play(JR6001_TRACK_SET);   }
