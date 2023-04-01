/** @file 		drv_system.h
 *  @brief
 *  	This file contains all the basic functions to run the timing
 *  	of the system as well as initialize the system.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#ifndef __DWT_SYSTEM_H__
#define __DWT_SYSTEM_H__

#include "super_secret_project.h"

extern volatile bool frame1Hz;
extern volatile bool frame10Hz;
extern volatile bool frame100Hz;
extern volatile bool frame1000Hz;

/* Function Prototypes */
void cycleCounterInit(void);

void delayMicroseconds(uint32_t us);
void delay(uint32_t ms);

uint32_t micros(void);
uint32_t millis(void);
uint32_t getSysUpTime(void);


#endif /* __DWT_SYSTEM_H_ */
