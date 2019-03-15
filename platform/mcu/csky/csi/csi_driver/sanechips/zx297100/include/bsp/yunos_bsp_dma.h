/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
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

#ifndef YUNOS_BSP_DMA_H
#define YUNOS_BSP_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
  DMA_STATE_FREE = 0,       ///< DMA not yet initialized or disabled
  DMA_STATE_READY,          ///< DMA process success and ready for use, but not start yet
  DMA_STATE_BUSY,           ///< DMA process is ongoing
  DMA_STATE_ERROR,          ///< DMA transfer error
  DMA_STATE_DONE,           ///< DMA transfer done
}dma_state_t;

/**
 * This function will init dma controller
 * @return      void
 */
void yunos_bsp_dma_init(void);

/**
 * This function will configure dma channel
 * @param[in]   ch          dma channel
 * @param[in]   psrcaddr    dma transfer source address
 * @param[in]   pstdaddr    dma transfer source address
 * @param[in]   length      dma transfer length 
 * @param[in]   dma_dir     dma transfer direction such as DMA_DIR_M_TO_M
 * @param[in]   callback    call back funtion after dma transfer completed or fail
 * @return      used dma channel
 */
int yunos_bsp_dma_func(int ch, void *psrcaddr, void *pstdaddr,
                                                          size_t length, uint8_t dma_dir, void (*callback)(int ));
/**
 * This function will get one free dma channel
 * @return      the channel index, others is -1, no channel can be used
 */
int yunos_bsp_dma_get_free_channel(void);

/**
 * This function will free dma channel and related resources
 * @param[in]   ch          dma channel
 * @return      the operation status, YUNOS_SUCCESS is OK, others is error
 */
int yunos_bsp_dma_release_channel(int ch);

/**
 * This function will start dma transfer. must configure dma(yunos_bsp_dma_config) before start
 * @param[in]   ch          dma channel
 * @return      the operation status, YUNOS_SUCCESS is OK, others is error
 */
int yunos_bsp_dma_start(int ch);

/**
 * This function will stop dma transfer.
 * @param[in]   ch          channel to be stopped
 * @return      the operation status, YUNOS_SUCCESS is OK, others is error
 */
int yunos_bsp_dma_stop(int ch);

/**
 * This function will get dma channel status
 * @param[in]   ch          channel to be stopped
 * @return      the dma state, refer to definition of dma_state_t
 */
dma_state_t yunos_bsp_dma_get_status(int ch);

/**
 * This function will set dma hand shake mode, soft or hardware
 * @param[in]   ch          dma channel 
 * @param[in]   Handshaking handshake mode
 * @param[in]   arg         argment
 * @return      the operation status, YUNOS_SUCCESS is OK, others is error
 */
int yunos_bsp_dma_set_handshaking(int ch, int Handshaking, int arg);

/**
 * This function will set transfer data with
 * @param[in]  ChannelId  dma channel
 * @param[in]  SWidth  Source width.
 * @param[in]  DWidth  Destination width.
 * @return the operation status, YUNOS_SUCCESS is OK, others is error
 */
 int yunos_bsp_dma_settransferwidth(int ChannelId, int SWidth, int DWidth);

/**
 * This function will Set the number of data items during a burst transfer.
 * @param[in]  ChannelId  dma channel
 * @param[in]  SBurstLength  number of data items
 * @return the operation status, YUNOS_SUCCESS is OK, others is error
 */
int yunos_bsp_dma_setburstlength(int ChannelId, int SBurstLength);
#ifdef __cplusplus
}
#endif

#endif /* YUNOS_BSP_DMA_H */

