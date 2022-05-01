#ifndef __UTILS_H
#define __UTILS_H

#include <stdint.h>

extern uint8_t g_screen_color_index;
extern uint8_t g_screen_luma;
extern uint8_t g_screen_auto_luma;

extern const uint16_t color_map[13];

int test(int input);
void erase_spi(void);
void write_spi(void);

void fill_pic_rainbow(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t *pic);
void fill_pic(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t *pic);
void adc_audio_in(void);
void auto_luma(void);

#endif
