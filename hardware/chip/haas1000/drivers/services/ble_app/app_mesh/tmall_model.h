/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BT_MESH_PROFILE_H_
#define BT_MESH_PROFILE_H_

typedef enum
{
    LIGHT_IND_NONE,
    LIGHT_IND_IN_MESH,
    LIGHT_IND_OUT_MESH,
    LIGHT_IND_FACTORY_MODE,
    LIGHT_IND_OTA_FINISH,
    LIGHT_IND_MAX
}LIGHT_IND_TYPE;


typedef void bt_mesh_led_control(uint8_t on);
/** @brief register Callback for control the led light
 */
void bt_mesh_led_control_register(bt_mesh_led_control led_control_cb);

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

uint32_t get_mesh_pbadv_time(void);
//void user_event(_GENIE_EVENT event, void *p_arg);
void mesh_indication(LIGHT_IND_TYPE type);

#endif // BT_MESH_PROFILE_H_
