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

/**
 * @brief Bluetooth Mesh Provisioning
 * @defgroup bt_mesh_prov Bluetooth Mesh Provisioning
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef bool
#define bool unsigned char
#endif

#define BITN(n) (1U << (n))

typedef enum {
    BT_MESH_NO_OUTPUT = 0,
    BT_MESH_BLINK = BITN(0),
    BT_MESH_BEEP = BITN(1),
    BT_MESH_VIBRATE = BITN(2),
    BT_MESH_DISPLAY_NUMBER = BITN(3),
    BT_MESH_DISPLAY_STRING = BITN(4),
} bt_mesh_output_action_t;

typedef enum {
    BT_MESH_NO_INPUT = 0,
    BT_MESH_PUSH = BITN(0),
    BT_MESH_TWIST = BITN(1),
    BT_MESH_ENTER_NUMBER = BITN(2),
    BT_MESH_ENTER_STRING = BITN(3),
} bt_mesh_input_action_t;

typedef enum {
    BT_MESH_PROV_ADV = BITN(0),
    BT_MESH_PROV_GATT = BITN(1),
} bt_mesh_prov_bearer_t;

typedef enum {
    BT_MESH_PROV_OOB_OTHER = BITN(0),
    BT_MESH_PROV_OOB_URI = BITN(1),
    BT_MESH_PROV_OOB_2D_CODE = BITN(2),
    BT_MESH_PROV_OOB_BAR_CODE = BITN(3),
    BT_MESH_PROV_OOB_NFC = BITN(4),
    BT_MESH_PROV_OOB_NUMBER = BITN(5),
    BT_MESH_PROV_OOB_STRING = BITN(6),
    /* 7 - 10 are reserved */
    BT_MESH_PROV_OOB_ON_BOX = BITN(11),
    BT_MESH_PROV_OOB_IN_BOX = BITN(12),
    BT_MESH_PROV_OOB_ON_PAPER = BITN(13),
    BT_MESH_PROV_OOB_IN_MANUAL = BITN(14),
    BT_MESH_PROV_OOB_ON_DEV = BITN(15),
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
    u8_t static_val_len;

    /** Maximum size of Output OOB supported */
    u8_t output_size;
    /** Supported Output OOB Actions */
    u16_t output_actions;

    /* Maximum size of Input OOB supported */
    u8_t input_size;
    /** Supported Input OOB Actions */
    u16_t input_actions;

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
    int (*output_number)(bt_mesh_output_action_t act, bt_u32_t num);

    /** @brief Output of a string is requested.
	 *
	 *  This callback notifies the application that it should
	 *  display the given string to the user.
	 *
	 *  @param str String to be displayed.
	 *
	 *  @return Zero on success or negative error code otherwise
	 */
    int (*output_string)(const char *str);

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
    int (*input)(bt_mesh_input_action_t act, u8_t size);

    /** @brief Provisioning link has been opened.
	 *
	 *  This callback notifies the application that a provisioning
	 *  link has been opened on the given provisioning bearer.
	 *
	 *  @param bearer Provisioning bearer.
	 */
    void (*link_open)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning link has been closed.
	 *
	 *  This callback notifies the application that a provisioning
	 *  link has been closed on the given provisioning bearer.
	 *
	 *  @param bearer Provisioning bearer.
	 */
    void (*link_close)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning is complete.
	 *
	 *  This callback notifies the application that provisioning has
	 *  been successfully completed, and that the local node has been
	 *  assigned the specified NetKeyIndex and primary element address.
	 *
	 *  @param net_idx NetKeyIndex given during provisioning.
	 *  @param addr Primary element address.
	 */
    void (*complete)(u16_t net_idx, u16_t addr);

    /** @brief Node has been reset.
	 *
	 *  This callback notifies the application that the local node
	 *  has been reset and needs to be reprovisioned. The node will
	 *  not automatically advertise as unprovisioned, rather the
	 *  bt_mesh_prov_enable() API needs to be called to enable
	 *  unprovisioned advertising on one or more provisioning bearers.
	 */
    void (*reset)(void);
};

struct bt_mesh_provisioner {
    /* Provisioner device uuid */
    const u8_t *prov_uuid;

    /*
	 * Primary element address of the provisioner, for
	 * temporary provisioner no need to initialize it.
	 */
    u16_t prov_unicast_addr;

    /*
	 * Starting unicast address going to assigned, for
	 * temporary provisioner no need to initialize it.
	 */
    u16_t prov_start_address;

    /* Attention timer contained in Provisioning Invite */
    u8_t prov_attention;

    /* Provisioner provisioning Algorithm */
    u8_t prov_algorithm;

    /* Provisioner public key oob */
    u8_t prov_pub_key_oob;

    /** @brief Input is requested.
	 *
	 *  This callback notifies the application that it should
	 *  read device's public key with OOB
	 *
	 *  @param remote_pub_key Public key pointer of the device.
	 *
	 *  @return Zero on success or negative error code otherwise
	 */
    int (*prov_pub_key_oob_cb)(u8_t remote_pub_key[64]);

    /* Provisioner static oob value */
    u8_t *prov_static_oob_val;

    /* Provisioner static oob value length */
    u8_t prov_static_oob_len;

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
    int (*prov_input_num)(bt_mesh_output_action_t act, u8_t size);
    /** @brief Provisioner input static oob
	 *
	 *  This callback notifies the application that it should
	 *
	 *  @param size: The output size of the device
	 *
	 *  @return Zero on success or negative error code otherwise
	 */

    int (*prov_input_static_oob)();

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
    int (*prov_output_num)(bt_mesh_input_action_t act, u8_t size);

    /*
	 * Key refresh and IV update flag, for temporary provisioner no
	 * need to initialize it.
	 */
    u8_t flags;

    /*
	 * IV index, for temporary provisioner no need to initialize it.
	 */
    bt_u32_t iv_index;

    /** @brief Provisioner has opened a provisioning link.
	 *
	 *  This callback notifies the application that a provisioning
	 *  link has been opened on the given provisioning bearer.
	 *
	 *  @param bearer Provisioning bearer.
	 *  @param link info.
	 */
    void (*prov_link_open)(bt_mesh_prov_bearer_t bearer, u8_t addr_val[6], u8_t addr_type, u8_t uuid[16],
                           u8_t prov_count);

    /** @brief Provisioner has closed a provisioning link.
	 *
	 *  This callback notifies the application that a provisioning
	 *  link has been closed on the given provisioning bearer.
	 *
	 *  @param bearer Provisioning bearer.
	 *  @param reason Provisioning link close reason(disconnect reason)
	 *  @param link info.
	 */
    void (*prov_link_close)(bt_mesh_prov_bearer_t bearer, u8_t reason, u8_t addr_val[6], u8_t addr_type, u8_t uuid[16],
                            u8_t prov_count);

    /** @brief Provision one device is complete.
	 *
	 *  This callback notifies the application that provisioner has
	 *  successfully provisioned a device, and the node has been assigned
	 *  with the specified NetKeyIndex and primary element address.
	 *
	 *  @param node_idx	 Node index within the node(provisioned device) queue.
	 *  @param device_uuid  Provisioned device device uuid pointer.
	 *  @param unicast_addr Provisioned device assigned unicast address.
	 *  @param element_num  Provisioned device element number.
	 *  @param netkey_idx   Provisioned device assigned netkey index.
	 */
    void (*prov_complete)(int node_idx, const u8_t device_uuid[16], u16_t unicast_addr, u8_t element_num,
                          u16_t netkey_idx, bool gatt_flag);
};

/** @brief Provide provisioning input OOB string.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BT_MESH_ENTER_STRING as the action.
 *
 *  @param str String.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_input_string(const char *str);

/** @brief Provide provisioning input OOB number.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BT_MESH_ENTER_NUMBER as the action.
 *
 *  @param num Number.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_input_number(bt_u32_t num);

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

/**
 * @}
 */

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh Bluetooth Mesh
 * @ingroup bluetooth
 * @{
 */

/* Primary Network Key index */
#define BT_MESH_NET_PRIMARY 0x000

#define BT_MESH_RELAY_DISABLED 0x00
#define BT_MESH_RELAY_ENABLED 0x01
#define BT_MESH_RELAY_NOT_SUPPORTED 0x02

#define BT_MESH_BEACON_DISABLED 0x00
#define BT_MESH_BEACON_ENABLED 0x01

#define BT_MESH_GATT_PROXY_DISABLED 0x00
#define BT_MESH_GATT_PROXY_ENABLED 0x01
#define BT_MESH_GATT_PROXY_NOT_SUPPORTED 0x02

#define BT_MESH_FRIEND_DISABLED 0x00
#define BT_MESH_FRIEND_ENABLED 0x01
#define BT_MESH_FRIEND_NOT_SUPPORTED 0x02

#define BT_MESH_NODE_IDENTITY_STOPPED 0x00
#define BT_MESH_NODE_IDENTITY_RUNNING 0x01
#define BT_MESH_NODE_IDENTITY_NOT_SUPPORTED 0x02

/* Features */
#define BT_MESH_FEAT_RELAY BITN(0)
#define BT_MESH_FEAT_PROXY BITN(1)
#define BT_MESH_FEAT_FRIEND BITN(2)
#define BT_MESH_FEAT_LOW_POWER BITN(3)
#define BT_MESH_FEAT_SUPPORTED (BT_MESH_FEAT_RELAY | BT_MESH_FEAT_PROXY | BT_MESH_FEAT_FRIEND | BT_MESH_FEAT_LOW_POWER)

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
int bt_mesh_init(const struct bt_mesh_prov *prov, const struct bt_mesh_comp *comp,
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

/** @brief Suspend the Mesh network temporarily.
 *
 *  This API can be used for power saving purposes, but the user should be
 *  aware that leaving the local node suspended for a long period of time
 *  may cause it to become permanently disconnected from the Mesh network.
 *  If at all possible, the Friendship feature should be used instead, to
 *  make the node into a Low Power Node.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_suspend(bool force);

/** @brief Resume a suspended Mesh network.
 *
 *  This API resumes the local node, after it has been suspended using the
 *  bt_mesh_suspend() API.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_resume(void);

/** @brief Provision the local Mesh Node.
 *
 *  This API should normally not be used directly by the application. The
 *  only exception is for testing purposes where manual provisioning is
 *  desired without an actual external provisioner.
 *
 *  @param net_key  Network Key
 *  @param net_idx  Network Key Index
 *  @param flags    Provisioning Flags
 *  @param iv_index IV Index
 *  @param addr     Primary element address
 *  @param dev_key  Device Key
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx, u8_t flags, bt_u32_t iv_index, u16_t addr,
                      const u8_t dev_key[16]);

/** @brief Check if the local node has been provisioned.
 *
 *  This API can be used to check if the local node has been provisioned
 *  or not. It can e.g. be helpful to determine if there was a stored
 *  network in flash, i.e. if the network was restored after calling
 *  settings_load().
 *
 *  @return True if the node is provisioned. False otherwise.
 */
bool bt_mesh_is_provisioned(void);

/** @brief Toggle the IV Update test mode
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @param enable true to enable IV Update test mode, false to disable it.
 */
void bt_mesh_iv_update_test(bool enable);

/** @brief Toggle the IV Update state
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @return true if IV Update In Progress state was entered, false otherwise.
 */
bool bt_mesh_iv_update(void);

/** @brief Toggle the Low Power feature of the local device
 *
 *  Enables or disables the Low Power feature of the local device. This is
 *  exposed as a run-time feature, since the device might want to change
 *  this e.g. based on being plugged into a stable power source or running
 *  from a battery power source.
 *
 *  @param enable  true to enable LPN functionality, false to disable it.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_lpn_set(bool enable);

/** @brief Send out a Friend Poll message.
 *
 *  Send a Friend Poll message to the Friend of this node. If there is no
 *  established Friendship the function will return an error.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_lpn_poll(void);

/** @brief Register a callback for Friendship changes.
 *
 *  Registers a callback that will be called whenever Friendship gets
 *  established or is lost.
 *
 *  @param cb Function to call when the Friendship status changes.
 */
void bt_mesh_lpn_set_cb(void (*cb)(u16_t friend_addr, bool established));

int bt_mesh_provisioner_enable(bt_mesh_prov_bearer_t bearers);
int bt_mesh_provisioner_disable(bt_mesh_prov_bearer_t bearers);
int bt_mesh_is_init();

struct adv_addr_t {
    u8_t type;
    u8_t val[6];
};

int bt_mesh_vnd_adv_set_cb(void (*cb)(const struct adv_addr_t *addr, s8_t rssi, u8_t adv_type, void *user_data,
                                      u16_t len));

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __BT_MESH_MAIN_H */
