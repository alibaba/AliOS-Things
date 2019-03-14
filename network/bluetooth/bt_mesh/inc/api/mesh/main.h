/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_MESH_MAIN_H
#define __BT_MESH_MAIN_H

#define BT_MESH_RELAY_DISABLED              0x00
#define BT_MESH_RELAY_ENABLED               0x01
#define BT_MESH_RELAY_NOT_SUPPORTED         0x02

#define BT_MESH_BEACON_DISABLED             0x00
#define BT_MESH_BEACON_ENABLED              0x01

#define BT_MESH_GATT_PROXY_DISABLED         0x00
#define BT_MESH_GATT_PROXY_ENABLED          0x01
#define BT_MESH_GATT_PROXY_NOT_SUPPORTED    0x02

#define BT_MESH_FRIEND_DISABLED             0x00
#define BT_MESH_FRIEND_ENABLED              0x01
#define BT_MESH_FRIEND_NOT_SUPPORTED        0x02

typedef enum {
    BT_MESH_NO_OUTPUT       = 0,
    BT_MESH_BLINK           =  1 << 0,
    BT_MESH_BEEP            =  1 << 1,
    BT_MESH_VIBRATE         =  1 << 2,
    BT_MESH_DISPLAY_NUMBER  =  1 << 3,
    BT_MESH_DISPLAY_STRING  =  1 << 4,
} bt_mesh_output_action_t;

typedef enum {
    BT_MESH_NO_INPUT      = 0,
    BT_MESH_PUSH          =  1 << 0,
    BT_MESH_TWIST         =  1 << 1,
    BT_MESH_ENTER_NUMBER  =  1 << 2,
    BT_MESH_ENTER_STRING  =  1 << 3,
} bt_mesh_input_action_t;

typedef enum {
    BT_MESH_PROV_ADV   =  1 << 0,
    BT_MESH_PROV_GATT  =  1 << 1,
} bt_mesh_prov_bearer_t;

typedef enum {
    BT_MESH_PROV_OOB_OTHER     =  1 << 0,
    BT_MESH_PROV_OOB_URI       =  1 << 1,
    BT_MESH_PROV_OOB_2D_CODE   =  1 << 2,
    BT_MESH_PROV_OOB_BAR_CODE  =  1 << 3,
    BT_MESH_PROV_OOB_NFC       =  1 << 4,
    BT_MESH_PROV_OOB_NUMBER    =  1 << 5,
    BT_MESH_PROV_OOB_STRING    =  1 << 6,
    /* 7 - 10 are reserved */
    BT_MESH_PROV_OOB_ON_BOX    =  1 << 11,
    BT_MESH_PROV_OOB_IN_BOX    =  1 << 12,
    BT_MESH_PROV_OOB_ON_PAPER  =  1 << 13,
    BT_MESH_PROV_OOB_IN_MANUAL =  1 << 14,
    BT_MESH_PROV_OOB_ON_DEV    =  1 << 15,
} bt_mesh_prov_oob_info_t;

/** Provisioning properties & capabilities. */
struct bt_mesh_prov {
    /** The UUID that's used when advertising as unprovisioned */
    const u8_t *uuid;

    /** Optional URI. This will be advertised separately from the
     *  unprovisioned beacon, however the unprovisioned beacon will
     *  contain a hash of it so the two can be associated by the
     *  provisioner.
     */
    const char *uri;

    /** Out of Band information field. */
    bt_mesh_prov_oob_info_t oob_info;

    /** Static OOB value */
    const u8_t *static_val;
    /** Static OOB value length */
    u8_t        static_val_len;

    /** Maximum size of Output OOB supported */
    u8_t        output_size;
    /** Supported Output OOB Actions */
    u16_t       output_actions;

    /* Maximum size of Input OOB supported */
    u8_t        input_size;
    /** Supported Input OOB Actions */
    u16_t       input_actions;

    /** @brief Output of a number is requested.
     *
     *  This callback notifies the application that it should
     *  output the given number using the given action.
     *
     *  @param act Action for outputting the number.
     *  @param num Number to be outputted.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*output_number)(bt_mesh_output_action_t act, u32_t num);

    /** @brief Output of a string is requested.
     *
     *  This callback notifies the application that it should
     *  display the given string to the user.
     *
     *  @param str String to be displayed.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*output_string)(const char *str);

    /** @brief Input is requested.
     *
     *  This callback notifies the application that it should
     *  request input from the user using the given action. The
     *  requested input will either be a string or a number, and
     *  the application needs to consequently call the
     *  bt_mesh_input_string() or bt_mesh_input_number() functions
     *  once the data has been acquired from the user.
     *
     *  @param act Action for inputting data.
     *  @param num Maximum size of the inputted data.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*input)(bt_mesh_input_action_t act, u8_t size);

    /** @brief Provisioning link has been opened.
     *
     *  This callback notifies the application that a provisioning
     *  link has been opened on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void        (*link_open)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning link has been closed.
     *
     *  This callback notifies the application that a provisioning
     *  link has been closed on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void        (*link_close)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning is complete.
     *
     *  This callback notifies the application that provisioning has
     *  been successfully completed, and that the local node has been
     *  assigned the specified NetKeyIndex and primary element address.
     *
     *  @param net_idx NetKeyIndex given during provisioning.
     *  @param addr Primary element address.
     */
    void        (*complete)(u16_t net_idx, u16_t addr);

    /** @brief Node has been reset.
     *
     *  This callback notifies the application that the local node
     *  has been reset and needs to be reprovisioned. The node will
     *  not automatically advertise as unprovisioned, rather the
     *  bt_mesh_prov_enable() API needs to be called to enable
     *  unprovisioned advertising on one or more provisioning bearers.
     */
    void        (*reset)(void);
};

struct bt_mesh_provisioner {
    /* Provisioner device uuid */
    const u8_t *prov_uuid;

    /*
     * Primary element address of the provisioner, for
     * temporary provisioner no need to initialize it.
     */
    const u16_t prov_unicast_addr;

    /*
     * Starting unicast address going to assigned, for
     * temporary provisioner no need to initialize it.
     */
    u16_t prov_start_address;

    /* Attention timer contained in Provisioning Invite */
    u8_t  prov_attention;

    /* Provisioner provisioning Algorithm */
    u8_t  prov_algorithm;

    /* Provisioner public key oob */
    u8_t  prov_pub_key_oob;

    /** @brief Input is requested.
     *
     *  This callback notifies the application that it should
     *  read device's public key with OOB
     *
     *  @param remote_pub_key Public key pointer of the device.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int  (*prov_pub_key_oob_cb)(u8_t remote_pub_key[64]);

    /* Provisioner static oob value */
    u8_t *prov_static_oob_val;

    /* Provisioner static oob value length */
    u8_t  prov_static_oob_len;

    /** @brief Provisioner input a number read from device output
     *
     *  This callback notifies the application that it should
     *  input the number given by the device.
     *
     *  @param act:  The output action of the device
     *  @param size: The output size of the device
     *
     *  @return Zero on success or negative error code otherwise
     */
    int  (*prov_input_num)(bt_mesh_output_action_t act, u8_t size);

    /** @brief Provisioner output a number to the device
     *
     *  This callback notifies the application that it should
     *  output the number to the device.
     *
     *  @param act:  The input action of the device
     *  @param size: The input size of the device
     *
     *  @return Zero on success or negative error code otherwise
     */
    int  (*prov_output_num)(bt_mesh_input_action_t act, u8_t size);

    /*
     * Key refresh and IV update flag, for temporary provisioner no
     * need to initialize it.
     */
    u8_t  flags;

    /*
     * IV index, for temporary provisioner no need to initialize it.
     */
    u32_t iv_index;

    /** @brief Provisioner has opened a provisioning link.
     *
     *  This callback notifies the application that a provisioning
     *  link has been opened on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void (*prov_link_open)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioner has closed a provisioning link.
     *
     *  This callback notifies the application that a provisioning
     *  link has been closed on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     *  @param reason Provisioning link close reason(disconnect reason)
     *                0xFF: disconnect due to provisioner_pb_gatt_disable()
     */
    void (*prov_link_close)(bt_mesh_prov_bearer_t bearer, u8_t reason);

    /** @brief Provision one device is complete.
     *
     *  This callback notifies the application that provisioner has
     *  successfully provisioned a device, and the node has been assigned
     *  with the specified NetKeyIndex and primary element address.
     *
     *  @param node_idx     Node index within the node(provisioned device) queue.
     *  @param device_uuid  Provisioned device device uuid pointer.
     *  @param unicast_addr Provisioned device assigned unicast address.
     *  @param element_num  Provisioned device element number.
     *  @param netkey_idx   Provisioned device assigned netkey index.
     */
    void (*prov_complete)(int node_idx, const u8_t device_uuid[16],
                          u16_t unicast_addr, u8_t element_num,
                          u16_t netkey_idx);
};

/** @brief Initialize Mesh support
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 *  @param prov Node provisioning information.
 *  @param comp Node Composition.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_init(const struct bt_mesh_prov *prov,
                 const struct bt_mesh_comp *comp,
                 const struct bt_mesh_provisioner *provisioner);

/** @brief Reset the state of the local Mesh node.
 *
 *  Resets the state of the node, which means that it needs to be
 *  reprovisioned to become an active node in a Mesh network again.
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 */
void bt_mesh_reset(void);

/** @brief Enable specific provisioning bearers
 *
 *  Enable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise or of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers);

/** @brief Disable specific provisioning bearers
 *
 *  Disable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise or of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers);

#endif /* __BT_MESH_MAIN_H */
