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

bool txDataReady = false;

uint32_t last_touch_time = 0;
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

/**
 * @brief Listens for a MQTT message, unpacks the data, and diplays the pixels on screen
 * @param Message
 */
void recvAndDisplayTouches(MQTT_Message_t *Message)
{ //! Doesn't always draw the whole list of coordinates
    //! Theory is that the frame100Hz is being interrupted by something that changes the buffer
    //? Maybe, instead of doing a single read each time inside the 100Hz loop, do one big
    //? read into a temp variable that then sends the data one by one
    //? Or increase buffer size so the write pointer doesn't overtake read
    static size_t find_idx = 0;
    char *str = "+MQTTSUBRECV";
    if (READ_RATE)
    {
        if (MQTT_ListenForMessage(Message, str, &find_idx))
        {
            serialWrite("Good receive\n");
            if(Message->data[0] == '*'){
                lcdReset = true;
            }
            stringToCoord(Message->data, &RxCoords.xPos, &RxCoords.yPos);
        }
        if (buffTempIdx == 0)
        {
            rbBytesAvailable = lwrb_get_full(&RxCoords.xPos);
            lwrb_read(&RxCoords.xPos, (void *)buffTempCoordX, rbBytesAvailable);
            lwrb_read(&RxCoords.yPos, (void *)buffTempCoordY, rbBytesAvailable);
        }
        READ_RATE = false;
    }
    /* Problem: The lwrb_read() below happens over the course of a second.
                If at any point during the read, more data is written,
                that will mess with the lwrb_get_full() as the write buffer
                will change and cause undefined behavior to happen
                (probably cutting it short)

                Brainstorming:
                - We don't want stringToCoord to be called until the buffer is empty
                and all data has been read and drawn to the screen
                - However, if we have to wait for the data to be read over the course
                of a second before writing more, we might miss some data
                - Could have a temp variable that we put the contents of the buffer
                after all the coordinates have been written.
                    - This way, we free the buffer for more writes without worrying
                    about the data being overwritten before it can be read
                    - Something about this feels redundant though because we write the
                    values to the rb and then immediately read them
                        - Why have the rb at all?
                        - I still think rb is a good way to control the flow of data

    */
    if (DRAW_RATE)
    {
        if ((bool)rbBytesAvailable) // buffer has been read
        {
            fillCircle(buffTempCoordX[buffTempIdx], buffTempCoordY[buffTempIdx], 2, BLUE);
            if(buffTempIdx < rbBytesAvailable)
                buffTempIdx++;          // check index during runtime
            else{
                buffTempIdx = 0;
                rbBytesAvailable = 0;
            }
        }
        DRAW_RATE = false;
    }
}