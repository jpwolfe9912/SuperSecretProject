#include "ft6206.h"

TouchData_t touchData;

bool ft6206Init(void)
{
#ifdef FT62XX_DEBUG
    uint8_t vendId, chipId, firmVers, pointRate, threshhold;

    {
        i2c1Read(FT62XX_ADDR, FT62XX_REG_VENDID, &vendId, 1);
        printf("Vend ID: 0x%X\n", vendId);
    }
    {
        i2c1Read(FT62XX_ADDR, FT62XX_REG_CHIPID, &chipId, 1);
        printf("Chip ID: 0x%X\n", chipId);
    }
    {
        i2c1Read(FT62XX_ADDR, FT62XX_REG_FIRMVERS, &firmVers, 1);
        printf("Firm V: %u\n", firmVers);
    }
    {
        i2c1Read(FT62XX_ADDR, FT62XX_REG_POINTRATE, &pointRate, 1);
        printf("Point Rate Hz: %u\n", pointRate);
    }
    {
        i2c1Read(FT62XX_ADDR, FT62XX_REG_THRESHHOLD, &threshhold, 1);
        printf("Thresh: %u\n", threshhold);
    }

    // dump all registers
    uint8_t data;
    for (int16_t i = 0; i < 0x10; i++)
    {
        i2c1Read(FT62XX_ADDR, i, &data, 1);
        printf("I2C $%X = 0x%X\n", i, data);
    }
#endif
    {
        // change threshhold to be higher/lower
        uint8_t data[] = {FT62XX_REG_THRESHHOLD, 40};
        i2c1Write(FT62XX_ADDR, data, sizeof(data));
    }
    {
        // change trigger type
        uint8_t data[] = {FT62XX_REG_G_MODE, 0x00};
        uint8_t mode;
        i2c1Write(FT62XX_ADDR, data, sizeof(data));

        i2c1Read(FT62XX_ADDR, FT62XX_REG_G_MODE, &mode, 1);
    }
    {
        // check if vendor ID matches
        uint8_t vendId;
        i2c1Read(FT62XX_ADDR, FT62XX_REG_VENDID, &vendId, 1);
        if (vendId != FT62XX_VENDID)
        {
            return false;
        }
    }
    {

        uint8_t chipId;
        i2c1Read(FT62XX_ADDR, FT62XX_REG_CHIPID, &chipId, 1);
        if ((chipId != FT6206_CHIPID) && (chipId != FT6236_CHIPID) &&
            (chipId != FT6236U_CHIPID))
        {
            return false;
        }
    }
    touchData.touched = false;
    touchData.touches = 0;
    touchData.touchId = 0;
    touchData.xPos = 0;
    touchData.yPos = 0;

    NVIC_EnableIRQ(EXTI9_5_IRQn);
    return true;
}

bool ft6206ReadData(void)
{
    bool dataReady = false;
    uint8_t rawData[16];
    i2c1Read(FT62XX_ADDR, 0x00, rawData, sizeof(rawData));

#ifdef FT62XX_DEBUG
    // for (int16_t i = 0; i < 16; i++)
    // {
    //     printf("I2C $%X = 0x%X\n", i, rawData[i]);
    // }
#endif
    touchData.touches = rawData[2] & 0x0F; // number of touches
    if ((touchData.touches > 2) || (touchData.touches == 0))
    {
        touchData.touches = 0;
        return dataReady = false;
    }
    else
    {
        touchData.yPos = constrain16(240 - (((rawData[3] & 0x0F) << 8U) | rawData[4]), 0, 240);
        touchData.xPos = constrain16(((rawData[5] & 0x0F) << 8U) | rawData[6], 0, 320);
        touchData.touchId = (rawData[5] >> 4U) & 0x0F;
        return dataReady = true;
    }

#ifdef FT62XX_DEBUG

    printf("ID #%u\t", touchData.touchId);
    printf("(%u, %u)\r", touchData.xPos, touchData.yPos);
#endif
}

void FT62XX_TOUCH_IRQ(void)
{
    /* USER CODE BEGIN EXTI9_5_IRQn 0 */

    /* USER CODE END EXTI9_5_IRQn 0 */
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5) != RESET)
    {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
        /* USER CODE BEGIN LL_EXTI_LINE_5 */
        if (!(GPIOB->IDR & GPIO_IDR_ID5)) // falling edge
            touchData.touched = true;
        else if (GPIOB->IDR & GPIO_IDR_ID5) // rising edge
            touchData.touched = false;
        /* USER CODE END LL_EXTI_LINE_5 */
    }
    /* USER CODE BEGIN EXTI9_5_IRQn 1 */

    /* USER CODE END EXTI9_5_IRQn 1 */
}