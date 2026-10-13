#include "dht11.h"
#include "main.h"

static void DHT11_SetOutput(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

static void DHT11_SetInput(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

#define DHT11_READ  GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)
#define DHT11_HIGH  GPIO_SetBits(DHT11_PORT, DHT11_PIN)
#define DHT11_LOW   GPIO_ResetBits(DHT11_PORT, DHT11_PIN)

void DHT11_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    DHT11_SetOutput();
    DHT11_HIGH;
}

uint8_t DHT11_Read(uint8_t *temp, uint8_t *humi) {
    uint8_t i, j, buf[5] = {0};
    uint16_t timeout;

    DHT11_SetOutput();
    DHT11_LOW;
    Delay_ms(18);
    DHT11_HIGH;
    Delay_us(30);

    DHT11_SetInput();
    timeout = 10000;
    while(DHT11_READ && timeout--);
    if(!timeout) return 1;
    timeout = 10000;
    while(!DHT11_READ && timeout--);
    if(!timeout) return 1;
    timeout = 10000;
    while(DHT11_READ && timeout--);
    if(!timeout) return 1;

    for(i = 0; i < 5; i++) {
        for(j = 0; j < 8; j++) {
            timeout = 10000;
            while(!DHT11_READ && timeout--);
            if(!timeout) return 1;
            Delay_us(40);
            buf[i] <<= 1;
            if(DHT11_READ) {
                buf[i] |= 1;
                timeout = 10000;
                while(DHT11_READ && timeout--);
            }
        }
    }

    if(buf[4] != (uint8_t)(buf[0] + buf[1] + buf[2] + buf[3])) return 1;

    *humi = buf[0];
    *temp = buf[2];
    return 0;
}
