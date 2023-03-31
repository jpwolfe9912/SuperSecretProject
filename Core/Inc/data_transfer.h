#ifndef __DATA_TRANSFER_H__
#define __DATA_TRANSFER_H__

#include "super_secret_project.h"
#include "ft6206.h"
#include "ili9341.h"
#include "graphics.h"
#include "utilities.h"
typedef struct
{
    uint16_t xPos[100];
    uint16_t yPos[100];
} Coords_t;

void initTxPacket(MQTT_Message_t* MQTT_TxPacket);
void initRxPacket(MQTT_Message_t* MQTT_RxPacket);
void readAndSendTouches(MQTT_Message_t* Message);
void recvAndDisplayTouches(MQTT_Message_t* Message);

#endif