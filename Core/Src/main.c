/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes */
#include "data_transfer.h"
#include "fatfs.h"
#include "esp32.h"
#include "ft6206.h"
#include "ili9341.h"
#include "graphics.h"
#include "images.h"

/* Private function prototypes */
void SystemClock_Config(void);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0U);

    SysTick_Config(SystemCoreClock / 1000);

    SystemClock_Config();

    cycleCounterInit();

    /* Initialize all configured peripherals */
    MX_DMA_Init();
    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_I2C1_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();

// MX_FATFS_Init();
/* USER CODE BEGIN 2 */
#ifdef USE_LCD
    ili9341Init();
#endif
#ifdef USE_TOUCH
    if (ft6206Init())
        printf("Init Success\n");
    else
        printf("Init Failed\n");
#endif

    ili9341FillScreen(BLUE);
    ili9341DrawImage(0, 0, ILI9341_SCREEN_WIDTH, ILI9341_SCREEN_HEIGHT, nellie);

    lwrb_init(&Buffs.RxBuffer, (void *)Buffs.RxBuffer_Data, sizeof(Buffs.RxBuffer_Data));

    Wifi_Init();
    while (!Wifi_GetApConnection())
    { // connect if not connected
        Wifi_Station_ConnectToAp("Jeremy Wolfe's iPhone", "aaaaaaaa", NULL);
        delay(1000);
    }
    SNTP_Init();
    MQTT_Init();

    MQTT_Message_t TxPacket;
    MQTT_Message_t RxPacket;
    initTxPacket(&TxPacket);
    initRxPacket(&RxPacket);

    char *str = "+MQTTSUBRECV";
    size_t find_idx = 0;
    while (1)
    {
        readAndSendTouches(&TxPacket);
        if(frame1000Hz)
        {
            if(MQTT_ListenForMessage(&RxPacket, str, &find_idx))
                serialWrite('y');
            lwrb_reset(&Buffs.RxBuffer);
            frame1000Hz = false;
        }
    }
}
/* USER CODE END 3 */

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
    {
    }
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    LL_RCC_MSI_Enable();

    /* Wait till MSI is ready */
    while (LL_RCC_MSI_IsReady() != 1)
    {
    }
    LL_RCC_MSI_EnableRangeSelection();
    LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
    LL_RCC_MSI_SetCalibTrimming(0);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1)
    {
    }
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_SetSystemCoreClock(80000000);

    /* Update the time base */
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void ledOn(void)
{
    GPIOB->BSRR |= GPIO_BSRR_BS_3;
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
