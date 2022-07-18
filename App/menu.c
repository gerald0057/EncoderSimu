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
#include "u8g2.h"
#include "ulog.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------------------------
// Main struct menu
typedef struct Struct
{
    struct Struct *up;     // pointer on up element of list
    struct Struct *down;   // pointer on down element of list
    struct Struct *child;  // Pointer on child menu
    struct Struct *parent; // Pointer on parent menu

    uint8_t id;                      // Number of menu
    char *name;                      // Name menu
    void (*updateScreen_up)(void);   // Function print graphic on OLED
    void (*updateScreen_down)(void); // Function print graphic on OLED
    void (*makeAction)(void);
} MenuItem_t;

extern u8g2_t u;

// How many menu items in menu
#define MENU_ITEM_NUM 3
#define MENU_1_ITEM_NUM 6
#define MENU_2_ITEM_NUM 3
#define MENU_3_ITEM_NUM 2
#define MENU_4_ITEM_NUM 2

// Create menu item array structure for all menus
MenuItem_t items[MENU_ITEM_NUM];
MenuItem_t items_menu_1[MENU_1_ITEM_NUM];
MenuItem_t items_menu_2[MENU_2_ITEM_NUM];
MenuItem_t items_menu_3[MENU_3_ITEM_NUM];
MenuItem_t items_menu_4[MENU_4_ITEM_NUM];

MenuItem_t *currentItem = &items[0]; // Create and set pointer on first element of list

char str_pointer[4] = "->"; // How look pointer on menu item

// Rows coordinates
uint16_t first_menu_row = 16 + 8;
uint16_t second_menu_row = 28 + 8;
uint16_t third_menu_row = 40 + 8;
uint16_t fourth_menu_row = 52 + 8;
uint8_t row_step = 12;
uint16_t start_print_id_menu_x = 15;
uint16_t start_print_name_menu_x = 30;

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

    uint8_t menu_items_counter = 1;

    // Знайти скільки емементві в меню. Відповідно до кількості елементів вирахоується довжини скролбару
    // 1. Скопіювати показник на меню в буффер
    // 2. Піднятися до останнього елемента меню
    // 3. Інкрементувати лічильник "menu_items_counter" елементів меню, до останього елемента меню

    MenuItem_t *currentItem_buff = currentItem;

    if ((currentItem_buff->up) != 0) // Step up if existing up
    {
        do
        {
            currentItem_buff = currentItem_buff->up; // Step up to the top of menu
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
    // Очистити частину керану де є скролбар
    u8g2_SetDrawColor(&u, 0);
    u8g2_DrawBox(&u, start_x_scrollbar, start_y_scrollbar, active_width, lenght_all_scrollbar - 2);
    u8g2_SetDrawColor(&u, 1);

    // Вивести подовгастий квадрат на край екрану
    u8g2_DrawFrame(&u, start_x_scrollbar - 1, start_y_scrollbar - 1, active_width + 1, 64 - start_y_scrollbar);

    // Print scroling line
    int id_for_line = currentItem->id; // Скопіювати порядковий номер меню
    // Print scroling line
    uint16_t line_lenght = (lenght_all_scrollbar / menu_items_counter + 1); // Довжина лінії яка відповідає одному меню
    uint16_t start_lenght = 16 + ((id_for_line - 1) * line_lenght); // Початок лінії
                                                                    // Print active scrollbar part (line)
    u8g2_DrawBox(&u, start_x_scrollbar, start_lenght, active_width, line_lenght);
}
// ----------------------------------------------------------------------------------------
void print_rectangle_on_head(void)
{
    char str[20] = ">> MAIN MENU <<";

    u8g2_ClearBuffer(&u);

    u8g2_SetFont(&u, u8g2_font_t0_11b_tf);
    u8g2_DrawStr(&u, 10, 11, str);
    u8g2_DrawFrame(&u, 0, 0, 127, 15);
    u8g2_SendBuffer(&u);
}
// ----------------------------------------------------------------------------------------
// Clear some menu items.
void clear_menu_items(bool first, bool second, bool third, bool fourth)
{
    uint8_t start_row_x = 15;
    char str[16] = "              "; // Must be 15

    if (first == true)
    {
        u8g2_DrawStr(&u, start_row_x, first_menu_row, str);
    }
    if (second == true)
    {
        u8g2_DrawStr(&u, start_row_x, second_menu_row, str);
    }
    if (first == true)
    {
        u8g2_DrawStr(&u, start_row_x, third_menu_row, str);
    }
    if (fourth == true)
    {
        u8g2_DrawStr(&u, start_row_x, fourth_menu_row, str);
    }
    u8g2_SendBuffer(&u);
}
// ----------------------------------------------------------------------------------------
void print_rows_on_oled_if_up(void)
{
    char str[16] = {0};

    clear_menu_items(true, true, true, true);

    print_rectangle_on_head();

    // Print pointer on first item menu
    u8g2_DrawStr(&u, 0, first_menu_row, str_pointer);

    MenuItem_t *currentItem_buff_up = currentItem; // Create buffer on selected current item pointer.
    for (uint8_t row = first_menu_row; row <= fourth_menu_row; row = row + row_step)
    {
        // Print number of menu item
        itoa(currentItem_buff_up->id, str, 10);
        u8g2_DrawStr(&u, start_print_id_menu_x, row, str);

        // Print name of menu item
        strncpy(str, currentItem_buff_up->name, 15);
        u8g2_DrawStr(&u, start_print_name_menu_x, row, str);

        currentItem_buff_up = currentItem_buff_up->down; // Make a step down

        // Print only existing items
        if (currentItem_buff_up == 0) // If no next item
        {
            break;
        }
    }
    scroll_bar();
    u8g2_SendBuffer(&u);
}
// ----------------------------------------------------------------------------------------
void print_rows_on_oled_if_down(void) // print text menu item
{
    char str[16] = {0};

    clear_menu_items(true, true, true, true);

    print_rectangle_on_head();

    // Print pointer on first item menu
    u8g2_DrawStr(&u, 0, first_menu_row, str_pointer);

    MenuItem_t *currentItem_buff = currentItem;
    for (uint8_t row = first_menu_row; row <= fourth_menu_row; row = row + row_step)
    {
        // Print number of menu item
        itoa(currentItem_buff->id, str, 10);
        u8g2_DrawStr(&u, start_print_id_menu_x, row, str);

        // Print name of menu item
        memset(str, 0, sizeof(str));
        strncpy(str, currentItem_buff->name, 15);
        u8g2_DrawStr(&u, start_print_name_menu_x, row, str);

        currentItem_buff = currentItem_buff->down; // Make a step down

        // Print only existing items
        if (currentItem_buff == 0) // If no next item
        {
            break;
        }
    }
    scroll_bar();
    u8g2_SendBuffer(&u);
}
// ----------------------------------------------------------------------------------------
// Print first 4 items of menu (only first time)
void print_menu_init(void)
{
    MenuItem_t *currentItem_buff = currentItem;

    print_rectangle_on_head();

    // Print selected name of menu
    char str[20] = {0};

    for (uint8_t row = first_menu_row; row <= fourth_menu_row; row = row + row_step)
    {
        if (row == first_menu_row)
        {
            // Print pointer on menu (On top)
            char str_pointer[4] = "->";
            u8g2_DrawStr(&u, 0, row, str_pointer);
        }
        // Print number of menu item
        itoa(currentItem_buff->id, str, 10);
        u8g2_DrawStr(&u, start_print_id_menu_x, row, str);
        memset(str, 0, sizeof(str));

        // Print menu of menu item
        strncpy(str, currentItem_buff->name, 15);
        u8g2_DrawStr(&u, start_print_name_menu_x, row, str);

        currentItem_buff = currentItem_buff->down;

        if (currentItem_buff == 0) // If no menu item
        {
            break;
        }
    }
    scroll_bar();
    u8g2_SendBuffer(&u);
}
// ----------------------------------------------------------------------------------------
void action(void)
{
    char str[16] = {0};

    print_rectangle_on_head();

    // Print selected name of menu on top of OLED
    MenuItem_t *currentItem_buff_parent = currentItem;
    currentItem_buff_parent = currentItem_buff_parent->parent;

    strncpy(str, currentItem_buff_parent->name, 15);
    u8g2_DrawStr(&u, 10, 3, str);
    memset(str, 0, sizeof(str));

    MenuItem_t *currentItem_buff = currentItem;

    for (uint8_t row = first_menu_row; row <= fourth_menu_row; row = row + row_step)
    {
        if (row == first_menu_row)
        {
            // Print pointer on menu
            char str_pointer[4] = "->";
            u8g2_DrawStr(&u, 0, row, str_pointer);
        }
        // Print number of menu item
        itoa(currentItem_buff->id, str, 10);
        u8g2_DrawStr(&u, start_print_id_menu_x, row, str);

        // Print menu of menu item
        strncpy(str, currentItem_buff->name, 15);
        u8g2_DrawStr(&u, start_print_name_menu_x, row, str);

        currentItem_buff = currentItem_buff->down;
        if (currentItem_buff == 0)
        {
            break;
        }
    }
    scroll_bar();
    u8g2_SendBuffer(&u);
}
// ----------------------------------------------------------------------------------------
void return_from_menu(void)
{
    currentItem = &items[0]; // Jump to main menu

    // Print "MAIN MENU:"
    char str[20] = ">> MAIN MENU <<";
    u8g2_DrawStr(&u, 10, 3, str);

    print_menu_init(); // Print all start menu
}
// ----------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void do_it_function_menu_1(void)
{

    // Print selected name of menu
    char str[20] = {0};
    strncpy(str, currentItem->name, 15);
    u8g2_DrawStr(&u, 10, 11, str);
    memset(str, 0, sizeof(str));

    strncpy(str, "Doing something 1", sizeof(str));
    u8g2_DrawStr(&u, 0, first_menu_row, str);

    // Return to first item of current menu
    currentItem = &items_menu_1[0]; // Set global pointer on first menu
    action();                       // Print items on OLED
}
// ----------------------------------------------------------------------------------------
void items_menu_1_set_par_1(void)
{

    // Print selected name of menu
    char str[20] = {0};
    static bool enabled = false;
    strncpy(str, currentItem->name, 15);
    u8g2_DrawStr(&u, 10, 11, str);
    memset(str, 0, sizeof(str));

    enabled = !enabled;

    if (enabled)
    {
        strncpy(str, "Channel 1     [x]", sizeof(str));
    }
    else
    {
        strncpy(str, "Channel 1     [ ]", sizeof(str));
    }
    u8g2_DrawStr(&u, start_print_name_menu_x, first_menu_row, str);


    // Return to first item of current menu
    currentItem = &items_menu_1[0]; // Set global pointer on first menu
    action();                       // Print items on OLED
}
// ----------------------------------------------------------------------------------------
void items_menu_1_set_par_2(void)
{

    // Print selected name of menu
    char str[20] = {0};
    strncpy(str, currentItem->name, 15);
    u8g2_DrawStr(&u, 10, 3, str);
    memset(str, 0, sizeof(str));

    strncpy(str, "Set parametr 2", sizeof(str));
    u8g2_DrawStr(&u, 0, first_menu_row, str);

    // Return to first item of current menu
    currentItem = &items_menu_1[0]; // Set global pointer on first menu
    action();                       // Print items on OLED
}
// ----------------------------------------------------------------------------------------
void items_menu_1_set_par_3(void)
{

    // Print selected name of menu
    char str[20] = {0};
    strncpy(str, currentItem->name, 15);
    u8g2_DrawStr(&u, 10, 3, str);
    memset(str, 0, sizeof(str));

    strncpy(str, "Set parametr 3", sizeof(str));
    u8g2_DrawStr(&u, 0, first_menu_row, str);


    // Return to first item of current menu
    currentItem = &items_menu_1[0]; // Set global pointer on first menu
    action();                       // Print items on OLED
}
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void do_it_function_menu_2(void)
{

    // Print selected name of menu
    char str[20] = {0};
    strncpy(str, currentItem->name, 15);
    u8g2_DrawStr(&u, 10, 3, str);
    memset(str, 0, sizeof(str));

    strncpy(str, "Doing something 2", sizeof(str));
    u8g2_DrawStr(&u, 0, first_menu_row, str);


    // Return to first item of current menu
    currentItem = &items_menu_2[0]; // Set global pointer on first menu
    action();                       // Print items on OLED
}
// ----------------------------------------------------------------------------------------
void items_menu_2_set_par_1(void)
{

    // Print selected name of menu
    char str[20] = {0};
    strncpy(str, currentItem->name, 15);
    u8g2_DrawStr(&u, 10, 3, str);
    memset(str, 0, sizeof(str));

    strncpy(str, "menu 2", sizeof(str));
    u8g2_DrawStr(&u, 0, first_menu_row, str);


    // Return to first item of current menu
    currentItem = &items_menu_2[0]; // Set global pointer on first menu
    action();                       // Print items on OLED
}
// ----------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void do_it_function_menu_3(void)
{

    // Print selected name of menu
    char str[20] = {0};
    strncpy(str, currentItem->name, 15);
    u8g2_DrawStr(&u, 10, 3, str);
    memset(str, 0, sizeof(str));

    strncpy(str, "Doing something 3", sizeof(str));
    u8g2_DrawStr(&u, 0, first_menu_row, str);


    // Return to first item of current menu
    currentItem = &items_menu_3[0]; // Set global pointer on first menu
    action();                       // Print items on OLED
}

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
void Menu_Init(void)
{
    // Make pointers on funsctions
    // Main functions
    void (*p_print_rows_on_oled_if_up)(void);
    p_print_rows_on_oled_if_up = print_rows_on_oled_if_up;

    void (*p_print_rows_on_oled_if_down)(void);                // Create pointer on function
    p_print_rows_on_oled_if_down = print_rows_on_oled_if_down; // Save function print on pointer print_p

    void (*p_return_from_menu)(void);
    p_return_from_menu = return_from_menu;

    // items_menu_1 menu functions
    void (*p_do_it_function_menu_1)(void); // Function "Do it". Works when select it
    p_do_it_function_menu_1 = do_it_function_menu_1;

    void (*p_items_menu_1_set_par_1)(void);
    p_items_menu_1_set_par_1 = items_menu_1_set_par_1;

    void (*p_items_menu_1_set_par_2)(void);
    p_items_menu_1_set_par_2 = items_menu_1_set_par_2;

    void (*p_items_menu_1_set_par_3)(void);
    p_items_menu_1_set_par_3 = items_menu_1_set_par_3;

    // items_menu_2 menu functions
    void (*p_do_it_function_menu_2)(void); // Function "Do it". Works when select it
    p_do_it_function_menu_2 = do_it_function_menu_2;

    void (*p_items_menu_2_set_par_1)(void);
    p_items_menu_2_set_par_1 = items_menu_2_set_par_1;

    // items_menu_3 menu functions
    void (*p_do_it_function_menu_3)(void); // Function "Do it". Works when select it
    p_do_it_function_menu_3 = do_it_function_menu_3;


    // Fill in elements(nodes) of list (7 items)
    // Main menu items
    /////////////////////////////////////////////////////////////////

    items[0].up = 0;
    items[0].down = &items[1];
    items[0].child = &items_menu_1[0];
    items[0].parent = 0;
    items[0].id = 1;
    items[0].name = "Channel Enable";
    items[0].updateScreen_up = p_print_rows_on_oled_if_up;
    items[0].updateScreen_down = p_print_rows_on_oled_if_down;
    items[0].makeAction = 0;

    items[1].up = &items[0];
    items[1].down = &items[2];
    items[1].child = &items_menu_2[0];
    items[1].parent = 0;
    items[1].id = 2;
    items[1].name = "Speed config";
    items[1].updateScreen_up = p_print_rows_on_oled_if_up;
    items[1].updateScreen_down = p_print_rows_on_oled_if_down;
    items[1].makeAction = 0;

    items[2].up = &items[1];
    items[2].down = 0;
    items[2].child = &items_menu_3[0];
    items[2].parent = 0;
    items[2].id = 3;
    items[2].name = "Mode";
    items[2].updateScreen_up = p_print_rows_on_oled_if_up;
    items[2].updateScreen_down = p_print_rows_on_oled_if_down;
    items[2].makeAction = 0;

    ///////////////////////////////////////////////////////////////////
    // Creating next menu
    items_menu_1[0].up = 0;
    items_menu_1[0].down = &items_menu_1[1];
    items_menu_1[0].id = 1;
    items_menu_1[0].child = 0;
    items_menu_1[0].parent = &items[0];
    items_menu_1[0].name = "Channel-1   [ ]";
    items_menu_1[0].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_1[0].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_1[0].makeAction = p_items_menu_1_set_par_1;

    items_menu_1[1].up = &items_menu_1[0];
    items_menu_1[1].down = &items_menu_1[2];
    items_menu_1[1].child = 0;
    items_menu_1[1].parent = &items[0];
    items_menu_1[1].id = 2;
    items_menu_1[1].name = "Channel-2   [ ]";
    items_menu_1[1].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_1[1].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_1[1].makeAction = p_items_menu_1_set_par_2;

    items_menu_1[2].up = &items_menu_1[1];
    items_menu_1[2].down = &items_menu_1[3];
    items_menu_1[2].child = 0;
    items_menu_1[2].parent = &items[0];
    items_menu_1[2].id = 3;
    items_menu_1[2].name = "Channel-3   [ ]";
    items_menu_1[2].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_1[2].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_1[2].makeAction = p_items_menu_1_set_par_3;

    items_menu_1[3].up = &items_menu_1[2];
    items_menu_1[3].down = &items_menu_1[4];
    items_menu_1[3].child = 0;
    items_menu_1[3].parent = &items[0];
    items_menu_1[3].id = 3;
    items_menu_1[3].name = "Channel-4   [ ]";
    items_menu_1[3].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_1[3].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_1[3].makeAction = p_items_menu_1_set_par_3;

    items_menu_1[4].up = &items_menu_1[3];
    items_menu_1[4].down = &items_menu_1[5];
    items_menu_1[4].child = 0;
    items_menu_1[4].parent = &items[0];
    items_menu_1[4].id = 4;
    items_menu_1[4].name = "DO IT"; // Name of item
    items_menu_1[4].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_1[4].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_1[4].makeAction = p_do_it_function_menu_1;

    items_menu_1[5].up = &items_menu_1[4];
    items_menu_1[5].down = 0;
    items_menu_1[5].child = 0;
    items_menu_1[5].parent = &items[0];
    items_menu_1[5].id = 5;
    items_menu_1[5].name = "EXIT"; // Name of item
    items_menu_1[5].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_1[5].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_1[5].makeAction = p_return_from_menu;

    ///////////////////////////////////////////////////////////////////
    // Creating next menu
    items_menu_2[0].up = 0;
    items_menu_2[0].down = &items_menu_2[1];
    items_menu_2[0].child = 0;
    items_menu_2[0].parent = &items[1];
    items_menu_2[0].id = 1;
    items_menu_2[0].name = "set par 1"; // Name of item
    items_menu_2[0].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_2[0].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_2[0].makeAction = p_items_menu_2_set_par_1;

    items_menu_2[1].up = &items_menu_2[0];
    items_menu_2[1].down = &items_menu_2[2];
    items_menu_2[1].child = 0;
    items_menu_2[1].parent = &items[1];
    items_menu_2[1].id = 2;
    items_menu_2[1].name = "DO IT"; // Name of item
    items_menu_2[1].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_2[1].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_2[1].makeAction = p_do_it_function_menu_2;

    items_menu_2[2].up = &items_menu_2[1];
    items_menu_2[2].down = 0;
    items_menu_2[2].child = 0;
    items_menu_2[2].parent = &items[1];
    items_menu_2[2].id = 3;
    items_menu_2[2].name = "EXIT"; // Name of item
    items_menu_2[2].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_2[2].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_2[2].makeAction = p_return_from_menu;

    ///////////////////////////////////////////////////////////////////
    // Creating next menu
    items_menu_3[0].up = 0;
    items_menu_3[0].down = &items_menu_3[1];
    items_menu_3[0].child = 0;
    items_menu_3[0].parent = &items[2];
    items_menu_3[0].id = 1;
    items_menu_3[0].name = "DO IT"; // Name of item
    items_menu_3[0].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_3[0].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_3[0].makeAction = p_do_it_function_menu_3;

    items_menu_3[1].up = &items_menu_3[0];
    items_menu_3[1].down = 0;
    items_menu_3[1].child = 0;
    items_menu_3[1].parent = &items[2];
    items_menu_3[1].id = 2;
    items_menu_3[1].name = "EXIT"; // Name of item
    items_menu_3[1].updateScreen_up = p_print_rows_on_oled_if_up;
    items_menu_3[1].updateScreen_down = p_print_rows_on_oled_if_down;
    items_menu_3[1].makeAction = p_return_from_menu;
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
    // Якщо є функція "makeAction" тоді виконати її
    if (currentItem->makeAction)
    {
        currentItem->makeAction();
        status = false;
    }
    // Якщо є перехід на  "child" і "makeAction" не була виконана, тоді виконати перехід
    if ((currentItem->child) && (status == true))
    {
        currentItem = currentItem->child;
        action();
    }
}
// ----------------------------------------------------------------------------------------
void menu(void)
{
    Menu_Init(); // Init all structures

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
    }
}

// ---------------------------------------------------------------------------------
