#include <stdio.h>
#include <string.h>
#include "stm32f1xx_hal.h"

#include "clock.h"
#include "key.h"
#include "font_num.h"
#include "PCF8563.h"
#include "font_lite.h"

static void draw4x7(uint8_t *font, int16_t x, int16_t y)
{
	uint8_t i, j;
	uint8_t tmpfont; // 暂存一字节的字符图形数据
	
	for(j=0; j<4; j++)
	{
		tmpfont = font[j];
		for(i=0; i<8; i++)
		{
			if(tmpfont & 0x01)
			{
				DrawPoint(x+j, y+i);
			}
			else
			{
				CleanPoint(x+j, y+i);
			}
			tmpfont >>= 1;
		}
	}
}

static void draw12x24(uint8_t *font, int16_t x, int16_t y)
{
	uint8_t i, j;
	uint8_t tmpfont; // 暂存一字节的字符图形数据
	
	for(j=0; j<12; j++)
	{
		tmpfont = font[j];
		for(i=0; i<8; i++)
		{
			if(tmpfont & 0x01)
			{
				DrawPoint(x+j, y+i);
			}
			else
			{
				CleanPoint(x+j, y+i);
			}
			tmpfont >>= 1;
		}
	}
	for(; j<24; j++)
	{
		tmpfont = font[j];
		for(i=0; i<8; i++)
		{
			if(tmpfont & 0x01)
			{
				DrawPoint(x+j-12, y+i+8);
			}
			else
			{
				CleanPoint(x+j-12, y+i+8);
			}
			tmpfont >>= 1;
		}
	}
	for(; j<36; j++)
	{
		tmpfont = font[j];
		for(i=0; i<8; i++)
		{
			if(tmpfont & 0x01)
			{
				DrawPoint(x+j-24, y+i+16);
			}
			else
			{
				CleanPoint(x+j-24, y+i+16);
			}
			tmpfont >>= 1;
		}
	}
}

void display_time()
{
	ReadTime();

	// 年十位，年个位，月十位，月个位，周，日十位，日个位，时十位，时个位，分十位，分个位，秒十位，秒个位
	// uint8_t time_val[13] = {0};
	// time_val[0] = (time_bcd[6] & 0xF0) >> 4;
	// time_val[1] = (time_bcd[6] & 0x0F) >> 0;
	// time_val[2] = (time_bcd[5] & 0xF0) >> 4;
	// time_val[3] = (time_bcd[5] & 0x0F) >> 0;
	// time_val[4] = (time_bcd[4] & 0x0F) >> 4;
	// time_val[5] = (time_bcd[3] & 0xF0) >> 4;
	// time_val[6] = (time_bcd[3] & 0x0F) >> 0;
	// time_val[7] = (time_bcd[2] & 0xF0) >> 4;
	// time_val[8] = (time_bcd[2] & 0x0F) >> 0;
	// time_val[9] = (time_bcd[1] & 0xF0) >> 4;
	// time_val[10] = (time_bcd[1] & 0x0F) >> 0;
	// time_val[11] = (time_bcd[0] & 0xF0) >> 4;
	// time_val[12] = (time_bcd[0] & 0x0F) >> 0;

	// 年
	draw4x7((uint8_t *)&num_font_mini[2*4], 2, 0);
	draw4x7((uint8_t *)&num_font_mini[0*4], 7, 0);
	draw4x7((uint8_t *)&num_font_mini[((time_bcd.year & 0xF0) >> 4) * 4], 12, 0);
	draw4x7((uint8_t *)&num_font_mini[((time_bcd.year & 0x0F) >> 0) * 4], 17, 0);
	// 月
	draw4x7((uint8_t *)&num_font_mini[((time_bcd.mon & 0xF0) >> 4) * 4], 27, 0);
	draw4x7((uint8_t *)&num_font_mini[((time_bcd.mon & 0x0F) >> 0) * 4], 32, 0);
	// 日
	draw4x7((uint8_t *)&num_font_mini[((time_bcd.day & 0xF0) >> 4) * 4], 42, 0);
	draw4x7((uint8_t *)&num_font_mini[((time_bcd.day & 0x0F) >> 0) * 4], 47, 0);
	// 时
	draw12x24((uint8_t *)&num_font_big[((time_bcd.hour & 0xF0) >> 4) * 36], 0, 8);
	draw12x24((uint8_t *)&num_font_big[((time_bcd.hour & 0x0F) >> 0) * 36], 13, 8);
	// 分
	draw12x24((uint8_t *)&num_font_big[((time_bcd.min & 0xF0) >> 4) * 36], 29, 8);
	draw12x24((uint8_t *)&num_font_big[((time_bcd.min & 0x0F) >> 0) * 36], 42, 8);
	// 秒
	draw4x7((uint8_t *)&num_font_mini[((time_bcd.sec & 0xF0) >> 4) * 4], 55, 24);
	draw4x7((uint8_t *)&num_font_mini[((time_bcd.sec & 0x0F) >> 0) * 4], 60, 24);

	if (time_bcd.sec & 0x01)
	{
		DrawPoint(26, 14);
		DrawPoint(27, 14);
		DrawPoint(26, 15);
		DrawPoint(27, 15);

		DrawPoint(26, 24);
		DrawPoint(27, 24);
		DrawPoint(26, 25);
		DrawPoint(27, 25);
	}
	else
	{
		CleanPoint(26, 14);
		CleanPoint(27, 14);
		CleanPoint(26, 15);
		CleanPoint(27, 15);

		CleanPoint(26, 24);
		CleanPoint(27, 24);
		CleanPoint(26, 25);
		CleanPoint(27, 25);
	}

	DrawPoint(22, 3);
	DrawPoint(23, 3);
	DrawPoint(24, 3);
	DrawPoint(25, 3);
	DrawPoint(37, 3);
	DrawPoint(38, 3);
	DrawPoint(39, 3);
	DrawPoint(40, 3);
}

static void set_time_key_handle(uint8_t *cursor_idx, uint8_t *time)
{
	const uint8_t cvt[] = {6,5,3,4,2,1,0};
	uint8_t idx = 255;
	uint8_t key = key_read();
	if(KEY_IS_RISING(key, 1))
	{
		*cursor_idx += 1;
	}
	else if(KEY_IS_RISING(key, 2))
	{
		*cursor_idx -= 1;
	}
	if (*cursor_idx >= 1 && *cursor_idx <= 7)
	{
		idx = cvt[*cursor_idx-1];
		if(KEY_IS_RISING(key, 3))
		{
			time[idx] -= 1;
		}
		if(KEY_IS_RISING(key, 4))
		{
			time[idx] += 1;
		}
	}

	// 检查数值
	switch (idx)
	{
		case 0:
		case 1:
			// 秒，分
			if (time[idx] == 60)
			{
				time[idx] = 0;
			}
			if (time[idx] == 255)
			{
				time[idx] = 59;
			}
			break;
		case 2:
			// 时
			if (time[idx] == 24)
			{
				time[idx] = 0;
			}
			if (time[idx] == 255)
			{
				time[idx] = 23;
			}
			break;
		case 3:
			// 日
			if (time[idx] == 32)
			{
				time[idx] = 1;
			}
			if (time[idx] == 0)
			{
				time[idx] = 31;
			}
			break;
		case 4:
			// 周
			if (time[idx] == 7)
			{
				time[idx] = 0;
			}
			if (time[idx] == 255)
			{
				time[idx] = 6;
			}
			break;
		case 5:
			// 月
			if (time[idx] == 13)
			{
				time[idx] = 1;
			}
			if (time[idx] == 0)
			{
				time[idx] = 12;
			}
			break;
		case 6:
			// 年
			if (time[idx] == 100)
			{
				time[idx] = 0;
			}
			if (time[idx] == 255)
			{
				time[idx] = 99;
			}
			break;
		
		default:
			break;
	}
}

void set_time()
{
	char strbuf[16] = {0};
	RTC_TIME_STRUCT time_tmp;

	uint8_t count = 0;
	uint8_t cursor_idx = 1;
	int16_t x = 0;
	int16_t y = 0;
	const uint8_t cursor_x_points[9] = {0,12,36, 0,48, 0,24, 0,0};
	const uint8_t cursor_y_points[9] = {0,15,15,31,31,15,15,31,0};
	
	mgl_clean();
	ReadTime();
	// FormatTime();
	time_to_dec();
	memcpy(&time_tmp, &time_dec, sizeof(time_tmp));

	while (1)
	{
		HAL_Delay(15);
		count += 1;
		if (count > 50)
		{
			mgl_clean();
			count = 0;
		}

		// 光标闪烁
		x = cursor_x_points[cursor_idx];
		y = cursor_y_points[cursor_idx];
		if (count > 25)
		{
			mgl_draw_line(x, y, x+10, y, 0xF800);
		}
		else
		{
			mgl_draw_line(x, y, x+10, y, 0x0000);
		}

		if (cursor_idx == 0)
		{
			// 不保存退出
			mgl_clean();
			PrintString12(2, 10, "时间未保存");
			HAL_Delay(2000);
			mgl_clean();
			break;
		}
		if (cursor_idx >= 1 && cursor_idx <= 4)
		{
			// 第一页
			sprintf(strbuf, "20%02d年%02d月", time_tmp.year, time_tmp.mon);
			PrintString12(0, 2, strbuf);
			sprintf(strbuf, "%02d日  周", time_tmp.day);
			PrintString12(0, 18, strbuf);

			switch(time_tmp.week)
			{
				case 0:
					PrintString12(48, 18, "日");
					break;
				case 1:
					PrintString12(48, 18, "一");
					break;
				case 2:
					PrintString12(48, 18, "二");
					break;
				case 3:
					PrintString12(48, 18, "三");
					break;
				case 4:
					PrintString12(48, 18, "四");
					break;
				case 5:
					PrintString12(48, 18, "五");
					break;
				case 6:
					PrintString12(48, 18, "六");
					break;
				default:
					break;
			}

			// PrintString12(0, 2,  "20  年  月");
			// PrintString12(0, 18, "  日  周");
		}
		if (cursor_idx >= 5 && cursor_idx <= 7)
		{
			// 第二页
			sprintf(strbuf, "%02d时%02d分", time_tmp.hour, time_tmp.min);
			PrintString12(0, 2, strbuf);
			sprintf(strbuf, "%02d秒", time_tmp.sec);
			PrintString12(0, 18, strbuf);

			// PrintString12(0, 2,  "  时  分");
			// PrintString12(0, 18, "  秒");
		}
		if (cursor_idx == 8)
		{
			// 保存退出
			memcpy(&time_dec, &time_tmp, sizeof(time_tmp));
			time_to_bcd();
			SaveTime();
			mgl_clean();
			PrintString12(2, 10, "时间已保存");
			HAL_Delay(2000);
			mgl_clean();
			break;
		}
		set_time_key_handle(&cursor_idx, (uint8_t *)&time_tmp);
	}
}
