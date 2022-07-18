/**
 * @file u8g2_porting_ssd1306.c
 * @brief
 * @author gerald (hongyi.zhu@aurefluidics.com)
 * @version 1.0
 * @date 2022-07-17
 *
 * @copyright Copyright (c) 2022  Shanghai AureFluidics Technology Co.,Ltd
 *
 * @par ChangeLog:
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-07-17     <td>1.0         <td>gerald      <td>the first version
 * </table>
 */

#include "main.h"
#include "u8g2_porting.h"
#include "spi.h"

/* ----------------------------- platform begin ----------------------------- */

/* u8g2 use hardware spi */
#define U8G2_HW_SPI_HANDLE  (hspi1)
#define U8G2_HW_PIN_CS      (0)
#define U8G2_HW_PIN_DC      (1)
#define U8G2_HW_PIN_RST     (2)

struct stm32_gpio
{
    GPIO_TypeDef *port;
    uint32_t      pin;
};

struct stm32_gpio u8g2_gpios[] =
{
    {OLED_CS_GPIO_Port,     OLED_CS_Pin     },
    {OLED_DC_GPIO_Port,     OLED_DC_Pin     },
    {OLED_RST_GPIO_Port,    OLED_RST_Pin    },
};

/* u8g2 delay */
static inline void u8g2_delay_milli(uint32_t ms)
{
    HAL_Delay(ms);
}

/* u8g2 hardware pin write */
static inline void u8g2_write_pin(int pin, int value)
{
    if (pin < 0 || pin > sizeof(u8g2_gpios) / sizeof(u8g2_gpios[0]))
    {
        return;
    }

    struct stm32_gpio *gpio = &u8g2_gpios[pin];

    HAL_GPIO_WritePin(gpio->port, gpio->pin, value);
}

/* ------------------------------ platform end ------------------------------ */

static uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
                                  U8X8_UNUSED void *arg_ptr)
{
    /* STM32 supports HW SPI, Remove unused cases like U8X8_MSG_DELAY_XXX & U8X8_MSG_GPIO_XXX */
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        /* Insert codes for initialization */
        break;
    case U8X8_MSG_DELAY_MILLI:
        /* ms Delay */
        u8g2_delay_milli(arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        /* Insert codes for SS pin control */
        u8g2_write_pin(U8G2_HW_PIN_CS, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        /* Insert codes for DC pin control */
        u8g2_write_pin(U8G2_HW_PIN_DC, arg_int);
        break;
    case U8X8_MSG_GPIO_RESET:
        /* Insert codes for RST pin control */
        u8g2_write_pin(U8G2_HW_PIN_RST, arg_int);
        break;
    }
    return 1;
}

static uint8_t u8x8_byte_stm32_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND:
        /* Insert codes to transmit data */
        if (HAL_SPI_Transmit(&U8G2_HW_SPI_HANDLE, arg_ptr, arg_int, 0xff) != HAL_OK)
            return 0;
        break;
    case U8X8_MSG_BYTE_INIT:
        /* Insert codes to begin SPI transmission */
        break;
    case U8X8_MSG_BYTE_SET_DC:
        /* Control DC pin, U8X8_MSG_GPIO_DC will be called */
        u8x8_gpio_SetDC(u8x8, arg_int);
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        /* Select slave, U8X8_MSG_GPIO_CS will be called */
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
        u8g2_delay_milli(1);
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        u8g2_delay_milli(1);
        /* Insert codes to end SPI transmission */
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
        break;
    default:
        return 0;
    }
    return 1;
}

void u8g2_oled_init(u8g2_t *u)
{
    u8g2_Setup_ssd1306_128x64_noname_f(u, U8G2_R0, u8x8_byte_stm32_hw_spi, u8g2_gpio_and_delay_stm32);
    u8g2_InitDisplay(u);
    u8g2_SetPowerSave(u, 0);
    u8g2_ClearBuffer(u);
    u8g2_SetDrawColor(u, 0);
    u8g2_DrawBox(u, 0, 0, 128, 64);
    u8g2_SendBuffer(u);
    u8g2_SetDrawColor(u, 1);
}
