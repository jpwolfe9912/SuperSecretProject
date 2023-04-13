/**
 * @file utilities.h
 * @author Jeremy Wolfe (jpwolfe@me.com)
 * @brief Miscellaneous useful functions
 */
#ifndef __UTILITIES_H__
#define __UTILITIES_H__

/* Includes */
#include "super_secret_project.h"

/* Function Prototypes */
void coords2string(lwrb_t *xCoords, lwrb_t *yCoords, char *output_str);
uint8_t stringToCoord(const char *str, lwrb_t *xCoords, lwrb_t *yCoords);
bool split_string(char* input_string, char* output_array[], uint8_t* output_size);
uint16_t constrain16(uint16_t input, uint16_t minValue, uint16_t maxValue);
char *my_strdup(char const *s);

#endif /* __UTILITIES_H__ */