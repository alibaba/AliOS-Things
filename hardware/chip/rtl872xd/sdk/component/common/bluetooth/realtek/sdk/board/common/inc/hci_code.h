/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _HCI_CODE_H_
#define _HCI_CODE_H_

/** HCI specific definitions */
#define HCI_CMD_PKT     0x01
#define HCI_ACL_PKT     0x02
#define HCI_SCO_PKT     0x03
#define HCI_EVT_PKT     0x04

/** HCI Spec Versions https://www.bluetooth.com/specifications/assigned-numbers/link-manager*/
#define HCI_VERSION_10B                         0
#define HCI_VERSION_11                          1
#define HCI_VERSION_12                          2
#define HCI_VERSION_20EDR                       3
#define HCI_VERSION_21EDR                       4
#define HCI_VERSION_30HS                        5
#define HCI_VERSION_40                          6
#define HCI_VERSION_41                          7
#define HCI_VERSION_42                          8

/** HCI OGF Codes */
#define HCI_LINK_CONTROL                        1
#define HCI_LINK_POLICY                         2
#define HCI_CONTROL                             3
#define HCI_INFORMATION                         4
#define HCI_STATUS                              5
#define HCI_TESTING                             6
#define HCI_LE_CONTROL                          8
#define HCI_VENDOR                              0x3F
#define HCI_TCI                                 0x3E        /**< Test Control Interface */

#define HCI_OGF(opcode)                         (opcode >> 10)

/** HCI Vendor codes */
#if F_BT_CONTROLLER_HANDSHAKE
#define HCI_VENDOR_HANDSHAKE                    0xFC97
#endif

/** HCI Command codes */
#define HCI_NOP                                         (0x00)
#define HCI_INQUIRY                                     (0x01 | (HCI_LINK_CONTROL << 10))
#define HCI_INQUIRY_CANCEL                              (0x02 | (HCI_LINK_CONTROL << 10))
#define HCI_PERIODIC_INQUIRY_MODE                       (0x03 | (HCI_LINK_CONTROL << 10))
#define HCI_EXIT_PERIODIC_INQUIRY_MODE                  (0x04 | (HCI_LINK_CONTROL << 10))
#define HCI_CREATE_CONNECTION                           (0x05 | (HCI_LINK_CONTROL << 10))
#define HCI_DISCONNECT                                  (0x06 | (HCI_LINK_CONTROL << 10))
#define HCI_ADD_SCO_CONNECTION                          (0x07 | (HCI_LINK_CONTROL << 10))
#define HCI_CREATE_CONNECTION_CANCEL                    (0x08 | (HCI_LINK_CONTROL << 10))
#define HCI_ACCEPT_CONNECTION_REQUEST                   (0x09 | (HCI_LINK_CONTROL << 10))
#define HCI_REJECT_CONNECTION_REQUEST                   (0x0a | (HCI_LINK_CONTROL << 10))
#define HCI_LINK_KEY_REQUEST_REPLY                      (0x0b | (HCI_LINK_CONTROL << 10))
#define HCI_LINK_KEY_REQUEST_NEG_REPLY                  (0x0c | (HCI_LINK_CONTROL << 10))
#define HCI_PIN_CODE_REQUEST_REPLY                      (0x0d | (HCI_LINK_CONTROL << 10))
#define HCI_PIN_CODE_REQUEST_NEG_REPLY                  (0x0e | (HCI_LINK_CONTROL << 10))
#define HCI_CHANGE_CONNECTION_PACKET_TYPE               (0x0f | (HCI_LINK_CONTROL << 10))
#define HCI_AUTHENTICATION_REQUESTED                    (0x11 | (HCI_LINK_CONTROL << 10))
#define HCI_SET_CONNECTION_ENCRYPTION                   (0x13 | (HCI_LINK_CONTROL << 10))
#define HCI_CHANGE_CONNECTION_LINK_KEY                  (0x15 | (HCI_LINK_CONTROL << 10))
#define HCI_MASTER_LINK_KEY                             (0x17 | (HCI_LINK_CONTROL << 10))
#define HCI_REMOTE_NAME_REQUEST                         (0x19 | (HCI_LINK_CONTROL << 10))
#define HCI_READ_REMOTE_SUPPORTED_FEATURES              (0x1b | (HCI_LINK_CONTROL << 10))
#define HCI_READ_REMOTE_EXTENDED_FEATURES               (0x1c | (HCI_LINK_CONTROL << 10))
#define HCI_READ_REMOTE_VERSION_INFORMATION             (0x1d | (HCI_LINK_CONTROL << 10))
#define HCI_READ_CLOCK_OFFSET                           (0x1f | (HCI_LINK_CONTROL << 10))
#define HCI_READ_LMP_HANDLE                             (0x20 | (HCI_LINK_CONTROL << 10))
#define HCI_EXCHANGE_FIXED_INFO                         (0x21 | (HCI_LINK_CONTROL << 10))
#define HCI_EXCHANGE_ALIAS_INFO                         (0x22 | (HCI_LINK_CONTROL << 10))
#define HCI_PRIVATE_PAIRING_REQUEST_REPLY               (0x23 | (HCI_LINK_CONTROL << 10))
#define HCI_PRIVATE_PAIRING_REQUEST_NEGATIVE_REPLY      (0x24 | (HCI_LINK_CONTROL << 10))
#define HCI_GENERATED_ALIAS                             (0x25 | (HCI_LINK_CONTROL << 10))
#define HCI_ALIAS_ADDRESS_REQUEST_REPLY                 (0x26 | (HCI_LINK_CONTROL << 10))
#define HCI_ALIAS_ADDRESS_REQUEST_NEGATIVE_REPLY        (0x27 | (HCI_LINK_CONTROL << 10))
#define HCI_SETUP_SYNCHRONOUS_CONNECTION                (0x28 | (HCI_LINK_CONTROL << 10))
#define HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST       (0x29 | (HCI_LINK_CONTROL << 10))
#define HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST       (0x2A | (HCI_LINK_CONTROL << 10))
#define HCI_IO_CAPABILITY_REQUEST_REPLY                 (0x2B | (HCI_LINK_CONTROL << 10))
#define HCI_USER_CONFIRMATION_REQUEST_REPLY             (0x2C | (HCI_LINK_CONTROL << 10))
#define HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY    (0x2D | (HCI_LINK_CONTROL << 10))
#define HCI_USER_PASSKEY_REQUEST_REPLY                  (0x2E | (HCI_LINK_CONTROL << 10))
#define HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY         (0x2F | (HCI_LINK_CONTROL << 10))
#define HCI_REMOTE_OOB_DATA_REQUEST_REPLY               (0x30 | (HCI_LINK_CONTROL << 10))
#define HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY      (0x33 | (HCI_LINK_CONTROL << 10))
#define HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY        (0x34 | (HCI_LINK_CONTROL << 10))

#define HCI_TRUNCATED_PAGE                              (0x3F | (HCI_LINK_CONTROL << 10))
#define HCI_TRUNCATED_PAGE_CANCEL                       (0x40 | (HCI_LINK_CONTROL << 10))
#define HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST          (0x41 | (HCI_LINK_CONTROL << 10))
#define HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST_RECEIVE  (0x42 | (HCI_LINK_CONTROL << 10))
#define HCI_START_SYNCHRONIZATION_TRAIN                 (0x43 | (HCI_LINK_CONTROL << 10))
#define HCI_RECEIVE_SYNCHRONIZATION_TRAIN               (0x44 | (HCI_LINK_CONTROL << 10))

#define HCI_HOLD_MODE                          (0x01 | (HCI_LINK_POLICY  << 10))
#define HCI_SNIFF_MODE                         (0x03 | (HCI_LINK_POLICY  << 10))
#define HCI_EXIT_SNIFF_MODE                    (0x04 | (HCI_LINK_POLICY  << 10))
#define HCI_PARK_MODE                          (0x05 | (HCI_LINK_POLICY  << 10))
#define HCI_EXIT_PARK_MODE                     (0x06 | (HCI_LINK_POLICY  << 10))
#define HCI_QOS_SETUP                          (0x07 | (HCI_LINK_POLICY  << 10))
#define HCI_ROLE_DISCOVERY                     (0x09 | (HCI_LINK_POLICY  << 10))
#define HCI_SWITCH_ROLE                        (0x0b | (HCI_LINK_POLICY  << 10))
#define HCI_READ_LINK_POLICY                   (0x0c | (HCI_LINK_POLICY  << 10))
#define HCI_WRITE_LINK_POLICY_SETTINGS         (0x0d | (HCI_LINK_POLICY  << 10))

#define HCI_READ_DEFAULT_LINK_POLICY_SETTINGS  (0x0E | (HCI_LINK_POLICY  << 10))
#define HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS (0x0F | (HCI_LINK_POLICY  << 10))
#define HCI_FLOW_SPECIFICATION                 (0x10 | (HCI_LINK_POLICY  << 10))
#define HCI_SNIFF_SUBRATING                    (0x11 | (HCI_LINK_POLICY  << 10))

#define HCI_SET_EVENT_MASK                     (0x01 | (HCI_CONTROL      << 10))
#define HCI_RESET                              (0x03 | (HCI_CONTROL      << 10))
#define HCI_SET_EVENT_FILTER                   (0x05 | (HCI_CONTROL      << 10))
#define HCI_FLUSH                              (0x08 | (HCI_CONTROL      << 10))
#define HCI_READ_PIN_TYPE                      (0x09 | (HCI_CONTROL      << 10))
#define HCI_WRITE_PIN_TYPE                     (0x0a | (HCI_CONTROL      << 10))
#define HCI_CREATE_NEW_UNIT_KEY                (0x0b | (HCI_CONTROL      << 10))
#define HCI_READ_STORED_LINK_KEY               (0x0d | (HCI_CONTROL      << 10))
#define HCI_WRITE_STORED_LINK_KEY              (0x11 | (HCI_CONTROL      << 10))
#define HCI_DELETE_STORED_LINK_KEY             (0x12 | (HCI_CONTROL      << 10))
#define HCI_WRITE_LOCAL_NAME                   (0x13 | (HCI_CONTROL      << 10))
#define HCI_READ_LOCAL_NAME                    (0x14 | (HCI_CONTROL      << 10))
#define HCI_READ_CONNECTION_ACCEPT_TIMEOUT     (0x15 | (HCI_CONTROL      << 10))
#define HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT    (0x16 | (HCI_CONTROL      << 10))
#define HCI_READ_PAGE_TIMEOUT                  (0x17 | (HCI_CONTROL      << 10))
#define HCI_WRITE_PAGE_TIMEOUT                 (0x18 | (HCI_CONTROL      << 10))
#define HCI_READ_SCAN_ENABLE                   (0x19 | (HCI_CONTROL      << 10))
#define HCI_WRITE_SCAN_ENABLE                  (0x1a | (HCI_CONTROL      << 10))
#define HCI_READ_PAGE_SCAN_ACTIVITY            (0x1b | (HCI_CONTROL      << 10))
#define HCI_WRITE_PAGE_SCAN_ACTIVITY           (0x1c | (HCI_CONTROL      << 10))
#define HCI_READ_INQUIRY_SCAN_ACTIVITY         (0x1d | (HCI_CONTROL      << 10))
#define HCI_WRITE_INQUIRY_SCAN_ACTIVITY        (0x1e | (HCI_CONTROL      << 10))
#define HCI_READ_AUTHENTICATION_ENABLE         (0x1f | (HCI_CONTROL      << 10))
#define HCI_WRITE_AUTHENTICATION_ENABLE        (0x20 | (HCI_CONTROL      << 10))
#define HCI_READ_ENCRYPTION_MODE               (0x21 | (HCI_CONTROL      << 10))
#define HCI_WRITE_ENCRYPTION_MODE              (0x22 | (HCI_CONTROL      << 10))
#define HCI_READ_CLASS_OF_DEVICE               (0x23 | (HCI_CONTROL      << 10))
#define HCI_WRITE_CLASS_OF_DEVICE              (0x24 | (HCI_CONTROL      << 10))
#define HCI_READ_VOICE_SETTING                 (0x25 | (HCI_CONTROL      << 10))
#define HCI_WRITE_VOICE_SETTING                (0x26 | (HCI_CONTROL      << 10))
#define HCI_READ_AUTOMATIC_FLUSH_TIMEOUT       (0x27 | (HCI_CONTROL      << 10))
#define HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT      (0x28 | (HCI_CONTROL      << 10))
#define HCI_READ_NUM_BROADCASTS_RETRANS        (0x29 | (HCI_CONTROL      << 10))
#define HCI_WRITE_NUM_BROADCASTS_RETRANS       (0x2a | (HCI_CONTROL      << 10))
#define HCI_READ_HOLD_MODE_ACTIVITY            (0x2b | (HCI_CONTROL      << 10))
#define HCI_WRITE_HOLD_MODE_ACTIVITY           (0x2c | (HCI_CONTROL      << 10))
#define HCI_READ_TRANSMIT_POWER_LEVEL          (0x2d | (HCI_CONTROL      << 10))
#define HCI_READ_SCO_FLOW_CONTROL_ENABLE       (0x2e | (HCI_CONTROL      << 10))
#define HCI_WRITE_SCO_FLOW_CONTROL_ENABLE      (0x2f | (HCI_CONTROL      << 10))
#define HCI_SET_CONTROLLER_TO_HOST_FLOW        (0x31 | (HCI_CONTROL      << 10))
#define HCI_HOST_BUFFER_SIZE                   (0x33 | (HCI_CONTROL      << 10))
#define HCI_HOST_NUMBER_OF_COMPLETED_PACKETS   (0x35 | (HCI_CONTROL      << 10))
#define HCI_READ_LINK_SUPERVISION_TIMEOUT      (0x36 | (HCI_CONTROL      << 10))
#define HCI_WRITE_LINK_SUPERVISION_TIMEOUT     (0x37 | (HCI_CONTROL      << 10))
#define HCI_READ_NUMBER_OF_SUPPORTED_IAC       (0x38 | (HCI_CONTROL      << 10))
#define HCI_READ_CURRENT_IAC_LAP               (0x39 | (HCI_CONTROL      << 10))
#define HCI_WRITE_CURRENT_IAC_LAP              (0x3a | (HCI_CONTROL      << 10))
#define HCI_READ_PAGE_SCAN_PERIOD_MODE         (0x3b | (HCI_CONTROL      << 10))
#define HCI_WRITE_PAGE_SCAN_PERIOD_MODE        (0x3c | (HCI_CONTROL      << 10))
#define HCI_READ_PAGE_SCAN_MODE                (0x3d | (HCI_CONTROL      << 10))
#define HCI_WRITE_PAGE_SCAN_MODE               (0x3e | (HCI_CONTROL      << 10))

#define HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION         (0x3F | (HCI_CONTROL      << 10))
#define HCI_READ_INQUIRY_SCAN_TYPE                      (0x42 | (HCI_CONTROL      << 10))
#define HCI_WRITE_INQUIRY_SCAN_TYPE                     (0x43 | (HCI_CONTROL      << 10))
#define HCI_READ_INQUIRY_MODE                           (0x44 | (HCI_CONTROL      << 10))
#define HCI_WRITE_INQUIRY_MODE                          (0x45 | (HCI_CONTROL      << 10))
#define HCI_READ_PAGE_SCAN_TYPE                         (0x46 | (HCI_CONTROL      << 10))
#define HCI_WRITE_PAGE_SCAN_TYPE                        (0x47 | (HCI_CONTROL      << 10))
#define HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE            (0x48 | (HCI_CONTROL      << 10))
#define HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE           (0x49 | (HCI_CONTROL      << 10))
#define HCI_READ_ANONYMITY_MODE                         (0x4A | (HCI_CONTROL      << 10))
#define HCI_WRITE_ANONYMITY_MODE                        (0x4B | (HCI_CONTROL      << 10))
#define HCI_READ_ALIAS_AUTHENTICATION_ENABLE            (0x4C | (HCI_CONTROL      << 10))
#define HCI_WRITE_ALIAS_AUTHENTICATION_ENABLE           (0x4D | (HCI_CONTROL      << 10))
#define HCI_READ_ANONYMOUS_ADDRESS_CHANGE_PARAMETERS    (0x4E | (HCI_CONTROL      << 10))
#define HCI_WRITE_ANONYMOUS_ADDRESS_CHANGE_PARAMETERS   (0x4F | (HCI_CONTROL      << 10))
#define HCI_RESET_FIXED_ADDRESS_ATTEMPT_COUNTER         (0x50 | (HCI_CONTROL      << 10))
#define HCI_READ_EXTENDED_INQUIRY_RESPONSE              (0x51 | (HCI_CONTROL      << 10))
#define HCI_WRITE_EXTENDED_INQUIRY_RESPONSE             (0x52 | (HCI_CONTROL      << 10))
#define HCI_REFRESH_ENCRYPTION_KEY                      (0x53 | (HCI_CONTROL      << 10))
#define HCI_READ_SIMPLE_PAIRING_MODE                    (0x55 | (HCI_CONTROL      << 10))
#define HCI_WRITE_SIMPLE_PAIRING_MODE                   (0x56 | (HCI_CONTROL      << 10))
#define HCI_READ_LOCAL_OOB_DATA                         (0x57 | (HCI_CONTROL      << 10))
#define HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL  (0x58 | (HCI_CONTROL      << 10))
#define HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL          (0x59 | (HCI_CONTROL      << 10))
#define HCI_READ_DEFAULT_ERROUNEOUS_DATA_REPORTING      (0x5A | (HCI_CONTROL      << 10))
#define HCI_WRITE_DEFAULT_ERROUNEOUS_DATA_REPORTING     (0x5B | (HCI_CONTROL      << 10))
#define HCI_ENHANCED_FLUSH                              (0x5F | (HCI_CONTROL      << 10))
#define HCI_SEND_KEYPRESS_NOTIFICATION                  (0x60 | (HCI_CONTROL      << 10))

#define HCI_SET_EVENT_MASK_PAGE_2                       (0x63 | (HCI_CONTROL      << 10))
#define HCI_READ_LE_HOST_SUPPORTED                      (0x6C | (HCI_CONTROL      << 10))
#define HCI_WRITE_LE_HOST_SUPPORTED                     (0x6D | (HCI_CONTROL      << 10))

/** Connectionless_Slave_Broadcast cmd**/
#define HCI_SET_RESERVED_LT_ADDR                        (0x74 | (HCI_CONTROL      << 10))
#define HCI_DELETE_RESERVED_LT_ADDR                     (0x75 | (HCI_CONTROL      << 10))
#define HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST_DATA     (0x76 | (HCI_CONTROL      << 10))
#define HCI_READ_SYNCHRONIZATION_TRAIN_PARAMETERS       (0x77 | (HCI_CONTROL      << 10))
#define HCI_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS      (0x78 | (HCI_CONTROL      << 10))

#define HCI_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT       (0x7A | (HCI_CONTROL      << 10))


#define HCI_READ_LOCAL_VERSION_INFORMATION     (0x01 | (HCI_INFORMATION  << 10))
#define HCI_READ_LOCAL_SUPPORTED_COMMANDS      (0x02 | (HCI_INFORMATION  << 10))
#define HCI_READ_LOCAL_SUPPORTED_FEATURES      (0x03 | (HCI_INFORMATION  << 10))
#define HCI_READ_LOCAL_EXTENDED_FEATURES       (0x04 | (HCI_INFORMATION  << 10))
#define HCI_READ_BUFFER_SIZE                   (0x05 | (HCI_INFORMATION  << 10))
#define HCI_READ_COUNTRY_CODE                  (0x07 | (HCI_INFORMATION  << 10))
#define HCI_READ_BD_ADDR                       (0x09 | (HCI_INFORMATION  << 10))

#define HCI_READ_FAILED_CONTACT_COUNTER        (0x01 | (HCI_STATUS       << 10))
#define HCI_RESET_FAILED_CONTACT_COUNTER       (0x02 | (HCI_STATUS       << 10))
#define HCI_GET_LINK_QUALITY                   (0x03 | (HCI_STATUS       << 10))
#define HCI_READ_RSSI                          (0x05 | (HCI_STATUS       << 10))

#define HCI_READ_AFH_CHANNEL_MAP               (0x06 | (HCI_STATUS       << 10))
#define HCI_READ_CLOCK                         (0x07 | (HCI_STATUS       << 10))

#define HCI_READ_LOOPBACK_MODE                 (0x01 | (HCI_TESTING      << 10))
#define HCI_WRITE_LOOPBACK_MODE                (0x02 | (HCI_TESTING      << 10))
#define HCI_ENABLE_DEVICE_UNDER_TEST_MODE      (0x03 | (HCI_TESTING      << 10))
#define HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE    (0x04 | (HCI_TESTING      << 10))


#define HCI_LE_SET_EVENT_MASK                           (0x01 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_BUFFER_SIZE                         (0x02 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_LOCAL_SUPPORTED_FEATURES            (0x03 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_RANDOM_ADDRESS                       (0x05 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_ADVERTISING_PARAMETERS               (0x06 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER        (0x07 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_ADVERTISING_DATA                     (0x08 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_SCAN_RESPONSE_DATA                   (0x09 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_ADVERTISE_ENABLE                     (0x0A | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_SCAN_PARAMETERS                      (0x0B | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_SCAN_ENABLE                          (0x0C | (HCI_LE_CONTROL   << 10))
#define HCI_LE_CREATE_CONNECTION                        (0x0D | (HCI_LE_CONTROL   << 10))
#define HCI_LE_CREATE_CONNECTION_CANCEL                 (0x0E | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_WHITE_LIST_SIZE                     (0x0F | (HCI_LE_CONTROL   << 10))
#define HCI_LE_CLEAR_WHITE_LIST                         (0x10 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_ADD_DEVICE_TO_WHITE_LIST                 (0x11 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST            (0x12 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_CONNECTION_UPDATE                        (0x13 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION          (0x14 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_CHANNEL_MAP                         (0x15 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_REMOTE_USED_FEATURES                (0x16 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_ENCRYPT                                  (0x17 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_RAND                                     (0x18 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_START_ENCRYPTION                         (0x19 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_LONG_TERM_KEY_REQUEST_REPLY              (0x1A | (HCI_LE_CONTROL   << 10))
#define HCI_LE_LONG_TERM_KEY_REQUESTED_NEGATIVE_REPLY   (0x1B | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_SUPPORTED_STATES                    (0x1C | (HCI_LE_CONTROL   << 10))
#define HCI_LE_RECEIVER_TEST                            (0x1D | (HCI_LE_CONTROL   << 10))
#define HCI_LE_TRANSMITTER_TEST                         (0x1E | (HCI_LE_CONTROL   << 10))
#define HCI_LE_TEST_END                                 (0x1F | (HCI_LE_CONTROL   << 10))
#define HCI_LE_REMOTE_CONN_PARAM_REQUEST_REPLY          (0x20 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_REMOTE_CONN_PARAM_REQUEST_NEGATIVE_REPLY (0x21 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_DATA_LENGTH                          (0x22 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH       (0x23 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH      (0x24 | (HCI_LE_CONTROL   << 10))
#if 0
#define HCI_LE_READ_LOCAL_P256_PUBLIC_KEY               (0x25 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_GENERATE_DHKEY                           (0x26 | (HCI_LE_CONTROL   << 10))
#endif
#define HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST             (0x27 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST        (0x28 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_CLEAR_RESOLVING_LIST                     (0x29 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_RESOLVING_LIST_SIZE                 (0x2A | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_PEER_RESOLVABLE_ADDRESS             (0x2B | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS            (0x2C | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE            (0x2D | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT   (0x2E | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_MAXIMUM_DATA_LENGTH                 (0x2F | (HCI_LE_CONTROL   << 10))


#define HCI_LE_READ_PHY                                 (0x30 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_DEFAULT_PHY                          (0x31 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_PHY                                  (0x32 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_ENHANCED_RECEIVER_TEST                   (0x33 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_ENHANCED_TRANSMITTER_TEST                (0x34 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS       (0x35 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS      (0x36 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_EXTENDED_ADVERTISING_DATA            (0x37 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA          (0x38 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE          (0x39 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH     (0x3A | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_NUM_OF_SUPPORTED_ADVERTISING_SETS   (0x3B | (HCI_LE_CONTROL   << 10))
#define HCI_LE_REMOVE_ADVERTISING_SET                   (0x3C | (HCI_LE_CONTROL   << 10))
#define HCI_LE_CLEAR_ADVERTISING_SETS                   (0x3D | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_PERIODIC_ADV_PARAMS                  (0x3E | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_PERIODIC_ADV_DATA                    (0x3F | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_PERIODIC_ADV_ENABLE                  (0x40 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_EXTENDED_SCAN_PARAMS                 (0x41 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_EXTENDED_SCAN_ENABLE                 (0x42 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_EXTENDED_CREATE_CONNECTION               (0x43 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_PERIODIC_ADV_CREATE_SYNC                 (0x44 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL          (0x45 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_PERIODIC_ADV_TERMINATE_SYNC              (0x46 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_ADD_DEVICE_TO_PERIODIC_ADV_LIST          (0x47 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADV_LIST     (0x48 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_CLEAR_PERIODIC_ADVERTISER_LIST           (0x49 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE       (0x4A | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_TRANSMIT_POWER                      (0x4B | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_RF_PATH_COMPENSATION                (0x4C | (HCI_LE_CONTROL   << 10))
#define HCI_LE_WRITE_RF_PATH_COMPENSATION               (0x4D | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_PRIVACY_MODE                         (0x4E | (HCI_LE_CONTROL   << 10))

#define HCI_LE_SET_SUPPL_RECEIVE_TEST_PARAMS            (0x4F | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_SUPPL_TRANSMIT_TEST_PARAMS           (0x50 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_CONNLESS_SUPPL_TRANSMIT_PARAMS       (0x51 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_CONNLESS_SUPPL_TRANSMIT_ENABLE       (0x52 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_CONNLESS_IQ_SAMPLING_ENABLE          (0x53 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_CONN_SUPPL_RECEIVE_PARAMS            (0x54 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_SET_CONN_SUPPL_TRANSMIT_PARAMS           (0x55 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_CONN_SUPPL_REQUEST_ENABLE                (0x56 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_CONN_SUPPL_RESPONSE_ENABLE               (0x57 | (HCI_LE_CONTROL   << 10))
#define HCI_LE_READ_ANTENNA_INFORMATION                 (0x58 | (HCI_LE_CONTROL   << 10))

#define HCI_OP_VND_SET_BD_ADDR                          0xFC0D

/** local supported commands: format: octet, bit */
/**--- octet 0 */
#define HCI_SUPPORTED_COMMANDS_INQUIRY                                  0*8+0
#define HCI_SUPPORTED_COMMANDS_INQUIRY_CANCEL                           0*8+1
#define HCI_SUPPORTED_COMMANDS_PERIODIC_INQUIRY_MODE                    0*8+2
#define HCI_SUPPORTED_COMMANDS_EXIT_PERIODIC_INQUIRY_MODE               0*8+3
#define HCI_SUPPORTED_COMMANDS_CREATE_CONNECTION                        0*8+4
#define HCI_SUPPORTED_COMMANDS_DISCONNECT                               0*8+5
#define HCI_SUPPORTED_COMMANDS_ADD_SCO_CONNECTION                       0*8+6
#define HCI_SUPPORTED_COMMANDS_CANCEL_CREATE_CONNECTION                 0*8+7
/**--- octet 1 */
#define HCI_SUPPORTED_COMMANDS_ACCEPT_CONNECTION_REQUEST                1*8+0
#define HCI_SUPPORTED_COMMANDS_REJECT_CONNECTION_REQUEST                1*8+1
#define HCI_SUPPORTED_COMMANDS_LINK_KEY_REQUEST_REPLY                   1*8+2
#define HCI_SUPPORTED_COMMANDS_LINK_KEY_REQUEST_NEGATIVE_REPLY          1*8+3
#define HCI_SUPPORTED_COMMANDS_PIN_CODE_REQUEST_REPLY                   1*8+4
#define HCI_SUPPORTED_COMMANDS_PIN_CODE_REQUEST_NEGATIVE_REPLY          1*8+5
#define HCI_SUPPORTED_COMMANDS_CHANGE_CONNECTION_PACKET_TYPE            1*8+6
#define HCI_SUPPORTED_COMMANDS_AUTHENTICATION_REQUEST                   1*8+7
/**--- octet 2 */
#define HCI_SUPPORTED_COMMANDS_SET_CONNECTION_ENCRYPTION                2*8+0
#define HCI_SUPPORTED_COMMANDS_CHANGE_CONNECTION_LINK_KEY               2*8+1
#define HCI_SUPPORTED_COMMANDS_MASTER_LINK_KEY                          2*8+2
#define HCI_SUPPORTED_COMMANDS_REMOTE_NAME_REQUEST                      2*8+3
#define HCI_SUPPORTED_COMMANDS_CANCEL_REMOTE_NAME_REQUEST               2*8+4
#define HCI_SUPPORTED_COMMANDS_READ_REMOTE_SUPPORTED_FEATURES           2*8+5
#define HCI_SUPPORTED_COMMANDS_READ_REMOTE_EXTENDED_FEATURES            2*8+6
#define HCI_SUPPORTED_COMMANDS_READ_REMOTE_VERSION_INFORMATION          2*8+7
/**--- octet 3 */
#define HCI_SUPPORTED_COMMANDS_READ_CLOCK_OFFSET                        3*8+0
#define HCI_SUPPORTED_COMMANDS_READ_LMP_HANDLE                          3*8+1
#define HCI_SUPPORTED_COMMANDS_RESERVED3_2                              3*8+2
#define HCI_SUPPORTED_COMMANDS_RESERVED3_3                              3*8+3
#define HCI_SUPPORTED_COMMANDS_RESERVED3_4                              3*8+4
#define HCI_SUPPORTED_COMMANDS_RESERVED3_5                              3*8+5
#define HCI_SUPPORTED_COMMANDS_RESERVED3_6                              3*8+6
#define HCI_SUPPORTED_COMMANDS_RESERVED3_7                              3*8+7
/**--- octet 4 */
#define HCI_SUPPORTED_COMMANDS_RESERVED4_0                              4*8+0
#define HCI_SUPPORTED_COMMANDS_HOLD_MODE                                4*8+1
#define HCI_SUPPORTED_COMMANDS_SNIFF_MODE                               4*8+2
#define HCI_SUPPORTED_COMMANDS_EXIT_SNIFF_MODE                          4*8+3
#define HCI_SUPPORTED_COMMANDS_PARK_STATE                               4*8+4
#define HCI_SUPPORTED_COMMANDS_EXIT_PARK_STATE                          4*8+5
#define HCI_SUPPORTED_COMMANDS_QOS_SETUP                                4*8+6
#define HCI_SUPPORTED_COMMANDS_ROLE_DISCOVERY                           4*8+7
/**--- octet 5 */
#define HCI_SUPPORTED_COMMANDS_SWITCH_ROLE                              5*8+0
#define HCI_SUPPORTED_COMMANDS_READ_LINK_POLICY_SETTING                 5*8+1
#define HCI_SUPPORTED_COMMANDS_WRITE_LINK_POLICY_SETTING                5*8+2
#define HCI_SUPPORTED_COMMANDS_READ_DEFAULT_LINK_POLICY_SETTING         5*8+3
#define HCI_SUPPORTED_COMMANDS_WRITE_DEFAULT_LINK_POLICY_SETTING        5*8+4
#define HCI_SUPPORTED_COMMANDS_FLOW_SPECIFICATION                       5*8+5
#define HCI_SUPPORTED_COMMANDS_SET_EVENT_MARK                           5*8+6
#define HCI_SUPPORTED_COMMANDS_RESET                                    5*8+7
/**--- octet 6 */
#define HCI_SUPPORTED_COMMANDS_SET_EVENT_FILTER                         6*8+0
#define HCI_SUPPORTED_COMMANDS_FLUSH                                    6*8+1
#define HCI_SUPPORTED_COMMANDS_READ_PIN_TYPE                            6*8+2
#define HCI_SUPPORTED_COMMANDS_WRITE_PIN_TYPE                           6*8+3
#define HCI_SUPPORTED_COMMANDS_CREATE_NEW_UNIT_KEY                      6*8+4
#define HCI_SUPPORTED_COMMANDS_READ_STORED_LINK_KEY                     6*8+5
#define HCI_SUPPORTED_COMMANDS_WRITE_STORED_LINK_KEY                    6*8+6
#define HCI_SUPPORTED_COMMANDS_DELETE_STORED_LINK_KEY                   6*8+7
/**--- octet 7 */
#define HCI_SUPPORTED_COMMANDS_WRITE_LOCAL_NAME                         7*8+0
#define HCI_SUPPORTED_COMMANDS_READ_LOCAL_NAME                          7*8+1
#define HCI_SUPPORTED_COMMANDS_READ_CONNECTION_ACCEPT_TIMEOUT           7*8+2
#define HCI_SUPPORTED_COMMANDS_WRITE_CONNECTION_ACCEPT_TIMEOUT          7*8+3
#define HCI_SUPPORTED_COMMANDS_READ_PAGE_TIMEOUT                        7*8+4
#define HCI_SUPPORTED_COMMANDS_WRITE_PAGE_TIMEOUT                       7*8+5
#define HCI_SUPPORTED_COMMANDS_READ_SCAN_ENABLE                         7*8+6
#define HCI_SUPPORTED_COMMANDS_WRITE_SCAN_ENABLE                        7*8+7
/**--- octet 8 */
#define HCI_SUPPORTED_COMMANDS_READ_PAGE_SCAN_ACTIVITY                  8*8+0
#define HCI_SUPPORTED_COMMANDS_WRITE_PAGE_SCAN_ACTIVITY                 8*8+1
#define HCI_SUPPORTED_COMMANDS_READ_INQUIRY_SCAN_ACTIVITY               8*8+2
#define HCI_SUPPORTED_COMMANDS_WRITE_INQUIRY_SCAN_ACTIVITY              8*8+3
#define HCI_SUPPORTED_COMMANDS_READ_AUTHENTICATION_ENABLE               8*8+4
#define HCI_SUPPORTED_COMMANDS_WRITE_AUTHENTICATION_ENABLE              8*8+5
#define HCI_SUPPORTED_COMMANDS_READ_ENCRYPTION_MODE                     8*8+6
#define HCI_SUPPORTED_COMMANDS_WRITE_ENCRYPTION_MODE                    8*8+7
/**--- octet 9 */
#define HCI_SUPPORTED_COMMANDS_READ_CLASS_OF_DEVICE                     9*8+0
#define HCI_SUPPORTED_COMMANDS_WRITE_CLASS_OF_DEVICE                    9*8+1
#define HCI_SUPPORTED_COMMANDS_READ_VOICE_SETTING                       9*8+2
#define HCI_SUPPORTED_COMMANDS_WRITE_VOICE_SETTING                      9*8+3
#define HCI_SUPPORTED_COMMANDS_READ_AUTOMATIC_FLUSH_TIMEOUT             9*8+4
#define HCI_SUPPORTED_COMMANDS_WRITE_AUTOMATIC_FLUSH_TIMEOUT            9*8+5
#define HCI_SUPPORTED_COMMANDS_READ_NUM_BROADCAST_RETRANSMISSIONS       9*8+6
#define HCI_SUPPORTED_COMMANDS_WRITE_NUM_BROADCAST_RETRANSMISSIONS      9*8+7
/**--- octet 10 */
#define HCI_SUPPORTED_COMMANDS_READ_HOLD_MODE_ACTIVITY                  10*8+0
#define HCI_SUPPORTED_COMMANDS_WRITE_HOLD_MODE_ACTIVITY                 10*8+1
#define HCI_SUPPORTED_COMMANDS_READ_TRANSMIT_POWER_LEVEL                10*8+2
#define HCI_SUPPORTED_COMMANDS_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE     10*8+3
#define HCI_SUPPORTED_COMMANDS_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE    10*8+4
#define HCI_SUPPORTED_COMMANDS_SET_HOST_CONTROLLER_TO_HOST_FLOW_CONTROL 10*8+5
#define HCI_SUPPORTED_COMMANDS_HOST_BUFFER_SIZE                         10*8+6
#define HCI_SUPPORTED_COMMANDS_HOST_NUMBER_OF_COMPLETED_PACKETS         10*8+7
/**--- octet 11 */
#define HCI_SUPPORTED_COMMANDS_READ_LINK_SUPERVISION_TIMEOUT            11*8+0
#define HCI_SUPPORTED_COMMANDS_WRITE_LINK_SUPERVISION_TIMEOUT           11*8+1
#define HCI_SUPPORTED_COMMANDS_READ_NUMBER_OF_SUPPORTED_IAC             11*8+2
#define HCI_SUPPORTED_COMMANDS_READ_CURRENT_IAC_LAP                     11*8+3
#define HCI_SUPPORTED_COMMANDS_WRITE_CURRENT_IAC_LAP                    11*8+4
#define HCI_SUPPORTED_COMMANDS_READ_PAGE_SCAN_PERIOD_MODE               11*8+5
#define HCI_SUPPORTED_COMMANDS_WRITE_PAGE_SCAN_PERIOD_MODE              11*8+6
#define HCI_SUPPORTED_COMMANDS_READ_PAGE_SCAN_MODE                      11*8+7
/**--- octet 12 */
#define HCI_SUPPORTED_COMMANDS_WRITE_PAGE_SCAN_MODE                     12*8+0
#define HCI_SUPPORTED_COMMANDS_SET_AFH_CHANNEL_CLASSIFICATION           12*8+1
#define HCI_SUPPORTED_COMMANDS_RESERVED12_2                             12*8+2
#define HCI_SUPPORTED_COMMANDS_RESERVED12_3                             12*8+3
#define HCI_SUPPORTED_COMMANDS_READ_INQUIRY_SCAN_TYPE                   12*8+4
#define HCI_SUPPORTED_COMMANDS_WRITE_INQUIRY_SCAN_TYPE                  12*8+5
#define HCI_SUPPORTED_COMMANDS_READ_INQUIRY_MODE                        12*8+6
#define HCI_SUPPORTED_COMMANDS_WRITE_INQUIRY_MODE                       12*8+7
/**--- octet 13 */
#define HCI_SUPPORTED_COMMANDS_READ_PAGE_SCAN_TYPE                      13*8+0
#define HCI_SUPPORTED_COMMANDS_WRITE_PAGE_SCAN_TYPE                     13*8+1
#define HCI_SUPPORTED_COMMANDS_READ_AFH_CHANNEL_ASSESSMENT_MODE         13*8+2
#define HCI_SUPPORTED_COMMANDS_WRITE_AFH_CHANNEL_ASSESSMENT_MODE        13*8+3
#define HCI_SUPPORTED_COMMANDS_RESERVED13_4                             13*8+4
#define HCI_SUPPORTED_COMMANDS_RESERVED13_5                             13*8+5
#define HCI_SUPPORTED_COMMANDS_RESERVED13_6                             13*8+6
#define HCI_SUPPORTED_COMMANDS_RESERVED13_7                             13*8+7
/**--- octet 14 */
#define HCI_SUPPORTED_COMMANDS_RESERVED14_0                             14*8+0
#define HCI_SUPPORTED_COMMANDS_RESERVED14_1                             14*8+1
#define HCI_SUPPORTED_COMMANDS_RESERVED14_2                             14*8+2
#define HCI_SUPPORTED_COMMANDS_READ_LOCAL_VERSION_INFORMATION           14*8+3
#define HCI_SUPPORTED_COMMANDS_RESERVED14_4                             14*8+4
#define HCI_SUPPORTED_COMMANDS_READ_LOCAL_SUPPORTED_FEATURES            14*8+5
#define HCI_SUPPORTED_COMMANDS_READ_LOCAL_EXTENDED_FEATURES             14*8+6
#define HCI_SUPPORTED_COMMANDS_READ_BUFFER_SIZE                         14*8+7
/**--- octet 15 */
#define HCI_SUPPORTED_COMMANDS_READ_COUNTRY_CODE                        15*8+0
#define HCI_SUPPORTED_COMMANDS_READ_BD_ADDRESS                          15*8+1
#define HCI_SUPPORTED_COMMANDS_READ_FAILED_CONTACT_COUNT                15*8+2
#define HCI_SUPPORTED_COMMANDS_RESET_FAILED_CONTACT_COUNT               15*8+3
#define HCI_SUPPORTED_COMMANDS_GET_LINK_QUALITY                         15*8+4
#define HCI_SUPPORTED_COMMANDS_READ_RSSI                                15*8+5
#define HCI_SUPPORTED_COMMANDS_READ_AFH_CHANNEL_MAP                     15*8+6
#define HCI_SUPPORTED_COMMANDS_READ_BD_CLOCK                            15*8+7
/**--- octet 16 */
#define HCI_SUPPORTED_COMMANDS_READ_LOOPBACK_MODE                       16*8+0
#define HCI_SUPPORTED_COMMANDS_WRITE_LOOPBACK_MODE                      16*8+1
#define HCI_SUPPORTED_COMMANDS_ENABLE_DEVICE_UNDER_TEST_MODE            16*8+2
#define HCI_SUPPORTED_COMMANDS_SETUP_SYNCHRONOUS_CONNECTION             16*8+3
#define HCI_SUPPORTED_COMMANDS_ACCEPT_SYNCHRONOUS_CONNECTION            16*8+4
#define HCI_SUPPORTED_COMMANDS_REJECT_SYNCHRONOUS_CONNECTION            16*8+5
#define HCI_SUPPORTED_COMMANDS_RESERVED16_6                             16*8+6
#define HCI_SUPPORTED_COMMANDS_RESERVED16_7                             16*8+7

/** octet 22**/
#define HCI_SUPPORTED_COMMANDS_SET_EVENT_MASK_PAGE_2                    22*8+2
/** octet 31**/
#define HCI_SUPPORTED_COMMANDS_SET_CSB                                  31*8+0
#define HCI_SUPPORTED_COMMANDS_SET_CSB_RECEIVE                          31*8+1
#define HCI_SUPPORTED_COMMANDS_START_SYNC_TRIAN                         31*8+2
#define HCI_SUPPORTED_COMMANDS_RECEIVE_SYNC_TRIAN                       31*8+3
#define HCI_SUPPORTED_COMMANDS_SET_RESERVED_LT_ADDR                     31*8+4
#define HCI_SUPPORTED_COMMANDS_DELETE_RESERVED_LT_ADDR                  31*8+5
#define HCI_SUPPORTED_COMMANDS_SET_CSB_DATA                             31*8+6
#define HCI_SUPPORTED_COMMANDS_READ_SYNC_TRAIN_PARAM                    31*8+7
/** octet 32**/
#define HCI_SUPPORTED_COMMANDS_WRITE_SYNC_TRAIN_PARAM                   32*8+0


/** HCI Event Codes */
#define HCI_INQUIRY_COMPLETE                        0x01
#define HCI_INQUIRY_RESULT                          0x02
#define HCI_CONNECTION_COMPLETE                     0x03
#define HCI_CONNECTION_REQUEST                      0x04
#define HCI_DISCONNECTION_COMPLETE                  0x05
#define HCI_AUTHENTICATION_COMPLETE                 0x06
#define HCI_REMOTE_NAME_REQUEST_COMPLETE            0x07
#define HCI_ENCRYPTION_CHANGE                       0x08
#define HCI_CHANGE_CONNECTION_LINK_COMPLETE         0x09
#define HCI_MASTER_LINK_KEY_COMPLETE                0x0a
#define HCI_READ_REMOTE_FEATURES_COMPLETE           0x0b
#define HCI_READ_REMOTE_VERSION_COMPLETE            0x0c
#define HCI_QOS_SETUP_COMPLETE                      0x0d
#define HCI_COMMAND_COMPLETE                        0x0e
#define HCI_COMMAND_STATUS                          0x0f
#define HCI_HARDWARE_ERROR                          0x10
#define HCI_FLUSH_OCCURRED                          0x11
#define HCI_ROLE_CHANGE                             0x12
#define HCI_NUM_OF_COMPLETED_PACKETS                0x13
#define HCI_MODE_CHANGE                             0x14
#define HCI_RETURN_LINK_KEYS                        0x15
#define HCI_PIN_CODE_REQUEST                        0x16
#define HCI_LINK_KEY_REQUEST                        0x17
#define HCI_LINK_KEY_NOTIFICATION                   0x18
#define HCI_LOOPBACK_COMMAND                        0x19
#define HCI_DATA_BUFFER_OVERFLOW                    0x1a
#define HCI_MAX_SLOTS_CHANGE                        0x1b
#define HCI_READ_CLOCK_OFFSET_COMPLETE              0x1c
#define HCI_CONNECTION_PACKET_TYPE_CHANGED          0x1d
#define HCI_QOS_VIOLATION                           0x1e
#define HCI_PAGE_SCAN_MODE_CHANGE                   0x1f
#define HCI_PAGE_SCAN_REPETITION_CHANGE             0x20

#define HCI_FLOW_SPECIFICATION_COMPLETE             0x21
#define HCI_INQUIRY_RESULT_WITH_RSSI                0x22
#define HCI_READ_REMOTE_EXTENDED_FEATURES_COMPLETE  0x23
#define HCI_FIXED_ADDRESS                           0x24
#define HCI_ALIAS_ADDRESS                           0x25
#define HCI_GENERATE_ALIAS_REQUEST                  0x26
#define HCI_ACTIVE_ADDRESS                          0x27
#define HCI_ALLOW_PRIVATE_PAIRING                   0x28
#define HCI_ALIAS_ADDRESS_REQUEST                   0x29
#define HCI_ALIAS_NOT_RECOGNIZED_EVENT              0x2A
#define HCI_FIXED_ADDRESS_ATTEMPT_EVENT             0x2B
#define HCI_SYNCHRONOUS_CONNECTION_COMPLETE         0x2C
#define HCI_SYNCHRONOUS_CONNECTION_CHANGED          0x2D

#define HCI_SNIFF_SUBRATING_EVENT                   0x2E
#define HCI_EXTENDED_INQUIRY_RESULT                 0x2F
#define HCI_ENCRYPTION_KEY_REFRESH_COMPLETE         0x30
#define HCI_IO_CAPABILITY_REQUEST                   0x31
#define HCI_IO_CAPABILITY_RESPONSE                  0x32
#define HCI_USER_CONFIRMATION_REQUEST               0x33
#define HCI_USER_PASSKEY_REQUEST                    0x34
#define HCI_REMOTE_OOB_DATA_REQUEST                 0x35
#define HCI_SIMPLE_PAIRING_COMPLETE                 0x36
#define HCI_LINK_SUPERVISION_TIMEOUT_CHANGED        0x38
#define HCI_ENHANCED_FLUSH_COMPLETE                 0x39
#define HCI_USER_PASSKEY_NOTIFICATION               0x3B
#define HCI_KEYPRESS_NOTIFICATION                   0x3C
#define HCI_REMOTE_HOST_SUPPORTED_FEATURES          0x3D

/** Connectionless_Slave_Broadcast **/
#define HCI_SYNCHRONIZATION_TRAIN_COMPLETE          0x4F
#define HCI_SYNCHRONIZATION_TRAIN_RECEIVED          0x50
#define HCI_CONNECTIONLESS_SLAVE_BROADCAST_RECEIVE  0x51
#define HCI_CONNECTIONLESS_SLAVE_BROADCAST_TIMEOUT  0x52
#define HCI_TRUNCATED_PAGE_COMPLETE                 0x53
#define HCI_SLAVE_PAGE_RESPONSE_TIMEOUT             0x54
#define HCI_CSB_CHANNEL_MAP_CHANGE                  0x55

#define HCI_LE_EVENT                                0x3E
#define HCI_VENDOR_SPECIFIC_DBG_EVENT               0xFF

/** LE subevents */
#define HCI_LE_CONNECTION_COMPLETE_EVENT                 0x01
#define HCI_LE_ADVERTISING_REPORT_EVENT                  0x02
#define HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT          0x03
#define HCI_LE_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT  0x04
#define HCI_LE_LONG_TERM_KEY_REQUEST_EVENT               0x05
#define HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT 0x06
#define HCI_LE_DATA_LENGTH_CHANGE_EVENT                  0x07
#define HCI_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE       0x08
#define HCI_LE_GENERATE_DHKEY_COMPLETE                   0x09
#define HCI_LE_ENHANCED_CONNECTION_COMPLETE              0x0A
#define HCI_LE_DIRECT_ADVERTISING_REPORT                 0x0B
#define HCI_LE_PHY_UPDATE_COMPLETE_EVENT                 0x0C
#define HCI_LE_EXTENDED_ADVERTISING_REPORT_EVENT         0x0D
#define HCI_LE_PERIODIC_ADV_SYNC_ESTABLISHED_EVENT       0x0E
#define HCI_LE_PERIODIC_ADV_SYNC_REPORT_EVENT            0x0F
#define HCI_LE_PERIODIC_ADV_SYNC_LOST_EVENT              0x10
#define HCI_LE_SCAN_TIMEOUT_EVENT                        0x11
#define HCI_LE_ADVERTISING_SET_TERMINATED_EVENT          0x12
#define HCI_LE_SCAN_REQUEST_RECEIVED_EVENT               0x13
#define HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT         0x14

#define HCI_LE_CONNECTIONLESS_IQ_REPORT_EVENT            0x15
#define HCI_LE_CONNECTION_IQ_REPORT_EVENT                0x16
#define HCI_LE_SUPPLEMENTAL_REQUEST_FAILED_EVENT         0x17

/** vendor subevent */
#define HCI_VENDOR_CSB_RX_DATA_NOTIFY_EVENT     0x36
#define HCI_VENDOR_CSB_TX_CMPL                  0x37

/** Parameter Definition */
#define HCI_PACKET_TYPE_NO_2DH1                 0x0002
#define HCI_PACKET_TYPE_NO_3DH1                 0x0004
#define HCI_PACKET_TYPE_DM1                     0x0008
#define HCI_PACKET_TYPE_DH1                     0x0010
#define HCI_PACKET_TYPE_HV1                     0x0020
#define HCI_PACKET_TYPE_HV2                     0x0040
#define HCI_PACKET_TYPE_HV3                     0x0080
#define HCI_PACKET_TYPE_NO_2DH3                 0x0100
#define HCI_PACKET_TYPE_NO_3DH3                 0x0200
#define HCI_PACKET_TYPE_DM3                     0x0400
#define HCI_PACKET_TYPE_DH3                     0x0800
#define HCI_PACKET_TYPE_NO_2DH5                 0x1000
#define HCI_PACKET_TYPE_NO_3DH5                 0x2000
#define HCI_PACKET_TYPE_DM5                     0x4000
#define HCI_PACKET_TYPE_DH5                     0x8000

#define HCI_ESCO_PACKET_TYPE_HV1                0x0001
#define HCI_ESCO_PACKET_TYPE_HV2                0x0002
#define HCI_ESCO_PACKET_TYPE_HV3                0x0004
#define HCI_ESCO_PACKET_TYPE_EV3                0x0008
#define HCI_ESCO_PACKET_TYPE_EV4                0x0010
#define HCI_ESCO_PACKET_TYPE_EV5                0x0020

/** Linktype Values (BT Core Spec, p. 710) */
#define HCI_LINK_TYPE_SCO                       0x00
#define HCI_LINK_TYPE_ACL                       0x01
#define HCI_LINK_TYPE_ESCO                      0x02

/** hci filter type */
#define HCI_FILTER_TYPE_CLEAR_ALL               0x00
#define HCI_FILTER_TYPE_INQUIRY_RESULTS         0x01
#define HCI_FILTER_TYPE_CONNECTION_SETUP        0x02

/** HCI_FILTER_TYPE_INQUIRY_RESULTS: hci filter condition type */
#define HCI_INQUIRY_RESULTS_FILTER_CONDITION_TYPE_ALL     0x00
#define HCI_INQUIRY_RESULTS_FILTER_CONDITION_TYPE_COD     0x01
#define HCI_INQUIRY_RESULTS_FILTER_CONDITION_TYPE_BD_ADDR 0x02

/** HCI_FILTER_TYPE_CONNECTION_SETUP: hci filter condition type */
#define HCI_CONNECTION_SETUP_FILTER_CONDITION_TYPE_ALL     0x00
#define HCI_CONNECTION_SETUP_FILTER_CONDITION_TYPE_COD     0x01
#define HCI_CONNECTION_SETUP_FILTER_CONDITION_TYPE_BD_ADDR 0x02

/** HCI_CONNECTION_SETUP_FILTER_CONDITION_TYPE_ALL: hci filter condition */
#define HCI_CONNECTION_SETUP_FILTER_CONDITION_TYPE_ALL_DO_NOT_AUTO_ACCEPT                 0x01
#define HCI_CONNECTION_SETUP_FILTER_CONDITION_TYPE_ALL_AUTO_ACCEPT_ROLE_SWITCH_DISABLED   0x02
#define HCI_CONNECTION_SETUP_FILTER_CONDITION_TYPE_ALL_AUTO_ACCEPT_ROLE_SWITCH_ENABLED    0x03

/** Write Voice Settings: Air Coding Format Definition */
#define HCI_AIRCODING_CVSD          0x0000
#define HCI_AIRCODING_ULAW          0x0001
#define HCI_AIRCODING_ALAW          0x0002
#define HCI_AIRCODING_TRANSPARENT   0x0003

/** Write Voice Settings: Linear Input Data Format */
#define HCI_INCODING_1COMP          0x0000
#define HCI_INCODING_2COMP          0x0040
#define HCI_INCODING_SIGNMAG        0x0080

/** Write Voice Settings: Linear Input Size */
#define HCI_INCODING_8BIT           0x0000
#define HCI_INCODING_16BIT          0x0020

/** Write Voice Settings: Input Coding Format Definition */
#define HCI_INCODING_LINEAR         0x0000
#define HCI_INCODING_ULAW           0x0100
#define HCI_INCODING_ALAW           0x0200

/** Write Link Policy Command: Policy Options */
#define HCI_LINK_POLICY_ENABLE_SWITCH   0x0001
#define HCI_LINK_POLICY_ENABLE_HOLD     0x0002
#define HCI_LINK_POLICY_ENABLE_SNIFF    0x0004
#define HCI_LINK_POLICY_ENABLE_PARK     0x0008

/** Mode Change Event, Link Status */
#define HCI_LINK_ACTIVE                 0
#define HCI_LINK_HOLD                   1
#define HCI_LINK_SNIFF                  2
#define HCI_LINK_PARK                   3

/** HCI Max User Payload  */
#define HCI_DM1_PAYLOAD   17
#define HCI_DH1_PAYLOAD   27
#define HCI_2DH1_PAYLOAD  54
#define HCI_3DH1_PAYLOAD  83
#define HCI_DM3_PAYLOAD   121
#define HCI_DH3_PAYLOAD   183
#define HCI_DM5_PAYLOAD   224
#define HCI_DH5_PAYLOAD   339
#define HCI_2DH3_PAYLOAD  367
#define HCI_3DH3_PAYLOAD  552
#define HCI_2DH5_PAYLOAD  679
#define HCI_3DH5_PAYLOAD  1021

#endif /* _HCI_CODE_H_ */
