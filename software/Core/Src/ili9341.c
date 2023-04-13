/**
 * @file ili9341.c
 * @author Jeremy Wolfe (jpwolfe@me.com)
 * @brief Functions to write to the ILI9341 touchscreen
 */
#include "ili9341.h"

/* Global Variables */
volatile uint16_t _lcd_height = ILI9341_SCREEN_HEIGHT;
volatile uint16_t _lcd_width = ILI9341_SCREEN_WIDTH;

uint8_t rxBuf;
uint8_t txBuf;

/* Static Function Prototypes */
static void ili9341WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);

/**
 * @brief Send command (char) to LCD
 * @param command: Character to send
 */
void ili9341WriteCommand(uint8_t command)
{
    static uint8_t dummy __attribute__((unused));

    ILI9341_COMMAND;
    ILI9341_CS_RESET;
    spi1Transfer(&command, &dummy, 1);
    ILI9341_CS_SET;
}

/**
 * @brief Send Data (char) to LCD
 * @param pData: Command to be sent
 * @param size: Size of the data
 */
void ili9341WriteData(uint8_t *pData, size_t size)
{
    static uint8_t dummy __attribute__((unused));

    ILI9341_DATA;
    ILI9341_CS_RESET;
    spi1Transfer(pData, &dummy, size);
    ILI9341_CS_SET;
}

/**
 * @brief Read 8 Bits of Data from LCD
 * @param reg: Register to read from
 * @param pData: Variable to read data into
 * @attention Not recommended to use this function without more testing
 */
void ili9341Read8(uint8_t reg, uint8_t *pData)
{
    ILI9341_COMMAND;
    ILI9341_CS_RESET;
    spi1Transfer(&reg, &rxBuf, 1);
    ILI9341_DATA;
    spi1Transfer(&txBuf, pData, 1);
    ILI9341_CS_SET;
}

/**
 * @brief Set Address - Location block - to draw into
 * @param x0: Starting x coordinate
 * @param y0: Starting y coordinate 
 * @param x1: Ending x coordinate
 * @param y1: Ending y coordinate
 */
void ili9341SetAddress(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // column address set
    ili9341WriteCommand(ILI9341_COL_ADDR_SET); // CASET
    {
        uint8_t data[] = {(x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF};
        ili9341WriteData(data, sizeof(data));
    }

    // row address set
    ili9341WriteCommand(ILI9341_PAGE_ADDR_SET); // RASET
    {
        uint8_t data[] = {(y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF};
        ili9341WriteData(data, sizeof(data));
    }

    // write to RAM
    ili9341WriteCommand(ILI9341_MEM_WRITE); // RAMWR
}

/**
 * @brief Reset ILI9341 hardware
 * @param  
 */
void ili9341Reset(void)
{
    GPIOB->BSRR |= GPIO_BSRR_BR1;
    delay(200);
    ILI9341_CS_RESET;
    delay(200);
    GPIOB->BSRR |= GPIO_BSRR_BS1;
}

/**
 * @brief Set rotation of the screen - changes x0 and y0
 * @param rotation How much to rotate
 *                  SCREEN_VERTICAL_1 = 0
 *                  SCREEN_HORIZONTAL_1 = 1
 *                  SCREEN_VERTICAL_2 = 2
 *                  SCREEN_HORIZONTAL_2 = 3
 */
void ili9341SetRotation(ScreenRotation_e rotation)
{
    uint8_t rot_code;
    uint8_t screen_rotation = rotation;

    ili9341WriteCommand(ILI9341_MEM_ACCESS_CTRL);
    delay(1);

    switch (screen_rotation)
    {
    case SCREEN_VERTICAL_1:
        rot_code = (0x40 | 0x08);
        ili9341WriteData(&rot_code, 1);
        _lcd_width = 240;
        _lcd_height = 320;
        break;
    case SCREEN_HORIZONTAL_1:
        rot_code = (0x20 | 0x08);
        ili9341WriteData(&rot_code, 1);
        _lcd_width = 320;
        _lcd_height = 240;
        break;
    case SCREEN_VERTICAL_2:
        rot_code = (0x80 | 0x08);
        ili9341WriteData(&rot_code, 1);
        _lcd_width = 240;
        _lcd_height = 320;
        break;
    case SCREEN_HORIZONTAL_2:
        rot_code = (0x40 | 0x80 | 0x20 | 0x08);
        ili9341WriteData(&rot_code, 1);
        _lcd_width = 320;
        _lcd_height = 240;
        break;
    default:
        // EXIT IF SCREEN rotation NOT VALID!
        break;
    }
}

/**
 * @brief Initialize LCD display
 * @param  
 */
void ili9341Init(void)
{
    ili9341Reset();
    ILI9341_CS_SET;

    // SOFTWARE RESET
    ili9341WriteCommand(ILI9341_SOFTWARE_RESET);
    delay(1000);

    // POWER CONTROL A
    ili9341WriteCommand(ILI9341_POWER_CTRL_A);
    {
        uint8_t data[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
        ili9341WriteData(data, sizeof(data));
    }

    // POWER CONTROL B
    ili9341WriteCommand(ILI9341_POWER_CTRL_B);
    {
        uint8_t data[] = {0x00, 0xC1, 0x30};
        ili9341WriteData(data, sizeof(data));
    }

    // DRIVER TIMING CONTROL A
    ili9341WriteCommand(ILI9341_DRIVER_TIM_CTRL_A);
    {
        uint8_t data[] = {0x85, 0x00, 0x78};
        ili9341WriteData(data, sizeof(data));
    }

    // DRIVER TIMING CONTROL B
    ili9341WriteCommand(ILI9341_DRIVER_TIM_CTRL_B);
    {
        uint8_t data[] = {0x00, 0x00};
        ili9341WriteData(data, sizeof(data));
    }

    // POWER ON SEQUENCE CONTROL
    ili9341WriteCommand(ILI9341_POWER_ON_SEQ_CTRL);
    {
        uint8_t data[] = {0x64, 0x03, 0x12, 0x81};
        ili9341WriteData(data, sizeof(data));
    }

    // PUMP RATIO CONTROL
    ili9341WriteCommand(ILI9341_PUMP_RATIO_CTRL);
    {
        uint8_t data[] = {0x20};
        ili9341WriteData(data, sizeof(data));
    }

    // POWER CONTROL,VRH[5:0]
    ili9341WriteCommand(ILI9341_POWER_CTRL1);
    {
        uint8_t data[] = {0x23};
        ili9341WriteData(data, sizeof(data));
    }

    // POWER CONTROL,SAP[2:0];BT[3:0]
    ili9341WriteCommand(ILI9341_POWER_CTRL2);
    {
        uint8_t data[] = {0x10};
        ili9341WriteData(data, sizeof(data));
    }

    // VCM CONTROL
    ili9341WriteCommand(ILI9341_VCOM_CTRL1);
    {
        uint8_t data[] = {0x3E, 0x28};
        ili9341WriteData(data, sizeof(data));
    }

    // VCM CONTROL 2
    ili9341WriteCommand(ILI9341_VCOM_CTRL2);
    {
        uint8_t data[] = {0x86};
        ili9341WriteData(data, sizeof(data));
    }

    // MEMORY ACCESS CONTROL
    ili9341WriteCommand(ILI9341_MEM_ACCESS_CTRL);
    {
        uint8_t data[] = {0x48};
        ili9341WriteData(data, sizeof(data));
    }

    // PIXEL FORMAT
    ili9341WriteCommand(ILI9341_PIXEL_FORMAT_SET);
    {
        uint8_t data[] = {0x55};
        ili9341WriteData(data, sizeof(data));
    }

    // FRAME RATIO CONTROL, STANDARD RGB COLOR
    ili9341WriteCommand(ILI9341_FRAME_CTRL_NORM);
    {
        uint8_t data[] = {0x00, 0x18};
        ili9341WriteData(data, sizeof(data));
    }

    // DISPLAY FUNCTION CONTROL
    ili9341WriteCommand(ILI9341_DISP_FUNC_CTRL);
    {
        uint8_t data[] = {0x08, 0x82, 0x27};
        ili9341WriteData(data, sizeof(data));
    }

    // 3GAMMA FUNCTION DISABLE
    ili9341WriteCommand(ILI9341_ENABLE_3G);
    {
        uint8_t data[] = {0x00};
        ili9341WriteData(data, sizeof(data));
    }

    // GAMMA CURVE SELECTED
    ili9341WriteCommand(ILI9341_GAMMA_SET);
    {
        uint8_t data[] = {0x01};
        ili9341WriteData(data, sizeof(data));
    }

    // POSITIVE GAMMA CORRECTION
    ili9341WriteCommand(ILI9341_POS_GAMMA_CORRECTION);
    {
        uint8_t data[] = {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                          0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00};
        ili9341WriteData(data, sizeof(data));
    }

    // NEGATIVE GAMMA CORRECTION
    ili9341WriteCommand(ILI9341_NEG_GAMMA_CORRECTION);
    {
        uint8_t data[] = {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                          0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F};
        ili9341WriteData(data, sizeof(data));
    }

    // EXIT SLEEP
    ili9341WriteCommand(ILI9341_SLEEP_OUT);
    delay(120);

    // TURN ON DISPLAY
    ili9341WriteCommand(ILI9341_DISP_ON);

    // STARTING rotation
    ili9341SetRotation(SCREEN_HORIZONTAL_2);
}

/**
 * @brief Sends single pixel color information to LCD
 * @param color: Color to draw
 * @attention INTERNAL FUNCTION OF LIBRARY, USAGE NOT RECOMENDED, USE Draw_Pixel INSTEAD
 */
void ili9341DrawColor(uint16_t color)
{
    // SENDS color
    unsigned char tempBuffer[2] = {color >> 8, color};
    ILI9341_DATA;
    ILI9341_CS_RESET;
    spi1Transfer((uint8_t *)tempBuffer, &rxBuf, 2);
    ILI9341_CS_SET;
}

// INTERNAL FUNCTION OF LIBRARY
/*Sends block color information to LCD*/

/**
 * @brief Sends block color information to LCD
 * @param color: Color to send
 * @param size: Draw color to array of addresses
 */
void ili9341DrawColorBurst(uint16_t color, uint32_t size)
{
    // SENDS color
    static uint8_t dummy __attribute__((unused));
    uint32_t buffer_size = 0;
    if ((size * 2) < BURST_MAX_SIZE)
    {
        buffer_size = size * 2;
    }
    else
    {
        buffer_size = BURST_MAX_SIZE;
    }

    unsigned char chifted = color >> 8;

    unsigned char burst_buffer[buffer_size];
    for (uint32_t j = 0; j < buffer_size; j += 2)
    {
        burst_buffer[j] = chifted;
        burst_buffer[j + 1] = color;
    }
    uint32_t sending_size = size * 2;
    uint32_t sending_in_block = sending_size / buffer_size;
    uint32_t remainder_from_block = sending_size % buffer_size;

    ILI9341_DATA;
    ILI9341_CS_RESET;

    if (sending_in_block != 0)
    {
        for (uint32_t j = 0; j < (sending_in_block); j++)
        {
            spi1Transfer((uint8_t *)burst_buffer, &dummy, buffer_size);
        }
    }

    // REMAINDER!
    if (remainder_from_block)
        spi1Transfer((uint8_t *)burst_buffer, &dummy, remainder_from_block);

    ILI9341_CS_SET;
}

/**
 * @brief Fill the entire screen with the selected color
 * @param color: Color to send
 * @note Can use either predefined colors or a custom 16-bit
 */
void ili9341FillScreen(uint16_t color)
{
    ili9341SetAddress(0, 0, _lcd_width, _lcd_height);
    ili9341DrawColorBurst(color, (_lcd_width * _lcd_height));
}

// DRAW PIXEL AT XY POSITION WITH SELECTED color
//
// Location is dependant on screen orientation. x0 and y0 locations change with orientations.
// Using pixels to draw big simple structures is not recommended as it is really slow
// Try using either rectangles or lines if possible
//

/**
 * @brief Draw pixel at x,y position with selected color
 * @param x: X coordinate to write to
 * @param y: Y coordinate to write to
 * @param color: Color to draw
 * @note Location is dependant on screen orientation. x0 and y0 locations change with orientations.
 *          Using pixels to draw big simple structures is not recommended as it is really slow
 *          Try using either rectangles or lines if possible
 */
void ili9341DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x >= _lcd_width) || (y >= _lcd_height))
        return; // OUT OF BOUNDS!

    // ADDRESS
    ili9341WriteCommand(ILI9341_COL_ADDR_SET);

    // xDATA
    uint8_t temp_buffer[4] = {x >> 8, x, (x + 1) >> 8, (x + 1)};
    ili9341WriteData(temp_buffer, 4);

    // ADDRESS
    ili9341WriteCommand(ILI9341_PAGE_ADDR_SET);

    // yDATA
    uint8_t temp_buffer1[4] = {y >> 8, y, (y + 1) >> 8, (y + 1)};
    ili9341WriteData(temp_buffer1, 4);

    // ADDRESS
    ili9341WriteCommand(ILI9341_MEM_WRITE);

    // color
    uint8_t temp_buffer2[2] = {color >> 8, color};
    ili9341WriteData(temp_buffer2, 2);
}

/**
 * @brief Draw an image to the screen
 * @param x: Start x coordinate to draw to
 * @param y: Start y coordinate to draw to
 * @param w: Width of image
 * @param h: Height of the image
 * @param image: Image to draw
 */
void ili9341DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t image[240][320])
{
    if ((x >= ILI9341_SCREEN_WIDTH) || (y >= ILI9341_SCREEN_HEIGHT))
        return;
    if ((x + w - 1) >= ILI9341_SCREEN_WIDTH)
        return;
    if ((y + h - 1) >= ILI9341_SCREEN_HEIGHT)
        return;

    ili9341SetAddress(x, y, x + w - 1, y + h - 1);

    uint8_t burst_idx = 0;
    uint8_t bursts_to_send;
    uint16_t burst_remainder;

    if ((sizeof(uint16_t) * w * h) < BURST_MAX_SIZE)
        ili9341WriteData((uint8_t *)image, sizeof(uint16_t) * w * h);
    else
    {
        bursts_to_send = (sizeof(uint16_t) * w * h) / BURST_MAX_SIZE;
        burst_remainder = (sizeof(uint16_t) * w * h) % BURST_MAX_SIZE;
        for (; burst_idx < bursts_to_send; burst_idx++)
            ili9341WriteData((uint8_t *)image + (BURST_MAX_SIZE * burst_idx), BURST_MAX_SIZE);
        if (burst_remainder)
            ili9341WriteData((uint8_t *)image + (BURST_MAX_SIZE * burst_idx), burst_remainder);
    }
}

/**
 * @brief Write a letter to the screen
 * @param x: Start x coordinate to draw to
 * @param y: Start y coordinate to draw to
 * @param ch: Letter to write
 * @param font: Font to write
 * @param color: Color of letter to write
 * @param bgcolor: Background color to write
 */
static void ili9341WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;

    ili9341SetAddress(x, y, x + font.width - 1, y + font.height - 1);

    for (i = 0; i < font.height; i++)
    {
        b = font.data[(ch - 32) * font.height + i];
        for (j = 0; j < font.width; j++)
        {
            if ((b << j) & 0x8000)
            {
                uint8_t data[] = {color >> 8, color & 0xFF};
                ili9341WriteData(data, sizeof(data));
            }
            else
            {
                uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
                ili9341WriteData(data, sizeof(data));
            }
        }
    }
}

/**
 * @brief Write character from USART
 * @param x: Start x coordinate to draw to
 * @param y: Start y coordinate to draw to
 * @param ch: Letter to write
 * @param font: Font to write
 * @param color: Color of letter to write
 * @param bgcolor: Background color to write
 */
void ili9341WriteCharFromUART(uint16_t x, uint16_t y, FontDef font, uint16_t color, uint16_t bgcolor)
{
    char ch = '\0';
    uint8_t locX = 0;
    uint8_t locY = 0;
    uint8_t maxLocX = ILI9341_SCREEN_WIDTH / font.width;
    uint8_t maxLocY = ILI9341_SCREEN_HEIGHT / font.height;

    drawHorizontalLine(locX, locY + font.height, font.width, color);
    while (1)
    {
        ch = serialRead();
        drawHorizontalLine(locX * font.width, (locY * font.height) + font.height, font.width, bgcolor);
        if (ch == 0x08) // delete key
        {
            ch = ' ';
            if ((locX == 0) && (locY != 0)) // deleting at the beginning of a line
            {
                locY--;
                locX = maxLocX - 2;
            }
            else if ((locX == 0) && (locY == 0)) // very beginning
            {
                locX = maxLocX - 2;
                locY = maxLocY - 1;
            }
            else if (locX != 0) // normal case
                locX--;
            ili9341WriteChar(x + (locX * font.width), y + (locY * font.height), ch, font, color, bgcolor);
        }
        else if (ch == 0x0D) // return key
        {
            if (locY + 1 == maxLocY)
            {
                locX = 0;
                locY = 0;
            }
            else
            {
                locX = 0;
                locY++;
            }
        }
        else if (ch >= 0x20) // normal character
        {
            ili9341WriteChar(x + (locX * font.width), y + (locY * font.height), ch, font, color, bgcolor);
            locX++;
            if (locX + 1 == maxLocX)
            {
                locX = 0;
                locY++;
            }
            if (locY == maxLocY)
            {
                locY = 0;
            }
        }
        drawHorizontalLine(locX * font.width, (locY * font.height) + font.height, font.width, color);
    }
}

/**
 * @brief Write a string of letters to the screen
 * @param x: Start x coordinate to draw to
 * @param y: Start y coordinate to draw to
 * @param str: String to write
 * @param font: Font to write
 * @param color: Color of letter to write
 * @param bgcolor: Background color to write
 */
void ili9341WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    ILI9341_CS_RESET;

    while (*str)
    {
        if (x + font.width >= ILI9341_SCREEN_WIDTH)
        {
            x = 0;
            y += font.height;
            if (y + font.height >= ILI9341_SCREEN_HEIGHT)
            {
                break;
            }

            if (*str == ' ')
            {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ili9341WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    ILI9341_CS_SET;
}

/**
 * @brief Invert colors on screen
 * @param invert: `1` to invert `0` to not invert
 */
void ili9341InvertColors(bool invert)
{
    ili9341WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
}