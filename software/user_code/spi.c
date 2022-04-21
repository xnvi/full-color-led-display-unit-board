#include "spi.h"

// 初始化等代码由 CubeMX 生成，HAL库的SPI读写函数太复杂，单独实现一个超级简易版
// 小心与CubeMX生成的 spi.c 冲突

unsigned char SPI1_ReadWriteByte(unsigned char dat)
{
	unsigned short int timer = 0;
	
	while((SPI1->SR & 1 << 1) == 0) //等待发送区空
	{
		timer++;
		if(timer >= 65530)
		{
			return 0;//超时退出
		}
	}
	SPI1->DR = dat;//发送一字节

	timer = 0;
	while((SPI1->SR & 1 << 0) == 0) //等待收完一字节
	{
		timer++;
		if(timer >= 65530)
		{
			return 0;//超时退出
		}
	}
	return SPI1->DR;//返回收到的数据
}

