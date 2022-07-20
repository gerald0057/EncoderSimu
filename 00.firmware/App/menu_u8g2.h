/**
 * @file menu_u8g2.h
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

#ifndef __MENU_U8G2_H
#define __MENU_U8G2_H

#ifdef __cplusplus
extern "C" {
#endif

int menu_api_register_handle(void *handle);
void menu_api_draw_box(int x, int y, int w, int h);
void menu_api_draw_frame(int x, int y, int w, int h);
void menu_api_draw_str(int x, int y, const char *str);
void menu_api_update_screen(void);
void menu_api_set_draw_color(int color);
int menu_api_set_font(const char *fontname);
void menu_api_clear_buf(void);

#ifdef __cplusplus
}
#endif

#endif /* __MENU_U8G2_H */
