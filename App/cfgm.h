/**
 * @file cfgm.h
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

#ifndef __CFGM_H
#define __CFGM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "encoder_simu.h"

typedef struct cfg cfg_t;

struct cfg
{
    const double PI;        /* mathematical PI */

    /* rotary encoder setting */
    double diameter;        /* wheel diameter */
    uint32_t ppr;           /* pulse per revolution */

    /* speed setting */
    double speed[ENC_SIMU_CHANNEL_MAX];
};

cfg_t *cfgm(void);

#ifdef __cplusplus
}
#endif

#endif /* __CFGM_H */
