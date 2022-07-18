/**
 * @file menu.h
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
 * <tr><td>2022-07-18     <td>1.0         <td>gerald      <td>the first version
 * </table>
 */

#ifndef __MENU_H
#define __MENU_H

#ifdef __cplusplus
extern "C" {
#endif

// Define button status
#define BOTTON_DOESENT_PRESS 0
#define BOTTON_UP 1
#define BUTTON_ENTER 2
#define BUTTON_DOWN 3

void menu(void);

#ifdef __cplusplus
}
#endif

#endif /* __MENU_H */
