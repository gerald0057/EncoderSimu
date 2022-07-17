/**
 * @file task_protocol.c
 * @brief
 * @author gerald (hongyi.zhu@aurefluidics.com)
 * @version 1.0
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022  Shanghai AureFluidics Technology Co.,Ltd
 *
 * @par ChangeLog:
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-07-15     <td>1.0         <td>gerald      <td>the first version
 * </table>
 */

#include "main.h"
#include <stdlib.h>
#include <stdint.h>

void task_protocol(void const *parameter)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(LED_SYS_GPIO_Port, LED_SYS_Pin);
        HAL_Delay(500);
    }
}
