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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs.h"
#include "images.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    // HAL_Init();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0U);

    SysTick_Config(SystemCoreClock / 1000);

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */
    cycleCounterInit();
    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_DMA_Init();
    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_I2C1_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    // MX_FATFS_Init();
    /* USER CODE BEGIN 2 */
    ili9341Init();
    if (ft6206Init())
        printf("Init Success\n");
    else
        printf("Init Failed\n");

    ili9341FillScreen(BLUE);
    ili9341DrawImage(0, 0, ILI9341_SCREEN_WIDTH, ILI9341_SCREEN_HEIGHT, &nellie);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    Wifi_Init();
    SNTP_Init();
    MQTT_Init();

    MQTT_Message_t Test;
    strcpy(Test.topic, "topic/esp32atRx");
    strcpy(Test.TxData, "Hello from STM32L432");
    MQTT_Subscribe(Test);
    strcpy(Test.topic, "topic/esp32atTx");
    MQTT_Unsubscribe(Test);
    memset(Test.RxData, '\0', sizeof(Test.RxData));

    TouchData_t txTouchData10[10];
    TouchData_t rxTouchData10[10];
    memset(rxTouchData10, '\0', sizeof(rxTouchData10));
    uint8_t txTouchIdx = 0;
    uint8_t rxTouchIdx = 0;
    bool dataReady = false;

    bool dataRecv = false;
    uint32_t start;
    volatile uint32_t elapsed;

    while (1)
    {
        /*
        5 Things to do
            1) Listen for and display touch
                a) In 100Hz loop, listen for TouchData.touched
                b) Write small circle at touched spot
                c) Scan touches into buffer
            2) Send touches
                a) Wait for 10 touches to occur
                b) Send buffer of touches
            3) Listen for data
                a) If "+MQTTSUBRECV" received
                b) Read 10 values into buffer
                c) Set recv data ready flag
            4) Display data
                a) Display data to screen when ready
        */

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        if (!dataRecv)
        {
            if (MQTT_ListenForMessage(&Test))
            {
                start = micros();
                if (sscanf(Test.RxData, "%u;%u %u;%u %u;%u %u;%u %u;%u %u;%u %u;%u %u;%u %u;%u %u;%u", &rxTouchData10[0].xPos, &rxTouchData10[0].yPos, &rxTouchData10[1].xPos, &rxTouchData10[1].yPos, &rxTouchData10[2].xPos, &rxTouchData10[2].yPos, &rxTouchData10[3].xPos, &rxTouchData10[3].yPos, &rxTouchData10[4].xPos, &rxTouchData10[4].yPos, &rxTouchData10[5].xPos, &rxTouchData10[5].yPos, &rxTouchData10[6].xPos, &rxTouchData10[6].yPos, &rxTouchData10[7].xPos, &rxTouchData10[7].yPos, &rxTouchData10[8].xPos, &rxTouchData10[8].yPos, &rxTouchData10[9].xPos, &rxTouchData10[9].yPos))
                        /* sscanf function takes over 500us to complete... could be causing problems */
                    {
                        dataRecv = true;
                        elapsed = micros() - start;
                    }
            }
        }
        if (dataRecv)
        {
            if (frame100Hz)
            {

                fillRect(rxTouchData10[rxTouchIdx].xPos, rxTouchData10[rxTouchIdx].yPos, 5, 5, GREEN);
                if (++rxTouchIdx > 10)
                {
                    rxTouchIdx = 0;
                    dataRecv = false;
                    memset(rxTouchData10, '\0', sizeof(rxTouchData10));
                }
            }
        }

        // if (touchData.touched)
        // {
        //     ft6206ReadData();
        //     txTouchData10[txTouchIdx].xPos = touchData.xPos;
        //     txTouchData10[txTouchIdx].yPos = touchData.yPos;
        //     fillCircle(touchData.xPos, touchData.yPos, 3, RED);
        //     if (txTouchIdx++ > 10)
        //         dataReady = true;

        //     touchData.touched = false;
        // }

        // if (dataReady)
        // {
        //     sprintf(Test.TxData, "%u;%u %u;%u %u;%u %u;%u %u;%u %u;%u %u;%u %u;%u %u;%u %u;%u", txTouchData10[0].xPos, txTouchData10[0].yPos, txTouchData10[1].xPos, txTouchData10[1].yPos, txTouchData10[2].xPos, txTouchData10[2].yPos, txTouchData10[3].xPos, txTouchData10[3].yPos, txTouchData10[4].xPos, txTouchData10[4].yPos, txTouchData10[5].xPos, txTouchData10[5].yPos, txTouchData10[6].xPos, txTouchData10[6].yPos, txTouchData10[7].xPos, txTouchData10[7].yPos, txTouchData10[8].xPos, txTouchData10[8].yPos, txTouchData10[9].xPos, txTouchData10[9].yPos);
        //     strcpy(Test.topic, "topic/esp32atTx");
        //     MQTT_Publish(Test);
        //     dataReady = false;
        //     txTouchIdx = 0;
        // }
    }
    /* USER CODE END 3 */
}

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
