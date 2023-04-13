#include "leds.h"

/**
 * @brief Turn on the LED
 * @param  
 */
void ledOn(void)
{
    LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin);
}