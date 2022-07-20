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

enum stm32_enc_simu_type
{
    ENC_SIMU_HW,
    ENC_SIMU_SW,
};

struct stm32_timer_enc
{
    TIM_TypeDef *instance;
    GPIO_TypeDef *channel_port;
    uint32_t channels[2];
    GPIO_TypeDef *led_port;
    uint32_t led_pin;
    enum stm32_enc_simu_type type;
    uint32_t sw_index;
};

struct stm32_timer_ops
{
    int (*init)  (void);
    int (*deinit)(void);
    int (*open)  (int channel);
    int (*close) (int channel);
    int (*config)(int channel, uint32_t psc, uint32_t arr);
};

struct stm32_timer_enc enc_simu_map[] =
{
    {TIM3,  NULL,  {LL_TIM_CHANNEL_CH1, LL_TIM_CHANNEL_CH2}, LED_CH1_GPIO_Port, LED_CH1_Pin, ENC_SIMU_HW},
    {TIM14, GPIOB, {LL_GPIO_PIN_0,      LL_GPIO_PIN_1     }, LED_CH2_GPIO_Port, LED_CH2_Pin, ENC_SIMU_SW},
    {TIM1,  NULL,  {LL_TIM_CHANNEL_CH1, LL_TIM_CHANNEL_CH2}, LED_CH3_GPIO_Port, LED_CH3_Pin, ENC_SIMU_HW},
    {TIM16, GPIOA, {LL_GPIO_PIN_10,     LL_GPIO_PIN_11    }, LED_CH4_GPIO_Port, LED_CH4_Pin, ENC_SIMU_SW},
};

#define __TIM_SET_COMPARE(TIMx, __CHANNEL__, __COMPARE__) \
  (((__CHANNEL__) == LL_TIM_CHANNEL_CH1) ? (TIMx->CCR1 = (__COMPARE__)) :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH2) ? (TIMx->CCR2 = (__COMPARE__)) :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH3) ? (TIMx->CCR3 = (__COMPARE__)) :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH4) ? (TIMx->CCR4 = (__COMPARE__)) :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH5) ? (TIMx->CCR5 = (__COMPARE__)) :\
   (TIMx->CCR6 = (__COMPARE__)))

static int stm32_timer_enc_start(int channel)
{
    struct stm32_timer_enc *device = NULL;

    if (INDEX_IN_ARRAY(channel, enc_simu_map))
    {
        device = &(enc_simu_map[channel]);

        if (ENC_SIMU_HW == device->type)
        {
            LL_TIM_CC_EnableChannel(device->instance, device->channels[0]);
            LL_TIM_CC_EnableChannel(device->instance, device->channels[1]);

            if (IS_TIM_BREAK_INSTANCE(device->instance))
            {
                LL_TIM_EnableAllOutputs(device->instance);
            }

            LL_TIM_EnableCounter(device->instance);
        }
        else
        {
            LL_TIM_ClearFlag_UPDATE(device->instance);
            LL_TIM_EnableIT_UPDATE(device->instance);
            LL_TIM_EnableCounter(device->instance);
        }

        LL_GPIO_ResetOutputPin(device->led_port, device->led_pin);
    }

    return 0;
}

static int stm32_timer_enc_stop(int channel)
{
    struct stm32_timer_enc *device = NULL;

    if (INDEX_IN_ARRAY(channel, enc_simu_map))
    {
        device = &(enc_simu_map[channel]);

        if (ENC_SIMU_HW == device->type)
        {
            LL_TIM_OC_SetMode(device->instance, device->channels[0], LL_TIM_OCMODE_FORCED_INACTIVE);
            LL_TIM_OC_SetMode(device->instance, device->channels[1], LL_TIM_OCMODE_FORCED_INACTIVE);

            if (IS_TIM_BREAK_INSTANCE(device->instance))
            {
                LL_TIM_DisableAllOutputs(device->instance);
            }

            LL_TIM_DisableCounter(device->instance);
            LL_TIM_CC_DisableChannel(device->instance, device->channels[0]);
            LL_TIM_CC_DisableChannel(device->instance, device->channels[1]);
        }
        else
        {
            LL_TIM_DisableCounter(device->instance);
            LL_TIM_DisableIT_UPDATE(device->instance);
            LL_TIM_ClearFlag_UPDATE(device->instance);
        }

        LL_GPIO_SetOutputPin(device->led_port, device->led_pin);
    }

    return 0;
}

static int stm32_timer_enc_config(int channel, uint32_t psc, uint32_t arr)
{
    struct stm32_timer_enc *device = NULL;

    if (INDEX_IN_ARRAY(channel, enc_simu_map))
    {
        device = &(enc_simu_map[channel]);

        if (ENC_SIMU_HW == device->type)
        {
            device->instance->PSC = psc - 1;
            device->instance->ARR = arr - 1;
            device->instance->CNT = 0;

            LL_TIM_OC_SetMode(device->instance, device->channels[0], LL_TIM_OCMODE_TOGGLE);
            LL_TIM_OC_SetMode(device->instance, device->channels[1], LL_TIM_OCMODE_TOGGLE);

            if (ENC_SIMU_DIR_LR == cfgm()->dir[channel])
            {
                __TIM_SET_COMPARE(device->instance, device->channels[0], 0);
                __TIM_SET_COMPARE(device->instance, device->channels[1], (arr / 2) - 1);
            }
            else if (ENC_SIMU_DIR_RL == cfgm()->dir[channel])
            {
                __TIM_SET_COMPARE(device->instance, device->channels[0], (arr / 2) - 1);
                __TIM_SET_COMPARE(device->instance, device->channels[1], 0);
            }
        }
        else
        {
            device->instance->PSC = psc - 1;
            device->instance->CNT = 0;
            device->instance->ARR = (arr / 2) - 1;

            if (ENC_SIMU_DIR_LR == cfgm()->dir[channel])
            {
                device->sw_index = 0;
                LL_GPIO_SetOutputPin(device->channel_port, device->channels[1]);
                LL_GPIO_ResetOutputPin(device->channel_port, device->channels[0]);
            }
            else if (ENC_SIMU_DIR_RL == cfgm()->dir[channel])
            {
                device->sw_index = 1;
                LL_GPIO_SetOutputPin(device->channel_port, device->channels[0]);
                LL_GPIO_ResetOutputPin(device->channel_port, device->channels[1]);
            }
        }
    }

    return 0;
}

void TimerUpdate_Callback(TIM_TypeDef *TIMx)
{
    int channel = -1;
    struct stm32_timer_enc *device = NULL;

    if (TIM14 == TIMx)
    {
        channel = 1;
    }
    else if (TIM16 == TIMx)
    {
        channel = 3;
    }

    device = &(enc_simu_map[channel]);

    if (NULL == device->channel_port)
    {
        return;
    }

    LL_GPIO_TogglePin(device->channel_port, device->channels[device->sw_index]);
    device->sw_index = !!!device->sw_index;
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
    uint32_t pulse_per_sec = (uint32_t)((speed * ppr * 1000.0) / (PI * d * 60));
    uint32_t period_ns = 1000000000 / pulse_per_sec;
    uint32_t pulse_ns = period_ns / 2;

    ULOG_DEBUG("speed (%f)(%d)(%f)", speed, ppr, speed * ppr * 1000.0);
    ULOG_DEBUG("pulse ns (%d)(%d)(%d)", pulse_per_sec, period_ns, pulse_ns);

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
