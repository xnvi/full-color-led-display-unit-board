#include "usart1.h"
#include "ringbuffer.h"
extern rb_handle rb_ctx;

unsigned char USART1_RX_BUF[USART1_REC_LEN];//接收缓冲,最大USART1_REC_LEN个字节.

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
unsigned short int USART1_RX_STA = 0;     //接收状态标记

unsigned char USART1_RX_RAW = 0; // 接收裸数据标记，用于透传


/*
函数原型：void Uart1SendByte(unsigned char dat)
函数功能：通过串口1发送一个字节
参    数：dat 要发送的数据
返 回 值：无
备    注：
*/
void Usart1SendByte(unsigned char dat)
{
	while((USART1->SR & 0x40) == 0);//等待上一次串口数据发送完成
	USART1->DR = (unsigned char)dat;//写入要发送的数据
}

/*
函数原型：void USART1_IRQHandler(void)
函数功能：串口1中断服务函数
参    数：无
返 回 值：无
备    注：HAL库自带的串口中断服务函数调用关系复杂效率低，我编写自己的中断服务函数
*/
void USART1_IRQHandler(void)
{
	unsigned char res;

	if(USART1->SR & 0x20)	//接收到数据
	{
		res = USART1->DR;

		if(USART1_RX_RAW)
		{
			rb_write_byte(&rb_ctx, res);
			return;
		}
		
		if((USART1_RX_STA & 0x8000) == 0) //接收未完成
		{
			if(USART1_RX_STA & 0x4000) //接收到了0x0d
			{
				if(res != 0x0a)USART1_RX_STA = 0; //接收错误,重新开始
				else USART1_RX_STA |= 0x8000;	//接收完成了
			}
			else  //还没收到0X0D
			{
				if(res == 0x0d)USART1_RX_STA |= 0x4000;
				else
				{
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = res;
					USART1_RX_STA++;
					if(USART1_RX_STA > (USART1_REC_LEN - 1))USART1_RX_STA = 0; //接收数据错误,重新开始接收
				}
			}
		}
	}
}
