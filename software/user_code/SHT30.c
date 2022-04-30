#include "SHT30.h"

static uint8_t SHT30Data[6] = {0};

// 低7位地址
// #define SHT30_ADDR 0x44
// #define SHT30_ADDR2 0x45

// 右移一位
#define SHT30_ADDR 0x88
#define SHT30_ADDR2 0x8A


void SHT30WriteCmd(SHT30_CMDS cmd)
{
	IICStart();
	IICWrite(SHT30_ADDR);//发送设备地址+写信号
	IICSlaRes();
	IICWrite(cmd >> 8);
	IICSlaRes();
	IICWrite(cmd & 0xFF);
	IICSlaRes();
	IICStop();
}

void SHT30SoftRst()
{
	SHT30WriteCmd(CMD_SOFT_RESET);
}

void SHT30Init()
{
	SHT30WriteCmd(CMD_SOFT_RESET);
	HAL_Delay(100);
	SHT30WriteCmd(CMD_CLEAR_STATUS);
	HAL_Delay(10);
	SHT30WriteCmd(CMD_HEATER_DISABLE);
	HAL_Delay(10);
	SHT30WriteCmd(CMD_MEAS_PERI_05_L);
	HAL_Delay(10);
}

// 不检查crc
uint16_t SHT3X_Read2BytesAndCrc()
{
	IICStart();
	IICWrite((unsigned char)(SHT30_ADDR+1));//发送设备地址+读信号
	IICSlaRes();

	SHT30Data[0] = IICRead();
	IICMasRes();
	SHT30Data[1] = IICRead();
	IICMasRes();
	SHT30Data[2] = IICRead();
	IICNoRes();//一定要有不应答信号
	IICStop();

	return (SHT30Data[0] << 8) + SHT30Data[1];
}

uint16_t SHT30ReadStatus()
{
	SHT30WriteCmd(CMD_READ_STATUS);
	return SHT3X_Read2BytesAndCrc();
}

void SHT30ReadAll()
{
	unsigned char i;

	IICStart();
	IICWrite((unsigned char)(SHT30_ADDR+1));//发送设备地址+读信号
	IICSlaRes();

	for(i=0; i<5; i++)
	{
		SHT30Data[i] = IICRead();
		IICMasRes();
	}
	SHT30Data[i] = IICRead();
	IICNoRes();//一定要有不应答信号
	IICStop();
}

// 摄氏度
float SHT30GetCTemp()
{
	int temp = (SHT30Data[0] * 256 + SHT30Data[1]);
	float cTemp = -45.0 + (175.0 * temp / 65535.0);
	return cTemp;
}

// 华氏度
float SHT30GetFTemp()
{
	int temp = (SHT30Data[0] * 256 + SHT30Data[1]);
	float fTemp = -49.0 + (315.0 * temp / 65535.0);
	return fTemp;
}

float SHT30GetHumi()
{
	float humidity = 100.0 * (SHT30Data[3] * 256 + SHT30Data[4]) / 65535.0;
	return humidity;
}
