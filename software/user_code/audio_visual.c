#include <stdio.h>
#include <string.h>
#include "stm32f1xx_hal.h"

#include "audio_visual.h"
#include "key.h"
#include "kiss_fft.h"
#include "kiss_fftr.h"
#include "sfud.h"
#include "mgl.h"

// �������Ѿ��л�����
#define AUDIO_VISUAL_WITH_RICK_ROLL 0

#define AUDIO_SAMPLE_NUM 256
uint8_t audio_adc_done = 0;
int16_t audio_sample_count = 0;
int16_t audio_buf[AUDIO_SAMPLE_NUM];
int16_t audio_buf_fft[AUDIO_SAMPLE_NUM];


// #define FFT_STATIC_MEM_LEN (256 * sizeof(int16_t) * 2 + 56)
#define FFT_LEN AUDIO_SAMPLE_NUM
#define FFT_STATIC_MEM_LEN 1536
kiss_fftr_cfg fftr_cfg;
size_t fft_mem_len = FFT_STATIC_MEM_LEN;
__align(4) uint8_t fft_mem[FFT_STATIC_MEM_LEN];
kiss_fft_cpx fft_out[FFT_LEN];


#define PEAK_HOLD_COUNT 30 // ��ֵ����ʱ��
typedef struct {
	uint8_t val; // ��ֵ
	uint8_t hold; // ��ֵ���ּ���
} peak_val;
peak_val peak_list[64];

uint8_t show_val[64];


void audio_visual_init()
{
	fftr_cfg = kiss_fftr_alloc(FFT_LEN, 0, fft_mem, &fft_mem_len);
}

// ţ�ٵ���������������a����ֵx�����err
// double Newton(double a, double x, double err)
// {	
// 	while(x*x - a > err || x*x - a < -err)
// 	{
// 		x = (x + a / x) / 2;
// 	}
	
// 	if(x > 0)
// 	{
// 		return x;
// 	}
// 	else 
// 	{
// 		return -x;
// 	}
// }

// ţ�ٵ���������������a����ֵx�����err
int32_t Newton_sqrt(int32_t a)
{	
	int32_t x = 5;
	int32_t err = 1;
	int32_t count = 0; // ���������������޷�����������Ҫ�ľ��ȣ�һ��10�ε����Ͳ����
	while(x*x - a > err || x*x - a < -err)
	{
		x = (x + a / x) / 2;
		count+=1;
		if(count > 8)
		{
			break;
		}
	}
	
	if(x > 0)
	{
		return x;
	}
	else 
	{
		return -x;
	}
}

int32_t Newton_cbrt(int32_t a)
{	
	int32_t x = 5;
	int32_t err = 1;
	int32_t count = 0; // ���������������޷�����������Ҫ�ľ��ȣ�һ��10�ε����Ͳ����
	while(x*x*x - a > err || x*x*x - a < -err)
	{
		x = (2 * x + a / (x * x)) / 3;
		count+=1;
		if(count > 8)
		{
			break;
		}
	}
	
	if(x > 0)
	{
		return x;
	}
	else 
	{
		return -x;
	}
}



void adc_audio_in(void)
{
	extern ADC_HandleTypeDef hadc1;
	ADC_ChannelConfTypeDef sConfig;

	//����ADC������Ϊͨ��1��Ҳ����PA1
	sConfig.Channel = ADC_CHANNEL_1;
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
	audio_buf[audio_sample_count] = HAL_ADC_GetValue(&hadc1);
	audio_sample_count += 1;

	if (audio_sample_count == AUDIO_SAMPLE_NUM)
	{
		audio_sample_count = 0;
		memcpy(audio_buf_fft, audio_buf, sizeof(audio_buf));
		audio_adc_done = 1;
	}
}


#if AUDIO_VISUAL_WITH_RICK_ROLL

uint8_t img_read_buf_sp[48*32*2];
void audio_visual(void)
{
	uint8_t key = 0;
	int32_t count = 0;

	uint32_t frame_count = 0;
	uint32_t spi_addr = 0;
	uint32_t i, j = 0;
	uint32_t timer = 0;
	uint32_t base_time = 0;
	extern uint16_t *gram_rgb_565;
	extern sfud_flash *flash;
	base_time = HAL_GetTick();

	HAL_Delay(20);

	while (1)
	{
		timer = frame_count * 1000 / 30;
		if (HAL_GetTick() > base_time + timer)
		{
			sfud_read(flash, spi_addr, 48 * 32 * 2, img_read_buf_sp);
			spi_addr += 48 * 32 * 2;
			
			for (i = 0; i < 4; i++)
			{
				for (j = 0; j < 8; j++)
				{
					memcpy(&((uint8_t *)gram_rgb_565)[i * 1024 + j * 64 * 2 + 16 * 2], &img_read_buf_sp[i * 8 * 48 * 2 + j * 48 * 2], 48 * 2);
				}
			}

			frame_count += 1;
			if (frame_count >= 440)
			{
				spi_addr = 48*32*2*10;
				frame_count = 0;
				base_time = HAL_GetTick();
			}
		}

		if (audio_adc_done)
		{
			kiss_fftr(fftr_cfg, audio_buf_fft, fft_out);

			for (i = 0; i < 64; i++)
			{
				int32_t val = 0;
				int16_t val_fix = 0;
				int16_t id = 0;
				id = i + 1;
				// ��ȡ���ָ�Ƶ
				if (id > 48)
				{
					id = 64 + (id - 48) * 4;
				}
				else if (id > 32)
				{
					id = 32 + (id - 32) * 2;
				}
				val = fft_out[id].r * fft_out[id].r + fft_out[id].i * fft_out[id].i;
				val = Newton_sqrt(val);
				val_fix = val % 4;
				if (val > 16)
				{
					id = 0;
					while (val >>= 1)
					{
						id += 1;
					}
					val = id * 4 + val_fix;
				}
				val = val > 31 ? 31 : val;
				val = val == 0 ? 1 : val;

				// ����
				if (val > show_val[i])
				{
					show_val[i] = val;
				}
				else {
					val = show_val[i];
					if (count % 2 == 0)
					{
						show_val[i] -= 1;
					}
				}

				if (i < 16)
				{
					mgl_draw_line(i, 0, i, 31 - val, 0x0000); // ����
				}
				mgl_draw_line(i, 32 - val, i, 31, mgl_rgb32_to_rgb16(mgl_color_ring((count/2+i*12)%1536))); // ����

				// ��ֵ����
				if (val > peak_list[i].val)
				{
					peak_list[i].val = val;
					peak_list[i].hold = PEAK_HOLD_COUNT;
				}
				if (peak_list[i].hold != 0)
				{
					// ��ֵ���ֵ���ʱ
					peak_list[i].hold -= 1;
				}
				else
				{
					// ��ֵ����
					if (count % 3 == 0)
					{
						peak_list[i].val -= 1;
					}
				}
				mgl_draw_point(i, 31 - peak_list[i].val, 0xFFFF);
			}
			audio_adc_done = 0;
			count += 1;
		}

		key = key_read();
		if(KEY_IS_RISING(key, 2))
		{
			mgl_clean();
			printf("key out\n");
			break;
		}
	}
}

#else

void audio_visual(void)
{
	uint8_t key = 0;
	int32_t count = 0;
	uint32_t i = 0;

	HAL_Delay(20);

	while (1)
	{
		if (audio_adc_done)
		{
			kiss_fftr(fftr_cfg, audio_buf_fft, fft_out);
			// audio_adc_done = 0;

			for (i = 0; i < 64; i++)
			{
				int32_t val = 0;
				int16_t val_fix = 0;
				int16_t id = 0;
				id = i + 1;
				// ��ȡ���ָ�Ƶ
				if (id > 48)
				{
					id = 64 + (id - 48) * 4;
				}
				else if (id > 32)
				{
					id = 32 + (id - 32) * 2;
				}
				val = fft_out[id].r * fft_out[id].r + fft_out[id].i * fft_out[id].i;
				val = Newton_sqrt(val);
				
				// û��fpu��Ԫ������£������������ܷǳ���
				// val = Newton_cbrt(val);
				// val = sqrt(val);
				// val = log(val) * 4.5;

				// ֱ��ȡ2�Ķ����ٳ�4ͼ����ѿ����ʽ�4��������״��
				// ������ val_fix �ݴ� val % 4 ��ֵ����һ������ԣ�ʹͼ����ÿ�
				val_fix = val % 4;
				// С�źŵ���ֵ��ȡ�����͸�С�ˣ�Ϊ��ͼ����ÿ�С��16��ֵ��������
				if (val > 16)
				{
					// �൱����2Ϊ��ȡ����
					id = 0;
					while (val >>= 1)
					{
						// û�����˼�����ǵ������� id ��ֵ
						id += 1;
					}
					val = id * 4 + val_fix;
				}
				val = val > 31 ? 31 : val;
				val = val == 0 ? 1 : val;

				// ����
				if (val > show_val[i])
				{
					show_val[i] = val;
				}
				else {
					val = show_val[i];
					if (count % 2 == 0)
					{
						show_val[i] -= 1;
					}
					// show_val[i] -= 1;
				}

				mgl_draw_line(i, 0, i, 31 - val, 0x0000); // ����
				// mgl_draw_line(i, 31, i, 31 - val, 0xFFFF); // ����
				mgl_draw_line(i, 32 - val, i, 31, mgl_rgb32_to_rgb16(mgl_color_ring((count/2+i*12)%1536))); // ����

				// ��ֵ����
				if (val > peak_list[i].val)
				{
					peak_list[i].val = val;
					peak_list[i].hold = PEAK_HOLD_COUNT;
				}
				if (peak_list[i].hold != 0)
				{
					// ��ֵ���ֵ���ʱ
					peak_list[i].hold -= 1;
				}
				else
				{
					// ��ֵ����
					if (count % 3 == 0)
					{
						peak_list[i].val -= 1;
					}
				}
				mgl_draw_point(i, 31 - peak_list[i].val, 0xFFFF);
			}
			audio_adc_done = 0;
			count += 1;
		}

		key = key_read();
		if(KEY_IS_RISING(key, 2))
		{
			mgl_clean();
			printf("key out\n");
			break;
		}
	}
}

#endif
