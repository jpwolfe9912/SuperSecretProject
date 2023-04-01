#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "super_secret_project.h"

void coords2string(lwrb_t *xCoords, lwrb_t *yCoords, char *output_str);
uint8_t stringToCoord(const char *str, lwrb_t *xCoords, lwrb_t *yCoords);
bool split_string(char* input_string, char* output_array[], uint8_t* output_size);
int num2digits(int num);
uint16_t constrain16(uint16_t input, uint16_t minValue, uint16_t maxValue);

#endif