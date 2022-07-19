/**
 * @file menu_u8g2.c
 * @brief 
 * @author gerald (hongyi.zhu@aurefluidics.com)
 * @version 1.0
 * @date 2022-07-18
 * 
 * @copyright Copyright (c) 2022  Shanghai AureFluidics Technology Co.,Ltd
 * 
 * @par ChangeLog:
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-04-02     <td>1.0         <td>gerald      <td>the first version
 * </table>
 */

#include "menu_u8g2.h"
#include "u8g2.h"
#include <string.h>

struct u8g2_api
{
    u8g2_t *u;
    int color;
};

static struct u8g2_api uapi;

int menu_api_register_handle(void *handle)
{
    if (NULL == handle)
    {
        return -1;
    }

    uapi.u = (u8g2_t *)handle;

    return 0;
}

void menu_api_draw_box(int x, int y, int w, int h)
{
    u8g2_DrawBox(uapi.u, x, y, w, h);
}

void menu_api_draw_frame(int x, int y, int w, int h)
{
    u8g2_DrawFrame(uapi.u, x, y, w, h);
}

void menu_api_draw_str(int x, int y, const char *str)
{
    u8g2_DrawStr(uapi.u, x, y, str);
}

void menu_api_update_screen(void)
{
    u8g2_SendBuffer(uapi.u);
}

void menu_api_set_draw_color(int color)
{
    uapi.color = color;
    u8g2_SetDrawColor(uapi.u, uapi.color);
}

void menu_api_clear_buf(void)
{
    u8g2_ClearBuffer(uapi.u);
}

int menu_api_set_font(const char *fontname)
{
    if (!strcmp(fontname, "u8g2_font_t0_11b_tf"))
    {
        u8g2_SetFont(uapi.u, u8g2_font_t0_11b_tf);
    }

    return 0;
}
