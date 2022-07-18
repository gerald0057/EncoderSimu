/**
 * @file button.h
 * @brief 
 * @author gerald (hongyi.zhu@aurefluidics.com)
 * @version 1.0
 * @date 2022-05-19
 * 
 * @copyright Copyright (c)2022  Shanghai AureFluidics Technology Co.,Ltd
 * 
 * @par ChangeLog:
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-05-19     <td>1.0         <td>gerald      <td>the first version
 * </table>
 */

#ifndef __BUTTON_H
#define __BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "list.h"

/* button name length */
#define BTN_NAME_MAX    8
/* button_maintain called in this tick */
#define BTN_TICKS       20
/* button debug enable */
// #define BTN_DEBUG

/* button flag */
#define BTN_FLAG_START  0x01            /* Start button after create/init */

/* button default configuration */
#define BTN_CFG_DEFAULE                 \
{                                       \
    .hold       = (1000 / BTN_TICKS),   \
    .debounce   = 3,                    \
    .def_long   = (1000 / BTN_TICKS),   \
    .def_short  = (300  / BTN_TICKS),   \
    .trig_level = 0                     \
}

#ifndef sys_kprintf
    #ifdef BTN_DEBUG
        #include "printf.h"
        #define sys_kprintf(...) printf_(__VA_ARGS__)
    #else
        #define sys_kprintf(...)
    #endif
#endif

typedef enum button_event button_evt_t;
typedef struct button_cfg button_cfg_t;
typedef struct button     button_t;
typedef void            (*button_cb_t)(button_t *);

enum button_event
{
    BTN_PRESS_DOWN = 1,
    BTN_PRESS_UP,
    BTN_PRESS_LONG_START,
    BTN_PRESS_LONG_HOLD,
    BTN_CLICK_SINGLE,
    BTN_CLICK_DOUBLE,
    BTN_CLICK_REPEAT,
    /* Button Event Amount */
    BTN_EVENT_NUM,
    BTN_IDLE = 0
};

struct button_cfg
{
    uint32_t hold       : 10;
    uint32_t debounce   : 3;
    uint32_t def_short  : 8;
    uint32_t def_long   : 10;
    uint32_t trig_level : 1;

    int (*pin_read)(button_t *self);
};

struct button
{
    char name[BTN_NAME_MAX];

    uint32_t ticks    : 12;
    uint32_t hold     : 10;
    uint32_t repeat   : 3 ;
    uint32_t debounce : 3 ;
    uint32_t state    : 3 ;
    uint32_t pin_level: 1 ;

    button_cfg_t cfg;
    button_cb_t  cb[BTN_EVENT_NUM];

    list_t list;
};

/**
 * @brief init static button context
 * @param  btn              pointer of button context
 * @param  name             name of button
 * @param  config           configuration
 * @param  flag             operation flag
 * @return int
 */
int button_init(button_t *btn, const char *name, button_cfg_t config, uint32_t flag);

/**
 * @brief found a button from list
 * @param  name             name of button
 * @return button_t*
 */
button_t *button_found(const char *name);

/**
 * @brief attach a callback to btn according to a specific event 
 * @param  btn              pointer of button context
 * @param  event            button event
 * @param  cb               event callback function
 * @return int
 */
int button_attach(button_t *btn, button_evt_t event, button_cb_t cb);

/**
 * @brief detach the exist callback of event
 * @param  btn              pointer of button context
 * @param  event            button event
 * @return int
 */
int button_detach(button_t *btn, button_evt_t event);

/**
 * @brief start detecting button in button_maintain
 * @param  btn              pointer of button context
 * @return int
 */
int button_start(button_t *btn);

/**
 * @brief stop detecting button in button_maintain
 * @param  btn              pointer of button context
 * @return int
 */
int button_stop(button_t *btn);

/**
 * @brief button detecting
 */
void button_maintain(void);

/**
 * @brief show all button started
 *  depends on BTN_DEBUG
 */
void button_list(void);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H */
