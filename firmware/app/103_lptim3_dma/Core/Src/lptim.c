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
DMA_NodeTypeDef Node_LPDMA1_Channel3;
DMA_QListTypeDef List_LPDMA1_Channel3;
DMA_HandleTypeDef handle_LPDMA1_Channel3;

/* LPTIM3 init function */
void MX_LPTIM3_Init(void)
{

  /* USER CODE BEGIN LPTIM3_Init 0 */

  /* USER CODE END LPTIM3_Init 0 */

  LPTIM_IC_ConfigTypeDef sConfig = {0};

  /* USER CODE BEGIN LPTIM3_Init 1 */

  /* USER CODE END LPTIM3_Init 1 */
  hlptim3.Instance = LPTIM3;
  hlptim3.Init.Clock.Source = LPTIM_CLOCKSOURCE_ULPTIM;
  hlptim3.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
  hlptim3.Init.UltraLowPowerClock.Polarity = LPTIM_CLOCKPOLARITY_RISING;
  hlptim3.Init.UltraLowPowerClock.SampleTime = LPTIM_CLOCKSAMPLETIME_DIRECTTRANSITION;
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

void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef* lptimHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  DMA_NodeConfTypeDef nodeconfig = {0};
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

    /* LPTIM3 DMA Init */
    /* LPDMA1_REQUEST_LPTIM3_IC1 Init */
    nodeconfig.NodeType = DMA_LPDMA_LINEAR_NODE;
    nodeconfig.Init.Request = LPDMA1_REQUEST_LPTIM3_IC1;
    nodeconfig.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    nodeconfig.Init.Direction = DMA_PERIPH_TO_MEMORY;
    nodeconfig.Init.SrcInc = DMA_SINC_FIXED;
    nodeconfig.Init.DestInc = DMA_DINC_FIXED;
    nodeconfig.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_HALFWORD;
    nodeconfig.Init.DestDataWidth = DMA_DEST_DATAWIDTH_HALFWORD;
    nodeconfig.Init.Priority = DMA_HIGH_PRIORITY;
    nodeconfig.Init.SrcBurstLength = 1;
    nodeconfig.Init.DestBurstLength = 1;
    nodeconfig.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    nodeconfig.Init.Mode = DMA_NORMAL;
    nodeconfig.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
    nodeconfig.TriggerConfig.TriggerMode = DMA_TRIGM_BLOCK_TRANSFER;
    nodeconfig.TriggerConfig.TriggerSelection = 0;
    nodeconfig.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
    nodeconfig.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
    nodeconfig.RepeatBlockConfig.RepeatCount = 1;
    nodeconfig.RepeatBlockConfig.SrcAddrOffset = 0;
    nodeconfig.RepeatBlockConfig.DestAddrOffset = 0;
    nodeconfig.RepeatBlockConfig.BlkSrcAddrOffset = 0;
    nodeconfig.RepeatBlockConfig.BlkDestAddrOffset = 0;
    if (HAL_DMAEx_List_BuildNode(&nodeconfig, &Node_LPDMA1_Channel3) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DMAEx_List_InsertNode(&List_LPDMA1_Channel3, NULL, &Node_LPDMA1_Channel3) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DMAEx_List_SetCircularMode(&List_LPDMA1_Channel3) != HAL_OK)
    {
      Error_Handler();
    }

    handle_LPDMA1_Channel3.Instance = LPDMA1_Channel3;
    handle_LPDMA1_Channel3.InitLinkedList.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_LPDMA1_Channel3.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
    handle_LPDMA1_Channel3.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
    handle_LPDMA1_Channel3.InitLinkedList.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_LPDMA1_Channel3.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;
    if (HAL_DMAEx_List_Init(&handle_LPDMA1_Channel3) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DMAEx_List_LinkQ(&handle_LPDMA1_Channel3, &List_LPDMA1_Channel3) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(lptimHandle, hdma[LPTIM_DMA_ID_CC1], handle_LPDMA1_Channel3);

    if (HAL_DMA_ConfigChannelAttributes(&handle_LPDMA1_Channel3, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }

  /* USER CODE BEGIN LPTIM3_MspInit 1 */

  /* USER CODE END LPTIM3_MspInit 1 */
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

    /* LPTIM3 DMA DeInit */
    HAL_DMA_DeInit(lptimHandle->hdma[LPTIM_DMA_ID_CC1]);
  /* USER CODE BEGIN LPTIM3_MspDeInit 1 */

  /* USER CODE END LPTIM3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */







