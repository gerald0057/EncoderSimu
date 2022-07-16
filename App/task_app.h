/**
 * @file task_app.h
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

#ifndef __TASK_APP_H
#define __TASK_APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

void task_protocol(void const *parameter);

void OLED_WR_Byte(uint8_t dat, uint8_t cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *p);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_APP_H */
