#ifndef __MENU_H
#define __MENU_H

#include "stm32f1xx_hal.h"

// 屏幕内最多可显示的菜单条目数
#define MENU_MAX_SHOW_NUM 2

typedef struct _menu_node{
	uint8_t num; // _MenuItem 中菜单条目总数
	uint8_t cursor; // 菜单光标位置
	uint8_t head; // 菜单显示的第一条
	uint8_t fill[1]; // 填充，字节对齐
	struct _menu_node *parent; // 父菜单
	struct _menu_item *item; // 菜单项
} menu_node;

typedef struct _menu_item{
	char *item_name; // 本项菜单的名字
	void (*function)(void); // 功能函数，二选一
	struct _menu_node *child; // 子菜单节点，二选一
} menu_item;

extern menu_node menu_node_root;

void menu_show_select_item(menu_node *menu_p);

void set_time(void);
void show_temperature(void);
void show_humidity(void);
void audio_visual(void);
void set_luma(void);
void set_font_color(void);
void screen_self_test(void);
void play_nyan_cat(void);
void play_bad_apple(void);
void play_video(void);
void play_thumbs_up(void);
void show_icon(void);


#endif
