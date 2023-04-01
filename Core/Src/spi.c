/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    spi.c
 * @brief   This file provides code for the configuration
 *          of the SPI instances.
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
/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */
volatile bool transferComplete = false;

static void transferCompleteCallback(void);
/* USER CODE END 0 */

/* SPI1 init function */
void MX_SPI1_Init(void)
{

    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */

    LL_SPI_InitTypeDef SPI_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    /**SPI1 GPIO Configuration
    PA5   ------> SPI1_SCK
    PA6   ------> SPI1_MISO
    PA7   ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI1 DMA Init */

    /* SPI1_RX Init */
    LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_2,
                          LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
                              LL_DMA_PRIORITY_HIGH |
                              LL_DMA_MODE_NORMAL |
                              LL_DMA_PERIPH_NOINCREMENT |
                              LL_DMA_MEMORY_NOINCREMENT |
                              LL_DMA_PDATAALIGN_BYTE |
                              LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_2,
                           LL_SPI_DMA_GetRegAddr(SPI1),
                           (uint32_t)0x00,
                           LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, 1);
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_2, LL_DMA_REQUEST_1);

    /* (4) Configure the DMA1_Channel3 functional parameters */
    LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_3,
                          LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_HIGH | LL_DMA_MODE_NORMAL |
                              LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT |
                              LL_DMA_PDATAALIGN_BYTE | LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3, (uint32_t)0x00, LL_SPI_DMA_GetRegAddr(SPI1),
                           LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, 1);
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMA_REQUEST_1);

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);
    /* USER CODE END SPI1_Init 1 */
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 7;
    LL_SPI_Init(SPI1, &SPI_InitStruct);
    LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_EnableNSSPulseMgt(SPI1);
    /* USER CODE BEGIN SPI1_Init 2 */
    LL_SPI_SetRxFIFOThreshold(SPI1, LL_SPI_RX_FIFO_TH_QUARTER);
    LL_SPI_EnableDMAReq_RX(SPI1);
    LL_SPI_EnableDMAReq_TX(SPI1);
    /* USER CODE END SPI1_Init 2 */
}

/* USER CODE BEGIN 1 */
/** @brief Writes one byte of data.
 *
 *  @param reg The hex value of the register to write to.
 *  @param data The data to write.
 *  @return Void.
 */
void spi1Transfer(uint8_t *txData, uint8_t *rxData, size_t size)
{
    // RX Setup
    DMA1_Channel2->CCR &= ~DMA_CCR_EN;
    while (DMA1_Channel2->CCR & DMA_CCR_EN)
        ;

    DMA1_Channel2->CNDTR &= ~DMA_CNDTR_NDT;
    DMA1_Channel2->CNDTR = size;
    DMA1_Channel2->CMAR = (uint32_t)rxData;

    // TX Setup
    DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    while (DMA1_Channel3->CCR & DMA_CCR_EN)
        ;

    DMA1_Channel3->CNDTR &= ~DMA_CNDTR_NDT;
    DMA1_Channel3->CNDTR = size;
    DMA1_Channel3->CMAR = (uint32_t)txData;

    SPI1->CR1 |= SPI_CR1_SPE;

    DMA1_Channel2->CCR |= DMA_CCR_EN;
    DMA1_Channel3->CCR |= DMA_CCR_EN;

    transferCompleteCallback();
}

static void transferCompleteCallback(void)
{
    while (!transferComplete)
        ;
    SPI1->CR1 &= ~SPI_CR1_SPE;
    transferComplete = false;
}

/* Interrupt Handlers */

/**
 * @brief This function handles DMA2 stream1 global interrupt.
 */
void DMA1_Channel2_IRQHandler(void)
{
    /* RX Handler */
    if (DMA1->ISR & DMA_ISR_TCIF2)
    {
        DMA1->IFCR |= DMA_IFCR_CTCIF2;

        transferComplete = true;
    }
}

/**
 * @brief This function handles DMA2 stream3 global interrupt.
 */
void DMA1_Channel3_IRQHandler(void)
{
    /* TX Handler */
    if (DMA1->ISR & DMA_ISR_TCIF3)
    {
        DMA1->IFCR |= DMA_IFCR_CTCIF3;
    }
}
/* USER CODE END 1 */
