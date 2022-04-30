#include <stdio.h>
#include "stm32f1xx_ll_gpio.h"

#include "key.h"

/*
����ԭ�����4�ְ���״̬��ת��
*/

#define KEY_1_GPIO LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_13)
#define KEY_2_GPIO LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_14)
#define KEY_3_GPIO LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_15)
#define KEY_4_GPIO LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_15)

void key_init()
{
	// IO��ʼ��
}

// ���水��״̬
static key_data key_ret     = 0;
static key_data key_state   = 0;
static uint8_t  key_is_read = 1;

// Ӳ����ӿڣ����·���1���ɿ�����0�����Ϊ������ţ���0��ʼ
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

// ѭ�����ã����ڶ�ʱ���ж��ڵ���
void key_scan(void)
{
	key_data data = 0;
	uint8_t  i = 0;

	// ������һ�ΰ�������
	key_state = key_state & KEY_STATE_MASK;
	key_state = key_state << 1;

	// ɨ�谴��������
	for (i = 0; i < KEY_TOTAL_NUM; i++)
	{
		data = 0;
		data = key_hw_read(i);
		data = data << i;
		data = data << i;
		key_state = key_state | data;
	}

	// ���°������ݣ������־λ������ֻ֤�ж�ȡ���ϴΰ������ݣ��Ż�������µİ������ݹ��´ζ�ȡ
	// �����ڰ�����ȡƵ�ʸ��ڰ���ɨ��Ƶ�ʵĳ���
	if (key_is_read == 1)
	{
		key_is_read = 0;
		key_ret = key_state;
		return; // �������ֱ�ӷ��أ�����Ĵ���û��Ҫִ��
	}

	// ����ϴΰ�������û�б���ȡ�����������µİ���������и���
	// �����ڰ�����ȡƵ�ʵ��ڰ���ɨ��Ƶ�ʵĳ��������Ա�֤����ʧ���һ�εİ�������
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
