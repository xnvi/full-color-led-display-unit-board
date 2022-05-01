#ifndef __USART1_H
#define __USART1_H

#include "stm32f1xx_hal.h"
#include <stdio.h>

#define USART1_REC_LEN 128//定义最大接收字节数

extern unsigned char USART1_RX_BUF[];//接收缓冲
extern unsigned short int USART1_RX_STA;//接收状态标记
extern unsigned char USART1_RX_RAW; // 接收裸数据标记

//void Usart1Init(unsigned int pclk1, unsigned int bound);
void Usart1SendByte(unsigned char dat);

#endif
