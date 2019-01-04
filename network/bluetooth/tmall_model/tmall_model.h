/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BT_MESH_PROFILE_H_
#define BT_MESH_PROFILE_H_

/** @brief calculate the digest of static oob info
 *
 *  Calculate the sha256 of static oob info as AuthValue during
 *  the provisioning process.
 *
 *  AuthValue = SHA256(Product ID,MAC,Secret);
 *
 *  @param digest   buffer to store the calculated sha256 value.
 *  @param pid      product id, allocated from Alibaba.
 *  @param mac_addr device name, allocatd from Alibaba.
 *  @param secret   device key, allocated from Alibaba.
 */
void bt_mesh_profile_calculate_digest(const uint8_t *digest, const uint8_t *pid,
                                      const uint8_t *mac_addr, const char *secret);

/** @brief construct the new uuid followed Alibaba ble mesh spec
 *
 *  Construct the new 16bytes uuid followed Alibaba ble mesh spec
 *
 *   CID: company id (2 bytes)
 *   PID: 1 byte
 *   Product ID: 4 bytes
 *   Mac addr: device name (6 bytes)
 *   RFU
 *
 *  @param uuid     uuid pointer passed in.
 *  @param pid      product id, allocatd from Alibaba.
 *  @param mac_addr device name, allocated from Alibaba.
 */
void bt_mesh_profile_construct_uuid(char *uuid, const uint8_t *pid,
                                    const uint8_t *mac_addr);

/** @brief Callback for notifying that Bluetooth has been enabled
 *
 *  @param err  zero on success or (negative) error code otherwise
 */
typedef void (*bt_mesh_profile_ready_cb_t)(int err);

/** @brief initialize and start the ble mesh profile.
 *
 *  Initialize the hci driver, ble mesh stack and start profile.
 *
 *  @param cb  Callback to notify completion or NULL to perform the
 *             enabling synchronously
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_profile_start(void);

/** @brief Provisioning is complete.
 *
 *  This callback notifies the application that provisioning has
 *  been successfully completed, and that the local node has been
 *  assigned the specified NetKeyIndex and primary element address.
 *
 *  @param net_idx NetKeyIndex given during provisioning.
 *  @param addr    Primary element address.
 */
typedef void (*bt_mesh_profile_prov_complete_t)(uint16_t net_idx, uint16_t addr);

/** @brief Node has been reset.
 *
 *  This callback notifies the application that the local node
 *  has been reset and needs to be reprovisioned. The node will
 *  not automatically advertise as unprovisioned, rather the
 *  bt_mesh_prov_enable() API needs to be called to enable
 *  unprovisioned advertising on one or more provisioning bearers.
 */
typedef void (*bt_mesh_profile_prov_reset_t)(void);

/** @brief initialize the ble mesh provisioning.
 *
 *  Initialize the ble mesh provisioning, and select the specific
 *  provisioning bearer (current only PB-GATT bearer is supported).
 *
 *  @param dev_uuid         device uuid get from ble_mesh_profile_construct_uuid() api.
 *  @param digest           sha256 value calculate from ble_mesh_profile_calculate_digest() api.
 *  @param digest_len       length of digest, always 16 bytes.
 *  @param prov_complete_cb callback for provisioning is complete.
 *  @param prov_reset_cb    callback for provisioning is reset.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_profile_prov_init(const uint8_t *dev_uuid,
                              const uint8_t *digest, size_t digest_len,
                              bt_mesh_profile_prov_complete_t prov_complete_cb,
                              bt_mesh_profile_prov_reset_t prov_reset_cb);

/** @brief initialize the supported elements and models.
 *
 *  Initialize the supported elements and models in the specific product.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_profile_model_init(void);

/** @brief initialize the composistion data.
 *
 *  Initialize the composition data (mainly for Composition Data Page 0)
 *  of a node, these information will be used during provisioning.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_profile_composition_data_init(void);

#endif // BT_MESH_PROFILE_H_
