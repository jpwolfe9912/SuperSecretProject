/**
 * @file    data_transfer.h
 * @author  Jeremy Wolfe (jpwolfe@me.com)
 * @brief   Used to handle data transfer from pixels on screen to AWS
 */

#ifndef __DATA_TRANSFER_H__
#define __DATA_TRANSFER_H__

#include "super_secret_project.h"
#include "ft6206.h"
#include "ili9341.h"
#include "graphics.h"
#include "utilities.h"

#define COORDS_SIZE 200 // TODO Create separate macros for rx and tx

typedef union
{
    uint16_t whole;
    struct{
        uint8_t low_nib;
        uint8_t high_nib;
    }uint8s;
} uint16_8;
typedef struct
{
    lwrb_t xPos;
    uint16_8 xPos_Data[COORDS_SIZE];
    lwrb_t yPos;
    uint16_8 yPos_Data[COORDS_SIZE];
} Coords_t;

extern bool lcdReset;

extern Coords_t TxCoords;
extern Coords_t RxCoords;

void initDevID(void);
void initTxPacket(MQTT_Message_t* MQTT_TxPacket);
void initRxPacket(MQTT_Message_t* MQTT_RxPacket);
void initCoords(Coords_t* Coords);
void readAndSendTouches(MQTT_Message_t* Message);
void recvAndDisplayTouches(MQTT_Message_t* Message);

#endif /* __DATA_TRANSFER_H__ */