#include "utilities.h"

void coords2string(lwrb_t *xCoords, lwrb_t *yCoords, char *output_str)
{
    size_t coords_size = (lwrb_get_full(xCoords) + 1) / 2; // total number of coords
    uint8_t coord_idx = 0;                       // current coord index to copy
    uint16_t xCoord = 0;                         // temp x coord
    uint16_t yCoord = 0;                         // temp y coord
    uint8_t str_size = 0;                        // how large the coord pair string is
    uint8_t str_idx = 1;                         // index of where we are in the string
    *output_str = '\"';
    while (coord_idx < coords_size)
    {
        lwrb_read(xCoords, (void *)&xCoord, sizeof(uint16_t));
        lwrb_read(yCoords, (void *)&yCoord, sizeof(uint16_t));
        // xCoord = xCoords[coord_idx];
        // yCoord = yCoords[coord_idx];
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

uint8_t stringToCoord(const char *str, lwrb_t *xCoords, lwrb_t *yCoords)
{
    uint8_t numCoords = 0;
    uint16_t xCoord;
    uint16_t yCoord;
    char *str_c = strdup(str);

    char *pch = strtok(str_c, "; ");
    while (pch != NULL)
    {
        xCoord = atoi(pch);
        lwrb_write(xCoords, (void*)&xCoord, sizeof(uint16_t));
        pch = strtok(NULL, "; ");
        yCoord = atoi(pch);
        lwrb_write(yCoords, (void*)&yCoord, sizeof(uint16_t));
        pch = strtok(NULL, "; ");
        numCoords++;
    }
    free(str_c);
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