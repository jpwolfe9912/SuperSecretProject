/**
 * @file    main.h
 * @brief   Header for main.c file.
 *              This file contains the common defines of the application.
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */
#include "super_secret_project.h"

/* Defines */
#define USE_LCD
#define USE_TOUCH

/* Functions Prototypes */
void Error_Handler(void);

/* Defines */
#define DC_Pin LL_GPIO_PIN_3
#define DC_GPIO_Port GPIOA
#define LCD_CS_Pin LL_GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOA

#define LCD_RST_Pin LL_GPIO_PIN_1
#define LCD_RST_Port GPIOB
#define LED_Pin LL_GPIO_PIN_3
#define LED_GPIO_Port GPIOB
#define BTN_Pin LL_GPIO_PIN_4
#define BTN_GPIO_Port GPIOB
#define TOUCH_IRQ_Pin LL_GPIO_PIN_5
#define TOUCH_IRQ_GPIO_Port GPIOB

#define SEND_RATE frame1Hz

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
