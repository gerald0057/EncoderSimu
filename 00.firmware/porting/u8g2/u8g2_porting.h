/**
 * @file u8g2_porting.h
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

#ifndef __U8G2_PORTING_H
#define __U8G2_PORTING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "u8g2.h"

void u8g2_oled_init(u8g2_t *u);

#ifdef __cplusplus
}
#endif

#endif /* __U8G2_PORTING_H */
