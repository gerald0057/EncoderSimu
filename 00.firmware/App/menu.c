/**
 * @file menu.c
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

#include "menu.h"
#include "button.h"
#include "main.h"
#include "ulog.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu_u8g2.h"
#include "cfgm.h"
#include "encoder_simu.h"

// ----------------------------------------------------------------------------------
// Main struct menu
typedef struct Struct
{
    struct Struct *up;     // pointer on up element of list
    struct Struct *down;   // pointer on down element of list
    struct Struct *child;  // Pointer on child menu
    struct Struct *parent; // Pointer on parent menu

    char name[16];                      // Name menu
    void (*updateScreen_up)(void);   // Function print graphic on OLED
    void (*updateScreen_down)(void); // Function print graphic on OLED
    void (*makeAction)(void *);
} MenuItem_t;

// How many menu items in menu
#define MENU_ITEM_NUM 2
#define MENU_1_ITEM_NUM 4
#define MENU_2_ITEM_NUM 4

// Create menu item array structure for all menus
MenuItem_t items[MENU_ITEM_NUM];
MenuItem_t items_menu_1[MENU_1_ITEM_NUM];
MenuItem_t items_menu_2[MENU_2_ITEM_NUM];
MenuItem_t item_speed_bar;

MenuItem_t *currentItem = &items[0]; // Create and set pointer on first element of list

char str_pointer[4] = "->"; // How look pointer on menu item
uint8_t speed_changing_index = 0;
const int speed_bar_min = 20;
const int speed_bar_step = 20;
const int speed_bar_max = 200;

// Rows coordinates
uint16_t first_menu_row = 16 + 8;
uint16_t second_menu_row = 28 + 8;
uint16_t third_menu_row = 40 + 8;
uint16_t fourth_menu_row = 52 + 8;
uint8_t row_step = 12;
uint16_t start_print_name_menu_x = 15;

// ----------------------------------------------------------------------------------------
/*
This function print scrollbar on right part of OLED.
 */
void scroll_bar(void)
{
    // Work size scrollbar
    uint16_t start_x_scrollbar = 124;
    uint16_t start_y_scrollbar = 17;
    uint16_t active_width = 3;
    uint16_t lenght_all_scrollbar = 48;
    uint8_t menu_items_counter = 1, current_count = 1;

    MenuItem_t *currentItem_buff = currentItem;

    if ((currentItem_buff->up) != 0) // Step up if existing up
    {
        do
        {
            currentItem_buff = currentItem_buff->up; // Step up to the top of menu
            current_count++;
        } while ((currentItem_buff->up) != 0);       // If the top of item doesen't found
    }

    if ((currentItem_buff->up) == 0) // If found top of menu item
    {
        do
        {
            currentItem_buff = currentItem_buff->down;
            menu_items_counter++; // Count how many list on menu
        } while ((currentItem_buff->down) != 0);
    }

    menu_api_set_draw_color(0);
    menu_api_draw_box(start_x_scrollbar, start_y_scrollbar, active_width, lenght_all_scrollbar - 2);
    menu_api_set_draw_color(1);

    menu_api_draw_frame(start_x_scrollbar - 1, start_y_scrollbar - 1, active_width + 1, 65 - start_y_scrollbar);

    // Print scroling line
    uint16_t line_lenght = (lenght_all_scrollbar / menu_items_counter + 1);
    uint16_t start_lenght = 16 + ((current_count - 1) * line_lenght);

    menu_api_draw_box(start_x_scrollbar, start_lenght, active_width, line_lenght);
}
// ----------------------------------------------------------------------------------------
void menu_draw_title(const char *title)
{
    char str[16];
    int title_len = strlen(title);
    int space_len = (sizeof(str) - title_len) / 2;

    menu_api_draw_frame(0, 0, 127, 15);
    menu_api_set_draw_color(0);
    menu_api_draw_box(1, 1, 125, 13);
    menu_api_set_draw_color(1);

    sprintf(str, "%*s", space_len, title);
    menu_api_draw_str(10, 11, str);
}

void menu_clear_item(void)
{
    menu_api_set_draw_color(0);
    menu_api_draw_box(0, 15, 127, 50);
    menu_api_set_draw_color(1);
}

// ----------------------------------------------------------------------------------------
void print_rows_on_oled_if_up(void)
{
    char str[16] = {0};

    menu_clear_item();
    // Print pointer on first item menu
    menu_api_draw_str(0, first_menu_row, str_pointer);

    MenuItem_t *currentItem_buff_up = currentItem; // Create buffer on selected current item pointer.
    for (uint8_t row = first_menu_row; row <= fourth_menu_row; row = row + row_step)
    {

        // Print name of menu item
        strncpy(str, currentItem_buff_up->name, 15);
        menu_api_draw_str(start_print_name_menu_x, row, str);

        currentItem_buff_up = currentItem_buff_up->down; // Make a step down

        // Print only existing items
        if (currentItem_buff_up == 0) // If no next item
        {
            break;
        }
    }
    scroll_bar();
    menu_api_update_screen();
}
// ----------------------------------------------------------------------------------------
void print_rows_on_oled_if_down(void) // print text menu item
{
    char str[16] = {0};

    menu_clear_item();
    // Print pointer on first item menu
    menu_api_draw_str(0, first_menu_row, str_pointer);

    MenuItem_t *currentItem_buff = currentItem;
    for (uint8_t row = first_menu_row; row <= fourth_menu_row; row = row + row_step)
    {
        // Print name of menu item
        memset(str, 0, sizeof(str));
        strncpy(str, currentItem_buff->name, 15);
        menu_api_draw_str(start_print_name_menu_x, row, str);

        currentItem_buff = currentItem_buff->down; // Make a step down

        // Print only existing items
        if (currentItem_buff == 0) // If no next item
        {
            break;
        }
    }
    scroll_bar();
    menu_api_update_screen();
}
void print_speed_bar(void *param)
{
    int speed = cfgm()->speed[speed_changing_index];
    char str[16];

    ULOG_DEBUG("speed bar %d %d", speed_changing_index, speed);

    enc_simu_start(speed_changing_index);

    sprintf(str, "%d m/min", speed);
    menu_clear_item();
    menu_api_draw_str(30, 35, str);
    menu_api_draw_frame(10, 48, 107, 4);
    menu_api_draw_box(10, 48, speed * 107 / speed_bar_max, 4);
    menu_api_update_screen();
}
void print_speed_up(void)
{
    int speed = cfgm()->speed[speed_changing_index];
    
    speed += speed_bar_step;
    if (speed > speed_bar_max) speed = speed_bar_max;
    cfgm()->speed[speed_changing_index] = speed;

    print_speed_bar(NULL);
}
void print_speed_down(void)
{
    int speed = cfgm()->speed[speed_changing_index];

    speed -= speed_bar_step;
    if (speed < speed_bar_min) speed = speed_bar_min;
    cfgm()->speed[speed_changing_index] = speed;

    print_speed_bar(NULL);
}
void print_menu_speed_bar(void *param)
{
    uint8_t channel = (uint32_t)(param) & 0x03;
    char str[16];

    sprintf(str, "Channel-%d speed", channel + 1);
    speed_changing_index = channel;
    menu_draw_title(str);
    print_speed_bar(NULL);
}
// ----------------------------------------------------------------------------------------
// Print first 4 items of menu (only first time)
void print_menu_init(void)
{
    MenuItem_t *currentItem_buff = currentItem;

    menu_draw_title(">> MAIN MENU <<");
    menu_clear_item();
    menu_api_update_screen();

    // Print selected name of menu
    char str[16] = {0};

    for (uint8_t row = first_menu_row; row <= fourth_menu_row; row = row + row_step)
    {
        if (row == first_menu_row)
        {
            // Print pointer on menu (On top)
            char str_pointer[4] = "->";
            menu_api_draw_str(0, row, str_pointer);
        }

        // Print menu of menu item
        strncpy(str, currentItem_buff->name, 15);
        menu_api_draw_str(start_print_name_menu_x, row, str);

        currentItem_buff = currentItem_buff->down;

        if (currentItem_buff == 0) // If no menu item
        {
            break;
        }
    }
    scroll_bar();
    menu_api_update_screen();
}
// ----------------------------------------------------------------------------------------
void action(void)
{
    char str[16] = {0};

    // Print selected name of menu on top of OLED
    MenuItem_t *currentItem_buff_parent = currentItem;
    currentItem_buff_parent = currentItem_buff_parent->parent;

    menu_clear_item();
    if (NULL == currentItem_buff_parent)
    {
        menu_draw_title(">> MAIN MENU <<");
    }
    else
    {
        menu_draw_title(currentItem_buff_parent->name);
    }

    MenuItem_t *currentItem_buff = currentItem;

    for (uint8_t row = first_menu_row; row <= fourth_menu_row; row = row + row_step)
    {
        if (row == first_menu_row)
        {
            // Print pointer on menu
            char str_pointer[4] = "->";
            menu_api_draw_str(0, row, str_pointer);
        }

        // Print menu of menu item
        strncpy(str, currentItem_buff->name, 15);
        menu_api_draw_str(start_print_name_menu_x, row, str);

        currentItem_buff = currentItem_buff->down;
        if (currentItem_buff == 0)
        {
            break;
        }
    }
    scroll_bar();
    menu_api_update_screen();
}
// ----------------------------------------------------------------------------------------
void return_from_menu(void *param)
{
    currentItem = &items[0]; // Jump to main menu
    print_menu_init(); // Print all start menu
}
// ----------------------------------------------------------------------------------------
void menu_channel_enable(void *param)
{
    uint8_t channel = (int)(param) & 0x3;
    int enabled = cfgm()->enable[channel];

    enabled = !!!enabled;
    cfgm()->enable[channel] = enabled;

    if (enabled)
    {
        sprintf(items_menu_1[channel].name, "Channel-%1d   [v]", channel + 1);
        enc_simu_start(channel);
    }
    else
    {
        sprintf(items_menu_1[channel].name, "Channel-%1d   [ ]", channel + 1);
        enc_simu_stop(channel);
    }

    // Return to first item of current menu
    currentItem = &items_menu_1[channel]; // Set global pointer on first menu
    action();                       // Print items on OLED
}
void menu_channel_speed_config(void *param)
{
    item_speed_bar.parent = currentItem;
    currentItem = &item_speed_bar; // Set global pointer on first menu
    print_menu_speed_bar(param);
}
// ----------------------------------------------------------------------------------------
void Menu_Init(void)
{
    items[0].up = 0;
    items[0].down = &items[1];
    items[0].child = &items_menu_1[0];
    items[0].parent = 0;
    strncpy(items[0].name, "Channel Enable", 16);
    items[0].updateScreen_up = print_rows_on_oled_if_up;
    items[0].updateScreen_down = print_rows_on_oled_if_down;
    items[0].makeAction = 0;

    items[1].up = &items[0];
    items[1].down = 0;
    items[1].child = &items_menu_2[0];
    items[1].parent = 0;
    strncpy(items[1].name, "Speed config", 16);
    items[1].updateScreen_up = print_rows_on_oled_if_up;
    items[1].updateScreen_down = print_rows_on_oled_if_down;
    items[1].makeAction = 0;

    // Creating next menu
    items_menu_1[0].up = 0;
    items_menu_1[0].down = &items_menu_1[1];
    items_menu_1[0].child = 0;
    items_menu_1[0].parent = &items[0];
    strncpy(items_menu_1[0].name, "Channel-1   [ ]", 16);
    items_menu_1[0].updateScreen_up = print_rows_on_oled_if_up;
    items_menu_1[0].updateScreen_down = print_rows_on_oled_if_down;
    items_menu_1[0].makeAction = menu_channel_enable;

    items_menu_1[1].up = &items_menu_1[0];
    items_menu_1[1].down = &items_menu_1[2];
    items_menu_1[1].child = 0;
    items_menu_1[1].parent = &items[0];
    strncpy(items_menu_1[1].name, "Channel-2   [ ]", 16);
    items_menu_1[1].updateScreen_up = print_rows_on_oled_if_up;
    items_menu_1[1].updateScreen_down = print_rows_on_oled_if_down;
    items_menu_1[1].makeAction = menu_channel_enable;

    items_menu_1[2].up = &items_menu_1[1];
    items_menu_1[2].down = &items_menu_1[3];
    items_menu_1[2].child = 0;
    items_menu_1[2].parent = &items[0];
    strncpy(items_menu_1[2].name, "Channel-3   [ ]", 16);
    items_menu_1[2].updateScreen_up = print_rows_on_oled_if_up;
    items_menu_1[2].updateScreen_down = print_rows_on_oled_if_down;
    items_menu_1[2].makeAction = menu_channel_enable;

    items_menu_1[3].up = &items_menu_1[2];
    items_menu_1[3].down = 0;
    items_menu_1[3].child = 0;
    items_menu_1[3].parent = &items[0];
    strncpy(items_menu_1[3].name, "Channel-4   [ ]", 16);
    items_menu_1[3].updateScreen_up = print_rows_on_oled_if_up;
    items_menu_1[3].updateScreen_down = print_rows_on_oled_if_down;
    items_menu_1[3].makeAction = menu_channel_enable;

    // Creating next menu
    items_menu_2[0].up = 0;
    items_menu_2[0].down = &items_menu_2[1];
    items_menu_2[0].child = &item_speed_bar;
    items_menu_2[0].parent = &items[1];
    strncpy(items_menu_2[0].name, "Channel-1    ", 16);
    items_menu_2[0].updateScreen_up = print_rows_on_oled_if_up;
    items_menu_2[0].updateScreen_down = print_rows_on_oled_if_down;
    items_menu_2[0].makeAction = menu_channel_speed_config;

    items_menu_2[1].up = &items_menu_2[0];
    items_menu_2[1].down = &items_menu_2[2];
    items_menu_2[1].child = &item_speed_bar;
    items_menu_2[1].parent = &items[1];
    strncpy(items_menu_2[1].name, "Channel-2    ", 16);
    items_menu_2[1].updateScreen_up = print_rows_on_oled_if_up;
    items_menu_2[1].updateScreen_down = print_rows_on_oled_if_down;
    items_menu_2[1].makeAction = menu_channel_speed_config;

    items_menu_2[2].up = &items_menu_2[1];
    items_menu_2[2].down = &items_menu_2[3];
    items_menu_2[2].child = &item_speed_bar;
    items_menu_2[2].parent = &items[1];
    strncpy(items_menu_2[2].name, "Channel-3    ", 16);
    items_menu_2[2].updateScreen_up = print_rows_on_oled_if_up;
    items_menu_2[2].updateScreen_down = print_rows_on_oled_if_down;
    items_menu_2[2].makeAction = menu_channel_speed_config;

    items_menu_2[3].up = &items_menu_2[2];
    items_menu_2[3].down = 0;
    items_menu_2[3].child = &item_speed_bar;
    items_menu_2[3].parent = &items[1];
    strncpy(items_menu_2[3].name, "Channel-4    ", 16);
    items_menu_2[3].updateScreen_up = print_rows_on_oled_if_up;
    items_menu_2[3].updateScreen_down = print_rows_on_oled_if_down;
    items_menu_2[3].makeAction = menu_channel_speed_config;

    item_speed_bar.up = &item_speed_bar;
    item_speed_bar.down = &item_speed_bar;
    strncpy(item_speed_bar.name, "speed config", 16);
    item_speed_bar.makeAction = 0;
    item_speed_bar.updateScreen_up = print_speed_up;
    item_speed_bar.updateScreen_down = print_speed_down;
}
// ----------------------------------------------------------------------------------------
void up(void)
{
    if (currentItem->up)
    {
        currentItem = currentItem->up;
        if (currentItem->updateScreen_up)
        {
            currentItem->updateScreen_up();
        }
    }
}
// ----------------------------------------------------------------------------------------
void down(void)
{
    if (currentItem->down)
    {
        currentItem = currentItem->down;
        if (currentItem->updateScreen_down)
        {
            currentItem->updateScreen_down();
        }
    }
}
// ----------------------------------------------------------------------------------------
void enter(void)
{
    bool status = true;
    int current_count = 0;

    MenuItem_t *currentItem_buff = currentItem;

    if ((currentItem_buff->up) != 0) // Step up if existing up
    {
        do
        {
            currentItem_buff = currentItem_buff->up; // Step up to the top of menu
            current_count++;
        } while ((currentItem_buff->up) != 0);       // If the top of item doesen't found
    }
    // Якщо є функція "makeAction" тоді виконати її
    if (currentItem->makeAction)
    {
        currentItem->makeAction((void *)current_count);
        status = false;
    }
    // Якщо є перехід на  "child" і "makeAction" не була виконана, тоді виконати перехід
    if ((currentItem->child) && (status == true))
    {
        currentItem = currentItem->child;
        action();
    }
}

void home(void)
{
    if (currentItem->parent)
    {
        // Return to first item of current menu
        currentItem = currentItem->parent; // Set global pointer on first menu
        action();                       // Print items on OLED
    }
    else
    {
        return_from_menu(NULL);
    }
}
// ----------------------------------------------------------------------------------------
void menu(void)
{
    Menu_Init(); // Init all structures

    menu_api_clear_buf();
    menu_api_set_font("u8g2_font_t0_11b_tf");

    print_menu_init(); // Print start menu and scrolingbar

    HAL_Delay(10);

    uint32_t tick = HAL_GetTick();

    while (1)
    {
        if (tick + 500 < HAL_GetTick())
        {
            HAL_GPIO_TogglePin(LED_SYS_GPIO_Port, LED_SYS_Pin);
            tick = HAL_GetTick();
        }
        button_maintain();
        HAL_Delay(20);
        ULOG_DEBUG("TIM14: %d TIM16: %d", TIM14->CNT, TIM16->CNT);
    }
}

// ---------------------------------------------------------------------------------
