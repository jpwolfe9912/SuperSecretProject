#ifndef __DATA_TRANSFER_H__
#define __DATA_TRANSFER_H__

#include "super_secret_project.h"
#include "ft6206.h"
#include "ili9341.h"
#include "graphics.h"
#include "utilities.h"

#define COORDS_SIZE 100

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
    // uint8_t xPos_Data_l[COORDS_SIZE];
    // uint8_t xPos_Data_u[COORDS_SIZE];

    lwrb_t yPos;
    uint16_8 yPos_Data[COORDS_SIZE];
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