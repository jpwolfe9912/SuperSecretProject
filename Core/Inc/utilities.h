#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "utilities.h"
#include "super_secret_project.h"
#include "esp32.h"
#include "esp32Config.h"
#include "ft6206.h"
#include "ili9341.h"

void coords2string(TouchData_t txTouchData[100], size_t coords_size, char* output_str);
int num2digits(int num);

#endif