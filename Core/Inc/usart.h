/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "super_secret_project.h"
#include "../lwrb/lwrb.h"
#include "esp32.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define RX_DMA_SIZE 2048
#define RX_RB_SIZE 1024
#define TX_SIZE 1024

typedef struct 
{
    uint8_t RxBuffer_DMA[RX_DMA_SIZE];
    lwrb_t RxBuffer;
    uint8_t RxBuffer_Data[RX_RB_SIZE];

    uint8_t TxBuffer[TX_SIZE];
} UsartBuffs_t;


/* USER CODE END Private defines */

extern UsartBuffs_t Buffs;

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void serialWrite(uint8_t ch);
char serialRead(void);

void usart_rx_check(void);
void usart_process_data(const void* data, size_t len);
void usart_transmit_dma(const char *str);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

