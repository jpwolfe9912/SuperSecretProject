/**
 * @file        data_transfer.c
 * @author      Jeremy Wolfe (jpwolfe@me.com)
 * @brief       Used to handle data transfer from pixels on screen to AWS
 */

/* Includes */
#include "data_transfer.h"

/* Global Variables */
bool txDataReady = false;
uint32_t sendIdx = 0;

bool rxDataReady = false;
uint8_t numCoords = 0;

bool touchRecv = false;

uint32_t last_touch_time = 0;
uint32_t prev_touch_time = 0;
uint32_t elap_touch_time = 0;
uint32_t last_loop_time = 0;

Coords_t TxCoords;
Coords_t RxCoords;

/**
 * @brief Allocate memory for data transmission packet
 * @param MQTT_TxPacket 
 */
void initTxPacket(MQTT_Message_t *MQTT_TxPacket)
{
    MQTT_TxPacket->data = calloc(_MQTT_TX_SIZE, sizeof(uint8_t));
    strcpy(MQTT_TxPacket->topic, "topic/esp32atTx");
    MQTT_Unsubscribe(*MQTT_TxPacket);
}

/**
 * @brief Allocate memory for data reception packet
 * @param MQTT_RxPacket 
 */
void initRxPacket(MQTT_Message_t *MQTT_RxPacket)
{
    MQTT_RxPacket->data = calloc(_MQTT_RX_SIZE, sizeof(uint8_t));
    strcpy(MQTT_RxPacket->topic, "topic/esp32atRx");
    MQTT_Subscribe(*MQTT_RxPacket);
}

/**
 * @brief Initializes ring buffer for x and y coordinates
 * @param Coords 
 */
void initCoords(Coords_t *Coords)
{
    lwrb_init(&Coords->xPos, (void *)Coords->xPos_Data, sizeof(Coords->xPos_Data));
    lwrb_init(&Coords->yPos, (void *)Coords->yPos_Data, sizeof(Coords->yPos_Data));
}

/**
 * @brief Listens for and packs touches into a packet then sends over MQTT
 * @param Message 
 */
void readAndSendTouches(MQTT_Message_t *Message)
{
    if (touchData.touched) // check for touch
    {
        touchData.touched = false;
        // want to see how long it takes from one touch to the next
        // if the time from the previous touch to the loop is > 10ms then we know touched wasn't run
        prev_touch_time = last_touch_time;
        last_touch_time = millis();
        elap_touch_time = last_touch_time - prev_touch_time;

        if (ft6206ReadData())
        { // read data

            uint16_8 xPos;
            uint16_8 yPos;
            xPos.whole = touchData.xPos;
            yPos.whole = touchData.yPos;
            lwrb_write(&TxCoords.xPos, (void *)&xPos, sizeof(uint16_t));
            lwrb_write(&TxCoords.yPos, (void *)&yPos, sizeof(uint16_t));

            fillCircle(touchData.xPos, touchData.yPos, 2, RED);

            last_loop_time = millis();

            if (((last_loop_time - last_touch_time) > (4 * elap_touch_time)) ||
                (lwrb_get_free(&TxCoords.xPos) == 0))
            {
                touchRecv = false; // reset touched check

                memset(Message->data, '\0', strlen(Message->data));
                coords2string(&TxCoords.xPos, &TxCoords.yPos, Message->data);
                Message->length = strlen(Message->data);
                txDataReady = true;
            }

            if (SEND_RATE && txDataReady) // if 1s has passed and data is ready
            {
                MQTT_PublishRaw(*Message);
                SEND_RATE = false;
                txDataReady = false;
            }
        }
    }
}
// ! Consider deleting. Test without this variable
void resetTouchIdx(void)
{
    numCoords = 0;
}

/**
 * @brief Listens for a MQTT message, unpacks the data, and diplays the pixels on screen
 * @param Message 
 */
void recvAndDisplayTouches(MQTT_Message_t *Message)
{   //! Doesn't always draw the whole list of coordinates
    //! Theory is that the frame100Hz is being interrupted by something that changes the buffer
    //? Maybe, instead of doing a single read each time inside the 100Hz loop, do one big
    //? read into a temp variable that then sends the data one by one
    static size_t find_idx = 0;
    char *str = "+MQTTSUBRECV";
    if (frame1000Hz)
    {
        if (MQTT_ListenForMessage(Message, str, &find_idx))
        {
            serialWrite("Good receive\n");
            stringToCoord(Message->data, &RxCoords.xPos, &RxCoords.yPos);
            if(lwrb_get_full(&RxCoords.xPos) > 0)
                rxDataReady = true;
            else
                rxDataReady = false;
        }
        frame1000Hz = false;
    }
    if (frame100Hz)
    {
        if ((lwrb_get_full(&RxCoords.xPos)) && rxDataReady) //! rxDataReady overall bad syntax
        {
            uint16_t xCoord;
            uint16_t yCoord;
            lwrb_read(&RxCoords.xPos, (void*)&xCoord, sizeof(uint16_t));
            lwrb_read(&RxCoords.yPos, (void*)&yCoord, sizeof(uint16_t));
            fillCircle(xCoord, yCoord, 2, BLUE);
        }
        else
        {
            // touchDispIdx = 0;
            rxDataReady = false;
        }
        frame100Hz = false;
    }
}
