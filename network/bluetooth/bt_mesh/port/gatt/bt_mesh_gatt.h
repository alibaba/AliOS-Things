/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BT_MESH_GATT_H
#define BT_MESH_GATT_H

#include "bt_mesh_glue.h"

/** @brief Register connection callbacks.
 *
 *  Register callbacks to monitor the state of connections.
 *
 *  @param cb Callback struct.
 */
void bt_mesh_conn_cb_register(struct bt_mesh_conn_cb *cb);

/** @brief Increment a connection's reference count.
 *
 *  Increment the reference count of a connection object.
 *
 *  @param conn Connection object.
 *
 *  @return Connection object with incremented reference count.
 */
bt_mesh_conn_t bt_mesh_conn_ref(bt_mesh_conn_t conn);

/** @brief Decrement a connection's reference count.
 *
 *  Decrement the reference count of a connection object.
 *
 *  @param conn Connection object.
 */
void bt_mesh_conn_unref(bt_mesh_conn_t conn);

/** @brief Disconnect from a remote device or cancel pending connection.
 *
 *  Disconnect an active connection with the specified reason code or cancel
 *  pending outgoing connection.
 *
 *  @param conn Connection to disconnect.
 *  @param reason Reason code for the disconnection.
 *
 *  @return Zero on success or (negative) error code on failure.
 */
int bt_mesh_conn_disconnect(bt_mesh_conn_t conn, uint8_t reason);

/** @brief Register GATT service.
 *
 *  Register GATT service. Applications can make use of
 *  macros such as BT_GATT_PRIMARY_SERVICE, BT_GATT_CHARACTERISTIC,
 *  BT_GATT_DESCRIPTOR, etc.
 *
 *  @param svc Service containing the available attributes
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_mesh_gatt_service_register(struct bt_mesh_gatt_service *svc);

/** @brief Unregister GATT service.
 *
 *  @param svc Service to be unregistered.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_mesh_gatt_service_unregister(struct bt_mesh_gatt_service *svc);

/** @brief Notify attribute value change.
 *
 *  Send notification of attribute value change, if connection is NULL notify
 *  all peer that have notification enabled via CCC otherwise do a direct
 *  notification only the given connection.
 *
 *  The attribute object must be the so called Characteristic Value Descriptor,
 *  its usually declared with BT_GATT_DESCRIPTOR after BT_GATT_CHARACTERISTIC
 *  and before BT_GATT_CCC.
 *
 *  @param conn Connection object.
 *  @param attr Characteristic Value Descriptor attribute.
 *  @param data Pointer to Attribute data.
 *  @param len Attribute value length.
 */
int bt_mesh_gatt_notify(bt_mesh_conn_t conn, const struct bt_mesh_gatt_attr *attr,
                        const void *data, uint16_t len);

/** @brief Generic Read Attribute value helper.
 *
 *  Read attribute value storing the result into buffer.
 *
 *  @param conn Connection object.
 *  @param attr Attribute to read.
 *  @param buf Buffer to store the value.
 *  @param buf_len Buffer length.
 *  @param offset Start offset.
 *  @param value Attribute value.
 *  @param value_len Length of the attribute value.
 *
 *  @return int number of bytes read in case of success or negative values in
 *  case of error.
 */
int bt_mesh_gatt_attr_read(bt_mesh_conn_t conn, const struct bt_mesh_gatt_attr *attr,
                               void *buf, uint16_t buf_len, uint16_t offset,
                               const void *value, uint16_t value_len);

/** @brief Get ATT MTU for a connection
 *
 *  Get negotiated ATT connection MTU, note that this does not equal the largest
 *  amount of attribute data that can be transferred within a single packet.
 *
 *  @param conn Connection object.
 *
 *  @return MTU in bytes
 */
uint16_t bt_mesh_gatt_get_mtu(bt_mesh_conn_t conn);

#endif //BT_MESH_GATT_H
