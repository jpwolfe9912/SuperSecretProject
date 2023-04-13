/**
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *             the spi.c file
 */
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "super_secret_project.h"

/* Defines */
#define SPI_BR_PRESCALER_8			(0x2 << 3U)
#define SPI_BR_PRESCALER_16			(0x3 << 3U)
#define SPI_BR_PRESCALER_128		(0x6 << 3U)

/* Function Prototypes */
void MX_SPI1_Init(void);
void spi1Transfer(uint8_t* txData, uint8_t* rxData, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

