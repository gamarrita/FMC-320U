/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32u5xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DEBUG_LED_Pin GPIO_PIN_1
#define DEBUG_LED_GPIO_Port GPIOD
#define DEBUG_UART_Pin GPIO_PIN_2
#define DEBUG_UART_GPIO_Port GPIOD
#define SETUP_X_Pin GPIO_PIN_3
#define SETUP_X_GPIO_Port GPIOD
#define SETUP_Y_Pin GPIO_PIN_4
#define SETUP_Y_GPIO_Port GPIOD
#define LED_1_ERROR_Pin GPIO_PIN_5
#define LED_1_ERROR_GPIO_Port GPIOB
#define LED_2_ACTIVE_Pin GPIO_PIN_6
#define LED_2_ACTIVE_GPIO_Port GPIOB
#define LED_3_SIGNAL_Pin GPIO_PIN_7
#define LED_3_SIGNAL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
