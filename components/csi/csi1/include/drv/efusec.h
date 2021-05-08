/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv_efusec.h
 * @brief    header file for efusec driver
 * @version  V1.0
 * @date     22. Mar 2019
 * @model    efusec
 ******************************************************************************/
#ifndef _CSI_EFUSEC_H_
#define _CSI_EFUSEC_H_


#include <stdint.h>
#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/// definition for efusec handle.
typedef void *efusec_handle_t;

/**
\brief Flash information
*/
typedef struct {
    uint32_t          start;              ///< Chip Start address
    uint32_t          end;                ///< Chip End address (start+size-1)
    uint32_t          page_size;          ///< Optimal programming page size in bytes
    uint32_t          program_unit;       ///< Smallest programmable unit in bytes
} efusec_info_t;

/**
\brief Flash Status
*/
typedef struct {
    uint32_t mode  : 1;             ///< 0: read mode 1: program mode
    uint32_t lock_shadow : 1;       ///< lock shadow regs
    uint32_t lock_efuse : 1;        ///< lock efusec
} efusec_status_t;

typedef enum {
    EFUSEC_LOCK_SHADOW             = 0,    ///< all shadow regs can’t be programmed
    EFUSEC_LOCK_EFUSE                      ///< all Efuse can’t be programmed
} efusec_lock_e;

/**
\brief Flash Driver Capabilities.
*/
typedef struct {
    uint32_t lock_shadow   : 1;            ///< Supports lock shadow operation
} efusec_capabilities_t;

// Function documentation

/**
  \brief       Initialize EFUSEC Interface. 1. Initializes the resources needed for the EFUSEC interface
  \param[in]   idx  device id
  \return      pointer to efusec handle
*/
efusec_handle_t drv_efusec_initialize(int32_t idx);

/**
  \brief       De-initialize EFUSEC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  efusec handle to operate.
  \return      error code
*/
int32_t csi_efusec_uninitialize(efusec_handle_t handle);

/**
  \brief       Get driver capabilities.
  \param[in]   idx  device id
  \return      \ref efusec_capabilities_t
*/
efusec_capabilities_t drv_efusec_get_capabilities(int32_t idx);

/**
  \brief       control efusec power.
  \param[in]   handle  efusec handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t drv_efusec_power_control(efusec_handle_t handle, csi_power_stat_e state);

/**
  \brief       Read data from Flash.
  \param[in]   handle  efusec handle to operate.
  \param[in]   addr  Data address.
  \param[out]  data  Pointer to a buffer storing the data read from Flash.
  \param[in]   cnt   Number of data items to read.
  \return      number of data items read or error code
*/
int32_t drv_efusec_read(efusec_handle_t handle, uint32_t addr, void *data, uint32_t cnt);

/**
  \brief       Program data to Flash.
  \param[in]   handle  efusec handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash.
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or error code
*/
int32_t drv_efusec_program(efusec_handle_t handle, uint32_t addr, const void *data, uint32_t cnt);

/**
  \brief      lock efusec.
  \param[in]   handle  efusec handle to operate.
  \param[in]   lock   efusec lock type \ref efusec_lock_e.
  \return      error code
*/
int32_t drv_efusec_lock(efusec_handle_t handle, efusec_lock_e lock);

/**
  \brief       Get Flash information.
  \param[in]   handle  efusec handle to operate.
  \return      Pointer to Flash information \ref efusec_info_t
*/
efusec_info_t *drv_efusec_get_info(efusec_handle_t handle);
/**
  \brief       Get FLASH status.
  \param[in]   handle  efusec handle to operate.
  \return      EFUSEC status \ref efusec_status_t
*/
efusec_status_t drv_efusec_get_status(efusec_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_EFUSEC_H_ */
