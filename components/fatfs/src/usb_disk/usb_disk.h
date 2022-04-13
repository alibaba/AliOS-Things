/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MSD_DISKIO_H_
#define _MSD_DISKIO_H_

#include "usb_host_config.h"
#include "usb_host.h"
#include "usb_host_msd.h"
#include "diskio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief mass storage read/write retry time */
#define USB_HOST_FATFS_RW_RETRY_TIMES                   (2U)

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief fatfs call this function to initialize physical disk.
 *
 * @param pdrv   Physical drive number.
 *
 * @retval 0x00    success.
 */
extern DSTATUS USB_HostMsdInitializeDisk(BYTE pdrv);

/*!
 * @brief fatfs call this function to get physical disk status.
 *
 * @param pdrv           Physical drive number.
 *
 * @retval 0x00    OK.
 */
extern DSTATUS USB_HostMsdGetDiskStatus(BYTE pdrv);

/*!
 * @brief fatfs call this function to write data to physical disk.
 *
 * @param pdrv           Physical drive number.
 * @param buff           Pointer to the data buffer to store read data.
 * @param sector         Start sector number.
 * @param count          Number of sectors to read.
 *
 * @retval RES_PARERR    parameter error.
 * @retval RES_ERROR     usb stack driver error.
 * @retval RES_NOTRDY    read disk error.
 */
extern DRESULT USB_HostMsdReadDisk(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);

/*!
 * @brief fatfs call this function to write data to physical disk.
 *
 * @param pdrv           Physical drive number.
 * @param buff           Pointer to the data buffer to be written.
 * @param sector         Start sector number.
 * @param count          Number of sectors to read.
 *
 * @retval RES_PARERR    parameter error.
 * @retval RES_ERROR     usb stack driver error.
 * @retval RES_NOTRDY    write disk error.
 */
extern DRESULT USB_HostMsdWriteDisk(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);

/*!
 * @brief fatfs call this function to write data to physical disk.
 *
 * @param pdrv           Physical drive number.
 * @param cmd            ioctl command, please reference to diskio.h
 * @param buff           Parameter or data buffer.
 *
 * @retval RES_PARERR    parameter error.
 * @retval RES_ERROR     usb stack driver error.
 * @retval RES_NOTRDY    write disk error.
 */
extern DRESULT USB_HostMsdIoctlDisk(BYTE pdrv, BYTE cmd, void *buff);

#endif /* _MSD_DISKIO_H_ */

