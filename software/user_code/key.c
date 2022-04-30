#include <stdio.h>
#include "stm32f1xx_ll_gpio.h"

#include "key.h"

/*
基本原理就是4种按键状态的转移
*/

#define KEY_1_GPIO LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_13)
#define KEY_2_GPIO LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_14)
#define KEY_3_GPIO LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_15)
#define KEY_4_GPIO LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_15)

void key_init()
{
	// IO初始化
}

// 保存按键状态
static key_data key_ret     = 0;
static key_data key_state   = 0;
static uint8_t  key_is_read = 1;

// 硬件层接口，按下返回1，松开返回0，入参为按键编号，从0开始
key_data key_hw_read(key_data key_num)
{
	switch (key_num)
	{
	case 0:
		return !(KEY_1_GPIO);
		// break;
	case 1:
		return !(KEY_2_GPIO);
		// break;
	case 2:
		return !(KEY_3_GPIO);
		// break;
	case 3:
		return !(KEY_4_GPIO);
		// break;
	default:
		return 0;
		// break;
	}
}

// 循环调用，放在定时器中断内调用
void key_scan(void)
{
	key_data data = 0;
	uint8_t  i = 0;

	// 更新上一次按的数据
	key_state = key_state & KEY_STATE_MASK;
	key_state = key_state << 1;

	// 扫描按键并更新
	for (i = 0; i < KEY_TOTAL_NUM; i++)
	{
		data = 0;
		data = key_hw_read(i);
		data = data << i;
		data = data << i;
		key_state = key_state | data;
	}

	// 更新按键数据，这个标志位用来保证只有读取了上次按键数据，才会更新最新的按键数据供下次读取
	// 适用于按键读取频率高于按键扫描频率的场景
	if (key_is_read == 1)
	{
		key_is_read = 0;
		key_ret = key_state;
		return; // 这种情况直接返回，下面的代码没必要执行
	}

	// 如果上次按键数据没有被读取，但是有了新的按键，则进行更新
	// 适用于按键读取频率低于按键扫描频率的场景，可以保证不丢失最后一次的按键数据
	if (key_state && key_is_read == 0)
	{
		key_ret = key_state;
	}
}

key_data key_read(void)
{
	key_is_read = 1;
	return key_ret;
}
