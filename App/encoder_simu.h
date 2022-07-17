/**
 * @file encoder_simu.h
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

#ifndef __ENCODER_SIMU_H
#define __ENCODER_SIMU_H

#ifdef __cplusplus
extern "C" {
#endif

enum enc_simu_channel
{
    ENC_SIMU_CHANNLE_1,
    ENC_SIMU_CHANNLE_2,
    ENC_SIMU_CHANNLE_3,
    ENC_SIMU_CHANNLE_4,

    ENC_SIMU_CHANNEL_MAX,
};

int enc_simu_start(int channel);
int enc_simu_stop(int channel);

#ifdef __cplusplus
}
#endif

#endif /* __ENCODER_SIMU_H */
