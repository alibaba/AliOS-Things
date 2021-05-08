/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv/efuse.h
 * @brief    Header File for EFUSE Driver
 * @version  V1.0
 * @date     22. Mar 2020
 * @model    efuse
 ******************************************************************************/
#ifndef _DEV_EFUSEC_H_
#define _DEV_EFUSEC_H_

#include <stdint.h>
#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t            start;              ///< Efuse start address
    uint32_t            end;                ///< Efuse end address
} csi_efuse_info_t;

typedef struct {
    csi_dev_t           dev;
    csi_efuse_info_t    info;
} csi_efuse_t;

/**
  \brief       Initialize EFUSEC Interface. 1. Initializes the resources needed for the EFUSEC interface
  \param[in]   idx  Device id
  \return      Error code
*/
csi_error_t drv_efuse_init(csi_efuse_t *efuse, int32_t idx);

/**
  \brief       De-initialize EFUSEC Interface. stops operation and releases the software resources used by the interface
  \param[in]   efuse  Efuse efuse to operate.
  \return      None
*/
void drv_efuse_uninit(csi_efuse_t *efuse);

/**
  \brief       Read data from Efuse.
  \param[in]   efuse Efuse handle to operate.
  \param[in]   addr  Data address.
  \param[out]  data  Pointer to a buffer storing the data read from Efuse.
  \param[in]   size  Number of data items to read.
  \return      Number of data items read or error code
*/
int32_t drv_efuse_read(csi_efuse_t *efuse, uint32_t addr, void *data, uint32_t size);

/**
  \brief       Program data to Efuse.
  \param[in]   efuse Efuse handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Efuse.
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or error code
*/
int32_t drv_efuse_program(csi_efuse_t *efuse, uint32_t addr, const void *data, uint32_t size);

/**
  \brief       Get Efuse information.
  \param[in]   efuse  Efuse handle to operate.
  \param[out]  info   Efuse info \refs csi_efuse_info_t.
  \return      Error code
*/
csi_error_t drv_efuse_get_info(csi_efuse_t *efuse, csi_efuse_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_EFUSEC_H_ */
