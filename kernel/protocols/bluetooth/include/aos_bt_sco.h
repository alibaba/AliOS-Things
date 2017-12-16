/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* @file
 *
 * Bluetooth Synchronous Connection Oriented Channel Application Programming Interface
 *
 */

#ifndef _AOS_BT_SCO_H_
#define _AOS_BT_SCO_H_

#ifdef SCO_ENABLE //oscar for mindray

#include "aos.h"
#include "aos_bt_dev.h"
/*****************************************************
 *              Constants
 ******************************************************/

/*****************************************************
 *              Type Definitions
 ******************************************************/
/* Subset for the enhanced setup/accept synchronous connection paramters
 * See BT 4.1 or later HCI spec for details */
typedef struct {
    uint16_t max_latency;                   /* Maximum latency (0x4-0xFFFE in msecs) */
    uint16_t packet_types;                  /* Packet Types */
    uint8_t retrans_effort;                 /* 0x00-0x02, 0xFF don't care */
    aos_bool_t use_wbs;                   /* True to use wide band, False to use narrow band */

} aos_bt_sco_params_t;

/*
 *  @addtogroup  sco_api_functions       Synchronous Connection Oriented (SCO) Channel
 *  @ingroup     aosbt
 *
 *  SCO Functions
 *
 *  @{
 */

/*****************************************************
 *              Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Function         aos_bt_sco_create_as_initiator
 *
 *                  Creates a synchronous connection oriented connection as initiator.
 *
 *  @param[in]  bd_addr                 : Peer bd_addr
 *  @param[in]  p_params                : Pointer to the SCO parameter structure
 *  @param[out] p_sco_index             : SCO index returned
 *
 *  @return     <b> AOS_BT_UNKNOWN_ADDR </b>      : Create connection failed, ACL connection is not up
 *              <b> AOS_BT_BUSY </b>              : Create connection failed, another SCO is being
 *                                                    conncted to the same BD address
 *              <b> AOS_BT_WRONG_MODE </b>        : Create connection failed, wrong mode
 *              <b> AOS_BT_NO_RESOURCES </b>      : Create connection failed, max SCO limit has been
 *                                                    reached
 *              <b> BTM_CMD_STARTED </b>            : Create connection successfully, "p_sco_index" is returned
 */
aos_bt_dev_status_t aos_bt_sco_create_as_initiator (aos_bt_device_address_t bd_addr,
                                                    uint16_t *p_sco_index,
                                                    aos_bt_sco_params_t *p_params);

/*
 * Function         aos_bt_sco_create_as_acceptor
 *
 *                  Creates a synchronous connection oriented connection as acceptor.
 *
 *  @param[out] p_sco_index             : SCO index returned
 *
 *  @return     <b> AOS_BT_UNKNOWN_ADDR </b>      : Create connection failed, ACL connection is not up or
 *                                                    address is invalid
 *              <b> AOS_BT_BUSY </b>              : Create connection failed, a SCO connection is already
 *                                                    conncted to the same BD address
 *              <b> AOS_BT_WRONG_MODE </b>        : Create connection failed, link in park mode or
 *                                                    automatic un-park is not supported
 *              <b> AOS_BT_NO_RESOURCES </b>      : Create connection failed, max SCO limit has been
 *                                                    reached
 *              <b> BTM_CMD_STARTED </b>            : Create connection successfully, "p_sco_index" is returned
 */
aos_bt_dev_status_t aos_bt_sco_create_as_acceptor (uint16_t *p_sco_index);

/*
 * Function         aos_bt_sco_remove
 *
 *                  Removes a specific synchronous connection oriented connection.
 *
 *  @param[in]  sco_index           : SCO index to remove
 *
 *  @return     <b> AOS_BT_UNKNOWN_ADDR </b>      : Remove connection failed, invalid SCO index
 *              <b> AOS_BT_NO_RESOURCES </b>      : Remove connection failed, no resource
 *              <b> AOS_BT_SUCCESS </b>           : Remove connection successfully, device is still
 *                                                    listening for incoming connection
 *              <b> BTM_CMD_STARTED </b>            : Remove connection successfully
 */
aos_bt_dev_status_t aos_bt_sco_remove (uint16_t sco_index);

/*
 * Function         aos_bt_sco_accept_connection
 *
 *                  Called to handle (e)SCO connection request event (aos_bt_sco_connect_request_event).
 *
 *
 *  @param[in]  sco_index           : SCO index to remove
 *
 *  @param[in]  HCI status code     : HCI_SUCCESS 0x00
 *                                    HCI_ERR_HOST_REJECT_RESOURCES 0x0D
 *                                    HCI_ERR_HOST_REJECT_SECURITY  0x0E
 *                                    HCI_ERR_HOST_REJECT_DEVICE    0x0F
 *  @param[in]  p_params            : Pointer to the SCO parameter structure
 *
 */
void aos_bt_sco_accept_connection (uint16_t sco_index, uint8_t hci_status,
                                   aos_bt_sco_params_t *p_params);

#ifdef __cplusplus
}
#endif
/* @} aosbt_sco */

#endif

#endif
