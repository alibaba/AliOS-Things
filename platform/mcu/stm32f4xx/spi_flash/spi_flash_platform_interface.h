/**
 ******************************************************************************
 * @file    spi_flash_platform_interface.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide all the headers of platform functions for spi
 *          flash driver
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#ifndef INCLUDED_SPI_FLASH_PLATFORM_INTERFACE_H
#define INCLUDED_SPI_FLASH_PLATFORM_INTERFACE_H

#include "common.h"

#ifdef __cplusplus
 extern "C" {
#endif


 typedef struct
 {
     /*@null@*/ /*@observer@*/  const void*   tx_buffer;
     /*@null@*/ /*@dependent@*/ void*         rx_buffer;
                                unsigned long length;
 } sflash_platform_message_segment_t;

extern int sflash_platform_init      ( /*@shared@*/ void* peripheral_id, /*@out@*/ void** platform_peripheral_out );
extern int sflash_platform_send_recv ( const void* platform_peripheral, /*@in@*/ /*@out@*/ sflash_platform_message_segment_t* segments, unsigned int num_segments  );
extern int sflash_platform_deinit    ( void );

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_SPI_FLASH_PLATFORM_INTERFACE_H */
