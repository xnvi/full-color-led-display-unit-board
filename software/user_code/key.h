#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>

// ����������
#define KEY_TOTAL_NUM 4

#if KEY_TOTAL_NUM > 16
#error "too many key"
#elif KEY_TOTAL_NUM > 8
typedef uint32_t key_data;
#define KEY_STATE_MASK 0x55555555
#elif KEY_TOTAL_NUM > 4
typedef uint16_t key_data;
#define KEY_STATE_MASK 0x5555
#elif KEY_TOTAL_NUM > 0
typedef uint8_t key_data;
#define KEY_STATE_MASK 0x55
#else
#error "key num is 0"
#endif

// ״̬��ʶ
#define KEY_STATE_RELEASE 0x00
#define KEY_STATE_RISING  0x02
#define KEY_STATE_PUSH    0x03
#define KEY_STATE_FALLING 0x01


// �ⲿ��д�ò������ţ���ʱ�����Ż�
#define KEY_MASK (0x03)

// ��ȡĳ�����������ݣ�n��1����
#define KEY_READ_NUM(data, n) ((data >> (n - 1) * 2) & KEY_MASK)

// �ж�ĳ�������Ƿ�������״̬��n��1����
#define KEY_IS_RISING(data, n)   (((data >> (n - 1) * 2) & KEY_MASK) == KEY_STATE_RISING)
#define KEY_IS_PUSH(data, n)     (((data >> (n - 1) * 2) & KEY_MASK) == KEY_STATE_PUSH)
#define KEY_IS_FALLINGS(data, n) (((data >> (n - 1) * 2) & KEY_MASK) == KEY_STATE_FALLING)


key_data key_hw_read(key_data key_num);
void key_init(void);
void key_scan(void);
key_data key_read(void);

#endif
