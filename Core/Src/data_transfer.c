#include "data_transfer.h"

uint8_t txTouchIdx = 0;
bool txDataReady = false;
uint32_t sendIdx = 0;

uint8_t touchCounter = 0;
uint8_t loopCounter = 0;
bool touchRecv = false;

uint32_t last_touch_time = 0;
uint32_t prev_touch_time = 0;
uint32_t elap_touch_time = 0;
uint32_t last_loop_time = 0;

TouchData_t txTouchData[100];

void initTxTouches(MQTT_Message_t *MQTT_TxPacket)
{
    MQTT_TxPacket->data = malloc(_MQTT_SIZE);
    strcpy(MQTT_TxPacket->topic, "topic/esp32atTx");
    strcpy(MQTT_TxPacket->data, "Hello from STM32L432");
    MQTT_Unsubscribe(*MQTT_TxPacket);
}

void readAndSendTouches(MQTT_Message_t *Message)
{
    if (touchData.touched)
    {
        // want to see how long it takes from one touch to the next
        // if the time from the previous touch to the loop is > 10ms then we know touched wasn't run
        prev_touch_time = last_touch_time;
        last_touch_time = millis();
        elap_touch_time = last_touch_time - prev_touch_time;

        touchData.touched = false;
        touchRecv = true;

        ft6206ReadData(); // read data

        txTouchData[txTouchIdx].xPos = touchData.xPos; // set x data to correct array index
        txTouchData[txTouchIdx].yPos = touchData.yPos; // set x data to correct array index

        if (txTouchIdx == 0)
            ili9341DrawPixel(touchData.xPos, touchData.yPos, RED); // draw pixel if first of index
        else if ((txTouchData[txTouchIdx].xPos != txTouchData[txTouchIdx - 1].xPos) ||
                 (txTouchData[txTouchIdx].yPos != txTouchData[txTouchIdx - 1].yPos))
            ili9341DrawPixel(touchData.xPos, touchData.yPos, RED); // draw pixel if not the same as previous

        txTouchIdx++;
    }
    /* Check if Data is collected and put into string */
    if (touchRecv)
    {
        last_loop_time = millis();

        if (((last_loop_time - last_touch_time) > (4 * elap_touch_time)) ||
            (txTouchIdx >= 100))
        {
            loopCounter = touchCounter; // reset counters to be equal
            touchRecv = false;          // reset touched check

            Message->data = coords2string(txTouchData, txTouchIdx);
            Message->length = strlen(Message->data);
            txTouchIdx = 0;
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