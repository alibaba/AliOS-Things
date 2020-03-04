/**
 * @file aos_canopen.h
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_CANOPEN__H
#define AOS_CANOPEN__H

/** @addtogroup aos_canopen canopen
 *  Optional policy of canopen.
 *
 *  @{
 */

#include <stdint.h>
#include <stdbool.h>
#include "canopen/co_errno.h"

#define CANOPEN_MASTER 1
#define CANOPEN_SLAVE  0

#define CO_NODE_SIZE   4

#define MIN_HEART_BEAT_CYCLE 100 /* ms */

#define CO_SCHEDULE_POLL_CYCLE    10

#define co_event_schedule     63

#define co_event_user_base    64

#define CO_SECURITY_KEY2_WIDTH 8

#define CO_SECURITY_KEY2_AES_WIDTH 16

typedef uint8_t co_event_t;

typedef void (*schedule_user_cb)(const uint8_t len, const void *param, void *user_data);

/**
 * AliOS Things CANopen support nmt_control_service_reset, nmt_control_service_start
 */
typedef enum {
    nmt_control_service_start      = 1,
    nmt_control_service_stop       = 2,
    nmt_control_service_enter_pre  = 128,
    nmt_control_service_reset      = 129,
    nmt_control_service_reset_comm = 130
} nmt_control_service_t;

typedef enum {
    co_sdo_attr_read_mask = 0x02,     /* bit 1 */
    co_sdo_attr_write_mask = 0x04,    /* bit 2 */
    co_sdo_attr_len_flexiable = 0x08, /* bit 3 */
    co_sdo_attr_saved         = 0x10, /* bit 4 */
} co_sdo_attr_mask_t;

typedef enum {
    nmt_state_init            = 0,
    nmt_state_stopped         = 4,
    nmt_state_operational,
    nmt_state_upgrade         = 6, /* non DS301 Standard, extend for Upgrade */
    nmt_state_upgrade_base    = 10,
    nmt_state_pre_operational = 127
} nmt_state_t;

typedef enum {
    co_sdo_role_client,
    co_sdo_role_server,
} co_sdo_role_t;

typedef enum {
    pdo_idx_emergency = 0,
    pdo_idx_1,
    pdo_idx_2,
    pdo_idx_3,
    pdo_idx_4,
    pdo_idx_size
} pdo_idx_type_t;

typedef enum {
    co_nmt_node_heart_beat,
    co_nmt_node_guarding
} co_nmt_node_link_t;

typedef struct {
    uint16_t sdo_timeout;          /* unit is ms */
    uint8_t  enable_link_maintain; /* 0 indicates not active link maintain, others is reverse */
} canopen_config_t;

typedef struct {
    uint8_t       port;
    uint8_t       node;
} can_node_id_t;

typedef struct {
    uint8_t        len;
    uint8_t        start_byte;
    void          *data;
} pdo_tx_set_t;

typedef struct {
    uint8_t        port;
    uint8_t        node;
    pdo_idx_type_t idx;
    uint8_t        len;
    uint8_t        data[8];
} pdo_rx_cb_para_t;

/**
 * len, data: modified data;
 * recorded: If new data has been recored into OD object, if true the data has been recorded,
 *           If false(usually the Profile/App Layler always take the entry mutex ) then user shall considring
 *           handling it by self
 */
typedef void (*sdo_modify_cb_t)(const uint16_t len, const void *data, const bool recorded, void *user_data);

typedef struct {
    uint16_t        index;
    uint8_t         sub_index;
    uint8_t         attr;           /* @ co_sdo_attr_mask_t */
    uint16_t        size;
    uint16_t        actual_data_num;
    void           *object_data;
    sdo_modify_cb_t cb;
    void           *user_data;
} od_entry_t;

typedef void (*node_state_change_cb_t)(const can_node_id_t *node, const nmt_state_t state, void *user_data);

typedef void (*pdo_rx_cb_t)(const pdo_rx_cb_para_t *rx_data, void *user_data);

#ifdef __cplusplus
extern "C" {
#endif
/* Common APIs for both CANopen master and Components */

/**
 * Function prototype for canopen initialization.
 *
 * @param[in]  port      Specify CAN Port used for CANopen
 * @param[in]  role      Specify CANopen role
 * @param[in]  config    Specify CANopen configuration.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t canopen_init(const uint8_t port, const uint8_t role, const canopen_config_t *config);

/**
 * Function prototype for Event Notify, used for user notify event.
 *
 * @param[in]  port      Specify CAN Port used for CANopen
 * @param[in]  type      Event type
 * @param[in]  len       Param len append this event
 * @param[in]  param     Param text append this event
 *
 * @return  0 on success, negative error on failure.
 */
int32_t co_event_notify(const uint8_t port, const co_event_t type, const uint8_t len, const void *param);

/**
 * Function prototype for Register user call-back.
 *
 * @param[in]  port      Specify CAN Port used for CANopen
 * @param[in]  type      Event type
 * @param[in]  cb        User call-back routine
 * @param[in]  user_data User data
 *
 * @return  0 on success, negative error on failure.
 */
int32_t co_schedule_register_user(const uint8_t port, const co_event_t type, schedule_user_cb cb, void *user_data);

/**
 * Function prototype for Register user install node.
 *
 * @param[in]  port            Specify CAN Port used for CANopen
 * @param[in]  node_cnt        Event type
 * @param[in]  node_list       User call-back routine
 * @param[in]  state_change_cb call-back if state change
 * @param[in]  user_data       User data
 *
 * @return  0 on success, negative error on failure.
 */
int32_t node_install(const uint8_t port, const uint8_t node_cnt, const uint8_t *node_list, node_state_change_cb_t state_change_cb, void *user_data);

/**
 * Function prototype for Register pdo cal-back, use for PDOTx(components)/PDORx(master).
 *
 * @param[in]  port      Specify CAN Port used for CANopen
 * @param[in]  node      Specify this cal-back reigister which CANopen node
 * @param[in]  cb        User call-back routine
 * @param[in]  user_data User data
 *
 * @return  0 on success, negative error on failure.
 */
int32_t pdo_register_cb(const uint8_t port, const uint8_t node, pdo_rx_cb_t cb, void *user_data);

/**
 * Function prototype for PDO setting.
 *
 * @param[in]  port  Specify CAN Port used for CANopen
 * @param[in]  node  Specify this cal-back reigister which CANopen node
 * @param[in]  idx   Using which PDO index for setting
 * @param[in]  param Text to be set
 * @param[in]  send  User decide if send out rightly
 *
 * @return  0 on success, negative error on failure.
 */
int32_t pdo_set(const uint8_t port, const uint8_t node, const pdo_idx_type_t idx, pdo_tx_set_t *param, const bool send);

/**
 * Function prototype for PDO Sending.
 *
 * @param[in]  port  Specify CAN Port used for CANopen
 * @param[in]  node  Specify this cal-back reigister which CANopen node
 * @param[in]  idx   Using which PDO index for setting
 *
 * @return  0 on success, negative error on failure.
 */
int32_t pdo_send(const uint8_t port, const uint8_t node, const pdo_idx_type_t idx);

/**
 * Function prototype for install SDO.
 *
 * @param[in]  port   Specify CAN Port used for CANopen
 * @param[in]  node   Specify this cal-back reigister which CANopen node
 * @param[in]  role   SDO role(Service/Client)
 * @param[in]  od     OD to be install
 * @param[in]  od_cnt OD count
 *
 * @return  0 on success, negative error on failure.
 */
int32_t sdo_install(const uint8_t port, const uint8_t node, const co_sdo_role_t role, od_entry_t *od, const uint8_t od_cnt);

/* APIs for CANopen master operate on SDO */

/**
 * Function prototype for modify OD text.
 *
 * @param[in]  node    Node information
 * @param[in]  idx     OD index
 * @param[in]  sub_idx OD sub-index
 * @param[in]  data    Data to be set
 * @param[in]  len     Data length to be modify
 *
 * @return  0 on success, negative error on failure.
 */
int32_t co_sdo_write_od(const can_node_id_t *node, const uint16_t idx, const uint8_t sub_idx, const void *data, const uint16_t len);

/**
 * Function prototype for Read OD text.
 *
 * @param[in]  node    Node information
 * @param[in]  idx     OD index
 * @param[in]  sub_idx OD sub-index
 * @param[in]  data    Buffer used to read
 * @param[in]  len     Buffer length
 *
 * @return  0 on success, negative error on failure.
 */
int32_t co_sdo_read_od(const can_node_id_t *node, const uint16_t idx, const uint8_t sub_idx, const void *buffer, const uint16_t buffer_len);

/* APIs for CANopen Slaver Setting OD text */

/**
 * Function prototype for modify OD text.
 *
 * @param[in]  node    Node information
 * @param[in]  idx     OD index
 * @param[in]  sub_idx OD sub-index
 * @param[in]  data    Data to be set
 * @param[in]  len     Data length to be modify
 *
 * @return  0 on success, negative error on failure.
 */
int32_t co_sdo_set_od(const can_node_id_t *node, const uint16_t idx, const uint8_t sub_idx, const void *data, const uint16_t len);

/**
 * Function prototype for register user notification if sdo is been modified(by SDO Client).
 *
 * @param[in]  node      Node information
 * @param[in]  idx       OD index
 * @param[in]  sub_idx   OD sub-index
 * @param[in]  cb        User Call-back routine
 * @param[in]  user_data User data to cb
 *
 * @return  0 on success, negative error on failure.
 * @REMARK: This is not thread safety if cusomer try to modify both call-back and user-data,
 *         please call this function in init process if you try to register both of them
 */
int32_t co_sdo_register_modify_event(const can_node_id_t *node_id, const uint16_t idx, const uint16_t sub_idx, sdo_modify_cb_t cb, void *user_data);

/* APIs for Node Manager */

/**
 * Function prototype for Command component into operation state.
 *
 * @param[in]  port      CANopen port to be operation
 * @param[in]  node      Node information
 *
 * @return  0 on success, negative error on failure.
 *
 */
int32_t co_node_enter_operation(const uint8_t port, const uint8_t node);

/**
 * Function prototype for Command component into operation state.
 *
 * @param[in]  port      CANopen port to be operation
 *
 * @return  0 on success, negative error on failure.
 *
 */
int32_t co_node_reset_all(const uint8_t port);

/**
 * Function prototype for when try to send out boot up message.
 *
 * @param[in]  port      CANopen port to be operation
 * @param[in]  node      Node information
 *
 * @return  0 on success, negative error on failure.
 *
 */
int32_t comp_boot_up_finish(const uint8_t port, const uint8_t node);

/**
 * Function prototype initial Security function for CANopen.
 *
 * @param[in]  node_id  node information
 *
 * @return  0 on success, negative error on failure.
 *
 */
int32_t co_init_security(const can_node_id_t *node_id );

/**
 * Function prototype for decrpt security data.
 *
 * @param[in]   data       data to decrpt
 * @param[in]   src_len    data length
 * @param[out]  out_buffer output buffer
 * @param[out]  buffer_len output buffer length
 *
 * @return  0 on success, negative error on failure.
 *
 */
int32_t co_decrpt(const void *data, const uint16_t src_len, void *out_buffer, const uint16_t buffer_len);

/**
 * Function prototype for encryp security data.
 *
 * @param[in]   node_id  node information
 * @param[in]   data       data to decrpt
 * @param[in]   src_len    data length
 * @param[out]  out_buffer output buffer
 * @param[out]  buffer_len output buffer length
 *
 * @return  0 on success, negative error on failure.
 *
 */
int32_t co_encrypt(const can_node_id_t *node_id, const void *data, const uint16_t src_len, void *out_buffer, const uint16_t buffer_len);

/**
 * Function prototype for Set Key.
 *
 * @param[in]  node_id  node information
 * @param[in]  data     data to set
 * @param[in]  len      data length
 *
 * @return  0 on success, negative error on failure.
 *
 */
int32_t co_set_key2(const can_node_id_t *node_id, const void *data, const uint16_t len);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* AOS_CANOPEN__H */
