#include "data_transfer.h"

uint8_t txTouchIdx = 0;
bool txDataReady = false;
uint32_t sendIdx = 0;

bool rxDataReady = false;
uint8_t touchDispIdx = 0;
uint8_t rxTouchIdx = 0;

bool touchRecv = false;

uint32_t last_touch_time = 0;
uint32_t prev_touch_time = 0;
uint32_t elap_touch_time = 0;
uint32_t last_loop_time = 0;

Coords_t TxCoords;
Coords_t RxCoords;

void initTxPacket(MQTT_Message_t *MQTT_TxPacket)
{
    MQTT_TxPacket->data = calloc(_MQTT_TX_SIZE, sizeof(uint8_t));
    strcpy(MQTT_TxPacket->topic, "topic/esp32atTx");
    MQTT_Unsubscribe(*MQTT_TxPacket);
}

void initRxPacket(MQTT_Message_t *MQTT_RxPacket)
{
    MQTT_RxPacket->data = calloc(_MQTT_RX_SIZE, sizeof(uint8_t));
    strcpy(MQTT_RxPacket->topic, "topic/esp32atRx");
    MQTT_Subscribe(*MQTT_RxPacket);
}

void initCoords(Coords_t *Coords)
{
    Coords->xPos = calloc(100, sizeof(uint16_t));
    Coords->yPos = calloc(100, sizeof(uint16_t));
}

void readAndSendTouches(MQTT_Message_t *Message)
{
    if (touchData.touched) // check for touch
    {
        // want to see how long it takes from one touch to the next
        // if the time from the previous touch to the loop is > 10ms then we know touched wasn't run
        prev_touch_time = last_touch_time;
        last_touch_time = millis();
        elap_touch_time = last_touch_time - prev_touch_time;

        // touchData.touched = false;
        // touchRecv = true;

        if (ft6206ReadData())
        { // read data

            TxCoords.xPos[txTouchIdx] = touchData.xPos; // set x data to correct array index
            TxCoords.yPos[txTouchIdx] = touchData.yPos; // set x data to correct array index

            if (txTouchIdx == 0)
                fillCircle(touchData.xPos, touchData.yPos, 2, RED);
            else if ((TxCoords.xPos[txTouchIdx] != TxCoords.xPos[txTouchIdx - 1]) ||
                     (TxCoords.yPos[txTouchIdx] != TxCoords.yPos[txTouchIdx - 1]))
                fillCircle(touchData.xPos, touchData.yPos, 2, RED);

            txTouchIdx++;
            // }
            /* Check if Data is collected and put into string */
            // if (touchRecv)
            // {
            last_loop_time = millis();

            if (((last_loop_time - last_touch_time) > (4 * elap_touch_time)) ||
                (txTouchIdx >= 100))
            {
                touchRecv = false; // reset touched check

                coords2string(TxCoords.xPos, TxCoords.yPos, txTouchIdx, Message->data);
                Message->length = strlen(Message->data);
                txTouchIdx = 0;
                txDataReady = true;
                TxCoords.xPos = calloc(100, sizeof(uint16_t));
                TxCoords.yPos = calloc(100, sizeof(uint16_t));
            }

            if (SEND_RATE && txDataReady) // if 1s has passed and data is ready
            {
                MQTT_PublishRaw(*Message);
                memset(Message->data, '\0', strlen(Message->data));
                SEND_RATE = false;
                txDataReady = false;
            }
        }
    }
}

void resetTouchIdx(void)
{
    rxTouchIdx = 0;
}

void recvAndDisplayTouches(MQTT_Message_t *Message)
{
    static size_t find_idx = 0;
    char *str = "+MQTTSUBRECV";
    if (frame1000Hz)
    {
        if (MQTT_ListenForMessage(Message, str, &find_idx))
        {
            serialWrite("Good receive\n");
            rxTouchIdx = stringToCoord(Message->data, RxCoords.xPos, RxCoords.yPos);
            if (rxTouchIdx > 0)
            {
                rxDataReady = true;
            }
            else
                rxDataReady = false;
        }
        lwrb_reset(&Buffs.RxBuffer);
        frame1000Hz = false;
    }
    if (frame100Hz)
    {
        if ((touchDispIdx < rxTouchIdx) && rxDataReady)
        {
            fillCircle(RxCoords.xPos[touchDispIdx], RxCoords.yPos[touchDispIdx], 2, BLUE);
            touchDispIdx++;
        }
        else
        {
            touchDispIdx = 0;
            rxDataReady = false;
        }
        frame100Hz = false;
    }
}
