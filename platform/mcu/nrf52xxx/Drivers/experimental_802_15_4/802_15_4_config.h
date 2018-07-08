/**
 * Copyright (c) 2016 - 2017 Nordic Semiconductor ASA and Luxoft Global Operations Gmbh.
 * 
 * All Rights Reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#ifndef STACK_802_15_4_CONFIG_H__
#define STACK_802_15_4_CONFIG_H__

#define CONFIG_DEBUG 1
#define CONFIG_MM_DEBUG 1
#define CONFIG_TEST_CHANNEL 15
#define CONFIG_TEST_CHANNEL_MASK (1 << CONFIG_TEST_CHANNEL) 
#define CONFIG_BEACON_ENABLED 1
#define CONFIG_SECURE 1
#define CONFIG_FFD_DEVICE 1
#define CONFIG_RXE_ENABLED 1
#define CONFIG_GTS_ENABLED 1
#define CONFIG_SYNC_REQ_ENABLED 1
#define CONFIG_ASSOCIATE_IND_ENABLED 1
#define CONFIG_ORPHAN_ENABLED 1
#define CONFIG_START_ENABLED 1
#define CONFIG_ED_SCAN_ENABLED 1
#define CONFIG_ACTIVE_SCAN_ENABLED 1
#define CONFIG_PURGE_ENABLED 1
#define CONFIG_PANID_CONFLICT_RESOLUTION_ENABLED 1
#define CONFIG_ASSOCIATION_REQ_CMD_RX_ENABLED 1
#define CONFIG_ASSOCIATION_REQ_CMD_TX_ENABLED 1
#define CONFIG_ASSOCIATION_RESP_CMD_RX_ENABLED 1
#define CONFIG_ASSOCIATION_RESP_CMD_TX_ENABLED 1
#define CONFIG_DISASSOCIATION_NTF_CMD_RX_ENABLED 1
#define CONFIG_DISASSOCIATION_NTF_CMD_TX_ENABLED 1
#define CONFIG_DATA_REQ_CMD_RX_ENABLED 1
#define CONFIG_DATA_REQ_CMD_TX_ENABLED 1
#define CONFIG_PAN_CONFLICT_NTF_CMD_RX_ENABLED 1
#define CONFIG_PAN_CONFLICT_NTF_CMD_TX_ENABLED 1
#define CONFIG_ORPHAN_NTF_CMD_RX_ENABLED 1
#define CONFIG_ORPHAN_NTF_CMD_TX_ENABLED 1
#define CONFIG_BEACON_REQ_CMD_RX_ENABLED 1
#define CONFIG_BEACON_REQ_CMD_TX_ENABLED 1
#define CONFIG_COORD_REALIGN_CMD_RX_ENABLED 1
#define CONFIG_COORD_REALIGN_CMD_TX_ENABLED 1
#define CONFIG_GTS_REQ_CMD_RX_ENABLED 1
#define CONFIG_GTS_REQ_CMD_TX_ENABLED 1
#define CONFIG_INDIRECT_ENGINE_ENABLED 1
#define CONFIG_ASSOCIATE_REQ_ENABLED 1
#define CONFIG_DISASSOCIATE_ENABLED 1
#define CONFIG_PANID_CONFLICT_ENABLED 1
#define CONFIG_SYNC_ENABLED 1
#define CONFIG_NONE_ADDR_SUPPORT_ENABLED 1
#define CONFIG_PASSIVE_ORPHAN_SCAN_ENABLED 1
#define CONFIG_PROMISCUOUS_MODE_ENABLED 1
#define CONFIG_USE_SYS_TASK_NOTIFIER 1
#define CONFIG_POOL_SIZE 0x2000
#define CONFIG_PHY_FRAME_POOL_SIZE 4
#define CONFIG_MEMORY_POLLING_INTERVAL_MS 10
#define CONFIG_MAX_SLEEP_APPROVERS 32
#define CONFIG_HAL_UART_CHANNELS 1
#define CONFIG_MAC_KEY_TABLE_SIZE 8
#define CONFIG_MAC_DEVICE_TABLE_SIZE 8
#define CONFIG_MAC_SECURITY_LEVEL_TABLE_SIZE 8
#define CONFIG_MAC_KEY_ID_LOOKUP_LIST_SIZE 8
#define CONFIG_MAC_KEY_DEVICE_LIST_SIZE 8
#define CONFIG_MAC_KEY_USAGE_LIST_SIZE 8
#define CONFIG_IEEE_ADDRESS 0x0123456789ABCDEFULL
#define CONFIG_BEACON_RX_TIME 5000
#define CONFIG_TEST_SUBTREE_MAC 1

#endif // STACK_802_15_4_CONFIG_H__

