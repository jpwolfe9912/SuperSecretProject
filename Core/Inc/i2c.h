/**
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
  */

#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "super_secret_project.h"
#include "leds.h"

/* Defines */
#define I2C1_TIMEOUT 5U

/* Function Prototypes */
void MX_I2C1_Init(void);
void i2c1Write(uint8_t slaveAdd, uint8_t *pData, size_t size);
void i2c1Read(uint8_t slaveAdd, uint8_t reg, uint8_t *pData, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

