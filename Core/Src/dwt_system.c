/** @file 		drv_system.c
 *  @brief
 *  	This file contains all the basic functions to run the timing
 *  	of the system as well as initialize the system.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#include "dwt_system.h"

/* Static Variables */
static volatile uint32_t usTicks = 0;
static volatile uint32_t sysTickUptime = 0;
static volatile uint32_t sysTickCycleCounter = 0;
uint16_t frameCounter = 0;

volatile bool frame1Hz = false;
volatile bool frame10Hz = false;
volatile bool frame100Hz = false;
volatile bool frame1000Hz = false;

/* Static Function Prototypes */

/** @brief Gets system time in microseconds.
 *
 *		This can be called from within IRQ Handlers, so uses LDREX/STREX.
 *		If a higher priority IRQ or DMA or anything happens the STREX will fail
 *		and restart the loop. Otherwise the same number that was read is harmlessly
 *		written back.
 *
 *  @return uint32_t Time in microseconds.
 */
uint32_t
micros(void)
{
    register uint32_t oldCycle, cycle, timeMs;

    do
    {
        timeMs = __LDREXW(&sysTickUptime);
        cycle = DWT->CYCCNT;
        oldCycle = sysTickCycleCounter;
    } while (__STREXW(timeMs, &sysTickUptime));

    return (timeMs * 1000) + (cycle - oldCycle) / usTicks;
}

/** @brief Gets system time in milliseconds.
 *
 *  @return uint32_t Time in milliseconds.
 */
uint32_t
millis(void)
{
    return sysTickUptime;
}

/** @brief Delay in microseconds.
 *
 *  @return Void.
 */
void delayMicroseconds(uint32_t us)
{
    uint32_t elapsed = 0;
    uint32_t lastCount = DWT->CYCCNT;

    for (;;)
    {
        register uint32_t current_count = DWT->CYCCNT;
        uint32_t elapsed_us;

        // measure the time elapsed since the last time we checked
        elapsed += current_count - lastCount;
        lastCount = current_count;

        // convert to microseconds
        elapsed_us = elapsed / usTicks;
        if (elapsed_us >= us)
            break;

        // reduce the delay by the elapsed time
        us -= elapsed_us;

        // keep fractional microseconds for the next iteration
        elapsed %= usTicks;
    }
}

/** @brief Delay in milliseconds.
 *
 *  @return Void.
 */
void delay(uint32_t ms)
{
    while (ms--)
        delayMicroseconds(1000);
}

/** @brief Initializes the cycle counter so we can use delay
 *  and getTime functions
 *
 *  @return Void.
 */
void cycleCounterInit(void)
{
    usTicks = SystemCoreClock / 1000000;

    // enable DWT access
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // enable the CPU cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void SysTick_Handler(void)
{
    sysTickCycleCounter = DWT->CYCCNT;
    sysTickUptime++;
    frameCounter++;
    frame1000Hz = true;
    if (!(frameCounter % 10))
        frame100Hz = true;
    if (!(frameCounter % 100))
        frame10Hz = true;
    if (!(frameCounter % 1000))
        frame1Hz = true;
    HAL_IncTick();
}