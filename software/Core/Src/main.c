/**
 * @file        main.c
 * @brief       Main program body
 */
/* Includes */
#include "main.h"
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
    NVIC_SetPriorityGrouping(NVIC_GetPriorityGrouping());
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(0, 15, 0U));

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
#ifdef USE_LCD
    ili9341Init();
#endif
#ifdef USE_TOUCH
    if (ft6206Init())
        serialWrite("Init Success\n");
    else
        serialWrite("Init Failed\n");
#endif

    ili9341FillScreen(BLACK);
    ili9341DrawImage(0, 0, _lcd_width, _lcd_height, nellie);

    lwrb_init(&Buffs.RxBuffer, (void *)Buffs.RxBuffer_Data, sizeof(Buffs.RxBuffer_Data));
    Buffs.RxBuffer.buff = malloc(RX_RB_SIZE);
    Wifi_Init();
    while (!Wifi_GetApConnection())
    {
        Wifi_Station_ConnectToAp(AP_SSID, AP_PASSWORD, NULL);
        delay(1000);
    }
    SNTP_Init();
    MQTT_Init();

    MQTT_Message_t TxPacket;
    MQTT_Message_t RxPacket;
    initTxPacket(&TxPacket);
    initRxPacket(&RxPacket);

    initCoords(&TxCoords);
    initCoords(&RxCoords);

    while (1)
    {
        if ((GPIOB->IDR & GPIO_IDR_ID4) || lcdReset)
        {
            ili9341FillScreen(BLACK);
            memcpy(&TxPacket.data[0], "*\0", 2);
            MQTT_Publish(TxPacket);
            memset(RxPacket.data, '\0', _MQTT_RX_SIZE);
            delay(100);
            lcdReset = false;
        }
        readAndSendTouches(&TxPacket);
        recvAndDisplayTouches(&RxPacket);
    }
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
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
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