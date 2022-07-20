/**
 * @file sample_button.c
 * @brief 
 * @author gerald (hongyi.zhu@aurefluidics.com)
 * @version 1.0
 * @date 2022-05-20
 * 
 * @copyright Copyright (c) 2022  Shanghai AureFluidics Technology Co.,Ltd
 * 
 * @par ChangeLog:
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-05-20     <td>1.0         <td>gerald      <td>the first version
 * </table>
 */

#include "button.h"

#ifndef sys_kprintf
    #include "printf.h"
    #define sys_kprintf(...) printf_(__VA_ARGS__)
#endif

static button_t *btn_test;

int pin_read(void)
{
    /**
     * debounce         : 1
     * trigger level    : 0
     * action:
     *  idle->press->up->press->up->press->up
     * event:
     *  down        x 3
     *  up          x 3
     *  repeat(3)   x 1
     */
    static int simu_pin[] = {1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1};
    static int simu_idx = 0;

    int ret = simu_pin[simu_idx];

    if (simu_idx < sizeof(simu_pin) / sizeof(simu_pin[0]) - 1)
    {
        simu_idx += 1;
    }

    return ret;
}

void btn_handle_press(button_t *btn)
{
    sys_kprintf("press_down(%s)\n", btn->name);
}

void btn_handle_release(button_t *btn)
{
    sys_kprintf("press_up(%s)\n", btn->name);
}

void btn_handle_click_single(button_t *btn)
{
    sys_kprintf("click_single(%s)\n", btn->name);
}

void btn_handle_click_double(button_t *btn)
{
    sys_kprintf("click_double(%s)\n", btn->name);
}

void btn_handler_click_repeat(button_t *btn)
{
    sys_kprintf("repeat(%s)(%d)\n", btn->name, btn->repeat);
}

void btn_handler_long_start(button_t *btn)
{
    sys_kprintf("long_start(%s)\n", btn->name);
}

void btn_handler_long_hold(button_t *btn)
{
    sys_kprintf("long_hold(%s)\n", btn->name);
}

int sample_button_entry(void)
{
    button_cfg_t cfg = BTN_CFG_DEFAULE;

    cfg.pin_read = pin_read;
    cfg.debounce = 1;
    btn_test = button_create("btnA", cfg, BTN_FLAG_START);

    button_attach(btn_test, BTN_PRESS_DOWN, btn_handle_press);
    button_attach(btn_test, BTN_PRESS_UP, btn_handle_release);
    button_attach(btn_test, BTN_PRESS_LONG_START, btn_handler_long_start);
    button_attach(btn_test, BTN_PRESS_LONG_HOLD, btn_handler_long_hold);
    button_attach(btn_test, BTN_CLICK_SINGLE, btn_handle_click_single);
    button_attach(btn_test, BTN_CLICK_DOUBLE, btn_handle_click_double);
    button_attach(btn_test, BTN_CLICK_REPEAT, btn_handler_click_repeat);

    button_list();

    return (btn_test != NULL);
}

int sample_button_leave(void)
{
    return button_delete(btn_test);
}

/* sample code */

static void task_delay(int ms)
{
    /* delay supported by os/bare */
    return;
}

void while_condition(void)
{
    for (;;)
    {
        task_delay(5);
        button_maintain();
    }
}
