#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>

// 按键数设置
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

// 状态标识
#define KEY_STATE_RELEASE 0x00
#define KEY_STATE_RISING  0x02
#define KEY_STATE_PUSH    0x03
#define KEY_STATE_FALLING 0x01


// 这部分写得不够优雅，有时间再优化
#define KEY_MASK (0x03)

// 读取某个按键的数据，n从1计数
#define KEY_READ_NUM(data, n) ((data >> (n - 1) * 2) & KEY_MASK)

// 判断某个按键是否处于这种状态，n从1计数
#define KEY_IS_RISING(data, n)   (((data >> (n - 1) * 2) & KEY_MASK) == KEY_STATE_RISING)
#define KEY_IS_PUSH(data, n)     (((data >> (n - 1) * 2) & KEY_MASK) == KEY_STATE_PUSH)
#define KEY_IS_FALLINGS(data, n) (((data >> (n - 1) * 2) & KEY_MASK) == KEY_STATE_FALLING)


key_data key_hw_read(key_data key_num);
void key_init(void);
void key_scan(void);
key_data key_read(void);

#endif
