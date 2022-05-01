#ifndef __LCD_SCREEN_C
#define __LCD_SCREEN_C

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_gpio.h"

// 声明端口和GPIO号，为了高效使用LL库
// 注意这里不要和CubeMX生成的代码重名
#define LCD_SCREEN_PORT GPIOB

#define R1_PIN     LL_GPIO_PIN_0
#define G1_PIN     LL_GPIO_PIN_1
#define B1_PIN     LL_GPIO_PIN_2
#define R2_PIN     LL_GPIO_PIN_5
#define G2_PIN     LL_GPIO_PIN_4
#define B2_PIN     LL_GPIO_PIN_3
#define ADDR_A_PIN LL_GPIO_PIN_12
#define ADDR_B_PIN LL_GPIO_PIN_13
#define ADDR_C_PIN LL_GPIO_PIN_14
#define ADDR_D_PIN LL_GPIO_PIN_15
#define LAT_PIN    LL_GPIO_PIN_10
#define OE_PIN     LL_GPIO_PIN_11
#define CLK_PIN    LL_GPIO_PIN_8

void LED_screen_update_rgb_565(uint8_t brightness);
void LED_screen_update_rgb_888(uint8_t brightness);

#endif
