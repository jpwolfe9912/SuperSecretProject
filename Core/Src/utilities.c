#include "utilities.h"

void coords2string(uint16_t xCoords[100], uint16_t yCoords[100], size_t coords_size, char *output_str)
{
    // char *strBuff = calloc(_MQTT_TX_SIZE, sizeof(uint8_t)); // allocate and clear memory
    uint8_t coord_idx = 0;
    uint16_t xCoord = 0;
    uint16_t yCoord = 0;
    uint8_t str_size = 0;
    uint8_t str_idx = 1;
    *output_str = '\"';
    while (coord_idx < coords_size)
    {
        xCoord = xCoords[coord_idx];
        yCoord = yCoords[coord_idx];
        str_size = 0;
        str_idx = 1;
        char temp_str[16];

        if (coord_idx < (coords_size - 1))
            str_size = sprintf(temp_str, "%u;%u ", xCoord, yCoord);
        else
        {
            str_size = sprintf(temp_str, "%u;%u", xCoord, yCoord);
        }
        strcat(&output_str[str_idx], temp_str);
        str_idx += str_size;

        coord_idx++;
    }
    strcat(&output_str[str_idx], "\"");
}

uint8_t stringToCoord(const char *str, uint16_t *xCoords, uint16_t *yCoords)
{
    uint8_t numCoords = 0;
    char *str_c = strdup(str);

    char *pch = strtok(str_c, "; ");
    while (pch != NULL)
    {
        xCoords[numCoords] = atoi(pch);
        pch = strtok(NULL, "; ");
        yCoords[numCoords] = atoi(pch);
        pch = strtok(NULL, "; ");
        numCoords++;
    }
    return numCoords;
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

bool split_string(char *input_string, char *output_array[], uint8_t *output_size)
{
    char *token;
    int i = 0;

    // Get the first token
    token = strtok(input_string, ",");
    token = strtok(NULL, ","); // disregard header string

    // Loop through the input string and split it by delimiter
    while (token != NULL)
    {
        if (*token == '\"') // strip "" from string
        {
            if (*(token + 1) == '\"')
                token = NULL;
            else
                sscanf(token, "\"%[^\"]\"", token);
        }
        output_array[i] = token;
        i++;
        token = strtok(NULL, ",");
    }

    *output_size = i;

    // Return true if string is successfully split, false otherwise
    return i > 0;
}

/** @brief Constrains an input uint16_t between two values.
 *
 *  @param input Value to be constrained.
 *  @param minValue Lower threshold.
 *  @param maxValue Upper threshold.
 *  @return uint16_t Constrained value.
 */
uint16_t constrain16(uint16_t input, uint16_t minValue, uint16_t maxValue)
{
    if (input < minValue)
        return minValue;
    else if (input > maxValue)
        return maxValue;
    else
        return input;
}