#include "utilities.h"

char *coords2string(TouchData_t txTouchData[100], size_t coords_size)
{
    char *strBuff = calloc(_MQTT_SIZE, sizeof(uint8_t)); // allocate and clear memory
    uint8_t coord_idx = 0;
    while (coord_idx < coords_size)
    {
        uint16_t xCoord = txTouchData[coord_idx].xPos;
        uint16_t yCoord = txTouchData[coord_idx].yPos;
        uint8_t str_size = 0;
        uint8_t str_idx = 0;
        char temp_str[10];

        if (coord_idx < (coords_size - 1))
            str_size = sprintf(temp_str, "%u;%u ", xCoord, yCoord);
        else
        {
            str_size = sprintf(temp_str, "%u;%u", xCoord, yCoord);
        }
        strcat(&strBuff[str_idx], temp_str);
        str_idx += str_size;

        coord_idx++;
    }
    return strBuff;
}

int num2digits(int num)
{
    int digits = 1;
    int divisor = 10;

    while (1)
    {
        if ((num / divisor) == 0)
            return digits;
        else
        {
            divisor *= 10;
            digits++;
        }
    }
}