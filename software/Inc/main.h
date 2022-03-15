/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY1_Pin GPIO_PIN_13
#define KEY1_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_14
#define KEY2_GPIO_Port GPIOC
#define KEY3_Pin GPIO_PIN_15
#define KEY3_GPIO_Port GPIOC
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define R1_Pin GPIO_PIN_0
#define R1_GPIO_Port GPIOB
#define G1_Pin GPIO_PIN_1
#define G1_GPIO_Port GPIOB
#define B1_Pin GPIO_PIN_2
#define B1_GPIO_Port GPIOB
#define LAT_Pin GPIO_PIN_10
#define LAT_GPIO_Port GPIOB
#define OE_Pin GPIO_PIN_11
#define OE_GPIO_Port GPIOB
#define ADDR_A_Pin GPIO_PIN_12
#define ADDR_A_GPIO_Port GPIOB
#define ADDR_B_Pin GPIO_PIN_13
#define ADDR_B_GPIO_Port GPIOB
#define ADDR_C_Pin GPIO_PIN_14
#define ADDR_C_GPIO_Port GPIOB
#define ADDR_D_Pin GPIO_PIN_15
#define ADDR_D_GPIO_Port GPIOB
#define PCF8563_INT_Pin GPIO_PIN_12
#define PCF8563_INT_GPIO_Port GPIOA
#define KEY_4_Pin GPIO_PIN_15
#define KEY_4_GPIO_Port GPIOA
#define B2_Pin GPIO_PIN_3
#define B2_GPIO_Port GPIOB
#define G2_Pin GPIO_PIN_4
#define G2_GPIO_Port GPIOB
#define R2_Pin GPIO_PIN_5
#define R2_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
#define CLK_Pin GPIO_PIN_8
#define CLK_GPIO_Port GPIOB
#define PWM_OUT_Pin GPIO_PIN_9
#define PWM_OUT_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
