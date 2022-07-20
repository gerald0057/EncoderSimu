/**
 * @file button.c
 * @brief 
 * @author gerald (hongyi.zhu@aurefluidics.com)
 * @version 1.0
 * @date 2022-05-19
 * 
 * @copyright Copyright (c) 2022  Shanghai AureFluidics Technology Co.,Ltd
 * 
 * @par ChangeLog:
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-05-19     <td>1.0         <td>gerald      <td>the first version
 * </table>
 */

#include <string.h>
#include "button.h"

#define BTN_EVENT_CALL(b,e) if (b->cb[e]) b->cb[e](b)
#define foreach_btn(x)                            \
{                                                 \
    list_t *node = NULL; button_t *btn = NULL;    \
    list_for_each(node, &container_btn)           \
    {                                             \
        btn = container_of(node, button_t, list); \
        x                                         \
    }                                             \
}

enum button_state
{
    STATE_IDLE,
    STATE_FIRST_PRESS,
    STATE_WAIT_REPEAT,
    STATE_WAIT_RELEASE,
    STATE_LONG_PRESS
};

static list_t container_btn  = LIST_OBJECT_INIT(container_btn);
static void button_sm(button_t *btn);

int button_init(button_t *btn, const char *name, button_cfg_t config, uint32_t flag)
{
    int ret = 0;

    if (NULL == btn || button_found(name))
    {
        ret = -1;
        goto __exit;
    }

    memset(btn, 0, sizeof(button_t));

    memcpy(btn->name, name, BTN_NAME_MAX);
    btn->name[BTN_NAME_MAX-1] = 0;
    btn->cfg = config;

    if (btn->cfg.pin_read)
    {
        btn->pin_level = btn->cfg.pin_read(btn);
    }

    if (flag & BTN_FLAG_START)
    {
        ret = button_start(btn);
    }

__exit:
    return ret;
}

button_t *button_found(const char *name)
{
    foreach_btn
    (
        if (strncmp(btn->name, name, BTN_NAME_MAX) == 0)
        {
            return btn;
        }
    )

    return NULL;
}

int button_attach(button_t *btn, button_evt_t event, button_cb_t cb)
{
    int ret = 0;

    if (NULL == btn || event >= BTN_EVENT_NUM)
    {
        ret = -1;
        goto __exit;
    }

    btn->cb[event] = cb;

__exit:
    return ret;
}

int button_detach(button_t *btn, button_evt_t event)
{
    return button_attach(btn, event, NULL);
}

int button_start(button_t *btn)
{
    int ret = 0;

    if (NULL == btn)
    {
        ret = -1;
        goto __exit;
    }

    if (button_found(btn->name))
    {
        ret = -2;
        goto __exit;
    }

    list_insert_after(&(container_btn), &(btn->list));

__exit:
    return ret;
}

int button_stop(button_t *btn)
{
    int ret = 0;

    if (NULL == btn)
    {
        ret = -1;
        goto __exit;
    }

    if (!button_found(btn->name))
    {
        ret = -2;
        goto __exit;
    }

    list_remove(&(btn->list));

__exit:
    return ret;
}

void button_maintain(void)
{
    foreach_btn
    (
        button_sm(btn);
    )
}

static void button_sm(button_t *btn)
{
    int pin_level_read = 0;

    if (NULL == btn || NULL == btn->cfg.pin_read)
    {
        return;
    }

    pin_level_read = btn->cfg.pin_read(btn);

    if(btn->state != 0)
    {
        btn->ticks++;
    }

    /* pin level changed, start debounce counter */
    if (pin_level_read != btn->pin_level)
    {
        if (++(btn->debounce) >= btn->cfg.debounce)
        {
            btn->pin_level = pin_level_read;
            btn->debounce  = 0;
        }
    }
    else
    {
        btn->debounce  = 0;
    }

    /* button state machine */
    switch (btn->state)
    {
        case STATE_IDLE:
            /* push */
            if (btn->pin_level == btn->cfg.trig_level)
            {
                btn->ticks  = 0;
                btn->repeat = 1;
                btn->state  = STATE_FIRST_PRESS;
                BTN_EVENT_CALL(btn, BTN_PRESS_DOWN);
            }
            break;

        case STATE_FIRST_PRESS:
            /* release */
            if (btn->pin_level != btn->cfg.trig_level)
            {
                BTN_EVENT_CALL(btn, BTN_PRESS_UP);
                btn->ticks = 0;
                btn->state = STATE_WAIT_REPEAT;
            }
            /* still push */
            else
            {
                if(btn->ticks > btn->cfg.def_long)
                {
                    BTN_EVENT_CALL(btn, BTN_PRESS_LONG_START);
                    btn->hold  = btn->cfg.hold;
                    btn->state = STATE_LONG_PRESS;
                }
            }
            break;

        case STATE_WAIT_REPEAT:
            /* push again */
            if(btn->pin_level == btn->cfg.trig_level)
            {
                BTN_EVENT_CALL(btn, BTN_PRESS_DOWN);
                /* prevent inversion of btn->repeat:(?bitfield) */
                if (!++(btn->repeat))
                {
                    btn->repeat -= 1;
                }
                btn->ticks = 0;
                btn->state = STATE_WAIT_RELEASE;
            }
            /* release */
            else
            {
                /* waiting for other action */
                if(btn->ticks > btn->cfg.def_short)
                {
                    if(btn->repeat == 1)
                    {
                        BTN_EVENT_CALL(btn, BTN_CLICK_SINGLE);
                    }
                    else if(btn->repeat == 2)
                    {
                        BTN_EVENT_CALL(btn, BTN_CLICK_DOUBLE);
                    }
                    else
                    {
                        BTN_EVENT_CALL(btn, BTN_CLICK_REPEAT);
                    }
                    btn->state = STATE_IDLE;
                }
            }
            break;

        case STATE_WAIT_RELEASE:
            /* release again */
            if (btn->pin_level != btn->cfg.trig_level)
            {
                BTN_EVENT_CALL(btn, BTN_PRESS_UP);
                if(btn->ticks < btn->cfg.def_short)
                {
                    btn->ticks = 0;
                    btn->state = STATE_WAIT_REPEAT;
                }
                else
                {
                    btn->state = STATE_IDLE;
                }
            }
            /* still pressed */
            else
            {
                if (btn->ticks > btn->cfg.def_short)
                {
                    btn->ticks = 0;
                    btn->state = (btn->pin_level == btn->cfg.trig_level);
                }
            }
            break;
        
        case STATE_LONG_PRESS:
            /* long press */
            if (btn->pin_level == btn->cfg.trig_level)
            {
                btn->hold -= 1;
                if (btn->hold <= 0)
                {
                    BTN_EVENT_CALL(btn, BTN_PRESS_LONG_HOLD);
                    btn->hold = btn->cfg.hold;
                }
            }
            /* released */
            else
            {
                BTN_EVENT_CALL(btn, BTN_PRESS_UP);
                btn->ticks = 0;
                btn->state = STATE_IDLE;
            }
            break;
    }
}

void button_list(void)
{
    foreach_btn
    (
        sys_kprintf("button:(%s)\n", btn->name);
        /* #ifndef BTN_DEBUG */
        (void)(btn);
    )
}
