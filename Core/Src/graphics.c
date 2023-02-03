#include "graphics.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) \
    {                       \
        int16_t t = a;      \
        a = b;              \
        b = t;              \
    }
#endif

/**************************************************************************/
/*!
   @brief    Write a line.  Bresenham's algorithm - thx wikpedia
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
              uint16_t color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1)
    {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (steep)
        {
            ili9341DrawPixel(y0, x0, color);
        }
        else
        {
            ili9341DrawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly vertical line (this is often optimized in a
   subclass!)
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    height   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
// DRAW LINE FROM X,Y LOCATION to X,Y+Height LOCATION
void drawVerticalLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color)
{
    if ((x >= _lcd_width) || (y >= _lcd_height))
        return;
    if ((y + h - 1) >= _lcd_height)
    {
        h = _lcd_height - y;
    }
    ili9341SetAddress(x, y, x, y + h - 1);
    ili9341DrawColorBurst(color, h);
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly horizontal line (this is often optimized in a
   subclass!)
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void drawHorizontalLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{
    if ((x >= _lcd_width) || (y >= _lcd_height))
        return;
    if ((x + w - 1) >= _lcd_width)
    {
        w = _lcd_width - x;
    }
    ili9341SetAddress(x, y, x + w - 1, y);
    ili9341DrawColorBurst(color, w);
}

/**************************************************************************/
/*!
   @brief    Draw a circle outline
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawCircle(int16_t x0, int16_t y0, int16_t r,
                uint16_t color)
{
#if defined(ESP8266)
    yield();
#endif
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    ili9341DrawPixel(x0, y0 + r, color);
    ili9341DrawPixel(x0, y0 - r, color);
    ili9341DrawPixel(x0 + r, y0, color);
    ili9341DrawPixel(x0 - r, y0, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ili9341DrawPixel(x0 + x, y0 + y, color);
        ili9341DrawPixel(x0 - x, y0 + y, color);
        ili9341DrawPixel(x0 + x, y0 - y, color);
        ili9341DrawPixel(x0 - x, y0 - y, color);
        ili9341DrawPixel(x0 + y, y0 + x, color);
        ili9341DrawPixel(x0 - y, y0 + x, color);
        ili9341DrawPixel(x0 + y, y0 - x, color);
        ili9341DrawPixel(x0 - y, y0 - x, color);
    }
}

/**************************************************************************/
/*!
    @brief    Quarter-circle drawer, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of
   the circle we're doing
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawCircleHelper(int16_t x0, int16_t y0, int16_t r,
                      uint8_t cornername, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4)
        {
            ili9341DrawPixel(x0 + x, y0 + y, color);
            ili9341DrawPixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2)
        {
            ili9341DrawPixel(x0 + x, y0 - y, color);
            ili9341DrawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8)
        {
            ili9341DrawPixel(x0 - y, y0 + x, color);
            ili9341DrawPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1)
        {
            ili9341DrawPixel(x0 - y, y0 - x, color);
            ili9341DrawPixel(x0 - x, y0 - y, color);
        }
    }
}

/**************************************************************************/
/*!
   @brief    Draw a circle with filled color
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillCircle(int16_t x0, int16_t y0, int16_t r,
                uint16_t color)
{
    drawVerticalLine(x0, y0 - r, 2 * r + 0, color);
    fillCircleHelper(x0, y0, r, 3, 2, color);
}

/**************************************************************************/
/*!
    @brief  Quarter-circle drawer with fill, used for circles and roundrects
    @param  x0       Center-point x coordinate
    @param  y0       Center-point y coordinate
    @param  r        Radius of circle
    @param  corners  Mask bits indicating which quarters we're doing
    @param  delta    Offset from center-point, used for round-rects
    @param  color    16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
                      uint8_t corners, int16_t delta,
                      uint16_t color)
{

    // int16_t f = 1 - r;
    // int16_t ddF_x = 1;
    // int16_t ddF_y = -2 * r;
    // int16_t x = 0;
    // int16_t y = r;
    // int16_t px = x;
    // int16_t py = y;

    // delta++; // Avoid some +1's in the loop

    // while (x < y)
    // {
    //     if (f >= 0)
    //     {
    //         y--;
    //         ddF_y += 2;
    //         f += ddF_y;
    //     }
    //     x++;
    //     ddF_x += 2;
    //     f += ddF_x;
    //     // These checks avoid double-drawing certain lines, important
    //     // for the SSD1306 library which has an INVERT drawing mode.
    //     if (x < (y + 1))
    //     {
    //         if (corners & 1)
    //             drawVerticalLine(x0 + x, y0 - y, 2 * y + delta, color);
    //         if (corners & 2)
    //             drawVerticalLine(x0 - x, y0 - y, 2 * y + delta, color);
    //     }
    //     if (y != py)
    //     {
    //         if (corners & 1)
    //             drawVerticalLine(x0 + py, y0 - px, 2 * px + delta, color);
    //         if (corners & 2)
    //             drawVerticalLine(x0 - py, y0 - px, 2 * px + delta, color);
    //         py = y;
    //     }
    //     px = x;
    // }
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -r - r;
    int16_t x = 0;

    delta++;
    while (x < r)
    {
        if (f >= 0)
        {
            r--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (corners & 0x1)
        {
            drawVerticalLine(x0 + x, y0 - r, r + r + delta, color);
            drawVerticalLine(x0 + r, y0 - x, x + x + delta, color);
        }
        if (corners & 0x2)
        {
            drawVerticalLine(x0 - x, y0 - r, r + r + delta, color);
            drawVerticalLine(x0 - r, y0 - x, x + x + delta, color);
        }
    }
}

/**************************************************************************/
/*!
   @brief   Draw a rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
              uint16_t color)
{

    drawHorizontalLine(x, y, w, color);
    drawHorizontalLine(x, y + h - 1, w, color);
    drawVerticalLine(x, y, h, color);
    drawVerticalLine(x + w - 1, y, h, color);
}

/**************************************************************************/
/*!
   @brief   Draw a rectangle with fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if ((x >= _lcd_width) || (y >= _lcd_height))
        return;
    if ((x + w - 1) >= _lcd_width)
    {
        w = _lcd_width - x;
    }
    if ((y + h - 1) >= _lcd_height)
    {
        h = _lcd_height - y;
    }
    ili9341SetAddress(x, y, x + w - 1, y + h - 1);
    ili9341DrawColorBurst(color, h * w);
}

/**************************************************************************/
/*!
   @brief   Draw a rounded rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                   int16_t r, uint16_t color)
{
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (r > max_radius)
        r = max_radius;
    // smarter version

    drawHorizontalLine(x + r, y, w - 2 * r, color);         // Top
    drawHorizontalLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
    drawVerticalLine(x, y + r, h - 2 * r, color);           // Left
    drawVerticalLine(x + w - 1, y + r, h - 2 * r, color);   // Right
    // draw four corners
    drawCircleHelper(x + r, y + r, r, 1, color);
    drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
    drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

/**************************************************************************/
/*!
   @brief   Draw a rounded rectangle with fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw/fill with
*/
/**************************************************************************/
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                   int16_t r, uint16_t color)
{
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (r > max_radius)
        r = max_radius;
    // smarter version

    fillRect(x + r, y, w - 2 * r, h, color);
    // draw four corners
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

/**************************************************************************/
/*!
   @brief   Draw a triangle with no fill color
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                  int16_t x2, int16_t y2, uint16_t color)
{
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

/**************************************************************************/
/*!
   @brief     Draw a triangle with color-fill
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to fill/draw with
*/
/**************************************************************************/
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                  int16_t x2, int16_t y2, uint16_t color)
{

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1)
    {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }
    if (y1 > y2)
    {
        _swap_int16_t(y2, y1);
        _swap_int16_t(x2, x1);
    }
    if (y0 > y1)
    {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }

    if (y0 == y2)
    { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a)
            a = x1;
        else if (x1 > b)
            b = x1;
        if (x2 < a)
            a = x2;
        else if (x2 > b)
            b = x2;
        drawHorizontalLine(a, y0, b - a + 1, color);

        return;
    }

    int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
            dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2)
        last = y1; // Include y1 scanline
    else
        last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++)
    {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b)
            _swap_int16_t(a, b);
        drawHorizontalLine(a, y, b - a + 1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = (int32_t)dx12 * (y - y1);
    sb = (int32_t)dx02 * (y - y0);
    for (; y <= y2; y++)
    {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b)
            _swap_int16_t(a, b);
        drawHorizontalLine(a, y, b - a + 1, color);
    }
}