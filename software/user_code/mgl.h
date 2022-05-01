// 实现最基本功能的 mini graphics library

#ifndef __MGL_H
#define __MGL_H

#include <stdint.h>

//屏幕的长宽参数
// #define MGL_DISP_HOR  (128)
// #define MGL_DISP_VER  (64)
#define MGL_DISP_HOR  (64)
#define MGL_DISP_VER  (32)

// 颜色格式
// 1   黑白像素屏
// 8   RGB332
// 16  RGB565
// 32  ARGB8888
#define MGL_COLOR_DEPTH     16

// RGB565 和 BGR565 互换
#define MGL_COLOR_16_SWAP   0

// mgl 内静态分配显存
#define MGL_STATIC_GRAM     1


typedef uint8_t mgl_color1_t;
typedef uint8_t mgl_color8_t;
typedef uint16_t mgl_color16_t;
typedef uint32_t mgl_color32_t;


#if MGL_COLOR_DEPTH == 1
	typedef mgl_color1_t mgl_color_t;
#elif MGL_COLOR_DEPTH == 8
	typedef mgl_color8_t mgl_color_t;
#elif MGL_COLOR_DEPTH == 16
	typedef mgl_color16_t mgl_color_t;
#elif MGL_COLOR_DEPTH == 32
	typedef mgl_color32_t mgl_color_t;
#endif


#define MGL_RGB565_MASK_R 0xf800
#define MGL_RGB565_MASK_G 0x07e0
#define MGL_RGB565_MASK_B 0x001f



void mgl_init(mgl_color_t *buf, int32_t size);
mgl_color_t *mgl_get_gram_addr(void);
void mgl_clean(void);
void mgl_draw_point(int16_t x, int16_t y, mgl_color_t c);
mgl_color_t mgl_read_point(int16_t x, int16_t y);
void mgl_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, mgl_color_t c);
void mgl_draw_rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, mgl_color_t c, uint8_t mode);
void mgl_disp_flush(void);
void mgl_fill_pic(int16_t x, int16_t y, int16_t width, int16_t height, mgl_color_t *pic);
void mgl_inverse_color(int16_t x1, int16_t y1, int16_t x2, int16_t y2);


uint16_t mgl_rgb_to_rgb16(uint8_t r, uint8_t g, uint8_t b);
uint16_t mgl_rgb32_to_rgb16(uint32_t argb);
uint32_t mgl_rgb16_to_rgb32(uint16_t rgb);
uint32_t mgl_color_ring(int32_t index);

#endif
