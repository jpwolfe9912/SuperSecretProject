#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "super_secret_project.h"

void coords2string(uint16_t xCoords[100], uint16_t yCoords[100], size_t coords_size, char* output_str);
uint8_t stringToCoord(const char *str, uint16_t* xCoords, uint16_t* yCoords);
bool split_string(char* input_string, char* output_array[], uint8_t* output_size);
int num2digits(int num);

#endif