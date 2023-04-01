#ifndef __SUPER_SECRET_PROJECT_H__
#define __SUPER_SECRET_PROJECT_H__

#include "main.h"

#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_i2c.h"
#include "stm32l4xx_ll_crs.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include "lwrb.h"

/* Lower Level Includes */
#include "stm32l4xx_it.h"
#include "dwt_system.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

// #include "images.h"
// #include "fonts.h"
// #include "ili9341.h"
// #include "ft6206.h"
// #include "utilities.h"
// #include "data_transfer.h"
// #include "esp32.h"
// #include "graphics.h"

#endif