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
// #include "stm32l4xx_hal.h"

// #include "stm32l4xx_ll_dma.h"
// #include "stm32l4xx_ll_i2c.h"
// #include "stm32l4xx_ll_crs.h"
// #include "stm32l4xx_ll_rcc.h"
// #include "stm32l4xx_ll_bus.h"
// #include "stm32l4xx_ll_system.h"
// #include "stm32l4xx_ll_exti.h"
// #include "stm32l4xx_ll_cortex.h"
// #include "stm32l4xx_ll_utils.h"
// #include "stm32l4xx_ll_pwr.h"
// #include "stm32l4xx_ll_spi.h"
// #include "stm32l4xx_ll_usart.h"
// #include "stm32l4xx_ll_gpio.h"

// /* Private includes ----------------------------------------------------------*/
// /* USER CODE BEGIN Includes */
#include "super_secret_project.h"
// #include <stdbool.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdarg.h>
// #include <stdlib.h>
// #include <math.h>

// #include "stm32l4xx_it.h"
// #include "dwt_system.h"
// #include "dma.h"
// #include "i2c.h"
// #include "spi.h"
// #include "usart.h"
// #include "gpio.h"
// #include "fonts.h"
// #include "ili9341.h"
// #include "ft6206.h"
// // #include "utilities.h"
// #include "esp32.h"
// #include "graphics.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define USE_LCD
#define USE_TOUCH
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void ledOn(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define D_C_Pin LL_GPIO_PIN_3
#define D_C_GPIO_Port GPIOA
#define SPI1_CS_Pin LL_GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define FT_INT_Pin LL_GPIO_PIN_5
#define FT_INT_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define SEND_RATE frame1Hz
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
