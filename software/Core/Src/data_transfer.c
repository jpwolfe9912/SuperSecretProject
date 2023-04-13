/**
 * @file        data_transfer.c
 * @author      Jeremy Wolfe (jpwolfe@me.com)
 * @brief       Used to handle data transfer from pixels on screen to AWS
 */

/* Includes */
#include "data_transfer.h"

/* Global Variables */
uint16_t buffTempCoordX[100];
uint16_t buffTempCoordY[100];
uint8_t buffTempIdx = 0;

uint8_t rbBytesAvailable = 0;
uint8_t rbCoordsAvailable = 0;

bool txDataReady = false;
bool smallTouch = false;

uint32_t touch_time = 0;
uint32_t prev_touch_time = 0;
uint32_t elap_touch_time = 0;
uint32_t last_loop_time = 0;

bool lcdReset = false;

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
    lwrb_init(&Coords->xPos, (void *)Coords->xPos_Data, COORDS_SIZE);
    lwrb_init(&Coords->yPos, (void *)Coords->yPos_Data, COORDS_SIZE);
}

/**
 * @brief Listens for and packs touches into a packet then sends over MQTT
 * @param Message
 */
void readAndSendTouches(MQTT_Message_t *Message)
{
    last_loop_time = millis();
    if (touchData.touched) // check for touch
    {
        touchData.touched = false;
        // want to see how long it takes from one touch to the next
        // if the time from the previous touch to the loop is > 10ms then we know touched wasn't run
        prev_touch_time = touch_time;
        touch_time = millis();
        elap_touch_time = touch_time - prev_touch_time;

        if (ft6206ReadData())
        { // read data

            uint16_8 xPos;
            uint16_8 yPos;
            xPos.whole = touchData.xPos;
            yPos.whole = touchData.yPos;
            lwrb_write(&TxCoords.xPos, (void *)&xPos, sizeof(uint16_t));
            lwrb_write(&TxCoords.yPos, (void *)&yPos, sizeof(uint16_t));

            fillCircle(touchData.xPos, touchData.yPos, 2, RED);
            smallTouch = true;
        }
    }
    if ((((last_loop_time - touch_time) > (4 * elap_touch_time)) || // if #touches is less than 100
        (lwrb_get_free(&TxCoords.xPos) == 0)) &&                    // if touches fills up buffer
        smallTouch) // can only go if there was a touch previously
    {
        memset(Message->data, '\0', strlen(Message->data)); //? Try without this line
        coords2string(&TxCoords.xPos, &TxCoords.yPos, Message->data);
        Message->length = strlen(Message->data);
        txDataReady = true;
        smallTouch = false;
    }

    if (SEND_RATE && txDataReady) // if 1s has passed and data is ready
    {
        MQTT_PublishRaw(*Message);
        SEND_RATE = false;
        txDataReady = false;
    }
}

/**
 * @brief Listens for a MQTT message, unpacks the data, and diplays the pixels on screen
 * @param Message
 */
void recvAndDisplayTouches(MQTT_Message_t *Message)
{
    char *str = "+MQTTSUBRECV";
    if (READ_RATE)
    {
        if (MQTT_ListenForMessage(Message, str))
        {
            serialWrite("Good receive\n");
            if (Message->data[0] == '*')
            {
                lcdReset = true;
            }
            stringToCoord(Message->data, &RxCoords.xPos, &RxCoords.yPos);

            if (buffTempIdx == 0)
            {
                rbBytesAvailable = lwrb_get_full(&RxCoords.xPos);
                rbCoordsAvailable = rbBytesAvailable / 2;
                lwrb_read(&RxCoords.xPos, (void *)buffTempCoordX, rbBytesAvailable);
                lwrb_read(&RxCoords.yPos, (void *)buffTempCoordY, rbBytesAvailable);
            }
        }

        READ_RATE = false;
    }
    if (DRAW_RATE)
    {
        if (rbCoordsAvailable > 0) // buffer has been read
        {
            fillCircle(buffTempCoordX[buffTempIdx], buffTempCoordY[buffTempIdx], 2, BLUE);
            if (buffTempIdx < rbCoordsAvailable)
                buffTempIdx++; // check index during runtime
            else
            {
                buffTempIdx = 0;
                rbCoordsAvailable = 0;
            }
        }
        DRAW_RATE = false;
    }
}

//! Problem: Extra dots being drawn at the end
//? Turns out the the receiver is correct. The transmitter is inserting some weird data

//! Problem: Transmitter isn't sending short touches less than a second immediately after