
#pragma once

/* MiCO -> BTE Conversion  */
#define int8_t          INT8
#define int16_t         INT16
#define int32_t         INT32
#define uint8_t         UINT8
#define uint16_t        UINT16
#define uint32_t        UINT32
#define mico_bool_t     BOOLEAN

#define mico_bt_dev_status_t                                   tBTM_STATUS
#define mico_bt_management_evt_t                               tBTM_EVENT
#define mico_bt_dev_class_t                                    DEV_CLASS
#define mico_bt_device_address_t                               BD_ADDR
#define mico_bt_device_address_ptr_t                           BD_ADDR_PTR
#define mico_bt_dev_io_cap_t                                   tBTM_IO_CAP
#define mico_bt_dev_oob_data_t                                 tBTM_OOB_DATA
#define mico_bt_dev_le_auth_req_t                              tBTM_LE_AUTH_REQ
#define mico_bt_dev_le_key_type_t                              tBTM_LE_KEY_TYPE
#define mico_bt_dev_auth_req_t                                 tBTM_AUTH_REQ
#define mico_bt_device_type_t                                  tBT_DEVICE_TYPE
#define mico_bt_dev_bonded_device_info_t                       tBT_BONDED_DEVICE_INFO_TYPE
#define mico_bt_security_key_value_t                           tBTM_SEC_KEY_VALUE
#define mico_bt_transport_t                                    tBT_TRANSPORT
#define mico_bt_ble_address_t                                  tBLE_BD_ADDR
#define mico_bt_ble_address_type_t                             tBLE_ADDR_TYPE
#define mico_bt_ble_conn_mode_t                                tBLE_CONN_MODE
#define mico_bt_dev_passkey_entry_type_t                       tBTM_SP_KEY_TYPE
#define mico_bt_connection_status_change_cback_t               tBTM_ACL_DB_CHANGE_CB   /* modified from bte */
#define mico_bt_dev_inq_parms_t                                tBTM_INQ_PARMS          /* modified from bte */
#define mico_bt_dev_inquiry_scan_result_t                      tBTM_INQ_RESULTS        /* modified from bte */
#define mico_bt_inquiry_result_cback_t                         tBTM_INQ_RESULTS_CB
#define mico_bt_dev_vendor_specific_command_complete_cback_t   tBTM_VSC_CMPL_CB
#ifndef mico_bt_dev_vendor_specific_command_complete_params_t
#define mico_bt_dev_vendor_specific_command_complete_params_t  tBTM_VSC_CMPL
#endif
#define mico_bt_ble_advert_mask_t                              tBTM_BLE_AD_MASK
#define mico_bt_ble_advert_data_t                              tBTM_BLE_ADV_DATA
#define mico_bt_ble_conn_type_t                                tBTM_BLE_CONN_TYPE
#define mico_bt_ble_selective_conn_cback_t                     tBTM_BLE_SEL_CBACK
#define mico_bt_ble_scan_type_t                                tBTM_BLE_SCAN_TYPE          /* new */
#define mico_bt_ble_advert_mode_t                              tBTM_BLE_AVERT_MODE         /* new */
#define mico_bt_ble_scan_result_cback_t                        tBTM_BLE_SCAN_RESULT_CBACK  /* new */
#define mico_bt_ble_scan_results_t                             tBTM_BLE_SCAN_RESULT        /* new */
#define mico_bt_ble_scan_mode_t                                tBTM_BLE_SCAN_MODE
#define mico_bt_ble_advert_chnl_map_t                          tBTM_BLE_ADV_CHNL_MAP
#define mico_dev_ble_signature_t                               BLE_SIGNATURE
#define mico_bt_ble_advert_filter_policy_t                     tBTM_BLE_AFP
#define mico_bt_device_link_keys_t                             tBTM_PAIRED_DEVICE_LINK_KEYS
#define mico_bt_local_identity_keys_t                          tBTM_LOCAL_IDENTITY_KEYS

/* l2cap mico-to-bte translation */
#define mico_bt_l2c_appl_info_t                                tL2CAP_APPL_INFO
#define mico_bt_l2c_fixed_chnl_reg_t                           tL2CAP_FIXED_CHNL_REG
#define mico_bt_l2cap_le_appl_information_t                    tL2CAP_LE_APPL_INFO
#define mico_bt_l2c_cfg_info_t                                 tL2CAP_CFG_INFO
#define mico_bt_l2c_ch_cfg_bits_t                              tL2CAP_CH_CFG_BITS
#define mico_bt_l2cap_ertm_information_t                       tL2CAP_ERTM_INFO
#define mico_bt_l2cap_chnl_priority_t                          tL2CAP_CHNL_PRIORITY
#define mico_bt_l2cap_nocp_cback_t                             tL2CA_NOCP_CB
#define mico_bt_l2cap_chnl_data_rate_t                         tL2CAP_CHNL_DATA_RATE

#define mico_bt_gatt_appearance_t                              tGATT_APPEARANCE
#define mico_bt_gatt_status_t                                  tGATT_STATUS
#define mico_bt_gatt_write_t                                   tGATT_WRITE_REQ
#define mico_bt_gatt_read_t                                    tGATT_READ_REQ
#define mico_bt_gatt_exec_flag_t                               tGATT_EXEC_FLAG

#define mico_bt_hidd_status_t                                  tHID_STATUS
#define mico_bt_hidd_reg_info_t                                tHID_DEV_REG_INFO
#define mico_bt_hidd_callback_t                                tHID_DEV_CBACK_DATA

#define mico_bt_sco_enh_esco_params_t                          tBTM_ENH_ESCO_PARAMS
