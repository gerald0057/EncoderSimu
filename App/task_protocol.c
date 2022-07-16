/**
 * @file task_protocol.c
 * @brief
 * @author gerald (hongyi.zhu@aurefluidics.com)
 * @version 1.0
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022  Shanghai AureFluidics Technology Co.,Ltd
 *
 * @par ChangeLog:
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-07-15     <td>1.0         <td>gerald      <td>the first version
 * </table>
 */

#include "main.h"
#include <stdlib.h>
#include <stdint.h>
#include "oled_font.h"
#include "spi.h"

// OLED模式设置
// 0:4线串行模式
// 1:并行8080模式
#define OLED_MODE 0
#define SIZE 16
#define XLevelL 0x00
#define XLevelH 0x10
#define Max_Column 128
#define Max_Row 64
#define Brightness 0xFF
#define X_WIDTH 128
#define Y_WIDTH 64

//-----------------OLED端口定义----------------

// #define OLED_SCLK_Clr() LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1)//CLK
// #define OLED_SCLK_Set() LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1)

// #define OLED_SDIN_Clr() LL_GPIO_ResetOutputPin(GPIOA,LL_GPIO_PIN_2)//DIN
// #define OLED_SDIN_Set() LL_GPIO_SetOutputPin(GPIOA,LL_GPIO_PIN_2)

#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET) // RES
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET) // DC
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)

#define OLED_CS_Clr() HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET) // CS
#define OLED_CS_Set() HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET)

#define OLED_CMD 0  //写命令
#define OLED_DATA 1 //写数据

// OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

#if OLED_MODE == 1
//向SSD1106写入一个字节。
// dat:要写入的数据/命令
// cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
    DATAOUT(dat);
    if (cmd)
        OLED_DC_Set();
    else
        OLED_DC_Clr();
    OLED_CS_Clr();
    OLED_WR_Clr();
    OLED_WR_Set();
    OLED_CS_Set();
    OLED_DC_Set();
}
#else
//向SSD1106写入一个字节。
// dat:要写入的数据/命令
// cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
    // uint8_t i;
    if (cmd)
        OLED_DC_Set();
    else
        OLED_DC_Clr();
    OLED_CS_Clr();
    HAL_SPI_Transmit(&hspi1, &dat, 1, 0xf);

    // LL_SPI_TransmitData8(SPI1, dat);
    // while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    // for (int i = 0; i < 8; i++)
    // {
    //     OLED_SCLK_Clr();
    //     if (dat & 0x80)
    //         OLED_SDIN_Set();
    //     else
    //         OLED_SDIN_Clr();
    //     OLED_SCLK_Set();
    //     dat <<= 1;
    // }
    OLED_CS_Set();
    OLED_DC_Set();
}
#endif
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD);
}
//开启OLED显示
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
    OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON
    OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON
}
//关闭OLED显示
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
    OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
    OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + i, OLED_CMD); //设置页地址（0~7）
        OLED_WR_Byte(0x00, OLED_CMD);     //设置显示位置—列低地址
        OLED_WR_Byte(0x10, OLED_CMD);     //设置显示位置—列高地址
        for (n = 0; n < 128; n++)
            OLED_WR_Byte(0, OLED_DATA);
    } //更新显示
}

//在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// mode:0,反白显示;1,正常显示
// size:选择字体 16/12
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
    unsigned char c = 0, i = 0;
    c = chr - ' '; //得到偏移后的值
    if (x > Max_Column - 1)
    {
        x = 0;
        y = y + 2;
    }
    if (SIZE == 16)
    {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
        OLED_Set_Pos(x, y + 1);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
    }
    else
    {
        OLED_Set_Pos(x, y + 1);
        for (i = 0; i < 6; i++)
            OLED_WR_Byte(F6x8[c][i], OLED_DATA);
    }
}
// m^n函数
uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}
//显示2个数字
// x,y :起点坐标
// len :数字的位数
// size:字体大小
// mode:模式	0,填充模式;1,叠加模式
// num:数值(0~4294967295);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (size / 2) * t, y, ' ');
                continue;
            }
            else
                enshow = 1;
        }
        OLED_ShowChar(x + (size / 2) * t, y, temp + '0');
    }
}
//显示一个字符号串
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr)
{
    unsigned char j = 0;
    while (chr[j] != '\0')
    {
        OLED_ShowChar(x, y, chr[j]);
        x += 8;
        if (x > 120)
        {
            x = 0;
            y += 2;
        }
        j++;
    }
}
//显示汉字
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{
    uint8_t t, adder = 0;
    OLED_Set_Pos(x, y);
    for (t = 0; t < 16; t++)
    {
        OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
        adder += 1;
    }
    OLED_Set_Pos(x, y + 1);
    for (t = 0; t < 16; t++)
    {
        OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
        adder += 1;
    }
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;

    if (y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;
    for (y = y0; y < y1; y++)
    {
        OLED_Set_Pos(x0, y);
        for (x = x0; x < x1; x++)
        {
            OLED_WR_Byte(BMP[j++], OLED_DATA);
        }
    }
}

void oled_init_reg(void)
{
    uint8_t regs[] =
    {
        0xAE, //--turn off oled panel
        0x00, //---set low column address
        0x10, //---set high column address
        0x40, //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
        0x81, //--set contrast control register
        0xCF, // Set SEG Output Current Brightness
        0xA1, //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
        0xC8, // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
        0xA6, //--set normal display
        0xA8, //--set multiplex ratio(1 to 64)
        0x3f, //--1/64 duty
        0xD3, //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
        0x00, //-not offset
        0xd5, //--set display clock divide ratio/oscillator frequency
        0x80, //--set divide ratio, Set Clock as 100 Frames/Sec
        0xD9, //--set pre-charge period
        0xF1, // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
        0xDA, //--set com pins hardware configuration
        0x12, 
        0xDB, //--set vcomh
        0x40, // Set VCOM Deselect Level
        0x20, //-Set Page Addressing Mode (0x00/0x01/0x02)
        0x02, //
        0x8D, //--set Charge Pump enable/disable
        0x14, //--set(0x10) disable
        0xA4, // Disable Entire Display On (0xa4/0xa5)
        0xA6, // Disable Inverse Display On (0xa6/a7)
        0xAF, //--turn on oled panel
    };

    OLED_DC_Clr();
    OLED_CS_Clr();
    HAL_Delay(1);
    HAL_SPI_Transmit(&hspi1, regs, sizeof(regs), 0xff);
    OLED_CS_Set();
    OLED_DC_Set();
}

//初始化SSD1306
void OLED_Init(void)
{
    OLED_RST_Set();
    HAL_Delay(100);
    OLED_RST_Clr();
    HAL_Delay(200);
    OLED_RST_Set();

    oled_init_reg();

    HAL_Delay(100);
    OLED_Clear();
    OLED_Set_Pos(0, 0);
    OLED_Display_On();
}

void task_protocol(void const *parameter)
{
    OLED_Init();

    // OLED_ShowString(0, 0, (uint8_t *)"Encoder");
    // OLED_ShowString(0, 2, (uint8_t *)"Simulator");
    // for (;;)
    // {
    //     HAL_GPIO_TogglePin(LED_SYS_GPIO_Port, LED_SYS_Pin);
    //     HAL_Delay(500);
    // }
}
