/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     drv_norflash.h
 * @brief    header file for norflash driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CSI_NORFLASH_H_
#define _CSI_NORFLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <drv_common.h>


/// definition for norflash handle.
typedef void *norflash_handle_t;

/**
\brief Flash information
*/
typedef struct {
    uint32_t          start;              ///< Chip Start address
    uint32_t          end;                ///< Chip End address (start+size-1)
    uint32_t          sector_count;       ///< Number of sectors
    uint32_t          sector_size;        ///< Uniform sector size in bytes (0=sector_info used)
    uint32_t          page_size;          ///< Optimal programming page size in bytes
    uint32_t          program_unit;       ///< Smallest programmable unit in bytes
    uint8_t           erased_value;       ///< Contents of erased memory (usually 0xFF)
} norflash_info;

/**
\brief Flash Status
*/
typedef struct {
    uint32_t busy  : 1;                   ///< Flash busy flag
    uint32_t error : 1;                   ///< Read/Program/Erase error flag (cleared on start of next operation)
} norflash_status_t;

/****** NORFLASH Event *****/
typedef enum {
    NORFLASH_EVENT_READY           = 0,  ///< Flash Ready
    NORFLASH_EVENT_ERROR              ,  ///< Read/Program/Erase Error
} norflash_event_e;

typedef void (*norflash_event_cb_t)(int32_t idx, norflash_event_e event);   ///< Pointer to \ref norflash_event_cb_t : NORFLASH Event call back.

/**
\brief Flash Driver Capabilities.
*/
typedef struct {
    uint32_t event_ready  : 1;            ///< Signal Flash Ready event
    uint32_t data_width   : 2;            ///< Data width: 0=8-bit, 1=16-bit, 2=32-bit
    uint32_t erase_chip   : 1;            ///< Supports EraseChip operation
} norflash_capabilities_t;

// Function documentation

/**
  \brief       Get driver capabilities.
  \param[in]   handle norflash handle to operate.
  \return      \ref norflash_capabilities_t
*/
norflash_capabilities_t csi_norflash_get_capabilities(int32_t idx);

/**
  \brief       Initialize NORFLASH Interface. 1. Initializes the resources needed for the NORFLASH interface 2.registers event callback function
  \param[in]   idx  device id
  \param[in]   cb_event  Pointer to \ref norflash_event_cb_t
  \return      pointer to norflash handle
*/
norflash_handle_t csi_norflash_initialize(int32_t idx, norflash_event_cb_t cb_event);

/**
  \brief       control norflash power.
  \param[in]   handle  norflash handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_norflash_power_control(norflash_handle_t handle, csi_power_stat_e state);

/**
  \brief       De-initialize NORFLASH Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  norflash handle to operate.
  \return      error code
*/
int32_t csi_norflash_uninitialize(norflash_handle_t handle);

/**
  \brief       Read data from Flash.
  \param[in]   handle  norflash handle to operate.
  \param[in]   addroffset  read address offset.
  \param[out]  dstbuf  Pointer to a buffer storing the data read from Flash.
  \param[in]   len   Number of data items to read.
  \return      number of data items read or error code
*/

int32_t csi_norflash_read(norflash_handle_t handle, uint32_t addroffset, void *dstbuf, uint32_t len);

/**
  \brief       Read id num from Flash.
  \param[in]   handle  norflash handle to operate.
  \param[in]   id  Pointer to a buffer storing the id num.
  \return      error code
*/
int32_t csi_norflash_readid(norflash_handle_t handle, void *id);

/**
  \brief       Program data to Flash.
  \param[in]   handle  norflash handle to operate.
  \param[in]   addroffset  program  address offset.
  \param[in]   srcbuf  Pointer to a buffer containing the data to be programmed to Flash..
  \param[in]   len   Number of data items to program.
  \return      number of data items programmed or error code
*/
int32_t csi_norflash_write(norflash_handle_t handle, uint32_t addroffset, const void *srcbuf, uint32_t len);

/**
  \brief       Erase Flash Sector.
  \param[in]   handle  norflash handle to operate.
  \param[in]   addroffset  erase address offset
  \param[in]   num     erase sector num
  \return      error code
*/
int32_t csi_norflash_erase_sector(norflash_handle_t handle, uint32_t addroffset, uint32_t num);

/**
  \brief       Erase complete Flash.
  \param[in]   handle  norflash handle to operate.
  \return      error code
*/
int32_t csi_norflash_erase_chip(norflash_handle_t handle);

/**
  \brief       Get Flash information.
  \param[in]   handle  norflash handle to operate.
  \return      Pointer to Flash information \ref norflash_info
*/
norflash_info *csi_norflash_get_info(norflash_handle_t handle);

/**
  \brief       Get NORFLASH status.
  \param[in]   handle  norflash handle to operate.
  \return      NORFLASH status \ref norflash_status_t
*/
norflash_status_t csi_norflash_get_status(norflash_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_NORFLASH_H_ */
