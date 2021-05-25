/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_ETHERNET_API_H
#define MBED_ETHERNET_API_H

#include "device.h"

#if DEVICE_ETHERNET

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ethernet ETHERNET
 *  @ingroup    hal
 *  @brief      ETHERNET functions
 *  @{
 */

/** 
 *  @brief To initialize the Ethernet MAC controller.
 *
 *  @param  None
 *
 *  @returns    The result.
 */
int ethernet_init(void);

/** 
 *  @brief To de-initialize the Ethernet MAC controller.
 *
 *  @param None
 *
 *  @returns    void.
 */
void ethernet_free(void);

/** 
 *  @brief To write "size" bytes of data from "data" to the Tx packet buffer.
 *
 *  @param[in]  data The buffer of packet data.
 *  @param[in]  size The size of the packet data.
 *
 *  @returns    The number of bytes written, or (-1) if errors.
 */
int ethernet_write(const char *data, int size);

/** 
 *  @brief To send the packet from Tx packet buffer.
 *
 *  @param None
 *
 *  @returns    The packet size.
 */
int ethernet_send(void);

/** 
 *  @brief To receive a packet into the Rx packet buffer.
 *
 *  @param None
 *
 *  @returns    The packet size, or 0 if no packet received.
 */
int ethernet_receive(void);

/** 
 *  @brief To read packet data from Rx packet buffer to the "data" buffer.
 *
 *  @param[in]  data A buffer for the packet data.
 *  @param[in]  size The specified length (in bytes) to be read.
 *
 *  @returns    The actual size (in bytes) of data read.
 */
int ethernet_read(char *data, int size);

/** 
 *  @brief To get the ethernet MAC address.
 *
 *  @param[in]  mac The buffer of MAC address.
 *
 *  @returns    void.
 */
void ethernet_address(char *mac);

/** 
 *  @brief To get the link status.
 *
 *  @param None
 *
 *  @returns    1 for link up, 0 for link down.
 */
int ethernet_link(void);

/** 
 *  @brief To set the link speed and duplex mode.
 *
 *  @param[in]  speed The specified link speed.
 *  @param[in]  duplex The specifed duplex mode.
 *
 *  @returns    void.
 */
void ethernet_set_link(int speed, int duplex);

/** @} */ /* End of group ethernet */

#ifdef __cplusplus
}
#endif

#endif

#endif

