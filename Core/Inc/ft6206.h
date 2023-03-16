#ifndef __FT6206_H__
#define __FT6206_H__

#include "ft6206.h"
#include "super_secret_project.h"

#define FT62XX_TOUCH_IRQ EXTI9_5_IRQHandler

// #define FT62XX_DEBUG

#define FT62XX_ADDR 0x38           //!< I2C address
#define FT62XX_FT_ID 0xA8          //!< FocalTech's panel ID
#define FT62XX_REG_NUMTOUCHES 0x02 //!< Number of touch points

#define FT62XX_NUM_X 0x33 //!< Touch X position
#define FT62XX_NUM_Y 0x34 //!< Touch Y position

#define FT62XX_REG_MODE 0x00        //!< Device mode, either WORKING or FACTORY
#define FT62XX_REG_CALIBRATE 0x02   //!< Calibrate mode
#define FT62XX_REG_WORKMODE 0x00    //!< Work mode
#define FT62XX_REG_FACTORYMODE 0x40 //!< Factory mode
#define FT62XX_REG_THRESHHOLD 0x80  //!< Threshold for touch detection
#define FT62XX_REG_POINTRATE 0x88   //!< Point rate
#define FT62XX_REG_FIRMVERS 0xA6    //!< Firmware version
#define FT62XX_REG_CHIPID 0xA3      //!< Chip selecting
#define FT62XX_REG_VENDID 0xA8      //!< FocalTech's panel ID
#define FT62XX_REG_G_MODE 0xA4

#define FT62XX_VENDID 0x11  //!< FocalTech's panel ID
#define FT6206_CHIPID 0x06  //!< Chip selecting
#define FT6236_CHIPID 0x36  //!< Chip selecting
#define FT6236U_CHIPID 0x64 //!< Chip selecting

// calibrated for Adafruit 2.8" ctp screen
#define FT62XX_DEFAULT_THRESHOLD 128 //!< Default threshold for touch detection

typedef struct
{
    bool touched;
    uint8_t touches;
    uint8_t touchId;
    uint16_t xPos;
    uint16_t yPos;
} TouchData_t;

extern TouchData_t touchData;
bool ft6206Init(void);
void ft6206ReadData(void);

#endif