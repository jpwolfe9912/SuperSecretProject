/**
 * @file    i2c.c
 * @brief   This file provides code for the configuration
 *          of the I2C instances.
 */

/* Includes */
#include "i2c.h"

/* Global Variables */
volatile bool tx_finished;
volatile bool rx_finished;

uint8_t i2c1Timeout;


/**
 * @brief Initialize I2C low level registers
 * @param  
 */
void MX_I2C1_Init(void)
{
    NVIC_SetPriority(I2C1_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(I2C1_EV_IRQn);

    LL_I2C_InitTypeDef I2C_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    /**I2C1 GPIO Configuration
    PB6   ------> I2C1_SCL
    PB7   ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6 | LL_GPIO_PIN_7);

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    /* I2C1 DMA Init */

    /* I2C1_TX Init */
    DMA1_Channel6->CCR |= DMA_CCR_DIR; // mem to per
    DMA1_Channel6->CCR &= ~DMA_CCR_CIRC;
    DMA1_Channel6->CCR &= ~DMA_CCR_PINC;
    DMA1_Channel6->CCR |= DMA_CCR_MINC;
    DMA1_Channel6->CCR &= ~DMA_CCR_PSIZE;
    DMA1_Channel6->CCR &= ~DMA_CCR_MSIZE;
    DMA1_Channel6->CPAR = (uint32_t)(&(I2C1->TXDR));
    DMA1_Channel6->CCR |= DMA_CCR_PL;
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_6, LL_DMA_REQUEST_3);
    /* I2C1_RX Init */
    DMA1_Channel7->CCR &= ~DMA_CCR_DIR; // per to mem
    DMA1_Channel7->CCR &= ~DMA_CCR_CIRC;
    DMA1_Channel7->CCR &= ~DMA_CCR_PINC;
    DMA1_Channel7->CCR |= DMA_CCR_MINC;
    DMA1_Channel7->CCR &= ~DMA_CCR_PSIZE;
    DMA1_Channel7->CCR &= ~DMA_CCR_MSIZE;
    DMA1_Channel7->CPAR = (uint32_t)(&(I2C1->RXDR));
    DMA1_Channel7->CCR |= DMA_CCR_PL;
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_7, LL_DMA_REQUEST_3);

    /* Enable DMA interrupts complete/error */
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_6);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_6);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_7);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_7);
    /* USER CODE END I2C1_Init 1 */

    /** I2C Initialization
     */
    LL_I2C_Disable(I2C1);
    LL_I2C_EnableAutoEndMode(I2C1);
    LL_I2C_DisableOwnAddress2(I2C1);
    LL_I2C_DisableGeneralCall(I2C1);
    LL_I2C_EnableClockStretching(I2C1);
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.Timing = 0x10909CEC;
    I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
    I2C_InitStruct.DigitalFilter = 0;
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2C1, &I2C_InitStruct);
    LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);

    LL_I2C_EnableDMAReq_TX(I2C1);
    LL_I2C_EnableDMAReq_RX(I2C1);
}

/**
 * @brief Sends data over I2C1 using DMA
 * @param slaveAdd: Address of slave device
 * @param pData: Data to send
 * @param size: Size of the data
 */
void i2c1Write(uint8_t slaveAdd, uint8_t *pData, size_t size)
{
    i2c1Timeout = I2C1_TIMEOUT;
    /* Send Slave Address */
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;
    while (DMA1_Channel6->CCR & DMA_CCR_EN)
        ;
    DMA1_Channel6->CNDTR = size;
    DMA1_Channel6->CMAR = (uint32_t)pData;
    DMA1_Channel6->CCR |= DMA_CCR_EN;

    I2C1->CR2 &= ~I2C_CR2_SADD;
    I2C1->CR2 |= (slaveAdd << 1U);
    I2C1->CR2 &= ~I2C_CR2_RD_WRN; // write operation
    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |= (size << I2C_CR2_NBYTES_Pos);
    I2C1->CR2 |= I2C_CR2_AUTOEND;
    I2C1->CR2 |= I2C_CR2_START;

    while (!tx_finished)
    {
        if (LL_SYSTICK_IsActiveCounterFlag())
        {
            if (i2c1Timeout-- == 0)
            {
                /* Time-out occurred. Set LD3 to blinking mode */
                ledOn();
            }
        }
    }
    tx_finished = false;
}

/**
 * @brief Reads data over I2C1 using DMA
 * @param slaveAdd: Slave address to read from
 * @param reg: Register on the slave device to read from
 * @param pData: Variable to read data into
 * @param size: Size of data to read
 */
void i2c1Read(uint8_t slaveAdd, uint8_t reg, uint8_t *pData, uint8_t size)
{
    /* Send Slave Address */
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;
    while (DMA1_Channel6->CCR & DMA_CCR_EN)
        ;
    DMA1_Channel6->CNDTR = 1;
    DMA1_Channel6->CMAR = (uint32_t)&reg;
    DMA1_Channel6->CCR |= DMA_CCR_EN;

    I2C1->CR2 &= ~I2C_CR2_SADD;
    I2C1->CR2 |= (slaveAdd << 1U);
    I2C1->CR2 &= ~I2C_CR2_RD_WRN; // write operation
    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |= (1 << I2C_CR2_NBYTES_Pos);
    I2C1->CR2 |= I2C_CR2_AUTOEND;
    I2C1->CR2 |= I2C_CR2_START;

    uint32_t currTime = millis();
    while (!tx_finished)
    {
        if ((millis() - currTime) > 1000)
            break;
    }
    tx_finished = false;

    /* Receive */
    DMA1_Channel7->CCR &= ~DMA_CCR_EN;
    while (DMA1_Channel7->CCR & DMA_CCR_EN)
        ;
    DMA1_Channel7->CNDTR = size;
    DMA1_Channel7->CMAR = (uint32_t)pData;
    DMA1_Channel7->CCR |= DMA_CCR_EN;

    I2C1->CR2 |= (slaveAdd << 1U);
    I2C1->CR2 |= I2C_CR2_RD_WRN; // read operation
    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |= (size << I2C_CR2_NBYTES_Pos);
    I2C1->CR2 |= I2C_CR2_AUTOEND;
    I2C1->CR2 |= I2C_CR2_START;
    
    currTime = millis();
    while (!rx_finished)
    {
        if ((millis() - currTime) > 1000)
            break;
    }
    rx_finished = false;
}

/**
 * @brief DMA1 Channel 6 IRQ Handler
 * @param  
 */
void DMA1_Channel6_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF6)
    {
        DMA1->IFCR |= DMA_IFCR_CTCIF6;
        tx_finished = true;
    }
}

/**
 * @brief DMA1 Channel 7 IRQ Handler
 * @param  
 */
void DMA1_Channel7_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF7)
    {
        DMA1->IFCR |= DMA_IFCR_CTCIF7;
        rx_finished = true;
    }
}
