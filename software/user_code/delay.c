#include "delay.h"

/*
函数原型：void delay_us(u32 n)
函数功能：延时微秒
参    数：n 延时的微秒数
返 回 值：无
备    注：如果使用了HAL库，请调用本函数进行微秒延时，如果没有使用HAL库，本函数不能保证延时的正确性，
          函数中的部分参数请根据MCU主频进行修改
*/
void delay_us(uint32_t n)
{
	uint32_t ticks;
	uint32_t told, tnow, tcnt = 0;
	uint32_t reload = SysTick->LOAD;//LOAD的值
	ticks = n * 72;//需要的节拍数，72Mhz
	told = SysTick->VAL;//刚进入时的计数器值
	while(1)
	{
		tnow = SysTick->VAL;
		if(tnow != told)
		{
			if(tnow < told)
			{
				tcnt += told - tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
			}
			else
			{
				tcnt += reload - tnow + told;
			}
			told = tnow;
			
			if(tcnt >= ticks)
			{
				break;//时间超过、等于要延迟的时间,则退出.
			}
		}
	}
}
