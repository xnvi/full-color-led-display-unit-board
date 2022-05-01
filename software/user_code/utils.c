#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "stm32f1xx_hal.h"

#include "utils.h"
#include "sfud.h"
#include "usart1.h"
#include "mgl.h"

const uint16_t color_map[13] = {
	0xFFFF, // WHITE   ��ɫ
	0xFFE0, // YELLOW  ��ɫ
	0xF81F, // MAGENTA Ʒ��
	0xF800, // RED     ��ɫ
	// 0xC618, // LGRAY   �Ұ�ɫ
	// 0x7BEF, // DGRAY   ���ɫ
	0x7BE0, // OLIVE   �����
	0x780F, // PURPLE  ��ɫ
	0x7800, // MAROON  ���ɫ
	0x07FF, // CYAN    ��ɫ
	0x07E0, // GREEN   ��ɫ
	0x03EF, // DCYAN   ����ɫ
	0x03E0, // DGREEN  ����ɫ
	0x001F, // BLUE    ��ɫ
	0x000F, // NAVY    ����ɫ
	// 0x0000, // BLACK   ��ɫ
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
					// ��������ص㣬����ʵ�ֵ���Ч���������Ҫ��Ϊ�˷�����ʾ����
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

// ��������ֵ��ȥ��һ�����ֵȥ��һ����Сֵ������ƽ��ֵ
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

	//����ADC������Ϊͨ��0��Ҳ����PA0
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	// sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;//���ò������ڣ�ʱ��Խ��Խ׼ȷ
	sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;//���ò������ڣ�ʱ��Խ��Խ׼ȷ
	// sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;//���ò������ڣ�ʱ��Խ��Խ׼ȷ
	// sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;//���ò������ڣ�ʱ��Խ��Խ׼ȷ
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	// if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	// {
	// 	_Error_Handler(__FILE__, __LINE__);
	// }

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 2);//�ȴ�ת����������ʱ�ȴ�2ms
	adc_val = HAL_ADC_GetValue(&hadc1);

	// adc_val_old = adc_val_old * 970 / 1000 + adc_val * 30 / 1000;
	// printf("%d %d\r\n", adc_val, adc_val_old);
	
	// ���ַ������õ���Ч��һ�㣬�Ժ��о���������һ�����õ���������
	// // �൱����2Ϊ��ȡ����
	// tmp = adc_val_old;
	// i = 0;
	// while (tmp >>= 1)
	// {
	// 	i += 1;
	// }

	tmp = avg_max_min(&luma_ctx, adc_val);

	// ADC��ֵ����С��32������i��ֵһ����5��11֮��
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
