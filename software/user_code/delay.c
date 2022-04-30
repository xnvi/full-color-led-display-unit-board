#include "delay.h"

/*
����ԭ�ͣ�void delay_us(u32 n)
�������ܣ���ʱ΢��
��    ����n ��ʱ��΢����
�� �� ֵ����
��    ע�����ʹ����HAL�⣬����ñ���������΢����ʱ�����û��ʹ��HAL�⣬���������ܱ�֤��ʱ����ȷ�ԣ�
          �����еĲ��ֲ��������MCU��Ƶ�����޸�
*/
void delay_us(uint32_t n)
{
	uint32_t ticks;
	uint32_t told, tnow, tcnt = 0;
	uint32_t reload = SysTick->LOAD;//LOAD��ֵ
	ticks = n * 72;//��Ҫ�Ľ�������72Mhz
	told = SysTick->VAL;//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow = SysTick->VAL;
		if(tnow != told)
		{
			if(tnow < told)
			{
				tcnt += told - tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			}
			else
			{
				tcnt += reload - tnow + told;
			}
			told = tnow;
			
			if(tcnt >= ticks)
			{
				break;//ʱ�䳬��������Ҫ�ӳٵ�ʱ��,���˳�.
			}
		}
	}
}
