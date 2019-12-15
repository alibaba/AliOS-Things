/*!
 * @filename umesh.h
 * This is uMesh APIs header file
 *
 * @copyright  Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_MESH_H
#define UMESH_MESH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "umesh_config.h"
#include "umesh_types.h"

/** @addtogroup aos_umesh uMesh
 *  AliOS Things mesh networks
 *
 *  @{
 */

/**
 * Tx data via using sid as dest address
 *
 * @param[in] buf   pointer to tx buffer
 * @param[in] netid network id
 * @param[in] sid   dest sid address
 *
 * @return success or not
 */
ur_error_t umesh_output_sid(struct pbuf *buf, uint16_t netid, uint16_t sid);

/**
 * Tx data via using uuid as dest address
 *
 * @param[in] buf  pointer to tx buffer
 * @param[in] uuid dest uuid address
 *
 * @return success or not
 */
ur_error_t umesh_output_uuid(struct pbuf *buf, uint8_t *uuid);

/**
 * initialize uMesh
 *
 * @param[in] mode mesh node mode
 *
 * @return success or not
 */
ur_error_t umesh_init(node_mode_t mode);

/**
 * uMesh start related callback structure
 *
 * @param[in] is_leader_down indicate whether leader is down or not
 */
typedef void (* umesh_status_t)(bool is_leader_down);

/**
 * Start uMesh
 *
 * @param[in] callback uMesh start result callbacks
 *
 * @return success or not
 */
ur_error_t umesh_start(umesh_status_t callback);

/**
 * Check if uMesh is initialized or not
 *
 * @return started or not
 */
bool umesh_is_initialized(void);

/**
 * Stop uMesh
 *
 * @return success or not
 */
ur_error_t umesh_stop(void);

/**
 * Register uMesh state callbacks
 *
 * @param[in] callback uMesh state callbacks
 *
 * @return success or not
 */
ur_error_t umesh_register_callback(ur_adapter_callback_t *callback);

#ifdef CONFIG_AOS_MESH_BCAST
/* provide broadcast transmit capability within the scope of overall
 * mesh network in the case that leader has been offline.
 */
typedef void (* umesh_bcast_recv_t)(uint8_t *buf, uint32_t len);

/**
 * Register uMesh broadcast callbacks
 *
 * @param[in] callback uMesh broadcast callbacks
 *
 * @return success or not
 */
ur_error_t umesh_register_bcast_callback(umesh_bcast_recv_t callback);

/**
 * Register uMesh broadcast callbacks
 *
 * @param[in] buf pointer to tx buf
 * @param[in] len buffer length
 *
 * @return success or not
 */
ur_error_t umesh_bcast_send(uint8_t *buf, uint32_t len);
#endif

/**
 * Get device state
 *
 * @return device statet
 */
uint8_t umesh_get_device_state(void);

/**
 * Get device mode
 *
 * @return device mode
 */
uint8_t umesh_get_mode(void);

/**
 * Set device mode
 *
 * @param[in] device mode
 *
 * @return success or not
 */
ur_error_t umesh_set_mode(uint8_t mode);

/**
 * Get mac address
 *
 * @param[in] type network media type
 *
 * @return device MAC address
 */
const mac_address_t *umesh_get_mac_address(media_type_t type);

/**
 * Get meshnet ID
 *
 * @return meshnet ID
 */
uint16_t umesh_get_meshnetid(void);

/**
 * Get sid address
 *
 * @return sid address
 */
uint16_t umesh_get_sid(void);

/**
 * get mesh network size
 *
 * @return mesh network size
 */
uint16_t umesh_get_meshnetsize(void);

/**
 * get mesh network neighbors
 *
 * @param[in] type network media type
 *
 * @return mesh network neighbors
 */
slist_t *umesh_get_nbrs(media_type_t type);

/**
 * Check whether is the multicast address subscribed
 *
 * @param[in] addr multicast address
 *
 * @return true indicates mcast addr subscribed, false indicates not subscribed
 *
 */
bool umesh_is_mcast_subscribed(const ur_ip6_addr_t *addr);

/**
 * Check whether is the multicast address subscribed
 *
 * @param[in] addr multicast address
 *
 */
void umesh_get_extnetid(umesh_extnetid_t *extnetid);

/**
 * Set extension network id
 *
 * @param[in] extnetid extension network id
 *
 * @return success or not
 */
ur_error_t umesh_set_extnetid(const umesh_extnetid_t *extnetid);

typedef void (*cmd_cb_t)(char *buf, int len, void *priv);

/**
 * uMesh CLI command proc
 *
 * @param[in] buf   pointer to command buffer
 * @param[in] len   buffer len
 * @param[in] cb    cmd related callback
 * @param[in] priv  pointer to private data
 *
 */
void umesh_cli_cmd(char *buf, int len, cmd_cb_t cb, void *priv);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* UMESH_MESH_H */
