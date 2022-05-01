#ifndef __FONT_LITE_H
#define __FONT_LITE_H

#include <stdint.h>
#include "mgl.h"

#define ENABLE_ZH_CN (1)
#define ENABLE_FONT_SIZE_12 (1)
#define ENABLE_FONT_SIZE_16 (0)

#define LCD_WIDTH 32
#define LCD_LENGTH 64

#ifndef LCD_WIDTH
#error "LCD_WIDTH NOT DEFINED"
#endif

#ifndef LCD_LENGTH
#error "LCD_LENGTH NOT DEFINED"
#endif



#if ENABLE_ZH_CN
uint8_t encodingCheck(uint8_t *pchar);

#if ENABLE_FONT_SIZE_12
typedef struct _FONT12_STRUCT
{
	uint8_t character[3];
	uint8_t reserve;
	uint8_t fontdata[24];
}font12_struct;
#endif // ENABLE_FONT_SIZE_12

#if ENABLE_FONT_SIZE_16
typedef struct _FONT16_STRUCT
{
	uint8_t character[3];
	uint8_t reserve;
	uint8_t fontdata[32];
}font16_struct;
#endif // ENABLE_FONT_SIZE_16

#endif // ENABLE_ZH_CN



#if ENABLE_FONT_SIZE_12
void DrawFontInGram12(uint8_t type, uint8_t *font, int16_t x, int16_t y);
uint8_t* font12_search(uint8_t *pchar);
void PrintString12(int16_t x, int16_t y, char *string);
#endif // ENABLE_FONT_SIZE_12


#if ENABLE_FONT_SIZE_16
void DrawFontInGram16(uint8_t type, uint8_t *font, int16_t x, int16_t y);
uint8_t* font16_search(uint8_t *pchar);
void PrintString16(int16_t x, int16_t y, char *string);
#endif // ENABLE_FONT_SIZE_16

void set_fg_color(mgl_color_t color);
void set_bg_color(mgl_color_t color);
void DrawPoint(int16_t x, int16_t y);
void CleanPoint(int16_t x, int16_t y);

#endif

