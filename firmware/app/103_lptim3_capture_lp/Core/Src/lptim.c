/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lptim.c
  * @brief   This file provides code for the configuration
  *          of the LPTIM instances.
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
/* Includes ------------------------------------------------------------------*/
#include "lptim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

LPTIM_HandleTypeDef hlptim3;
LPTIM_HandleTypeDef hlptim4;

/* LPTIM3 init function */
void MX_LPTIM3_Init(void)
{

  /* USER CODE BEGIN LPTIM3_Init 0 */

  /* USER CODE END LPTIM3_Init 0 */

  LPTIM_IC_ConfigTypeDef sConfig = {0};

  /* USER CODE BEGIN LPTIM3_Init 1 */

  /* USER CODE END LPTIM3_Init 1 */
  hlptim3.Instance = LPTIM3;
  hlptim3.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim3.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
  hlptim3.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
  hlptim3.Init.Period = 65535;
  hlptim3.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim3.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  hlptim3.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
  hlptim3.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
  hlptim3.Init.RepetitionCounter = 0;
  if (HAL_LPTIM_Init(&hlptim3) != HAL_OK)
  {
    Error_Handler();
  }
  sConfig.ICInputSource = LPTIM_IC1SOURCE_GPIO;
  sConfig.ICPrescaler = LPTIM_ICPSC_DIV1;
  sConfig.ICPolarity = LPTIM_ICPOLARITY_RISING;
  sConfig.ICFilter = LPTIM_ICFLT_CLOCK_DIV1;
  if (HAL_LPTIM_IC_ConfigChannel(&hlptim3, &sConfig, LPTIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPTIM3_Init 2 */

  /* USER CODE END LPTIM3_Init 2 */

}
/* LPTIM4 init function */
void MX_LPTIM4_Init(void)
{

  /* USER CODE BEGIN LPTIM4_Init 0 */

  /* USER CODE END LPTIM4_Init 0 */

  /* USER CODE BEGIN LPTIM4_Init 1 */

  /* USER CODE END LPTIM4_Init 1 */
  hlptim4.Instance = LPTIM4;
  hlptim4.Init.Clock.Source = LPTIM_CLOCKSOURCE_ULPTIM;
  hlptim4.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
  hlptim4.Init.UltraLowPowerClock.Polarity = LPTIM_CLOCKPOLARITY_RISING;
  hlptim4.Init.UltraLowPowerClock.SampleTime = LPTIM_CLOCKSAMPLETIME_DIRECTTRANSITION;
  hlptim4.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
  hlptim4.Init.Period = 65535;
  hlptim4.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim4.Init.CounterSource = LPTIM_COUNTERSOURCE_EXTERNAL;
  hlptim4.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
  hlptim4.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
  hlptim4.Init.RepetitionCounter = 0;
  if (HAL_LPTIM_Init(&hlptim4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPTIM4_Init 2 */

  /* USER CODE END LPTIM4_Init 2 */

}

void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef* lptimHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(lptimHandle->Instance==LPTIM3)
  {
  /* USER CODE BEGIN LPTIM3_MspInit 0 */

  /* USER CODE END LPTIM3_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPTIM34;
    PeriphClkInit.Lptim34ClockSelection = RCC_LPTIM34CLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* LPTIM3 clock enable */
    __HAL_RCC_LPTIM3_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**LPTIM3 GPIO Configuration
    PD14     ------> LPTIM3_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LPTIM3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* LPTIM3 interrupt Init */
    HAL_NVIC_SetPriority(LPTIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(LPTIM3_IRQn);
  /* USER CODE BEGIN LPTIM3_MspInit 1 */

  /* USER CODE END LPTIM3_MspInit 1 */
  }
  else if(lptimHandle->Instance==LPTIM4)
  {
  /* USER CODE BEGIN LPTIM4_MspInit 0 */

  /* USER CODE END LPTIM4_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPTIM34;
    PeriphClkInit.Lptim34ClockSelection = RCC_LPTIM34CLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* LPTIM4 clock enable */
    __HAL_RCC_LPTIM4_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**LPTIM4 GPIO Configuration
    PD13     ------> LPTIM4_IN1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_LPTIM4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN LPTIM4_MspInit 1 */

  /* USER CODE END LPTIM4_MspInit 1 */
  }
}

void HAL_LPTIM_MspDeInit(LPTIM_HandleTypeDef* lptimHandle)
{

  if(lptimHandle->Instance==LPTIM3)
  {
  /* USER CODE BEGIN LPTIM3_MspDeInit 0 */

  /* USER CODE END LPTIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPTIM3_CLK_DISABLE();

    /**LPTIM3 GPIO Configuration
    PD14     ------> LPTIM3_CH1
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_14);

    /* LPTIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(LPTIM3_IRQn);
  /* USER CODE BEGIN LPTIM3_MspDeInit 1 */

  /* USER CODE END LPTIM3_MspDeInit 1 */
  }
  else if(lptimHandle->Instance==LPTIM4)
  {
  /* USER CODE BEGIN LPTIM4_MspDeInit 0 */

  /* USER CODE END LPTIM4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPTIM4_CLK_DISABLE();

    /**LPTIM4 GPIO Configuration
    PD13     ------> LPTIM4_IN1
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_13);

  /* USER CODE BEGIN LPTIM4_MspDeInit 1 */

  /* USER CODE END LPTIM4_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
