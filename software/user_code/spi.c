#include "spi.h"

// ��ʼ���ȴ����� CubeMX ���ɣ�HAL���SPI��д����̫���ӣ�����ʵ��һ���������װ�
// С����CubeMX���ɵ� spi.c ��ͻ

unsigned char SPI1_ReadWriteByte(unsigned char dat)
{
	unsigned short int timer = 0;
	
	while((SPI1->SR & 1 << 1) == 0) //�ȴ���������
	{
		timer++;
		if(timer >= 65530)
		{
			return 0;//��ʱ�˳�
		}
	}
	SPI1->DR = dat;//����һ�ֽ�

	timer = 0;
	while((SPI1->SR & 1 << 0) == 0) //�ȴ�����һ�ֽ�
	{
		timer++;
		if(timer >= 65530)
		{
			return 0;//��ʱ�˳�
		}
	}
	return SPI1->DR;//�����յ�������
}

