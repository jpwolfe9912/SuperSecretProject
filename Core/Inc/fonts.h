/**
 * @file fonts.h
 * @author Jeremy Wolfe (jpwolfe@me.com)
 * @brief Defines the pixel array for each font
 */
#ifndef __FONTS_H__
#define __FONTS_H__

#include <stdint.h>

/* Typedef Structs */
typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;

/* Global Extern Variables */
extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

#endif // __FONTS_H__