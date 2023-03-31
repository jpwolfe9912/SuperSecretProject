#ifndef __DATA_TRANSFER_H__
#define __DATA_TRANSFER_H__

#include "data_transfer.h"
#include "super_secret_project.h"
#include "esp32.h"
#include "esp32Config.h"
#include "ft6206.h"
#include "ili9341.h"
#include "utilities.h"
#include "graphics.h"

void initTxPacket(MQTT_Message_t *MQTT_TxPacket);
void initRxPacket(MQTT_Message_t *MQTT_RxPacket);
void readAndSendTouches(MQTT_Message_t *Message);

#endif