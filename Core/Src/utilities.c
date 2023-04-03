/**
 * @file utilities.c
 * @author Jeremy Wolfe (jpwolfe@me.com)
 * @brief Miscellaneous useful functions
 */

/* Includes */
#include "utilities.h"

/**
 * @brief Convert x,y coordinates from ring buffer into a string
 * @param xCoords: Buffer of x coordinates
 * @param yCoords: Buffer of y coordinates
 * @param output_str: String to copy data into
 */
void coords2string(lwrb_t *xCoords, lwrb_t *yCoords, char *output_str)
{
    size_t coords_size = (lwrb_get_full(xCoords) + 1) / 2; // total number of coords
    uint8_t coord_idx = 0;                                 // current coord index to copy
    uint16_t xCoord = 0;                                   // temp x coord
    uint16_t yCoord = 0;                                   // temp y coord
    uint8_t str_size = 0;                                  // how large the coord pair string is
    uint8_t str_idx = 1;                                   // index of where we are in the string
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

/**
 * @brief Unpack a string of data and copy into a buffer of x,y coordinates
 * @param str: String to unpack
 * @param xCoords: Buffer of x coordinates
 * @param yCoords: Buffer of y coordinates
 * @return Number of coordinates read from the string
 */
uint8_t stringToCoord(const char *str, lwrb_t *xCoords, lwrb_t *yCoords)
{
    uint8_t numCoords = 0;
    uint16_t xCoord;
    uint16_t yCoord;
    char *str_c = my_strdup(str);

    char *pch = strtok(str_c, "; ");
    while (pch != NULL)
    {
        xCoord = atoi(pch);
        lwrb_write(xCoords, (void *)&xCoord, sizeof(uint16_t));
        pch = strtok(NULL, "; ");
        yCoord = atoi(pch);
        lwrb_write(yCoords, (void *)&yCoord, sizeof(uint16_t));
        pch = strtok(NULL, "; ");
        numCoords++;
    }
    free(str_c);
    return numCoords;
}

/**
 * @brief Split string by a `,` into an array of smaller strings
 * @param input_string: String to be parsed
 * @param output_array: Array of smaller strings
 * @param[out] output_size: Number of smaller strings
 * @return 
 */
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
    // free(output_array);
    *output_size = i;

    // Return true if string is successfully split, false otherwise
    return i > 0;
}

/** @brief Constrains an input uint16_t between two values
 *  @param input Value to be constrained.
 *  @param minValue Lower threshold
 *  @param maxValue Upper threshold
 *  @return Constrained value
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

char *my_strdup(char const *s) { 
    size_t len = strlen(s) + 1;
    char *ret = malloc(len);
    if (ret != NULL)
        strcpy(ret, s);
    return ret;
}