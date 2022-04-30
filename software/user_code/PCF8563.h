#ifndef __PCF8563_H
#define __PCF8563_H

#define PCF8563_READ_ADDRESS 0xA3
#define PCF8563_WRITE_ADDRESS 0xA2
#define PCF8563_CONTROL1 0x00
#define PCF8563_CONTROL2 0x01
#define PCF8563_SECOND 0x02
#define PCF8563_MINUTE 0x03
#define PCF8563_HOUR 0x04
#define PCF8563_DAY 0x05
#define PCF8563_WEEKDAY 0x06
#define PCF8563_MONTH 0x07
#define PCF8563_YEAR 0x08
#define PCF8563_MINUTE_ALARM 0x09
#define PCF8563_HOUR_ALARM 0x0A
#define PCF8563_DAY_ALARM 0x0B
#define PCF8563_WEEKDAY_ALARM 0x0C
#define PCF8563_CLKOUT_FREQ 0x0D
#define PCF8563_TIMER_CONTROL 0x0E
#define PCF8563_TIMER_COUNTDOWN_VALUE 0x0F

typedef struct
{
	unsigned char sec; //秒 [0,59]
	unsigned char min; //分 [0,59]
	unsigned char hour; //小时 [0,23]
	unsigned char day; //日 [1,31]
	unsigned char week; //星期 周日为0，其余的以此类推
	unsigned char mon; //月 [1,12]
	unsigned char year; //年，从2000年起，只保存后两位
} RTC_TIME_STRUCT;

extern RTC_TIME_STRUCT time_bcd;
extern RTC_TIME_STRUCT time_dec;


void PCF8563_Init(void);
void ReadTime(void);
void FormatTime(void);
void time_to_dec(void);
void SaveTime(void);
unsigned char BCD_INC(unsigned char num);
unsigned char BCD_DEC(unsigned char num);
unsigned char BCD_to_DEC (unsigned char num);
unsigned char DEC_to_BCD (unsigned char num);

#endif
