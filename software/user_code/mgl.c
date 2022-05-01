#include <math.h>
#include <string.h>
#include <stdio.h>

#include "mgl.h"

#if MGL_STATIC_GRAM == 0
	static mgl_color_t *p_gram = NULL;
	static int32_t gram_size = 0;
#else
	#if MGL_COLOR_DEPTH == 1
		static mgl_color_t mgl_gram[MGL_DISP_HOR * MGL_DISP_VER / 8];
		static int32_t gram_size = MGL_DISP_HOR * MGL_DISP_VER / 8 * sizeof(mgl_color_t);
	#elif MGL_COLOR_DEPTH == 8
		static mgl_color_t mgl_gram[MGL_DISP_HOR * MGL_DISP_VER];
		static int32_t gram_size = MGL_DISP_HOR * MGL_DISP_VER * sizeof(mgl_color_t);
	#elif MGL_COLOR_DEPTH == 16
		static mgl_color_t mgl_gram[MGL_DISP_HOR * MGL_DISP_VER];
		static int32_t gram_size = MGL_DISP_HOR * MGL_DISP_VER * sizeof(mgl_color_t);
	#elif MGL_COLOR_DEPTH == 32
		static mgl_color_t mgl_gram[MGL_DISP_HOR * MGL_DISP_VER];
		static int32_t gram_size = MGL_DISP_HOR * MGL_DISP_VER * sizeof(mgl_color_t);
	#endif
	static mgl_color_t *p_gram = mgl_gram;
#endif

uint16_t mgl_rgb_to_rgb16(uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t c = 0;
	c |= (r & 0xF8) << 11;
	c |= (g & 0xFC) << 5;
	c |= (b & 0xF8) >> 3;
	return c;
}

uint16_t mgl_rgb32_to_rgb16(uint32_t argb)
{
	uint16_t c = 0;
	c |= (argb & 0x00F80000) >> 8;
	c |= (argb & 0x0000FC00) >> 5;
	c |= (argb & 0x000000F8) >> 3;
	return c;
}

uint32_t mgl_rgb16_to_rgb32(uint16_t rgb)
{
	uint32_t c = 0;
	c |= (rgb & MGL_RGB565_MASK_R) << 8;
	c |= (rgb & MGL_RGB565_MASK_G) << 5;
	c |= (rgb & MGL_RGB565_MASK_B) << 3;
	return c;
}

// 0 到 1536
uint32_t mgl_color_ring(int32_t index)
{
	uint8_t r = 0, g = 0, b = 0;
	uint8_t hi, lo;

	hi = index >> 8;
	lo = index & 0x000000FF;

	switch (hi)
	{
		case 0:
			r = 255;
			g = lo;
			b = 0;
			break;
		case 1:
			r = 255 - lo;
			g = 255;
			b = 0;
			break;
		case 2:
			r = 0;
			g = 255;
			b = lo;
			break;
		case 3:
			r = 0;
			g = 255 - lo;
			b = 255;
			break;
		case 4:
			r = lo;
			g = 0;
			b = 255;
			break;
		case 5:
			r = 255;
			g = 0;
			b = 255 - lo;
			break;
		
		default:
			break;
	}

	return (r << 16) + (g << 8) + b;
}


// size 指字节数，不是 mgl_color_t 数
void mgl_init(mgl_color_t *buf, int32_t size)
{
	#if MGL_STATIC_GRAM == 0
	p_gram = buf;
	gram_size = size;
	#endif
	memset(p_gram, 0, gram_size);
}

mgl_color_t *mgl_get_gram_addr(void)
{
	return p_gram;
}

void mgl_clean(void)
{
	memset(p_gram, 0, gram_size);
}

void mgl_draw_point(int16_t x, int16_t y, mgl_color_t c)
{
	if(x >= 0 && y >= 0 && x < MGL_DISP_HOR && y < MGL_DISP_VER)
	{
		#if MGL_COLOR_DEPTH == 1
			if(c)
			{
				p_gram[x + MGL_DISP_HOR * (y >> 3)] = p_gram[x + MGL_DISP_HOR * (y >> 3)] | (0x01 << (y & 0x07));
			}
			else
			{
				p_gram[x + MGL_DISP_HOR * (y >> 3)] = p_gram[x + MGL_DISP_HOR * (y >> 3)] & (~(0x01 << (y & 0x07)));
			}
		#elif MGL_COLOR_DEPTH == 8
			p_gram[x + y * MGL_DISP_HOR] = c;
		#elif MGL_COLOR_DEPTH == 16
			p_gram[x + y * MGL_DISP_HOR] = c;
		#elif MGL_COLOR_DEPTH == 32
			p_gram[x + y * MGL_DISP_HOR] = c;
		#endif
	}
}

mgl_color_t mgl_read_point(int16_t x, int16_t y)
{
	if(x >= 0 && y >= 0 && x < MGL_DISP_HOR && y < MGL_DISP_VER)
	{
		#if MGL_COLOR_DEPTH == 1
			return p_gram[x + MGL_DISP_HOR * (y >> 3)] & (0x01 << (y & 0x07)) ? 1 : 0;
		#elif MGL_COLOR_DEPTH == 8
			return p_gram[x + y * MGL_DISP_HOR];
		#elif MGL_COLOR_DEPTH == 16
			return p_gram[x + y * MGL_DISP_HOR];
		#elif MGL_COLOR_DEPTH == 32
			return p_gram[x + y * MGL_DISP_HOR];
		#endif
	}
	return 0;
}

void mgl_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, mgl_color_t c)
{	
	//Bresenham算法
	int16_t dx = x2 - x1;
	int16_t dy = y2 - y1;
	int16_t ux = ((dx > 0) << 1) - 1;//x的增量方向，取1或-1
	int16_t uy = ((dy > 0) << 1) - 1;//y的增量方向，取1或-1
	int16_t x = x1, y = y1, eps;//eps为累加误差
	
	eps = 0;
	// dx = abs(dx);
	// dy = abs(dy);
	dx = (dx > 0) ? (dx) : (-dx);
	dy = (dy > 0) ? (dy) : (-dy);

	if (dx > dy)
	{
		for (x = x1; x != x2+ux; x += ux)
		{
			mgl_draw_point(x, y, c);
			eps += dy;
			if ((eps << 1) >= dx)
			{
				y += uy; eps -= dx;
			}
		}
	}
	else
	{
		for (y = y1; y != y2+uy; y += uy)
		{
			mgl_draw_point(x, y, c);
			eps += dx;
			if ((eps << 1) >= dy)
			{
				x += ux; eps -= dy;
			}
		}
	}
}

//模式0:边框
//模式1:填充
void mgl_draw_rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, mgl_color_t c, uint8_t mode)
{
	int16_t x, y;

	switch(mode)
	{
	case 0:
		mgl_draw_line(x1, y1, x2, y1, c);
		mgl_draw_line(x1, y2, x2, y2, c);
		mgl_draw_line(x1, y1, x1, y2, c);
		mgl_draw_line(x2, y1, x2, y2, c);
		break;
	case 1:
		for(y = y1; y<y2+1; y++)
		{
			for(x = x1; x<x2+1; x++)
			{
				mgl_draw_point(x, y, c);
			}
		}
		break;
	default: return;
	}
}

void mgl_disp_flush(void)
{
	// LED_screen_update_rgb_565(1);
}

// 对于黑白图像纵向的数值都必须是8的整数倍
// width ==> x
// hright ==> y
void mgl_fill_pic(int16_t x, int16_t y, int16_t width, int16_t height, mgl_color_t *pic)
{
	// unsigned char i,j;
	// unsigned char *p;
	int16_t dx, dy;
	
	#if MGL_COLOR_DEPTH == 1
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
						mgl_draw_point(dx, dy+i, 1);
					}
					else
					{
						// 不清除像素点，可以实现叠加效果，这个主要是为了方便显示文字
						// mgl_draw_point(dx, dy+i, 0);
					}
					tmp >>= 1;
				}
				pic++;
			}
		}
	#elif MGL_COLOR_DEPTH == 8
		for(dy = y; dy <= y + height; dy++) 
		{
			for(dx = x; dx <= x + width; dx++)
			{
				p_gram[dx + dy * MGL_DISP_HOR] = *pic;
				pic++;
			}
		}
	#elif MGL_COLOR_DEPTH == 16
		for(dy = y; dy <= y + height; dy++) 
		{
			for(dx = x; dx <= x + width; dx++)
			{
				p_gram[dx + dy * MGL_DISP_HOR] = *pic;
				pic++;
			}
		}
	#elif MGL_COLOR_DEPTH == 32
		for(dy = y; dy <= y + height; dy++) 
		{
			for(dx = x; dx <= x + width; dx++)
			{
				p_gram[dx + dy * MGL_DISP_HOR] = *pic;
				pic++;
			}
		}
	#endif
}

// 目前只对黑白像素平有用，以后改成所有颜色通用
void mgl_inverse_color(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
#if 0
	unsigned char a,b;

	for(a=x1; a<x2+1; a++)
	{
		for(b=y1; b<y2+1; b++)
		{
			//p_gram[a + GRAM_LENGTH * (b >> 3)] = p_gram[a + GRAM_LENGTH * (b >> 3)] ^ (0x80 >> (b & 0x07));//画点函数改成异或
			p_gram[a + MGL_DISP_HOR * (b >> 3)] = p_gram[a + MGL_DISP_HOR * (b >> 3)] ^ (0x01 << (b & 0x07));//画点函数改成异或
		}
	}
#endif
}
