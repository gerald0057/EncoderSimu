/**
 * @file encoder_simu.c
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
#include "main.h"
#include "tim.h"
#include "encoder_simu.h"

/* ----------------------------- platform begin ----------------------------- */

#define INDEX_IN_ARRAY(i, arr)  (i < (sizeof(arr) / sizeof(arr[0])))

struct stm32_timer_enc
{
    TIM_HandleTypeDef *htim;
    uint32_t channels[2];
};

struct stm32_timer_ops
{
    int (*open)  (int channel);
    int (*close) (int channel);
    int (*config)(int channel, uint32_t psc, uint32_t arr);
};

struct stm32_timer_enc enc_simu_map[] =
{
    {&htim1, {TIM_CHANNEL_1, TIM_CHANNEL_2}},
    {&htim1, {TIM_CHANNEL_3, TIM_CHANNEL_4}},
    {&htim3, {TIM_CHANNEL_1, TIM_CHANNEL_2}},
    {&htim3, {TIM_CHANNEL_3, TIM_CHANNEL_4}},
};

static int stm32_timer_enc_start(int channel)
{
    struct stm32_timer_enc *device = NULL;

    if (INDEX_IN_ARRAY(channel, enc_simu_map))
    {
        device = &(enc_simu_map[channel]);

        HAL_TIM_OC_Start(device->htim, device->channels[0]);
        HAL_TIM_OC_Start(device->htim, device->channels[1]);
    }

    return 0;
}

static int stm32_timer_enc_stop(int channel)
{
    struct stm32_timer_enc *device = NULL;

    if (INDEX_IN_ARRAY(channel, enc_simu_map))
    {
        device = &(enc_simu_map[channel]);

        HAL_TIM_OC_Stop(device->htim, device->channels[0]);
        HAL_TIM_OC_Stop(device->htim, device->channels[1]);
    }

    return 0;
}

static int stm32_timer_enc_config(int channel, uint32_t psc, uint32_t arr)
{
    struct stm32_timer_enc *device = NULL;

    if (INDEX_IN_ARRAY(channel, enc_simu_map))
    {
        device = &(enc_simu_map[channel]);

        __HAL_TIM_SET_AUTORELOAD(device->htim, arr - 1);
        __HAL_TIM_SET_PRESCALER(device->htim, psc - 1);
        __HAL_TIM_SET_COUNTER(device->htim, 0);
        __HAL_TIM_SET_COMPARE(device->htim, device->channels[0], 0);
        __HAL_TIM_SET_COMPARE(device->htim, device->channels[1], (arr - 1) / 2);
    }

    return 0;
}

struct stm32_timer_ops enc_ops =
{
    .open  = stm32_timer_enc_start,
    .close = stm32_timer_enc_stop,
    .config= stm32_timer_enc_config,
};

/* ------------------------------ platform end ------------------------------ */

static int enc_speed_to_pulse(double speed)
{
    double   d   = cfgm()->diameter;
    double   PI  = cfgm()->PI;
    uint32_t ppr = cfgm()->ppr;

    /**
     * speed            : meter per minute
     * ppr              : pulse per revolution
     * circumference    : pi * d
     * diameter         : in millimeter
     * 
     * meter per second : mps = speed / 60
     * d_in_meter       : diameter / 1000
     * num of turns     : not = mps / (pi * d_in_meter)
     * pulse_per_sec    : pps = not * ppr
     * 
     * pps  = (mps / (pi * d_in_meter)) * ppr
     *      = (mps * ppr) / (pi * d_in_meter)
     *      = ((speed / 60) * ppr) / (pi * diameter / 1000)
     *      = (speed * ppr / 60) / (pi * diameter / 1000)
     *      = (speed * ppr * 1000) / (pi * diameter * 60)
     */
    uint32_t pulse_per_sec = (uint32_t)(speed * ppr * 1000) / (PI * d * 60);
    uint32_t period_ns = 1000000000 / pulse_per_sec;
    uint32_t pulse_ns = period_ns / 2;

    return pulse_ns;
}

static void enc_config_timer(int channel, uint32_t pulse_ns)
{
    uint32_t period, psc;
    uint32_t timer_freq = HAL_RCC_GetPCLK1Freq() / 1000000;
    uint16_t timer_max = (uint16_t)(-1);

    /* Convert nanosecond to frequency and duty cycle. 1s = 1 * 1000 * 1000 * 1000 ns */
    period = pulse_ns * timer_freq / 1000 ;
    psc = period / timer_max + 1;
    period = period / psc;

    enc_ops.config(channel, psc, period);
}

int enc_simu_start(int channel)
{
    uint32_t pulse = 0;

    if (channel >= ENC_SIMU_CHANNEL_MAX)
    {
        return -1;
    }

    enc_ops.close(channel);
    pulse = enc_speed_to_pulse(cfgm()->speed[channel]);
    enc_config_timer(channel, pulse);
    enc_ops.open(channel);

    return 0;
}

int enc_simu_stop(int channel)
{
    return enc_ops.close(channel);
}
