#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "stm32f1xx_hal.h"

#include "utils.h"
#include "sfud.h"
#include "usart1.h"
#include "mgl.h"

const uint16_t color_map[13] = {
	0xFFFF, // WHITE   白色
	0xFFE0, // YELLOW  黄色
	0xF81F, // MAGENTA 品红
	0xF800, // RED     红色
	// 0xC618, // LGRAY   灰白色
	// 0x7BEF, // DGRAY   深灰色
	0x7BE0, // OLIVE   橄榄绿
	0x780F, // PURPLE  紫色
	0x7800, // MAROON  深红色
	0x07FF, // CYAN    青色
	0x07E0, // GREEN   绿色
	0x03EF, // DCYAN   深青色
	0x03E0, // DGREEN  深绿色
	0x001F, // BLUE    蓝色
	0x000F, // NAVY    深蓝色
	// 0x0000, // BLACK   黑色
};

uint8_t g_screen_color_index = 0;
uint8_t g_screen_luma = 3;
uint8_t g_screen_auto_luma = 1;

int test(int input)
{
	int t;
	return t += input;
}

void erase_spi(void)
{
	sfud_err result = SFUD_SUCCESS;

	// sfud_flash *flash = sfud_get_device_table() + 0;
	sfud_flash *flash = sfud_get_device(0);

	// result = sfud_erase(flash, addr, size);
	result = sfud_chip_erase(flash);
	if (result == SFUD_SUCCESS)
	{
		// printf("Erase the %s flash data finish. Start from 0x%08X, size is %ld.\r\n", flash->name, addr, size);
		printf("Erase the %s flash data finish\r\n", flash->name);
	}
	else
	{
		printf("Erase the %s flash data failed.\r\n", flash->name);
		return;
	}
}

void write_spi(void)
{
	USART1_RX_RAW = 1;
}

void fill_pic_rainbow(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t *pic)
{
	// unsigned char i,j;
	// unsigned char *p;
	int16_t dx, dy;

	uint8_t i;
	uint8_t tmp;

	uint16_t color16 = 0;
	uint32_t color32 = 0;
	static uint32_t color_count = 0;
	color_count += 20;
	if(color_count > 1536)
	{
		color_count = 0;
	}

	for(dy = y; dy < y + height; dy += 8)
	{
		for(dx = x; dx < x + width; dx++)
		{
			tmp = *pic;

			color32 = mgl_color_ring((color_count + dx*16)%1536);
			color16 = mgl_rgb32_to_rgb16(color32);

			for(i=0; i<8; i++)
			{
				if(tmp & 0x01)
				{
					// mgl_draw_point(dx, dy+i, 0x5555);
					mgl_draw_point(dx, dy+i, color16);
				}
				else
				{
					// 不清除像素点，可以实现叠加效果，这个主要是为了方便显示文字
					// mgl_draw_point(dx, y+i, 0);
					mgl_draw_point(dx, dy+i, 0);
				}
				tmp >>= 1;
			}
			pic++;
		}
	}
}


void fill_pic(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t *pic)
{
	int16_t dx, dy;
	uint8_t i;
	uint8_t tmp;

	for(dy = y; dy < y + height; dy += 8)
	{
		for(dx = x; dx < x + width; dx++)
		{
			tmp = *pic;
			for(i=0; i<8; i++)
			{
				if(tmp & 0x01)
				{
					mgl_draw_point(dx, dy+i, 0);
					// mgl_draw_point(dx, dy+i, 0xFF);
				}
				else
				{
					mgl_draw_point(dx, dy+i, 0xFFFF);
					// mgl_draw_point(dx, dy+i, 0);
				}
				tmp >>= 1;
			}
			pic++;
		}
	}
}

// 输入若干值，去掉一个最大值去掉一个最小值，计算平均值
#define MAX_AVG_LENGTH 10
typedef struct {
    int16_t index;
    int16_t buf[MAX_AVG_LENGTH];
} avg_context;

avg_context luma_ctx = {.index = 0};

static int32_t avg_max_min(avg_context *avg_ctx, int32_t val)
{
    int32_t sum = 0;
    int32_t max = INT32_MIN;
    int32_t min = INT32_MAX;
    int32_t i = 0;

    avg_ctx->buf[avg_ctx->index] = val;
    avg_ctx->index = (avg_ctx->index + 1) % MAX_AVG_LENGTH;

    for (i = 0; i < MAX_AVG_LENGTH; i++)
    {
        sum += avg_ctx->buf[i];
        if (avg_ctx->buf[i] > max) {
            max = avg_ctx->buf[i];
        }
        if (avg_ctx->buf[i] < min) {
            min = avg_ctx->buf[i];
        }
    }
    sum = sum - max - min;

    return sum / (MAX_AVG_LENGTH - 2);
}

void auto_luma()
{
	extern ADC_HandleTypeDef hadc1;
	ADC_ChannelConfTypeDef sConfig;
	uint32_t adc_val = 0;
	uint32_t tmp = 0;
	// int32_t i = 0;
	// static uint32_t adc_val_old = 0;

	//设置ADC的输入为通道0，也就是PA0
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	// sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;//设置采样周期，时间越长越准确
	sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;//设置采样周期，时间越长越准确
	// sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;//设置采样周期，时间越长越准确
	// sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;//设置采样周期，时间越长越准确
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	// if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	// {
	// 	_Error_Handler(__FILE__, __LINE__);
	// }

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 2);//等待转换结束，超时等待2ms
	adc_val = HAL_ADC_GetValue(&hadc1);

	// adc_val_old = adc_val_old * 970 / 1000 + adc_val * 30 / 1000;
	// printf("%d %d\r\n", adc_val, adc_val_old);
	
	// 这种方法能用但是效果一般，以后有精力可以做一个更好的亮度曲线
	// // 相当于以2为底取对数
	// tmp = adc_val_old;
	// i = 0;
	// while (tmp >>= 1)
	// {
	// 	i += 1;
	// }

	tmp = avg_max_min(&luma_ctx, adc_val);

	// ADC的值很难小于32，所以i的值一般在5到11之间
	if (tmp < 200) {
		g_screen_luma = 1;
	}
	else if (tmp < 500) {
		g_screen_luma = 2;
	}
	else if (tmp < 1000) {
		g_screen_luma = 3;
	}
	else if (tmp < 2000) {
		g_screen_luma = 5;
	}
	else {
		g_screen_luma = 8;
	}
}
