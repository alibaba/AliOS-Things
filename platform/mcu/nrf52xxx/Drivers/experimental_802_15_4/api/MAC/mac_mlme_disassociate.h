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
#ifndef MAC_MLME_DISASSOCIATE_H_INCLUDED
#define MAC_MLME_DISASSOCIATE_H_INCLUDED

#if (CONFIG_DISASSOCIATE_ENABLED == 1)

#include <stdint.h>
#include <stdbool.h>
#include "mac_common.h"
#include "mac_task_scheduler.h"

/** @file
 * The MAC MLME Disassociate module declares the MAC disassociation routines and
 * necessary types/macros according to the MAC specification.
 *
 * @defgroup mac_diassociate MAC MLME Disassociate API
 * @ingroup mac_15_4
 * @{
 * @brief Module to declare MAC MLME Disassociate API.
 * @details The MLME Disassociation module declares Disassociation MAC routines and necessary types
 * according to the MAC specification. More specifically, MLME Disassociation request aka
 * mlme_disassociate_req(), MLME Disassociation confirm callback typedef aka
 * mlme_disassociate_conf_cb_t, and MLME Disassociation indication as mlme_disassociate_ind()
 * primitives are declared.
 */

/**
 * @brief   MAC Disassociation Reason field
 *
 * In accordance with IEEE Std 802.15.4-2006, section 7.3.2.2
 */
typedef enum
{
    /** The coordinator wishes the device to leave the PAN. */
    MAC_COORD_REASON   = 1,
    /** The device wishes to leave the PAN. */
    MAC_DEV_REASON     = 2
} mac_disassociate_reason_t;


/**
 * @brief   MLME-DISASSOCIATE.confirm
 *
 * @details On receipt of the MLME-DISASSOCIATE.confirm primitive, the next
 * higher layer of the initiating device is notified of the result of the
 * disassociation attempt. If the disassociation attempt was successful,
 * the status parameter will be set to SUCCESS. Otherwise, the status parameter
 * indicates the error.
 *
 * In accordance with IEEE Std 802.15.4-2006, section 7.1.4.3
 */
typedef struct
{
    mac_status_t            status;                  /**< Status of operation. */
    mac_addr_mode_t         device_addr_mode;        /**< Device addressing mode. */
    uint16_t                device_pan_id;           /**< Device PAN ID. */
    mac_addr_t              device_address;          /**< Device address. */
} mlme_disassociate_conf_t;


/**
 * @brief   MLME-DISASSOCIATE.request
 *
 * @details The MLME-DISASSOCIATE.request primitive is generated by the next
 * higher layer of an associated device and issued to its MLME to request
 * disassociation from the PAN. It is also generated by the next higher layer
 * of the coordinator and issued to its MLME to instruct an
 * associated device to leave the PAN.
 *
 * In accordance with IEEE Std 802.15.4-2006, section 7.1.4.1
 */
typedef struct
{
    /** Do not edit this field. */
    mac_abstract_req_t        service;

    /** Confirm to this request. */
    mlme_disassociate_conf_t  confirm;

    mac_addr_mode_t           device_addr_mode;      /**< Device addressing mode. */
    uint16_t                  device_pan_id;         /**< Device PAN ID. */
    mac_addr_t                device_address;        /**< Device address. */
    mac_disassociate_reason_t disassociate_reason;   /**< Disassociation reason. */
    bool                      tx_indirect;           /**< Is TX indirect? */
#if (CONFIG_SECURE == 1)
    uint8_t                   security_level;        /**< Security level. */
    uint8_t                   key_id_mode;           /**< Key ID mode. */
    uint64_t                  key_source;            /**< Key source. */
    uint8_t                   key_index;             /**< Key index. */
#endif
} mlme_disassociate_req_t;


/**
 * @brief   MLME-DISASSOCIATE.indication
 *
 * @details Is generated by the MLME and issued to its next higher
 * layer on receipt of a disassociation notification command.
 *
 * In accordance with IEEE Std 802.15.4-2006, section 7.1.4.2
 */
typedef struct
{
    uint64_t                  device_address;        /**< Device address. */
    mac_disassociate_reason_t disassociate_reason;   /**< Disassociation reason. */
#if (CONFIG_SECURE == 1)
    uint8_t                   security_level;        /**< Security level. */
    uint8_t                   key_id_mode;           /**< Key ID mode. */
    uint64_t                  key_source;            /**< Key source. */
    uint8_t                   key_index;             /**< Key index. */
#endif
} mlme_disassociate_ind_t;


/**
 * @brief Customer's function of confirmation
 *
 * @details The MLME-DISASSOCIATE.confirm primitive is generated by the initiating
 * MLME and issued to its next higher layer in response to an MLME-DISASSOCIATE.request
 * primitive. This primitive returns a status of either SUCCESS, indicating that the
 * disassociation request was successful, or the appropriate error code.
 * The status values are fully described in 7.1.4.1.3 and subclauses referenced by 7.1.4.1.3.
 *
 * @param pointer to confirmation primitive
 */
typedef void (* mlme_disassociate_conf_cb_t)(mlme_disassociate_conf_t *);


/**
 * @brief   MLME-DISASSOCIATE request
 *
 * @details Request disassociation with a PAN
 * After request completion, user callback will be issued with
 * valid data stored in structure @ref mlme_disassociate_conf_t.
 *
 * @param req  MLME_DISASSOCIATE request structure.
 * @param conf_cb pointer to user callback.
 *
 * In accordance with IEEE Std 802.15.4-2006, section 7.1.4.4
 */
void mlme_disassociate_req(mlme_disassociate_req_t * req, mlme_disassociate_conf_cb_t conf_cb);


/**
 * @brief   MLME-DISASSOCIATE indication handler
 *
 * @details Indicates an disassociation with a PAN
 *
 * @param ind  MLME_DISASSOCIATE indication structure.
 *
 * In accordance with IEEE Std 802.15.4-2006, section 7.1.4.4
 */
extern void mlme_disassociate_ind(mlme_disassociate_ind_t * ind);

/** @} */

#endif // (CONFIG_DISASSOCIATE_ENABLED == 1)

#endif // MAC_MLME_DISASSOCIATE_H_INCLUDED
