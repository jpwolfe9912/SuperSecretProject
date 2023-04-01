#ifndef __DATA_TRANSFER_H__
#define __DATA_TRANSFER_H__

#include "super_secret_project.h"
#include "ft6206.h"
#include "ili9341.h"
#include "graphics.h"
#include "utilities.h"
typedef struct
{
    uint16_t* xPos;
    uint16_t* yPos;
} Coords_t;

extern Coords_t TxCoords;
extern Coords_t RxCoords;

void initTxPacket(MQTT_Message_t* MQTT_TxPacket);
void initRxPacket(MQTT_Message_t* MQTT_RxPacket);
void initCoords(Coords_t* Coords);
void readAndSendTouches(MQTT_Message_t* Message);
void resetTouchIdx(void);
void recvAndDisplayTouches(MQTT_Message_t* Message);

#endif