/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos_bt_ble.h"
#include "aos_bt_dev.h"
#include "aos_bt_gatt.h"
//#include "aos_bt_int.h"
//include "aos_bt_l2c.h"
//#include "aos_bt_rfcomm.h"
#include "aos_bt_sco.h"
#include "aos_bt_sdp.h"
#include "aos_bt_stack.h"
#include "aos_bt_nvram_access.h"

void aos_bt_dev_read_local_addr (aos_bt_device_address_t bd_addr)
{
    mico_bt_dev_read_local_addr(bd_addr);
}

void aos_bt_ble_security_grant(aos_bt_device_address_t bd_addr, uint8_t res)
{
    mico_bt_ble_security_grant(bd_addr, res);
}

void aos_bt_dev_pass_key_req_reply(aos_bt_result_t res, aos_bt_device_address_t bd_addr, uint32_t passkey)
{
    mico_bt_dev_pass_key_req_reply(res, bd_addr, passkey);
}

void aos_bt_dev_confirm_req_reply(aos_bt_result_t res, aos_bt_device_address_t bd_addr)
{
    mico_bt_dev_confirm_req_reply(res, bd_addr);
}

aos_bt_result_t aos_bt_stack_init(aos_bt_management_cback_t *p_bt_management_cback,
                                  const aos_bt_cfg_settings_t     *p_bt_cfg_settings,
                                  const aos_bt_cfg_buf_pool_t     aos_bt_cfg_buf_pools[AOS_BT_CFG_NUM_BUF_POOLS])
{
    return mico_bt_stack_init(p_bt_management_cback, p_bt_cfg_settings, aos_bt_cfg_buf_pools);
}

aos_bt_gatt_status_t aos_bt_gatt_send_indication_confirm (uint16_t conn_id, uint16_t handle)
{
    return mico_bt_gatt_send_indication_confirm(conn_id, handle);
}

aos_bt_gatt_status_t aos_bt_gatt_register (aos_bt_gatt_app_interface_t gatt_if, aos_bt_gatt_cback_t *p_gatt_cback)
{
    return mico_bt_gatt_register(gatt_if, p_gatt_cback);
}

aos_bt_ble_scan_type_t aos_bt_ble_get_current_scan_state(void)
{
    return mico_bt_ble_get_current_scan_state();
}

aos_bt_result_t  aos_bt_ble_scan (aos_bt_ble_scan_type_t scan_type, aos_bool_t duplicate_filter_enable,
                                  aos_bt_ble_scan_result_cback_t *p_scan_result_cback)
{
    return mico_bt_ble_scan(scan_type, duplicate_filter_enable, p_scan_result_cback);
}

aos_bt_gatt_status_t aos_bt_gatt_disconnect (uint16_t conn_id)
{
    return mico_bt_gatt_disconnect(conn_id);
}

aos_bt_gatt_status_t aos_bt_gatt_db_init (const uint8_t *p_gatt_db, uint16_t gatt_db_size)
{
    return mico_bt_gatt_db_init(p_gatt_db, gatt_db_size);
}

aos_bt_result_t aos_bt_ble_set_advertisement_data(aos_bt_ble_advert_mask_t  data_mask,
                                                  aos_bt_ble_advert_data_t *p_data)
{
    return mico_bt_ble_set_advertisement_data(data_mask, p_data);
}

aos_bt_dev_status_t aos_bt_ble_set_scan_response_data(aos_bt_ble_advert_mask_t data_mask,
                                                      aos_bt_ble_advert_data_t *p_data)
{
    return mico_bt_ble_set_scan_response_data(data_mask, p_data);
}

aos_bt_result_t aos_bt_dev_sec_bond (aos_bt_device_address_t bd_addr, aos_bt_ble_address_type_t bd_addr_type,
                                     aos_bt_transport_t transport, uint8_t pin_len, uint8_t *p_pin)
{
    return mico_bt_dev_sec_bond(bd_addr, bd_addr_type, transport, pin_len, p_pin);
}

aos_bt_result_t aos_bt_dev_set_encryption (aos_bt_device_address_t bd_addr, aos_bt_transport_t transport,
                                           void *p_ref_data)
{
    return mico_bt_dev_set_encryption(bd_addr, transport, p_ref_data);
}

aos_bool_t aos_bt_dev_find_bonded_device( aos_bt_device_address_t bd_addr)
{
    return mico_bt_dev_find_bonded_device(bd_addr);
}

aos_bt_result_t aos_bt_start_advertisements(aos_bt_ble_advert_mode_t advert_mode,
                                            aos_bt_ble_address_type_t directed_advertisement_bdaddr_type,
                                            aos_bt_device_address_ptr_t directed_advertisement_bdaddr_ptr)
{
    return mico_bt_start_advertisements(advert_mode, directed_advertisement_bdaddr_type, directed_advertisement_bdaddr_ptr);
}

aos_bool_t aos_bt_ble_update_advertising_white_list(aos_bool_t add, const aos_bt_device_address_t remote_bda)
{
    return mico_bt_ble_update_advertising_white_list(add, remote_bda);
}

aos_bt_result_t aos_bt_dev_sec_bond_cancel (aos_bt_device_address_t bd_addr)
{
    return mico_bt_dev_sec_bond_cancel(bd_addr);
}

aos_bt_result_t aos_bt_dev_delete_bonded_device(aos_bt_device_address_t bd_addr)
{
    return mico_bt_dev_delete_bonded_device(bd_addr);
}

aos_bt_gatt_status_t aos_bt_gatt_send_discover (uint16_t conn_id,
                                                aos_bt_gatt_discovery_type_t discovery_type,
                                                aos_bt_gatt_discovery_param_t *p_discovery_param )
{
    return mico_bt_gatt_send_discover(conn_id, discovery_type, p_discovery_param);
}

aos_bt_gatt_status_t aos_bt_gatt_send_read (uint16_t conn_id, aos_bt_gatt_read_type_t type,
                                            aos_bt_gatt_read_param_t *p_read)
{
    return mico_bt_gatt_send_read(conn_id, type, p_read);
}

aos_bt_gatt_status_t aos_bt_gatt_send_write (uint16_t conn_id, aos_bt_gatt_write_type_t type,
                                             aos_bt_gatt_value_t *p_write)
{
    return mico_bt_gatt_send_write(conn_id, type, p_write);
}

aos_bool_t aos_bt_ble_update_background_connection_device(aos_bool_t add_remove, aos_bt_device_address_t remote_bda)
{
    return mico_bt_ble_update_background_connection_device(add_remove, remote_bda);
}

aos_bool_t aos_bt_gatt_cancel_connect (aos_bt_device_address_t bd_addr, aos_bool_t is_direct)
{
    return mico_bt_gatt_cancel_connect(bd_addr, is_direct);
}

aos_bool_t aos_bt_gatt_le_connect (aos_bt_device_address_t bd_addr,
                                   aos_bt_ble_address_type_t bd_addr_type,
                                   aos_bt_ble_conn_mode_t conn_mode,
                                   aos_bool_t is_direct)
{
    return mico_bt_gatt_le_connect(bd_addr, bd_addr_type, conn_mode, is_direct);
}

aos_bt_result_t aos_bt_dev_read_tx_power (aos_bt_device_address_t remote_bda, aos_bt_transport_t transport,
                                          aos_bt_dev_cmpl_cback_t *p_cback)
{
    return mico_bt_dev_read_tx_power(remote_bda, transport, p_cback);
}

aos_bt_result_t aos_bt_dev_read_rssi (aos_bt_device_address_t remote_bda, aos_bt_transport_t transport,
                                      aos_bt_dev_cmpl_cback_t *p_cback)
{
    return mico_bt_dev_read_rssi(remote_bda, transport, p_cback);
}

aos_bt_gatt_status_t aos_bt_gatt_send_indication (uint16_t conn_id, uint16_t attr_handle, uint16_t *val_len,
                                                  uint8_t *p_val )
{
    return mico_bt_gatt_send_indication(conn_id, attr_handle, val_len, p_val);
}

aos_bt_result_t aos_bt_dev_get_bonded_devices(aos_bt_dev_bonded_device_info_t *p_paired_device_list,
                                              uint16_t *p_num_devices)
{
    return mico_bt_dev_get_bonded_devices(p_paired_device_list, p_num_devices);
}

aos_bt_result_t aos_bt_dev_get_key_by_keytype(aos_bt_device_address_t bd_addr, aos_bt_dev_le_key_type_t key_type,
                                              aos_bt_security_key_value_t *p_sec_keys)
{
    return mico_bt_dev_get_key_by_keytype(bd_addr, key_type, p_sec_keys);
}

aos_bool_t aos_bt_ble_get_background_connection_device_size(uint8_t *size)
{
    return mico_bt_ble_get_background_connection_device_size(size);
}

uint8_t *aos_bt_ble_check_advertising_data( uint8_t *p_adv, aos_bt_ble_advert_type_t type, uint8_t *p_length)
{
    return mico_bt_ble_check_advertising_data(p_adv, type, p_length);
}
