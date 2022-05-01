#include "usart1.h"
#include "ringbuffer.h"
extern rb_handle rb_ctx;

unsigned char USART1_RX_BUF[USART1_REC_LEN];//���ջ���,���USART1_REC_LEN���ֽ�.

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
unsigned short int USART1_RX_STA = 0;     //����״̬���

unsigned char USART1_RX_RAW = 0; // ���������ݱ�ǣ�����͸��


/*
����ԭ�ͣ�void Uart1SendByte(unsigned char dat)
�������ܣ�ͨ������1����һ���ֽ�
��    ����dat Ҫ���͵�����
�� �� ֵ����
��    ע��
*/
void Usart1SendByte(unsigned char dat)
{
	while((USART1->SR & 0x40) == 0);//�ȴ���һ�δ������ݷ������
	USART1->DR = (unsigned char)dat;//д��Ҫ���͵�����
}

/*
����ԭ�ͣ�void USART1_IRQHandler(void)
�������ܣ�����1�жϷ�����
��    ������
�� �� ֵ����
��    ע��HAL���Դ��Ĵ����жϷ��������ù�ϵ����Ч�ʵͣ��ұ�д�Լ����жϷ�����
*/
void USART1_IRQHandler(void)
{
	unsigned char res;

	if(USART1->SR & 0x20)	//���յ�����
	{
		res = USART1->DR;

		if(USART1_RX_RAW)
		{
			rb_write_byte(&rb_ctx, res);
			return;
		}
		
		if((USART1_RX_STA & 0x8000) == 0) //����δ���
		{
			if(USART1_RX_STA & 0x4000) //���յ���0x0d
			{
				if(res != 0x0a)USART1_RX_STA = 0; //���մ���,���¿�ʼ
				else USART1_RX_STA |= 0x8000;	//���������
			}
			else  //��û�յ�0X0D
			{
				if(res == 0x0d)USART1_RX_STA |= 0x4000;
				else
				{
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = res;
					USART1_RX_STA++;
					if(USART1_RX_STA > (USART1_REC_LEN - 1))USART1_RX_STA = 0; //�������ݴ���,���¿�ʼ����
				}
			}
		}
	}
}
