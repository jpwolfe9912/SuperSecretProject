/**
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  */
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "super_secret_project.h"
#include "lwrb.h"
#include "esp32.h"

/* Defines */
#define RX_DMA_SIZE 1024
#define RX_RB_SIZE 1024
#define TX_SIZE 1024

/* Typdef Structs */
typedef struct 
{
    uint8_t TxBuffer[TX_SIZE];

    uint8_t RxBuffer_DMA[RX_DMA_SIZE];
    uint8_t RxBuffer_Data[RX_RB_SIZE];
    lwrb_t RxBuffer;
} UsartBuffs_t;


/* Global Extern Variables */
extern UsartBuffs_t Buffs;

/* Function Prototypes */
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void serialWrite(const char *str);
char serialRead(void);

void usart_rx_check(void);
void usart_process_data(const void* data, size_t len);
void usart_transmit_dma(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

