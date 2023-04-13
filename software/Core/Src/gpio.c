/**
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
 */

/* Includes */
#include "gpio.h"

/**
 * @brief Initialize GPIO Pins not associated with an alternate function
 * @param  Void
 */
void MX_GPIO_Init(void)
{

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

    /* Setup GPIOA Pins as Output */
    LL_GPIO_ResetOutputPin(GPIOA, DC_Pin | LCD_CS_Pin);

    /**/
    GPIO_InitStruct.Pin = DC_Pin | LCD_CS_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_GPIO_SetOutputPin(GPIOA, DC_Pin | LCD_CS_Pin);

    /* Set GPIOB Pins as Output */
    LL_GPIO_ResetOutputPin(GPIOB, LCD_RST_Pin | LED_Pin);

    /**/
    GPIO_InitStruct.Pin = LCD_RST_Pin | LED_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    LL_GPIO_SetOutputPin(GPIOB, LCD_RST_Pin | LED_Pin);

    /* Set GPIOB Pins as Input */
    GPIO_InitStruct.Pin = BTN_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Enable External Interrupt */
    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

    /**/
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE5);

    /**/
    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_5;
    EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    /**/
    LL_GPIO_SetPinPull(TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin, LL_GPIO_PULL_UP);

    /**/
    LL_GPIO_SetPinMode(TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin, LL_GPIO_MODE_INPUT);

    /* EXTI interrupt init*/
    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));

    GPIOB->MODER &= ~GPIO_MODER_MODE4;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD4_1;
    GPIOB->MODER &= ~GPIO_MODER_MODE3;
    GPIOB->MODER |= GPIO_MODER_MODE3_0;
}