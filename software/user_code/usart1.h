#ifndef __USART1_H
#define __USART1_H

#include "stm32f1xx_hal.h"
#include <stdio.h>

#define USART1_REC_LEN 128//�����������ֽ���

extern unsigned char USART1_RX_BUF[];//���ջ���
extern unsigned short int USART1_RX_STA;//����״̬���
extern unsigned char USART1_RX_RAW; // ���������ݱ��

//void Usart1Init(unsigned int pclk1, unsigned int bound);
void Usart1SendByte(unsigned char dat);

#endif
