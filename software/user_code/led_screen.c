#include "led_screen.h"

uint8_t *gram_rgb_888 = NULL;
uint16_t *gram_rgb_565;

// 为了提高效率，忽略G的第六位，实际按照RGB555的格式输出
void LED_screen_update_rgb_565(uint8_t brightness)
{
	uint16_t i, j, k;
	uint16_t offset = 0;
	uint16_t delay = 0;
	uint16_t mask_r = 0;
	uint16_t mask_g = 0;
	uint16_t mask_b = 0;
	uint16_t color = 0;

	for(k=0; k<5; k++)
	{
		offset = 0;
		// mask_r = 0x0001 << + k;
		// mask_g = 0x0040 << + k;
		// mask_b = 0x0800 << + k;
		mask_b = 0x0001 << k;
		mask_g = 0x0040 << k;
		mask_r = 0x0800 << k;
		for (j=0; j<16; j++)
		{
			// 写入地址
			// port_data = LL_GPIO_ReadOutputPort(LCD_SCREEN_PORT);
			// port_data &= 0x00000FFF;
			// port_data |= i << 12;
			// LL_GPIO_WriteOutputPort(LCD_SCREEN_PORT, port_data);

			(j & 0x01) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, ADDR_A_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, ADDR_A_PIN);
			(j & 0x02) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, ADDR_B_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, ADDR_B_PIN);
			(j & 0x04) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, ADDR_C_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, ADDR_C_PIN);
			(j & 0x08) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, ADDR_D_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, ADDR_D_PIN);
			
			for (i=0; i<64; i++)
			{
				color = gram_rgb_565[offset];
				// color = (gram_rgb_565[pgram] << 8) | gram_rgb_565[pgram + 1];
				// color = (gram_rgb_565[pgram + 1] << 8) | gram_rgb_565[pgram];
				(color & mask_r) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, R1_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, R1_PIN);
				(color & mask_g) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, G1_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, G1_PIN);
				(color & mask_b) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, B1_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, B1_PIN);

				color = gram_rgb_565[offset + 64 * 32 / 2];
				// color = (gram_rgb_565[pgram + 64 * 32] << 8) | gram_rgb_565[pgram + 1 + 64 * 32];
				// color = (gram_rgb_565[pgram + 1 + 64 * 32] << 8) | gram_rgb_565[pgram + 64 * 32];
				(color & mask_r) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, R2_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, R2_PIN);
				(color & mask_g) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, G2_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, G2_PIN);
				(color & mask_b) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, B2_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, B2_PIN);

				offset += 1;

				LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, CLK_PIN);
				__NOP();
				__NOP();
				// __NOP();
				// __NOP();
				LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, CLK_PIN);
				__NOP();
				__NOP();
				// __NOP();
				// __NOP();
			}
			LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, CLK_PIN);

			//锁存
			LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, LAT_PIN);
			__NOP();
			__NOP();
			// __NOP();
			// __NOP();
			LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, LAT_PIN);
			__NOP();
			__NOP();
			// __NOP();
			// __NOP();
			LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, LAT_PIN);

			// 关闭中断，避免闪烁
			__disable_irq();
			LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, OE_PIN);
			//开启显示
			LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, OE_PIN);
			//用延时控制亮度
			delay = brightness << k;
			while(delay--);
			LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, OE_PIN);
			// __NOP();
			// __NOP();
			__enable_irq();
		}
	}
}


void LED_screen_update_rgb_888(uint8_t brightness)
{
	uint16_t i, j, k;
	uint16_t pgram = 0;
	uint16_t delay = 0;
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t mask = 0;

	// 为了提高效率，如果 6 位色深能满足需要就不要用 8 位
	for(k=0; k<6; k++)
	{
		pgram = 0;
		// mask = 1 << (k + 4);
		// mask = 0x80 >> k;
		mask = 0x01 << (k + 2);
		for (i=0; i<16; i++)
		{
			// 写入地址
			// port_data = LL_GPIO_ReadOutputPort(LCD_SCREEN_PORT);
			// port_data &= 0x00000FFF;
			// port_data |= i << 12;
			// LL_GPIO_WriteOutputPort(LCD_SCREEN_PORT, port_data);

			(i & 0x01) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, ADDR_A_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, ADDR_A_PIN);
			(i & 0x02) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, ADDR_B_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, ADDR_B_PIN);
			(i & 0x04) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, ADDR_C_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, ADDR_C_PIN);
			(i & 0x08) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, ADDR_D_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, ADDR_D_PIN);

			for (j=0; j<64; j++)
			{
				r = gram_rgb_888[pgram];
				g = gram_rgb_888[pgram+1];
				b = gram_rgb_888[pgram+2];

				(r & mask) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, R1_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, R1_PIN);
				(g & mask) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, G1_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, G1_PIN);
				(b & mask) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, B1_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, B1_PIN);

				r = gram_rgb_888[pgram+3072];
				g = gram_rgb_888[pgram+3072+1];
				b = gram_rgb_888[pgram+3072+2];
				(r & mask) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, R2_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, R2_PIN);
				(g & mask) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, G2_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, G2_PIN);
				(b & mask) ? LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, B2_PIN) : LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, B2_PIN);

				pgram += 3;
				LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, CLK_PIN);
				__NOP();
				__NOP();
				// __NOP();
				// __NOP();
				LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, CLK_PIN);
				__NOP();
				__NOP();
				// __NOP();
				// __NOP();
			}
			LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, CLK_PIN);

			//锁存
			LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, LAT_PIN);
			__NOP();
			__NOP();
			// __NOP();
			// __NOP();
			LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, LAT_PIN);
			__NOP();
			__NOP();
			// __NOP();
			// __NOP();
			LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, LAT_PIN);

			// 关闭中断，避免闪烁
			__disable_irq();
			LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, OE_PIN);
			//开启显示
			LL_GPIO_ResetOutputPin(LCD_SCREEN_PORT, OE_PIN);
			//用延时控制亮度
			delay = brightness << k;
			while(delay--);
			LL_GPIO_SetOutputPin(LCD_SCREEN_PORT, OE_PIN);
			// __NOP();
			// __NOP();
			__enable_irq();
		}
	}
}
