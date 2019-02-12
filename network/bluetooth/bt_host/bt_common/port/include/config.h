/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

/**
 * CONFIG_BLUETOOTH: Enable the bluetooh stack
 */
#ifndef CONFIG_BLUETOOTH
#error "CONFIG_BLUETOOTH not defined,this header shoudn't include"
#endif

#ifndef CONFIG_BT_HCI_RX_STACK_SIZE
#define CONFIG_BT_HCI_RX_STACK_SIZE 768  // ESP32
#endif

/**
 * CONFIG_BT_HCI_RX_PRIO: rx thread priority
 */

#ifndef CONFIG_BT_RX_PRIO
#define CONFIG_BT_RX_PRIO 20
#endif

/**
 * CONFIG_BT: Tx thread stack size
 */

#ifndef CONFIG_BT_HCI_TX_STACK_SIZE
#define CONFIG_BT_HCI_TX_STACK_SIZE 290
//#define CONFIG_BT_HCI_TX_STACK_SIZE 512  // ESP32
#endif

/**
 * CONFIG_BT_HCI_TX_PRIO: tx thread priority
 */
#ifndef CONFIG_BT_HCI_TX_PRIO
#define CONFIG_BT_HCI_TX_PRIO 21
#endif

#ifndef CONFIG_BT_CTLR_RX_PRIO
#define CONFIG_BT_CTLR_RX_PRIO 19
#endif

/**
 * CONFIG_BT_HCI_CMD_COUNT: hci cmd buffer count,range 2 to 64
 */
#ifndef CONFIG_BT_HCI_CMD_COUNT
#define CONFIG_BT_HCI_CMD_COUNT 2
#endif

/**
 * CONFIG_BT_RX_BUF_COUNT: number of buffer for incoming ACL packages or HCI
 * events,range 2 to 255
 */
#ifndef CONFIG_BT_RX_BUF_COUNT
//#define CONFIG_BT_RX_BUF_COUNT 10
#define CONFIG_BT_RX_BUF_COUNT 2
#endif

/**
 * CONFIG_BT_RX_BUF_LEN: the max length for rx buffer
 * range 73 to 2000
 */
#ifndef CONFIG_BT_RX_BUF_LEN
#define CONFIG_BT_RX_BUF_LEN 73
#endif

/**
 * CONFIG_BT_CENTRAL: Enable central Role
 */
#ifdef CONFIG_BT_CENTRAL
#define CONFIG_BT_CENTRAL 1
#endif

/**
 * CONFIG_BT_PERIPHERAL: Enable peripheral Role
 */
#ifdef CONFIG_BT_PERIPHERAL
#define CONFIG_BT_PERIPHERAL 1
#endif

#if defined(CONFIG_BT_CENTRAL) || defined(CONFIG_BT_PERIPHERAL)
#define CONFIG_BT_CONN 1
#endif

#ifdef CONFIG_BT_CONN

/**
 * CONFIG_BLUETOOTH_L2CAP_TX_BUF_COUNT: number of buffer for outgoing L2CAP
 * packages range 2 to 255
 */
#ifndef CONFIG_BT_L2CAP_TX_BUF_COUNT
//#define CONFIG_BT_L2CAP_TX_BUF_COUNT 10
#define CONFIG_BT_L2CAP_TX_BUF_COUNT 2
#endif

/**
 * CONFIG_BT_L2CAP_TX_MTU: Max L2CAP MTU for L2CAP tx buffer
* range 65 to 2000 if SMP enabled,otherwise range 23 to 2000
  */
#ifndef CONFIG_BT_L2CAP_TX_MTU
#ifdef CONFIG_BT_SMP
#define CONFIG_BT_L2CAP_TX_MTU 65
#else
#define CONFIG_BT_L2CAP_TX_MTU 23
#endif
#endif

/**
 * CONFIG_BT_ATT_PREPARE_COUNT: Number of buffers available for ATT prepare
 * write, setting this to 0 disables GATT long/reliable writes. range 0 to 64
 */
#ifndef CONFIG_BT_ATT_PREPARE_COUNT
#define CONFIG_BT_ATT_PREPARE_COUNT 0
#endif

/**
 *  CONFIG_BLUETOOTH_SMP:Eable the Security Manager Protocol
 *  (SMP), making it possible to pair devices over LE
 */
#ifdef CONFIG_BT_SMP

#define CONFIG_BT_SMP 1

/**
 *  CONFIG_BT_SIGNING:enables data signing which is used for transferring
 *  authenticated data in an unencrypted connection
 */
#ifdef CONFIG_BT_SIGNING
#define CONFIG_BT_SIGNING 1
#endif

/**
 *  CONFIG_BT_SMP_SC_ONLY:enables support for Secure Connection Only Mode. In
 * this mode device shall only use Security Mode 1 Level 4 with exception for
 * services that only require Security Mode 1 Level 1 (no security). Security
 * Mode 1 Level 4 stands for authenticated LE Secure Connections pairing with
 * encryption. Enabling this option disables legacy pairing
 */
#ifdef CONFIG_BT_SMP_SC_ONLY
#define CONFIG_BT_SMP_SC_ONLY 1
#endif

/**
 *  CONFIG_BT_USE_DEBUG_KEYS:This option places Security Manager in
 *  a Debug Mode. In this mode predefined
 *  Diffie-Hellman private/public key pair is used as described
 *  in Core Specification Vol. 3, Part H, 2.3.5.6.1. This option should
 *  only be enabled for debugging and should never be used in production.
 *  If this option is enabled anyone is able to decipher encrypted air
 *  traffic.
 */
#ifdef CONFIG_BT_USE_DEBUG_KEYS
#ifndef CONFIG_BT_TINYCRYPT_ECC
#error "CONFIG_BT_USE_DEBUG_KEYS depends on CONFIG_BT_TINYCRYPT_ECC"
#endif
#define CONFIG_BT_USE_DEBUG_KEYS 1
#endif

/**
 *  CONFIG_BT_L2CAP_DYNAMIC_CHANNEL:enables support for LE Connection
 *  oriented Channels,allowing the creation of dynamic L2CAP Channels
 */
#ifdef CONFIG_BT_L2CAP_DYNAMIC_CHANNEL
#define CONFIG_BT_L2CAP_DYNAMIC_CHANNEL 1
#endif

#endif  // CONFIG_BT_SMP

/**
 *   CONFIG_BT_PRIVACY:Enable local Privacy Feature support. This makes it
 * possible to use Resolvable Private Addresses (RPAs).
 */
#ifdef CONFIG_BT_PRIVACY

#ifndef CONFIG_BT_SMP
#error "CONFIG_BT_PRIVACY depends on CONFIG_BT_SMP"
#endif

#define CONFIG_BT_PRIVACY 1

/**
 * CONFIG_BT_RPA_TIMEOUT:Resolvable Private Address timeout
 * range 1 to 65535,seconds
 */
#ifndef CONFIG_BT_RPA_TIMEOUT
#define CONFIG_BT_RPA_TIMEOUT 900
#endif

#endif  // CONFIG_BT_PRIVACY

/**
 *  CONFIG_BT_GATT_DYNAMIC_DB:enables GATT services to be added dynamically to
 * database
 */
#ifdef CONFIG_BT_GATT_DYNAMIC_DB
#define CONFIG_BT_GATT_DYNAMIC_DB 1
#endif

/**
 *  CONFIG_BT_GATT_CLIENT:GATT client role support
 */
#ifdef CONFIG_BT_GATT_CLIENT
#define CONFIG_BT_GATT_CLIENT 1
#endif

/**
 *  CONFIG_BT_MAX_PAIRED:Maximum number of paired devices
 *  range 1 to 128
 */
#ifndef CONFIG_BT_MAX_PAIRED
#define CONFIG_BT_MAX_PAIRED 1
#endif

#endif  // CONFIG_BT_CONN

/**
 * If this option is set TinyCrypt library is used for emulating the
 * ECDH HCI commands and events needed by e.g. LE Secure Connections.
 * In builds including the BLE Host, if not set the controller crypto is
 * used for ECDH and if the controller doesn't support the required HCI
 * commands the LE Secure Connections support will be disabled.
 * In builds including the HCI Raw interface and the BLE Controller, this
 * option injects support for the 2 HCI commands required for LE Secure
 * Connections so that Hosts can make use of those
 */
#ifdef CONFIG_BT_TINYCRYPT_ECC
#define CONFIG_BT_TINYCRYPT_ECC 1
#endif

/**
 *  CONFIG_BLUETOOTH_MAX_CONN:Maximum number of connections
 *  range 1 to 128
 */
#ifndef CONFIG_BT_MAX_CONN
#define CONFIG_BT_MAX_CONN 2
#endif

/**
 *  CONFIG_BT_DEVICE_NAME:Bluetooth device name. Name can be up
 *  to 248 bytes long (excluding NULL termination). Can be empty string
 */
#ifndef CONFIG_BT_DEVICE_NAME
#define CONFIG_BT_DEVICE_NAME "AOS Device"
#endif

/**
 *  CONFIG_BT_CONTROLLER_NAME:Bluetooth controller name. default support ESP32.
 */
#ifndef CONFIG_BT_CONTROLLER_NAME
#define CONFIG_BT_CONTROLLER_NAME "ESP32"
#endif

/**
 *  CONFIG_BT_MAX_SCO_CONN:Maximum number of simultaneous SCO connections.
 */
#ifndef CONFIG_BT_MAX_SCO_CONN
#define CONFIG_BT_MAX_SCO_CONN 0
#endif

/**
 *  CONFIG_BT_HCI_RESERVE:Headroom that the driver needs for sending and
 * receiving buffers.
 */
#ifndef CONFIG_BT_HCI_RESERVE
#ifdef CONFIG_BLUETOOTH_H4
#define CONFIG_BT_HCI_RESERVE 0
#elif defined(CONFIG_BLUETOOTH_H5) || defined(CONFIG_BLUETOOTH_SPI)
#define CONFIG_BT_HCI_RESERVE 1
#else
#define CONFIG_BT_HCI_RESERVE 1
#endif
#endif

/**
 *  CONFIG_BLUETOOTH_DEBUG_LOG:Enable bluetooth debug log.
 */
#ifdef CONFIG_BT_DEBUG_LOG
#define CONFIG_BT_DEBUG_LOG 1
#define CONFIG_BT_DEBUG 1
#endif

/**
 *  CONFIG_BT_DEBUG_L2CAP:Enable bluetooth l2cap debug log.
 */
#ifdef CONFIG_BT_DEBUG_L2CAP
#define CONFIG_BT_DEBUG_L2CAP 1
#endif

/**
 *  CONFIG_BT_DEBUG_CONN:Enable bluetooth conn debug log.
 */
#ifdef CONFIG_BT_DEBUG_CONN
#define CONFIG_BT_DEBUG_CONN 1
#endif

/**
 *  CONFIG_BT_DEBUG_ATT:Enable bluetooth att debug log.
 */
#ifdef CONFIG_BT_DEBUG_ATT
#define CONFIG_BT_DEBUG_ATT 1
#endif

/**
 *  CONFIG_BT_DEBUG_GATT:Enable bluetooth gatt debug log.
 */
#ifdef CONFIG_BT_DEBUG_GATT
#define CONFIG_BT_DEBUG_GATT 1
#endif

/**
 *  CONFIG_BT_DEBUG_HCI_CORE:Enable bluetooth hci core debug log.
 */
#ifdef CONFIG_BT_DEBUG_HCI_CORE
#define CONFIG_BT_DEBUG_HCI_CORE 1
#endif

/**
 *  CONFIG_BT_DEBUG_HCI_DRIVER:Enable bluetooth hci driver debug log.
 */
#ifdef CONFIG_BT_DEBUG_HCI_DRIVER
#define CONFIG_BT_DEBUG_HCI_DRIVER 1
#endif

/**
 *  CONFIG_BT_TEST:Enable bluetooth test.
 */
#ifdef CONFIG_BT_TEST
#define CONFIG_BT_TEST 1
#endif

/**
 *  CONFIG_BT_DEBUG_CORE:Enable bluetooth core debug log.
 */
#ifdef CONFIG_BT_DEBUG_CORE
#define CONFIG_BT_DEBUG_CORE 1
#endif

#ifndef CONFIG_BT_CONN_TX_MAX
#define CONFIG_BT_CONN_TX_MAX 1
#endif

#ifndef CONFIG_BT_DEVICE_APPEARANCE
#define CONFIG_BT_DEVICE_APPEARANCE 833
#endif

#ifdef CONFIG_BT_CTLR

#ifndef CONFIG_BT_CTLR_WORKER_PRIO
#define CONFIG_BT_CTLR_WORKER_PRIO 0
#endif

#ifndef CONFIG_BT_CTLR_JOB_PRIO
#define CONFIG_BT_CTLR_JOB_PRIO 0
#endif

#ifndef CONFIG_BT_CTLR_RX_BUFFERS
#define CONFIG_BT_CTLR_RX_BUFFERS 1
#endif

#ifndef CONFIG_BT_CTLR_TX_BUFFERS
#define CONFIG_BT_CTLR_TX_BUFFERS 1
#endif

#ifndef CONFIG_BT_CTLR_XTAL_THRESHOLD
#define CONFIG_BT_CTLR_XTAL_THRESHOLD 5168
#endif

#define CONFIG_BT_RECV_IS_RX_THREAD

#endif /* endof CONFIG_BT_CTLR */

#if defined(__cplusplus)
}
#endif

#endif /* BLE_CONFIG_H */
