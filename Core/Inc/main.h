/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ulog.h"
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED_DC_Pin GPIO_PIN_9
#define OLED_DC_GPIO_Port GPIOB
#define OLED_RST_Pin GPIO_PIN_14
#define OLED_RST_GPIO_Port GPIOC
#define OLED_CS_Pin GPIO_PIN_15
#define OLED_CS_GPIO_Port GPIOC
#define LED_SYS_Pin GPIO_PIN_5
#define LED_SYS_GPIO_Port GPIOA
#define LED_CH1_Pin GPIO_PIN_2
#define LED_CH1_GPIO_Port GPIOB
#define LED_CH2_Pin GPIO_PIN_6
#define LED_CH2_GPIO_Port GPIOC
#define LED_CH3_Pin GPIO_PIN_12
#define LED_CH3_GPIO_Port GPIOA
#define LED_CH4_Pin GPIO_PIN_15
#define LED_CH4_GPIO_Port GPIOA
#define KEY_D_Pin GPIO_PIN_3
#define KEY_D_GPIO_Port GPIOB
#define KEY_R_Pin GPIO_PIN_4
#define KEY_R_GPIO_Port GPIOB
#define KEY_L_Pin GPIO_PIN_5
#define KEY_L_GPIO_Port GPIOB
#define KEY_U_Pin GPIO_PIN_8
#define KEY_U_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
