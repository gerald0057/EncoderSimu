/**
 * @file cfgm.c
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

#include "cfgm.h"

static cfg_t cfg = CFG_INITIAL;

cfg_t *cfgm(void)
{
    return &cfg;
}
