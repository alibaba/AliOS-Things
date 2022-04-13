/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include <string.h>
#include "aos_debug.h"
#include <k_api.h>
#include "common.h"
#include "aos_debug.h"
#include "board.h"
#include "mt_uart.h"
#include <sysregs.h>
#include <arch_timer.h>




/* UART runtime information --------------------*/
extern mtk_uart_setting uart_setting[DRV_SUPPORT_UART_PORTS];
extern const UINT32 UART_BaseAddr[DRV_SUPPORT_UART_PORTS];
extern const UINT32 UART_IRQ_NUM[DRV_SUPPORT_UART_PORTS];


extern int32_t g_cli_direct_read;

OSStatus platform_uart_init( uint8_t port, const UART_dcb_struct* config)
{
  	OSStatus err = kNoErr;
	/*perhaps init and request console irq earlier*/
	uart_init_early();
	if(g_cli_direct_read == 0){
		enable_console_uart_interrupt();
		memset(&uart_setting[port], 0x0, sizeof(uart_setting[port]));
		uart_hw_init(port, &(uart_setting[port]));
		uart_set_dcb_config(&(uart_setting[port]), config);
	}

  	return err;
}

void platform_uart_deinit( uint8_t port)
{
	mtk_uart_setting *u_setting = &(uart_setting[port]);
  
#ifdef CONFIG_AOS_SEM
  	aos_sem_free( &u_setting->rx_complete );
  	aos_sem_free( &u_setting->tx_complete );
  	aos_mutex_free( &u_setting->tx_mutex );
   	aos_sem_free( &u_setting->sem_wakeup);
#else
  	u_setting->rx_complete = false;
  	u_setting->tx_complete = true;
#endif
  	u_setting->rx_size              = 0;
  	u_setting->tx_size              = 0;
  	u_setting->last_transmit_result = kNoErr;
  	u_setting->last_receive_result  = kNoErr;
	free(&(u_setting->rx_buffer.buffer));

	memset(&uart_setting[port], 0x0, sizeof(uart_setting[port]));

}

OSStatus platform_uart_transmit_bytes( UINT8 port, const UINT8* data_out, UINT32 size )
{
  	OSStatus err = kNoErr;
	UINT32 idx =0;
	UINT32 uart_base = UART_BaseAddr[port];
	
	for (idx; idx < size; idx++)
		mtk_uart_putc(uart_base, *(data_out+idx));
  	return err;
}

OSStatus platform_uart_receive_bytes( UINT8 port, UINT8* data_in, UINT32 expected_data_size, UINT32 timeout_ms )
{
  	OSStatus err = kNoErr;
	mtk_uart_setting*  driver = &uart_setting[port];
	while (expected_data_size != 0){
		UINT32 transfer_size = MIN( driver->rx_buffer.size/2, expected_data_size );

		/* Check if ring buffer already contains the required amount of data. */
		if ( transfer_size > ring_buffer_used_space( &(driver->rx_buffer))){
			/* Set rx_size and wait in rx_complete semaphore until data reaches rx_size or timeout occurs */
			driver->rx_size = transfer_size;
#ifdef CONFIG_AOS_SEM
			if ( aos_sem_wait( &driver->rx_complete, timeout_ms) != kNoErr ){
  				driver->rx_size = 0;
  				return kTimeoutErr;
			}
#else
			driver->rx_complete = false;
			u64 delay_start;
			u64 delay_cur;
			arm_read_sysreg(CNTPCT, delay_start);
			/* rx_complete volatile: may changed by rx handler*/
			while(driver->rx_complete == false){
				arm_read_sysreg(CNTPCT, delay_cur);
					if(delay_cur >= delay_start + timeout_ms){
    					driver->rx_size = 0;
    					return kTimeoutErr;
  				}
			}
#endif
	 
			/* Reset rx_size to prevent semaphore being set while nothing waits for the data */
			driver->rx_size = 0;
		}
		expected_data_size -= transfer_size;

		// Grab data from the buffer
		do{
		UINT8* available_data;
		UINT32 bytes_available;

		ring_buffer_get_data(&(driver->rx_buffer), &available_data, &bytes_available );
		bytes_available = MIN( bytes_available, transfer_size );
		memcpy( data_in, available_data, bytes_available );
		transfer_size -= bytes_available;
		data_in = ( (UINT8*) data_in + bytes_available );
		ring_buffer_consume( &(driver->rx_buffer), bytes_available );
		} while ( transfer_size != 0 );
	}

	if ( expected_data_size != 0 )
		return kGeneralErr;
 	return err;
}

