/**
 * @file sfud_cfg.h
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

#ifndef __SFUD_CFG_H
#define __SFUD_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

#define SFUD_DEBUG_MODE

#define SFUD_USING_SFDP

#define SFUD_USING_FLASH_INFO_TABLE

enum {
    SFUD_SST25_DEVICE_INDEX = 0,
};

#define SFUD_FLASH_DEVICE_TABLE                                                \
{                                                                              \
    [SFUD_SST25_DEVICE_INDEX] = {.name = "W25Q128BV", .spi.name = "SPI1"},       \
}

#ifdef __cplusplus
}
#endif

#endif /* __SFUD_CFG_H */
