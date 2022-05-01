#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "delay.h"
#include "menu.h"
#include "font_lite.h"
#include "key.h"
#include "mgl.h"
#include "utils.h"
#include "sfud.h"
#include "SHT30.h"

menu_item menu_items[] =
{
	// {"����", NULL, NULL},
	{"����ʱ��", set_time, NULL},
	{"����ģʽ", set_luma, NULL},
	{"������ɫ", set_font_color, NULL},
	{"����ͼ��", show_icon, NULL},
	{"��Ļ�Լ�", screen_self_test, NULL},
	{"BadApple", play_bad_apple, NULL},
	{"Nyan Cat", play_nyan_cat, NULL},
	{"ǰ������", play_video, NULL},
	{"һ������", play_thumbs_up, NULL},
	// ��ɫģʽ ���䣬��ɫ
	// ����ģʽ
};

menu_node menu_node_root = 
{
	.num = sizeof(menu_items) / sizeof(menu_item),
	.cursor = 0,
	.head = 0,
	.parent = NULL,
	.item = &menu_items[0],
};

char menu_line1[16] = {0};
char menu_line2[16] = {0};

// uint8_t img_read_buf[1024];
uint8_t img_read_buf[768];
extern uint16_t *gram_rgb_565;

void menu_show_select_item(menu_node *menu_p)
{
	if(menu_p->cursor == menu_p->head)
	{
		menu_line1[0] = '>';
		menu_line2[0] = ' ';
	}
	else
	{
		menu_line1[0] = ' ';
		menu_line2[0] = '>';
	}
	
	sprintf(&menu_line1[1], "%s\n", menu_p->item[menu_p->head].item_name);
	sprintf(&menu_line2[1], "%s\n", menu_p->item[menu_p->head + 1].item_name);

	mgl_clean();

	PrintString12(0, 0, menu_line1);
	PrintString12(0, 14, menu_line2);
}


// void set_time(void);

static const uint8_t icon_temp[] = {
    0xFF, 0xFF, 0xFF, 0x07, 0xFB, 0xFD, 0xFD, 0xFD, 0xFB, 0x07, 0xFF, 0xDB, 0xDB, 0xDF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x03, 0x03, 0x03,
    0xFF, 0x00, 0xFF, 0xB6, 0xB6, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x7F, 0xBF, 0xC0, 0x7F, 0x00, 0x00, 0x00, 0x7F, 0xC0, 0xBF, 0x6D, 0xED, 0xFD, 0x7F, 0xFF,
    0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xF0, 0xEF, 0xD8, 0xB0, 0xA0, 0xA0, 0xA0,
    0xB0, 0xD8, 0xEF, 0xF0, 0xFF, 0xFE, 0xFD, 0xFE, 0xFF, 0xE1, 0xDE, 0xBF, 0xBF, 0xBF, 0xDE, 0xFF,
};

static const uint8_t icon_humi[] = {
    0xFF, 0xFF, 0x3F, 0xCF, 0xF3, 0xFD, 0xF3, 0xCF, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x9F,
    0xEF, 0x9F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xF7, 0xEF, 0xDF, 0xDF, 0xD7, 0xE8,
    0xF7, 0xF8, 0xFF, 0x3F, 0xCF, 0xF1, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0x71, 0xCF, 0x3F, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xDF, 0x3F, 0xFF, 0xE0, 0xDF, 0xBF, 0x7F, 0xFF, 0xFF,
    0xFF, 0xFF, 0xDF, 0x6E, 0xB1, 0xDF, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0xDC, 0xBF, 0xBF,
    0xA7, 0xDC, 0xE3, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static const uint8_t icon_funny[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0xCE, 0xD2, 0xD5, 0xB0, 0xD5, 0xB0, 0xD5,
    0xB0, 0xD5, 0xD2, 0xD5, 0x15, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xD1, 0xD5, 0x0C, 0xC5, 0x61, 0xBC, 0x20, 0xBC, 0x20, 0xC4, 0x20, 0xC4,
    0x20, 0xC4, 0x40, 0xBC, 0x61, 0xBC, 0x0C, 0xC5, 0xD1, 0xD5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x72, 0xBD, 0x20, 0xC4, 0x40, 0xC3, 0x40, 0xCB, 0x00, 0xD4, 0x80, 0xD4, 0xE0, 0xDC, 0x00, 0xDD,
    0xE0, 0xDC, 0x80, 0xD4, 0x00, 0xD4, 0x40, 0xCB, 0x40, 0xC3, 0x20, 0xC4, 0x72, 0xC5, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0xD6, 0x00, 0xC4,
    0x20, 0xC3, 0x40, 0xD4, 0x60, 0xDD, 0xE7, 0xE5, 0x09, 0xE6, 0x29, 0xE6, 0x2A, 0xE6, 0x2A, 0xE6,
    0x2A, 0xE6, 0x29, 0xE6, 0x09, 0xE6, 0xE7, 0xE5, 0x60, 0xDD, 0x40, 0xD4, 0x20, 0xC3, 0x00, 0xC4,
    0x14, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0xCD, 0x20, 0xCC, 0x00, 0xCC,
    0x20, 0xDD, 0xA0, 0xE5, 0x09, 0xE6, 0x4B, 0xE6, 0x4B, 0xE6, 0x4B, 0xE6, 0x4B, 0xE6, 0x4B, 0xE6,
    0x4B, 0xE6, 0x4B, 0xE6, 0x4B, 0xE6, 0x4B, 0xE6, 0x09, 0xE6, 0xA0, 0xE5, 0x20, 0xDD, 0x00, 0xCC,
    0x20, 0xCC, 0x6F, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0xD5, 0x40, 0xCC, 0x20, 0xD4, 0x40, 0xDD,
    0x28, 0xE6, 0x4A, 0xE6, 0x4A, 0xE6, 0x4A, 0xE6, 0x4A, 0xE6, 0x4A, 0xE6, 0x4A, 0xE6, 0x4A, 0xE6,
    0x4A, 0xE6, 0x4A, 0xE6, 0x4A, 0xE6, 0x4A, 0xE6, 0x4A, 0xE6, 0x4A, 0xE6, 0x28, 0xE6, 0x40, 0xDD,
    0x20, 0xD4, 0x40, 0xCC, 0x6F, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0xD6, 0x20, 0xCC, 0x20, 0xD4, 0x60, 0xDD, 0x49, 0xE6,
    0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6,
    0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6, 0x49, 0xE6,
    0x60, 0xDD, 0x20, 0xD4, 0x20, 0xCC, 0xF4, 0xD5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xD9, 0xDE, 0x00, 0xC4, 0xE0, 0xCB, 0x00, 0xDD, 0xE6, 0xE5, 0xE7, 0xE5,
    0xE7, 0xE5, 0xE7, 0xE5, 0xE7, 0xE5, 0x07, 0xE6, 0x28, 0xE6, 0x28, 0xE6, 0x28, 0xE6, 0x28, 0xE6,
    0x28, 0xE6, 0x28, 0xE6, 0x28, 0xE6, 0x07, 0xE6, 0xE7, 0xE5, 0xE7, 0xE5, 0xE7, 0xE5, 0xE7, 0xE5,
    0xE6, 0xE5, 0x00, 0xDD, 0xE0, 0xCB, 0x00, 0xC4, 0xD9, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x50, 0xBD, 0x20, 0xC3, 0xE0, 0xCB, 0xC0, 0xC3, 0x00, 0xB3, 0x00, 0xB3,
    0x20, 0xB3, 0x00, 0xB3, 0x60, 0xBB, 0x40, 0xC4, 0x80, 0xDD, 0x26, 0xE6, 0x27, 0xE6, 0x27, 0xE6,
    0x27, 0xE6, 0x26, 0xE6, 0x80, 0xDD, 0x40, 0xC4, 0x60, 0xBB, 0x00, 0xB3, 0x20, 0xB3, 0x00, 0xB3,
    0x00, 0xB3, 0xC0, 0xC3, 0xE0, 0xCB, 0x20, 0xC3, 0x50, 0xBD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x56, 0xDE, 0xC0, 0xBB, 0x20, 0xB2, 0x00, 0xC4, 0x4F, 0xCD, 0xD3, 0xD5, 0xF5, 0xD5,
    0x15, 0xD6, 0xF4, 0xD5, 0xD2, 0xD5, 0x2E, 0xCD, 0x40, 0xC4, 0xA0, 0xCC, 0xC0, 0xDD, 0x27, 0xE6,
    0xC0, 0xDD, 0xA0, 0xCC, 0x40, 0xC4, 0x2E, 0xCD, 0xD2, 0xD5, 0xF4, 0xD5, 0x15, 0xD6, 0xF5, 0xD5,
    0xD3, 0xD5, 0x4F, 0xCD, 0x00, 0xC4, 0x20, 0xB2, 0xC0, 0xBB, 0x56, 0xDE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xB2, 0xD5, 0x20, 0xB2, 0x40, 0xB3, 0x20, 0x8A, 0x89, 0x93, 0x94, 0xC5, 0x3C, 0xE7,
    0x3C, 0xE7, 0x3C, 0xE7, 0x1B, 0xE7, 0x78, 0xDE, 0x71, 0xCD, 0xE0, 0xBB, 0x40, 0xD5, 0xC0, 0xE5,
    0x40, 0xD5, 0xA0, 0xBB, 0xA0, 0x92, 0xA0, 0x8A, 0x11, 0xB5, 0xFB, 0xE6, 0x3C, 0xE7, 0x3C, 0xE7,
    0x1C, 0xE7, 0x99, 0xDE, 0x92, 0xCD, 0xC2, 0xBB, 0x40, 0xBA, 0xB2, 0xD5, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xAD, 0xC4, 0x00, 0xA8, 0xCF, 0xBC, 0x00, 0x48, 0x00, 0x38, 0xEC, 0x9B, 0x3C, 0xE7,
    0x3C, 0xE7, 0x3C, 0xE7, 0x3C, 0xE7, 0x3C, 0xE7, 0xFB, 0xE6, 0x6C, 0xBC, 0xC0, 0xCC, 0x20, 0xD5,
    0xC0, 0xCC, 0xC7, 0xB3, 0x60, 0x68, 0x00, 0x00, 0xC4, 0x82, 0xBA, 0xDE, 0x3C, 0xE7, 0x3C, 0xE7,
    0x3C, 0xE7, 0x3C, 0xE7, 0xFB, 0xE6, 0xD4, 0xD5, 0x00, 0xB1, 0xAD, 0xC4, 0x00, 0x00, 0x00, 0x00,
    0xF5, 0xCD, 0xA0, 0xB3, 0xE0, 0xA8, 0x32, 0xBD, 0x00, 0x48, 0x00, 0x38, 0xAA, 0x93, 0xDA, 0xE6,
    0xDA, 0xE6, 0xDA, 0xE6, 0xFB, 0xE6, 0x1B, 0xE7, 0xDA, 0xE6, 0x51, 0xCD, 0xA0, 0xCC, 0x80, 0xCC,
    0xA0, 0xCC, 0xAE, 0xBC, 0x00, 0x60, 0x00, 0x00, 0xA2, 0x7A, 0x58, 0xD6, 0xDA, 0xE6, 0xDA, 0xE6,
    0xFB, 0xE6, 0xFB, 0xE6, 0xFB, 0xE6, 0x78, 0xDE, 0x80, 0xB1, 0xA0, 0xB3, 0xF5, 0xCD, 0x00, 0x00,
    0xB2, 0xD5, 0x60, 0xBB, 0xC0, 0xB9, 0x80, 0xB3, 0x0B, 0xB4, 0x4C, 0xB4, 0x09, 0xB4, 0x40, 0xB3,
    0xE0, 0xB2, 0xC5, 0xBB, 0xCE, 0xC4, 0x93, 0xCD, 0x51, 0xCD, 0x00, 0xB3, 0xA0, 0xCC, 0x80, 0xDD,
    0xC0, 0xCC, 0xE0, 0xAA, 0xAE, 0xBC, 0xAE, 0xBC, 0x2B, 0xBC, 0xA4, 0xB3, 0xE0, 0xB2, 0x40, 0xB3,
    0x6C, 0xBC, 0x30, 0xC5, 0xEF, 0xC4, 0xC4, 0xBB, 0xC0, 0xB9, 0x60, 0xBB, 0xB2, 0xD5, 0x00, 0x00,
    0xB0, 0xD5, 0xA0, 0xC3, 0x00, 0xD3, 0x60, 0xCB, 0x40, 0xCB, 0x40, 0xCB, 0x40, 0xCB, 0x40, 0xD3,
    0xC0, 0xD3, 0x80, 0xD4, 0xC0, 0xCC, 0xC0, 0xCC, 0xA0, 0xCC, 0xC0, 0xCC, 0x80, 0xDD, 0xE0, 0xE5,
    0x80, 0xDD, 0xC0, 0xCC, 0xA0, 0xCC, 0xA0, 0xCC, 0xA0, 0xCC, 0x80, 0xD4, 0xE0, 0xD3, 0x60, 0xD3,
    0x40, 0xCB, 0x40, 0xCB, 0x40, 0xCB, 0x60, 0xCB, 0x00, 0xD3, 0x80, 0xC3, 0xB0, 0xD5, 0x00, 0x00,
    0xB0, 0xD5, 0xC0, 0xC3, 0xE0, 0xDB, 0x00, 0xDC, 0x80, 0xDB, 0x40, 0xDB, 0x80, 0xDB, 0x20, 0xE4,
    0xC0, 0xE4, 0x60, 0xE5, 0x60, 0xDD, 0x40, 0xDD, 0x40, 0xDD, 0xC0, 0xE5, 0xE0, 0xE5, 0x00, 0xE6,
    0xE0, 0xE5, 0xC0, 0xE5, 0x40, 0xDD, 0x40, 0xDD, 0x60, 0xDD, 0x60, 0xE5, 0xC0, 0xE4, 0x20, 0xE4,
    0x80, 0xDB, 0x40, 0xDB, 0x80, 0xDB, 0x00, 0xDC, 0xE0, 0xDB, 0xC0, 0xC3, 0xB0, 0xD5, 0x00, 0x00,
    0xB0, 0xD5, 0x00, 0xC4, 0x20, 0xDC, 0xC0, 0xE4, 0x80, 0xE4, 0x60, 0xE4, 0x80, 0xE4, 0xE0, 0xE4,
    0x60, 0xE5, 0xC0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5,
    0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xC0, 0xE5, 0x60, 0xE5, 0xE0, 0xE4,
    0x80, 0xE4, 0x60, 0xE4, 0x80, 0xE4, 0xC0, 0xE4, 0x20, 0xDC, 0x00, 0xC4, 0xB0, 0xD5, 0x00, 0x00,
    0xD2, 0xCD, 0x20, 0xC4, 0x40, 0xD4, 0x80, 0xE5, 0x20, 0xE5, 0x00, 0xE5, 0x20, 0xE5, 0x80, 0xE5,
    0xC0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5,
    0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xC0, 0xE5, 0x80, 0xE5,
    0x20, 0xE5, 0x00, 0xE5, 0x20, 0xE5, 0x80, 0xE5, 0x40, 0xD4, 0x20, 0xC4, 0xD2, 0xCD, 0x00, 0x00,
    0x15, 0xCE, 0x60, 0xBC, 0xC0, 0xD3, 0xA0, 0xE5, 0xE0, 0xD3, 0xA0, 0xD3, 0xC0, 0xDC, 0xC0, 0xE5,
    0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5,
    0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xE0, 0xE5, 0xC0, 0xE5,
    0xE0, 0xDC, 0xA0, 0xD3, 0xE0, 0xD3, 0xA0, 0xE5, 0xC0, 0xD3, 0x60, 0xBC, 0x15, 0xCE, 0x00, 0x00,
    0x99, 0xDE, 0x2C, 0xCD, 0x20, 0xCB, 0x80, 0xE5, 0x40, 0xD4, 0xC0, 0xCA, 0x20, 0xCB, 0xC0, 0xE5,
    0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5,
    0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5,
    0x40, 0xCB, 0xC0, 0xCA, 0x40, 0xD4, 0x80, 0xE5, 0x20, 0xCB, 0x2C, 0xCD, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xD1, 0xD5, 0x40, 0xC3, 0xE0, 0xDC, 0x00, 0xDD, 0xA0, 0xCA, 0x20, 0xC1, 0x40, 0xE5,
    0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5,
    0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0x40, 0xE5,
    0x40, 0xC1, 0xA0, 0xCA, 0xE0, 0xDC, 0xE0, 0xDC, 0x40, 0xC3, 0xD1, 0xD5, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x56, 0xDE, 0x00, 0xC4, 0x00, 0xD4, 0x20, 0xE5, 0xA0, 0xD3, 0x00, 0xC1, 0x60, 0xCB,
    0x60, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5,
    0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0x60, 0xE5, 0x60, 0xCB,
    0x00, 0xC1, 0xA0, 0xD3, 0x20, 0xE5, 0x00, 0xD4, 0x00, 0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x50, 0xBD, 0x20, 0xC3, 0xA0, 0xDC, 0x00, 0xDD, 0x40, 0xCB, 0x00, 0xB0,
    0x60, 0xDC, 0x60, 0xE5, 0xA0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5,
    0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xA0, 0xE5, 0x60, 0xE5, 0x60, 0xDC, 0x00, 0xB0,
    0x40, 0xCB, 0x00, 0xDD, 0xA0, 0xDC, 0x20, 0xC3, 0x30, 0xBD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xD9, 0xDE, 0x00, 0xC4, 0xE0, 0xCB, 0xC0, 0xDC, 0xE0, 0xDC, 0xC0, 0xCA,
    0x00, 0xB0, 0xA0, 0xD3, 0x60, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5,
    0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5, 0x60, 0xE5, 0xA0, 0xD3, 0x00, 0xB0, 0xC0, 0xCA,
    0xE0, 0xDC, 0xC0, 0xDC, 0xE0, 0xCB, 0x00, 0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4, 0xD5, 0x20, 0xCC, 0xE0, 0xD3, 0xC0, 0xDC, 0xE0, 0xDC,
    0x00, 0xCB, 0x00, 0xB8, 0x80, 0xC1, 0xA0, 0xD3, 0x20, 0xE5, 0xA0, 0xE5, 0xC0, 0xE5, 0xC0, 0xE5,
    0xC0, 0xE5, 0xA0, 0xE5, 0x20, 0xE5, 0xA0, 0xD3, 0x80, 0xC1, 0x00, 0xB8, 0x00, 0xCB, 0xE0, 0xDC,
    0xC0, 0xDC, 0xE0, 0xD3, 0x20, 0xCC, 0xF4, 0xD5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0xD5, 0x20, 0xCC, 0x00, 0xD4, 0xC0, 0xDC,
    0xC0, 0xDC, 0x40, 0xCB, 0x40, 0xC2, 0x20, 0xC2, 0x60, 0xC2, 0x80, 0xCA, 0xE0, 0xCA, 0x00, 0xCB,
    0xE0, 0xCA, 0x80, 0xCA, 0x60, 0xC2, 0x20, 0xC2, 0x40, 0xC2, 0x40, 0xCB, 0xC0, 0xDC, 0xC0, 0xDC,
    0x00, 0xD4, 0x20, 0xCC, 0x6F, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0xCD, 0x20, 0xCC, 0xE0, 0xCB,
    0xA0, 0xDC, 0x00, 0xE5, 0x80, 0xDC, 0x20, 0xCB, 0xE0, 0xC0, 0x00, 0xB8, 0x00, 0xB8, 0x00, 0xB8,
    0x00, 0xB8, 0x00, 0xB8, 0xE0, 0xC0, 0x20, 0xCB, 0x80, 0xDC, 0x00, 0xE5, 0xA0, 0xDC, 0xE0, 0xCB,
    0x20, 0xCC, 0x6F, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0xD6, 0x00, 0xC4,
    0x00, 0xC3, 0xE0, 0xD3, 0xC0, 0xDC, 0x60, 0xE5, 0x60, 0xE5, 0x60, 0xE5, 0x00, 0xE5, 0xE0, 0xDC,
    0x00, 0xE5, 0x60, 0xE5, 0x60, 0xE5, 0x60, 0xE5, 0xC0, 0xDC, 0x00, 0xD4, 0x00, 0xC3, 0x00, 0xC4,
    0x14, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x72, 0xBD, 0x20, 0xC4, 0x40, 0xC3, 0x00, 0xCB, 0xA0, 0xD3, 0x20, 0xD4, 0x60, 0xDC, 0x60, 0xDC,
    0x60, 0xDC, 0x20, 0xD4, 0xA0, 0xD3, 0x00, 0xCB, 0x40, 0xC3, 0x20, 0xC4, 0x72, 0xC5, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xB1, 0xD5, 0x0A, 0xC5, 0x40, 0xBC, 0x20, 0xC4, 0x00, 0xC4, 0x00, 0xC4,
    0x00, 0xC4, 0x20, 0xC4, 0x40, 0xBC, 0x0A, 0xC5, 0xB1, 0xD5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0xCE, 0xD2, 0xD5, 0xB0, 0xD5, 0xB0, 0xD5,
    0xB0, 0xD5, 0xD2, 0xD5, 0x15, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const uint8_t icon_stop[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8,
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void show_temperature(void)
{
	float val;
	char val_str[8];

	SHT30ReadAll();
	val = SHT30GetCTemp();

	mgl_clean();

	fill_pic(0, 0, 24, 32, (uint8_t *)icon_temp);
	sprintf(val_str, "%.1f", val);
	PrintString12(28, 10, val_str);
	HAL_Delay(3000);

	mgl_clean();
	
	return;
}

void show_humidity(void)
{
	float val;
	char val_str[8];

	SHT30ReadAll();
	val = SHT30GetHumi();

	mgl_clean();

	fill_pic(0, 0, 24, 32, (uint8_t *)icon_humi);
	sprintf(val_str, "%.1f%%", val);
	PrintString12(28, 10, val_str);
	HAL_Delay(3000);

	mgl_clean();
	
	return;
}

// void audio_visual(void);

void set_luma(void)
{
	int mode = 0;
	uint8_t key = 0;
	char strbuf[16] = {0};

	mgl_clean();
	if(g_screen_auto_luma)
	{
		mode = 0;
		sprintf(strbuf, "�Զ�   ");
		PrintString12(11, 10, strbuf);
	}
	else
	{
		mode = g_screen_luma;
		sprintf(strbuf, "�ֶ� %d  ", mode);
		PrintString12(11, 10, strbuf);
	}

	while (1)
	{
		HAL_Delay(50);
		
		key = key_read();

		if (KEY_IS_RISING(key, 3))
		{
			if (mode > 0)
			{
				mode -= 1;	
			}
		}
		if (KEY_IS_RISING(key, 4))
		{
			if (mode < 10)
			{
				mode += 1;	
			}
		}

		if (KEY_IS_RISING(key, 2))
		{
			break;
		}

		if (key)
		{
			if(mode == 0)
			{
				g_screen_auto_luma = 1;
				sprintf(strbuf, "�Զ�   ");
				PrintString12(11, 10, strbuf);
			}
			else
			{
				g_screen_luma = mode;
				g_screen_auto_luma = 0;
				sprintf(strbuf, "�ֶ� %d  ", mode);
				PrintString12(11, 10, strbuf);
			}
		}

		key = 0;
	}

	return;
}

void set_font_color(void)
{
	uint8_t key = 0;
	char strbuf[16] = {0};

	mgl_clean();
	sprintf(strbuf, "��ɫ %d  ", g_screen_color_index + 1);
	PrintString12(11, 10, strbuf);

	while (1)
	{
		HAL_Delay(50);
		
		key = key_read();

		if (KEY_IS_RISING(key, 3))
		{
			if (g_screen_color_index > 0)
			{
				g_screen_color_index -= 1;	
			}
		}
		if (KEY_IS_RISING(key, 4))
		{
			if (g_screen_color_index < sizeof(color_map) / sizeof(uint16_t) - 1)
			{
				g_screen_color_index += 1;	
			}
		}

		if (KEY_IS_RISING(key, 2))
		{
			break;
		}

		if (key)
		{
			set_fg_color(color_map[g_screen_color_index]);
			set_bg_color(0x0000);
			sprintf(strbuf, "��ɫ %d  ", g_screen_color_index + 1);
			PrintString12(11, 10, strbuf);
		}

		key = 0;
	}

	return;
}

 int32_t const self_test_color[7] = {
	0xFFFFFF,
	0xFF0000, 0x00FF00, 0x0000FF,
	0XFFFF00, 0x00FFFF, 0xFF00FF
};

void screen_self_test(void)
{
	uint8_t g_screen_luma_old = g_screen_luma;
	uint8_t g_screen_auto_luma_old = g_screen_auto_luma;
	int16_t i, j;
	mgl_color_t color = 0;
	mgl_color_t *gram = NULL;

	gram = mgl_get_gram_addr();
	g_screen_auto_luma = 0;
	g_screen_luma = 10;

	for (i = 0; i < 7; i++)
	{
		color = mgl_rgb32_to_rgb16(self_test_color[i]);
		for (j = 0; j < MGL_DISP_HOR * MGL_DISP_VER; j++)
		{
			gram[j] = color;
		}
		while (1)
		{
			HAL_Delay(100);
			if (KEY_IS_RISING(key_read(), 1))
			{
				break;
			}
		}
	}
	g_screen_luma = g_screen_luma_old;
	g_screen_auto_luma = g_screen_auto_luma_old;
	return;
}

void play_nyan_cat(void)
{
	uint32_t frame_count = 0;
	uint32_t spi_addr = 1382400 + 430080 + 204800;
	uint32_t i = 0;
	extern uint16_t *gram_rgb_565;
	extern sfud_flash *flash;

	mgl_clean();
	while (1)
	{
		for (i = 0; i < 4; i++)
		{
			sfud_read(flash, spi_addr, 1024, &((uint8_t *)gram_rgb_565)[i * 1024]);
			spi_addr += 1024;
		}

		frame_count += 1;
		if (frame_count >= 12)
		{
			frame_count = 0;
			spi_addr = 1382400 + 430080 + 204800;
		}

		HAL_Delay(120);

		if (KEY_IS_RISING(key_read(), 2))
		{
			break;
		}
	}

	return;
}

void play_bad_apple(void)
{
	uint32_t frame_count = 0;
	uint32_t spi_addr = 1382400 + 430080;
	uint32_t timer = 0;
	uint32_t base_time = 0;
	extern uint16_t *gram_rgb_565;
	extern sfud_flash *flash;

	mgl_clean();
	base_time = HAL_GetTick();
	while (1)
	{
		while (1)
		{
			timer = frame_count * 1000 / 30;
			if (HAL_GetTick() > base_time + timer)
			{
				break;
			}
		}
	
		sfud_read(flash, spi_addr, 160, img_read_buf);
		spi_addr += 160;
		fill_pic_rainbow(0, 0, 40, 32, img_read_buf);

		frame_count += 1;
		if (frame_count >= 1280)
		{
			break;
		}

		if (KEY_IS_RISING(key_read(), 2))
		{
			break;
		}
	}

	return;
}

void play_video(void)
{
	uint32_t frame_count = 0;
	uint32_t spi_addr = 0;
	uint32_t i, j = 0;
	uint32_t timer = 0;
	uint32_t base_time = 0;
	extern uint16_t *gram_rgb_565;
	extern sfud_flash *flash;

	mgl_clean();
	base_time = HAL_GetTick();
	while (frame_count < 450)
	{
		while (1)
		{
			timer = frame_count * 1000 / 30;
			if (HAL_GetTick() > base_time + timer)
			{
				break;
			}
		}

		for (i = 0; i < 4; i++)
		{
			sfud_read(flash, spi_addr, 48 * 8 * 2, img_read_buf);
			spi_addr += 48 * 8 * 2;

			for (j = 0; j < 8; j++)
			{
				memcpy(&((uint8_t *)gram_rgb_565)[i * 1024 + j * 64 * 2], &img_read_buf[j * 48 * 2], 48 * 2);
			}
		}

		frame_count += 1;
	}
	return;
}

void play_thumbs_up(void)
{
	uint32_t frame_count = 0;
	uint32_t spi_addr = 1382400;
	uint32_t i = 0;
	uint32_t timer = 0;
	uint32_t base_time = 0;
	extern uint16_t *gram_rgb_565;
	extern sfud_flash *flash;

	mgl_clean();
	base_time = HAL_GetTick();
	while (frame_count < 105)
	{
		while (1)
		{
			timer = frame_count * 1000 / 30;
			if (HAL_GetTick() > base_time + timer)
			{
				break;
			}
		}

		for (i = 0; i < 4; i++)
		{
			sfud_read(flash, spi_addr, 1024, &((uint8_t *)gram_rgb_565)[i * 1024]);
			spi_addr += 1024;
		}
		frame_count += 1;
	}
	HAL_Delay(2000);

	return;
}

void show_icon_funny(void)
{
	int i;
	for (i = 0; i < 32; i++)
	{
		memcpy(&((uint8_t *)gram_rgb_565)[i * 64 * 2], &icon_funny[i * 32 * 2], 32 * 2);
	}
	return;
}
void show_icon_stop(void)
{
	int i;
	char strbuf[8];
	for (i = 0; i < 32; i++)
	{
		memcpy(&((uint8_t *)gram_rgb_565)[i * 64 * 2], &icon_stop[i * 32 * 2], 32 * 2);
	}
	sprintf(strbuf, "��ֹ");
	PrintString12(34, 3, strbuf);
	sprintf(strbuf, "ͨ��");
	PrintString12(34, 18, strbuf);
	return;
}
void show_icon_left(void)
{
	mgl_draw_line(1, 15, 15, 1, 0xFFFF);
	mgl_draw_line(2, 15, 16, 1, 0xFFFF);
	mgl_draw_line(1, 16, 15, 30, 0xFFFF);
	mgl_draw_line(2, 16, 16, 30, 0xFFFF);
	mgl_draw_line(1, 15, 62, 15, 0xFFFF);
	mgl_draw_line(1, 16, 62, 16, 0xFFFF);
	return;
}
void show_icon_right(void)
{
	mgl_draw_line(61, 15, 47, 1, 0xFFFF);
	mgl_draw_line(62, 15, 48, 1, 0xFFFF);
	mgl_draw_line(61, 16, 47, 30, 0xFFFF);
	mgl_draw_line(62, 16, 48, 30, 0xFFFF);
	mgl_draw_line(1, 15, 62, 15, 0xFFFF);
	mgl_draw_line(1, 16, 62, 16, 0xFFFF);
	return;
}
void show_icon_girlfriend(void)
{
	char strbuf[16];
	sprintf(strbuf, "���˵���");
	PrintString12(7, 3, strbuf);
	sprintf(strbuf, "����Ů��");
	PrintString12(7, 18, strbuf);
	return;
}

void (*show_icon_func[5])(void) = {
	show_icon_funny,
	show_icon_stop,
	show_icon_left,
	show_icon_right,
	show_icon_girlfriend,
};

void show_icon(void)
{
	uint8_t key = 0;
	uint8_t index = 0;

	mgl_clean();
	show_icon_func[index]();

	while (1)
	{
		HAL_Delay(50);
		
		key = key_read();

		if (KEY_IS_RISING(key, 3))
		{
			if (index > 0)
			{
				index -= 1;	
			}
		}
		if (KEY_IS_RISING(key, 4))
		{
			if (index < 4)
			{
				index += 1;	
			}
		}

		if (KEY_IS_RISING(key, 2))
		{
			break;
		}

		if (key)
		{
			mgl_clean();
			show_icon_func[index]();
		}

		key = 0;
	}

	return;
}