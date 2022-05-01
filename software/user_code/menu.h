#ifndef __MENU_H
#define __MENU_H

#include "stm32f1xx_hal.h"

// ��Ļ��������ʾ�Ĳ˵���Ŀ��
#define MENU_MAX_SHOW_NUM 2

typedef struct _menu_node{
	uint8_t num; // _MenuItem �в˵���Ŀ����
	uint8_t cursor; // �˵����λ��
	uint8_t head; // �˵���ʾ�ĵ�һ��
	uint8_t fill[1]; // ��䣬�ֽڶ���
	struct _menu_node *parent; // ���˵�
	struct _menu_item *item; // �˵���
} menu_node;

typedef struct _menu_item{
	char *item_name; // ����˵�������
	void (*function)(void); // ���ܺ�������ѡһ
	struct _menu_node *child; // �Ӳ˵��ڵ㣬��ѡһ
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
