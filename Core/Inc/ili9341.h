#ifndef __ILI9341_H__
#define __ILI9341_H__

#include "super_secret_project.h"
#include "fonts.h"


#define ILI9341_COMMAND (GPIOA->BSRR |= GPIO_BSRR_BR3)
#define ILI9341_DATA (GPIOA->BSRR |= GPIO_BSRR_BS3)

#define ILI9341_CS_RESET (GPIOA->BSRR |= GPIO_BSRR_BR4)
#define ILI9341_CS_SET (GPIOA->BSRR |= GPIO_BSRR_BS4)

#define ILI9341_SCREEN_HEIGHT 240
#define ILI9341_SCREEN_WIDTH 320

#define BURST_MAX_SIZE 30000

#define BLACK 0x0000
#define NAVY 0x000F
#define DARKGREEN 0x03E0
#define DARKCYAN 0x03EF
#define MAROON 0x7800
#define PURPLE 0x780F
#define OLIVE 0x7BE0
#define LIGHTGREY 0xC618
#define DARKGREY 0x7BEF
#define BLUE 0x001F
#define GREEN 0x07E0
#define CYAN 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xFD20
#define GREENYELLOW 0xAFE5
#define PINK 0xF81F

#define SCREEN_VERTICAL_1 0
#define SCREEN_HORIZONTAL_1 1
#define SCREEN_VERTICAL_2 2
#define SCREEN_HORIZONTAL_2 3

/* Level 1 Commands */
#define ILI9341_NOP 0x00
#define ILI9341_SOFTWARE_RESET 0x01
#define ILI9341_READ_DISP_ID 0x04
#define ILI9341_READ_DISP_STATUS 0x09
#define ILI9341_READ_DISP_POWER_MODE 0x0A
#define ILI9341_READ_DISP_MADCTL 0x0B
#define ILI9341_READ_DISP_PIXEL_FORMAT 0x0C
#define ILI9341_READ_DISP_SIGNAL_MODE 0x0E
#define ILI9341_READ_DISP_SELF_DIAG 0x0F
#define ILI9341_ENTER_SLEEP_MODE 0x10
#define ILI9341_SLEEP_OUT 0x11
#define ILI9341_PARTIAL_MODE_ON 0x12
#define ILI9341_NORM_DISP_MODE_ON 0x13
#define ILI9341_DISP_INV_OFF 0x20
#define ILI9341_DISP_INV_ON 0x21
#define ILI9341_GAMMA_SET 0x26
#define ILI9341_DISP_OFF 0x28
#define ILI9341_DISP_ON 0x29
#define ILI9341_COL_ADDR_SET 0x2A
#define ILI9341_PAGE_ADDR_SET 0x2B
#define ILI9341_MEM_WRITE 0x2C
#define ILI9341_COLOR_SET 0x2D
#define ILI9341_MEM_READ 0x2E
#define ILI9341_PARTIAL_AREA 0x30
#define ILI9341_VERT_SCROLL_DEF 0x33
#define ILI9341_TEAR_EFF_LINE_OFF 0x34
#define ILI9341_TEAR_EFF_LINE_ON 0x35
#define ILI9341_MEM_ACCESS_CTRL 0x36
#define ILI9341_VERT_SCROLL_START_ADDR 0x37
#define ILI9341_IDLE_MODE_OFF 0x38
#define ILI9341_IDLE_MODE_ON 0x39
#define ILI9341_PIXEL_FORMAT_SET 0x3A
#define ILI9341_WRITE_MEM_CONT 0x3C
#define ILI9341_READ_MEM_CONT 0x3E
#define ILI9341_SET_TEAR_SCANLINE 0x44
#define ILI9341_GET_SCANLINE 0x45
#define ILI9341_WRITE_DISP_BRIGHTNESS 0x51
#define ILI9341_READ_DISP_BRIGHTNESS 0x52
#define ILI9341_WRITE_CTRL_DISP 0x53
#define ILI9341_READ_CTRL_DISP 0x54
#define ILI9341_WRITE_ADAPT_BRTN_CTRL 0x55
#define ILI9341_READ_ADAPT_BRTN_CTRL 0x56
#define ILI9341_WRITE_CABC_MIN_BRTN 0x5E
#define ILI9341_READ_CABC_MIN_BRTN 0x5F
#define ILI9341_READ_ID1 0xDA
#define ILI9341_READ_ID2 0xDB
#define ILI9341_READ_ID3 0xDC

/* Level 2 Commands */
#define ILI9341_RGB_INTFC_SIGN_CTRL 0xB0
#define ILI9341_FRAME_CTRL_NORM 0xB1
#define ILI9341_FRAME_CTRL_IDLE 0xB2
#define ILI9341_FRAME_CTRL_PARTIAL 0xB3
#define ILI9341_DISP_INV_CTRL 0xB4
#define ILI9341_BLANK_PORCH_CTRL 0xB5
#define ILI9341_DISP_FUNC_CTRL 0xB6
#define ILI9341_ENTRY_MODE_SET 0xB7
#define ILI9341_BACKLIGHT_CTRL1 0xB8
#define ILI9341_BACKLIGHT_CTRL2 0xB9
#define ILI9341_BACKLIGHT_CTRL3 0xBA
#define ILI9341_BACKLIGHT_CTRL4 0xBB
#define ILI9341_BACKLIGHT_CTRL5 0xBC
#define ILI9341_BACKLIGHT_CTRL7 0xBE
#define ILI9341_BACKLIGHT_CTRL8 0xBF
#define ILI9341_POWER_CTRL1 0xC0
#define ILI9341_POWER_CTRL2 0xC1
#define ILI9341_VCOM_CTRL1 0xC5
#define ILI9341_VCOM_CTRL2 0xC7
#define ILI9341_NV_MEM_WRITE 0xD0
#define ILI9341_NV_MEM_PROTECT_KEY 0xD1
#define ILI9341_NV_MEM_STATUS_READ 0xD2
#define ILI9341_READ_ID4 0xD3
#define ILI9341_POS_GAMMA_CORRECTION 0xE0
#define ILI9341_NEG_GAMMA_CORRECTION 0xE1
#define ILI9341_DIG_GAMMA_CTRL1 0xE2
#define ILI9341_DIG_GAMMA_CTRL2 0xE3
#define ILI9341_INTERFACE_CTRL 0xF6

/* Extended Commands */
#define ILI9341_POWER_CTRL_A 0xCB
#define ILI9341_POWER_CTRL_B 0xCF
#define ILI9341_DRIVER_TIM_CTRL_A 0xE8
#define ILI9341_DRIVER_TIM_CTRL_B 0xEA
#define ILI9341_POWER_ON_SEQ_CTRL 0xED
#define ILI9341_ENABLE_3G 0xF2
#define ILI9341_PUMP_RATIO_CTRL 0xF7

/* Global Variables ------------------------------------------------------------------*/
extern volatile uint16_t _lcd_height;
extern volatile uint16_t _lcd_width;

/* Function Prototypes */
void ili9341WriteCommand(uint8_t command);
void ili9341WriteData(uint8_t *pData, size_t size);
void ili9341Read8(uint8_t reg, uint8_t *pData);
void ili9341Reset(void);
void ili9341SetAddress(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ili9341SetRotation(uint8_t rotation);
void ili9341Init(void);
void ili9341FillScreen(uint16_t color);
void ili9341DrawColor(uint16_t color);
void ili9341DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ili9341DrawColorBurst(uint16_t color, uint32_t size);

void ili9341DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void ili9341DrawHorizontalLine(uint16_t x, uint16_t y, uint16_t width, uint16_t color);
void ili9341DrawVerticalLine(uint16_t x, uint16_t y, uint16_t height, uint16_t color);

void ili9341DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t image[240][320]);
void ili9341WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor);
void ili9341WriteCharFromUART(uint16_t startX, uint16_t startY, FontDef font, uint16_t color, uint16_t bgcolor);
void ili9341InvertColors(bool invert);

#endif