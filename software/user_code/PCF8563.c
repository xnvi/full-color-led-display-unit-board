#include "PCF8563.h"
#include "i2c.h"

RTC_TIME_STRUCT time_bcd;
RTC_TIME_STRUCT time_dec;

void PCF8563_Init()
{
	IICStart();
	IICWrite(PCF8563_WRITE_ADDRESS);
	IICSlaRes();
	IICWrite(PCF8563_CLKOUT_FREQ);
	IICSlaRes();
	
	IICWrite(0x00);//禁止时钟输出
	// IICWrite(0x80);//32.768Hz输出
	// IICWrite(0x81);//1024Hz输出
	// IICWrite(0x82);//32Hz输出
	// IICWrite(0x83);//1Hz输出
	
	IICSlaRes();
	IICStop();
	
	// TODO 中断在PA12，目前用不上
}

void ReadTime()
{
	unsigned char i = 0;

	IICStart();
	IICWrite(PCF8563_WRITE_ADDRESS);
	IICSlaRes();
	IICWrite(PCF8563_SECOND);
	IICSlaRes();

	IICStart();
	IICWrite(PCF8563_READ_ADDRESS);
	IICSlaRes();

	//i<6，否则后面的time[i] = IICRead();会越界
	for (i=0; i<6; i++)
	{
		((unsigned char *)&time_bcd)[i] = IICRead();
		IICMasRes();
	}
	//在VC6中，跳出循环时i=6，以防万一，直接使用time[6] = IICRead();
	((unsigned char *)&time_bcd)[6] = IICRead();
	//((unsigned char *)&time_bcd)[6] = IICRead();
	IICNoRes();

	IICStop();

	FormatTime();
}

void FormatTime()
{
	((unsigned char *)&time_bcd)[0] = ((unsigned char *)&time_bcd)[0] & 0x7F;//秒
	((unsigned char *)&time_bcd)[1] = ((unsigned char *)&time_bcd)[1] & 0x7F;//分
	((unsigned char *)&time_bcd)[2] = ((unsigned char *)&time_bcd)[2] & 0x3F;//时
	((unsigned char *)&time_bcd)[3] = ((unsigned char *)&time_bcd)[3] & 0x3F;//日
	((unsigned char *)&time_bcd)[4] = ((unsigned char *)&time_bcd)[4] & 0x07;//周0-6
	((unsigned char *)&time_bcd)[5] = ((unsigned char *)&time_bcd)[5] & 0x1F;//月
	//((unsigned char *)&time_bcd)[6] = ((unsigned char *)&time_bcd)[6] & 0xFF;//年
}

void time_to_dec()
{
	time_dec.sec  = BCD_to_DEC(time_bcd.sec );
	time_dec.min  = BCD_to_DEC(time_bcd.min );
	time_dec.hour = BCD_to_DEC(time_bcd.hour);
	time_dec.day  = BCD_to_DEC(time_bcd.day );
	time_dec.week = BCD_to_DEC(time_bcd.week);
	time_dec.mon  = BCD_to_DEC(time_bcd.mon );
	time_dec.year = BCD_to_DEC(time_bcd.year);
}

void time_to_bcd()
{
	time_bcd.sec  = DEC_to_BCD(time_dec.sec );
	time_bcd.min  = DEC_to_BCD(time_dec.min );
	time_bcd.hour = DEC_to_BCD(time_dec.hour);
	time_bcd.day  = DEC_to_BCD(time_dec.day );
	time_bcd.week = DEC_to_BCD(time_dec.week);
	time_bcd.mon  = DEC_to_BCD(time_dec.mon );
	time_bcd.year = DEC_to_BCD(time_dec.year);
}

void SaveTime()
{
	unsigned char i = 0;
	
	//格式化时间
	FormatTime();
	//月的最高位为0表示20世纪，为1表示19世纪，正好不用设置

	IICStart();
	IICWrite(PCF8563_WRITE_ADDRESS);
	IICSlaRes();
	IICWrite(PCF8563_SECOND);
	IICSlaRes();

	for (i=0; i<7; i++)
	{
		IICWrite(((unsigned char *)&time_bcd)[i]);
		IICSlaRes();
	}

	IICStop();
}

unsigned char BCD_INC(unsigned char num)
{
	if(num == 0x99)
	{
		//num = 0;
		return 0;
	}
	if((num & 0x0F) == 0x09)
	{
		num += 7;
	}
	else
	{
		num++;
	}
	return num;
}

unsigned char BCD_DEC(unsigned char num)
{
	if(num == 0x00)
	{
		//num = 0x99;
		return 0x99;
	}
	if((num & 0x0F) == 0x00)
	{
		num -= 7;
	}
	else
	{
		num--;
	}
	return num;
}

unsigned char BCD_to_DEC (unsigned char num)
{
//	num = ((num & 0xF0) >> 4) * 10 + (num & 0x0F);
//	if(num > 99)
//	{
//		return 99;
//	}
//	else
//	{
//		return num
//	}

	return ((num & 0xF0) >> 4) * 10 + (num & 0x0F);
}

unsigned char DEC_to_BCD (unsigned char num)
{
//	if(num > 99)
//	{
//		return 0x99;
//	}
//	else
//	{
//		return ((num / 10) << 4) + (num % 10);
//	}

	return ((num / 10) << 4) + (num % 10);
}
