/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _BLE_DEFAULT_CONFIG_H_
#define _BLE_DEFAULT_CONFIG_H_

#ifndef CONFIG_BT
#error "CONFIG_BT is not defined!!!"
#endif

#ifdef CONFIG_BT

#ifndef CONFIG_NET_BUF_USER_DATA_SIZE
#define CONFIG_NET_BUF_USER_DATA_SIZE 4
#endif

#ifndef CONFIG_BT_HCI
#define CONFIG_BT_HCI 1
#endif

#ifdef CONFIG_BT_HCI

#ifndef CONFIG_BT_H4
/* Bluetooth H:4 UART driver. Requires hardware flow control lines to be available. */
#define CONFIG_BT_H4 1
#endif

#ifdef CONFIG_BT_H5
#undef CONFIG_BT_H4
#endif

#ifdef CONFIG_BT_H4
//#define CONFIG_BT_RECV_IS_RX_THREAD 1
#endif

#ifndef CONFIG_BT_CENTRAL
//#define CONFIG_BT_CENTRAL 1
#endif

#ifndef CONFIG_BT_PERIPHERAL
#ifndef CONFIG_BT_CENTRAL
#define CONFIG_BT_PERIPHERAL 1
#endif
#endif

/* Select this for LE Peripheral role support. */
#ifdef CONFIG_BT_PERIPHERAL

#ifndef CONFIG_BT_BROADCASTER
#define CONFIG_BT_BROADCASTER 1
#endif

#ifndef CONFIG_BT_CONN
#define CONFIG_BT_CONN 1
#endif

#endif // CONFIG_BT_PERIPHERAL

/* Select this for LE Central role support. */
#ifdef CONFIG_BT_CENTRAL

#ifndef CONFIG_BT_OBSERVER
#define CONFIG_BT_OBSERVER 1
#endif

#ifndef CONFIG_BT_CONN
#define CONFIG_BT_CONN 1
#endif

#endif // CONFIG_BT_CENTRAL

/* Select this for LE Broadcaster role support. */
#ifndef CONFIG_BT_PERIPHERAL
#ifndef CONFIG_BT_OBSERVER
#ifndef CONFIG_BT_BROADCASTER
#define CONFIG_BT_BROADCASTER 1
#endif
#endif
#endif //!CONFIG_BT_PERIPHERAL

/* Select this for LE Observer role support. */
//#define CONFIG_BT_OBSERVER 1

/* Enable support for Bluetooth 5.0 PHY Update Procedure. */
#ifndef CONFIG_BT_PHY_UPDATE
#define CONFIG_BT_PHY_UPDATE 1
#endif

/* Enable support for Bluetooth 5.0 PHY Update Procedure. */
#ifndef CONFIG_BT_DATA_LEN_UPDATE
#define CONFIG_BT_DATA_LEN_UPDATE 1
#endif


#ifdef CONFIG_BT_CONN

/* Maximum number of simultaneous Bluetooth connections supported. */
#ifndef CONFIG_BT_MAX_CONN
#define CONFIG_BT_MAX_CONN 1
#endif

#ifndef CONFIG_BT_HCI_ACL_FLOW_CONTROL
//#define CONFIG_BT_HCI_ACL_FLOW_CONTROL 1
#endif

#endif //CONFIG_BT_CONN

#ifndef CONFIG_BT_HCI_HOST
#define CONFIG_BT_HCI_HOST 1
#endif

/* Number of buffers available for HCI commands. */
#ifndef CONFIG_BT_HCI_CMD_COUNT
#define CONFIG_BT_HCI_CMD_COUNT 2
#endif

/* Number of buffers available for incoming ACL packets or HCI events from the controller. */
#ifndef CONFIG_BT_RX_BUF_COUNT
#ifdef CONFIG_BT_RECV_IS_RX_THREAD
#define CONFIG_BT_RX_BUF_COUNT 3
#elif defined(CONFIG_BT_MESH)
#define CONFIG_BT_RX_BUF_COUNT 20
#else
#define CONFIG_BT_RX_BUF_COUNT 3
#endif
#endif

/* Number of discardable event buffers. */
#ifndef CONFIG_BT_DISCARDABLE_BUF_COUNT
#ifdef CONFIG_BT_MESH
#define CONFIG_BT_DISCARDABLE_BUF_COUNT 20
#else
#define CONFIG_BT_DISCARDABLE_BUF_COUNT 3
#endif
#endif

/* Size of discardable event buffers. */
#ifndef CONFIG_BT_DISCARDABLE_BUF_SIZE
#if defined(CONFIG_BT_BREDR) || defined(CONFIG_BT_EXT_ADV)
#define CONFIG_BT_DISCARDABLE_BUF_SIZE 257
#else
#define CONFIG_BT_DISCARDABLE_BUF_SIZE 45
#endif
#endif

/* GATT Service Changed support. */
#ifndef CONFIG_BT_GATT_SERVICE_CHANGED
#define CONFIG_BT_GATT_SERVICE_CHANGED 1
#endif

/* GATT dynamic database support. */
#ifndef CONFIG_BT_GATT_DYNAMIC_DB
#define CONFIG_BT_GATT_DYNAMIC_DB
#endif

/* Maximum data size for each HCI RX buffer. This size includes
	  everything starting with the ACL or HCI event headers. Note that
	  buffer sizes are always rounded up to the nearest multiple of 4,
	  so if this Kconfig value is something else then there will be some
	  wasted space. The minimum of 73 has been taken for LE SC which has
	  an L2CAP MTU of 65 bytes. On top of this there's the L2CAP header
	  (4 bytes) and the ACL header (also 4 bytes) which yields 73 bytes. */
#ifndef CONFIG_BT_RX_BUF_LEN
#ifdef CONFIG_BT_MESH_PROXY
#define CONFIG_BT_RX_BUF_LEN 77
#else
#define CONFIG_BT_RX_BUF_LEN 76
#endif
#endif

/* Stack size needed for executing bt_send with specified driver */
#ifndef CONFIG_BT_HCI_TX_STACK_SIZE
#define CONFIG_BT_HCI_TX_STACK_SIZE 1536
#endif

/* Stack size needed for executing bt_send with specified driver */
#ifndef CONFIG_BT_HCI_TX_PRIO
#define CONFIG_BT_HCI_TX_PRIO 7
#endif

/* Headroom that the driver needs for sending and receiving buffers. */
#ifndef CONFIG_BT_HCI_RESERVE
#if defined(CONFIG_BT_H4) || defined(CONFIG_BT_H5)
#define CONFIG_BT_HCI_RESERVE 1
#else
#define CONFIG_BT_HCI_RESERVE 1
#endif
#endif

/* Size of the receiving thread stack. This is the context from
	  which all event callbacks to the application occur. The
	  default value is sufficient for basic operation, but if the
	  application needs to do advanced things in its callbacks that
	  require extra stack space, this value can be increased to
	  accommodate for that. */
#ifndef CONFIG_BT_RX_STACK_SIZE
#define CONFIG_BT_RX_STACK_SIZE 2048
#endif

#ifndef CONFIG_BT_RX_PRIO
#define CONFIG_BT_RX_PRIO 8
#endif

#ifdef CONFIG_BT_HCI_HOST

/* Controller crypto feature */
#ifndef CONFIG_BT_CTLR_CRYPTO
//#define CONFIG_BT_CTLR_CRYPTO 1
#endif

/* Host crypto feature */
#ifndef CONFIG_BT_CTLR_CRYPTO
#define CONFIG_BT_HOST_CRYPTO 1
#endif

#ifndef CONFIG_BT_SETTINGS
#define CONFIG_BT_SETTINGS 1
#endif

/* 	  Store Client Configuration Characteristic value right after it has
	  been updated.

	  By default, CCC is only stored on disconnection.
	  Choosing this option is safer for battery-powered devices or devices
	  that expect to be reset suddenly. However, it requires additional
	  workqueue stack space. */
#ifndef CONFIG_BT_SETTINGS_CCC_STORE_ON_WRITE
#ifdef CONFIG_BT_SETTINGS
//#define CONFIG_BT_SETTINGS_CCC_STORE_ON_WRITE 1
#endif
#endif


#ifdef CONFIG_BT_CONN

#ifdef CONFIG_BT_HCI_ACL_FLOW_CONTROL

#ifndef CONFIG_BT_ACL_RX_COUNT
#define CONFIG_BT_ACL_RX_COUNT 6
#endif

/* Maximum size of each incoming L2CAP PDU. */
#ifndef CONFIG_BT_L2CAP_RX_MTU
#ifdef CONFIG_BT_SMP
#define CONFIG_BT_L2CAP_RX_MTU 65
#else
#define CONFIG_BT_L2CAP_RX_MTU 23
#endif
#endif

#endif //CONFIG_BT_HCI_ACL_FLOW_CONTROL

/* Number of buffers available for outgoing L2CAP packets. */
#ifndef CONFIG_BT_L2CAP_TX_BUF_COUNT
#define CONFIG_BT_L2CAP_TX_BUF_COUNT 3
#endif

/* Maximum number of queued outgoing ATT PDUs. */
#ifndef CONFIG_BT_ATT_TX_MAX
#define CONFIG_BT_ATT_TX_MAX CONFIG_BT_L2CAP_TX_BUF_COUNT
#endif

/* Number of buffers available for fragments of TX buffers. Warning:
	  setting this to 0 means that the application must ensure that
	  queued TX buffers never need to be fragmented, i.e. that the
	  controller's buffer size is large enough. If this is not ensured,
	  and there are no dedicated fragment buffers, a deadlock may occur.
	  In most cases the default value of 2 is a safe bet. */
#ifndef CONFIG_BT_L2CAP_TX_FRAG_COUNT
#define CONFIG_BT_L2CAP_TX_FRAG_COUNT 2
#endif

/*  Maximum L2CAP MTU for L2CAP TX buffers. */
#ifndef CONFIG_BT_L2CAP_TX_MTU
#ifdef CONFIG_BT_SMP
#define CONFIG_BT_L2CAP_TX_MTU 65
#else
#define CONFIG_BT_L2CAP_TX_MTU 23
#endif
#endif

/* Maximum number of pending TX buffers that have not yet been acknowledged by the controller.*/
#ifndef CONFIG_BT_CONN_TX_MAX
#define CONFIG_BT_CONN_TX_MAX 7
#endif

/* Initiate PHY Update Procedure on connection establishment.

	  Disable this if you want PHY Update Procedure feature supported but
	  want to rely on remote device to initiate the procedure at its
	  discretion.*/
#ifndef CONFIG_BT_AUTO_PHY_UPDATE
#ifdef CONFIG_BT_PHY_UPDATE
#define CONFIG_BT_AUTO_PHY_UPDATE 1
#endif
#endif

/* This option enables support for the Security Manager Protocol
	  (SMP), making it possible to pair devices over LE. */
#ifndef CONFIG_BT_SMP
//#define CONFIG_BT_SMP 1
#endif

#ifdef CONFIG_BT_SMP

#ifndef CONFIG_BT_RPA
#define CONFIG_BT_RPA 1
#endif

/* Enable local Privacy Feature support. This makes it possible
	  to use Resolvable Private Addresses (RPAs). */
#ifndef CONFIG_BT_PRIVACY
//#define CONFIG_BT_PRIVACY 1
#endif

/* This option defines how often resolvable private address is rotated.
	  Value is provided in seconds and defaults to 900 seconds (15 minutes). */
#ifdef CONFIG_BT_PRIVACY
#ifndef CONFIG_BT_RPA_TIMEOUT
#define CONFIG_BT_RPA_TIMEOUT 900
#endif
#endif

/*
	Number of buffers in a separate buffer pool for events which
	the HCI driver considers discardable. Examples of such events
	could be e.g. Advertising Reports. The benefit of having such
	a pool means that the if there is a heavy inflow of such events
	it will not cause the allocation for other critical events to
	block and may even eliminate deadlocks in some cases.
 */
#ifndef BT_DISCARDABLE_BUF_COUNT
#if defined(CONFIG_BT_MESH)
#define BT_DISCARDABLE_BUF_COUNT 20
#else
#define BT_DISCARDABLE_BUF_COUNT 3
#endif
#endif


/* This option enables data signing which is used for transferring
	  authenticated data in an unencrypted connection. */
#ifndef CONFIG_BT_SIGNING
//#define CONFIG_BT_SIGNING 1
#endif

/* This option enables support for Secure Connection Only Mode. In this
	  mode device shall only use Security Mode 1 Level 4 with exception
	  for services that only require Security Mode 1 Level 1 (no security).
	  Security Mode 1 Level 4 stands for authenticated LE Secure Connections
	  pairing with encryption. Enabling this option disables legacy pairing. */
#ifndef CONFIG_BT_SMP_SC_ONLY
//#define CONFIG_BT_SMP_SC_ONLY 1
#endif

/* This option disables LE legacy pairing and forces LE secure connection
	  pairing. All Security Mode 1 levels can be used with legacy pairing
	  disabled, but pairing with devices that do not support secure
	  connections pairing will not be supported.
	  To force a higher security level use "Secure Connections Only Mode"*/
#ifndef CONFIG_BT_SMP_SC_PAIR_ONLY
#ifdef CONFIG_BT_SMP_SC_ONLY
#define CONFIG_BT_SMP_SC_PAIR_ONLY 1
#endif
#endif

/* With this option enabled, the application will be able to call the
	  bt_passkey_set() API to set a fixed passkey. If set, the
	  pairing_confim() callback will be called for all incoming pairings. */
#ifndef CONFIG_BT_FIXED_PASSKEY
//#define CONFIG_BT_FIXED_PASSKEY 1
#endif

/* This option places Security Manager in a Debug Mode. In this mode
	  predefined Diffie-Hellman private/public key pair is used as described
	  in Core Specification Vol. 3, Part H, 2.3.5.6.1. This option should
	  only be enabled for debugging and should never be used in production.
	  If this option is enabled anyone is able to decipher encrypted air
	  traffic. */
#ifndef CONFIG_BT_USE_DEBUG_KEYS
//#define CONFIG_BT_USE_DEBUG_KEYS 1
#endif

/* This option enables support for Bondable Mode. In this mode,
	  Bonding flag in AuthReq of SMP Pairing Request/Response will be set
	  indicating the support for this mode. */
#ifndef CONFIG_BT_BONDABLE
#define CONFIG_BT_BONDABLE 1
#endif

/* With this option enabled, the Security Manager will set MITM option in
	  the Authentication Requirements Flags whenever local IO Capabilities
	  allow the generated key to be authenticated. */
#ifndef CONFIG_BT_SMP_ENFORCE_MITM
#define CONFIG_BT_SMP_ENFORCE_MITM 1
#endif

#endif //CONFIG_BT_SMP

/* This option enables support for LE Connection oriented Channels,
	  allowing the creation of dynamic L2CAP Channels. */
#ifndef CONFIG_BT_L2CAP_DYNAMIC_CHANNEL
//#define CONFIG_BT_L2CAP_DYNAMIC_CHANNEL 1
#endif

/* Enforce strict flow control semantics for incoming PDUs */
#ifndef CONFIG_BT_ATT_ENFORCE_FLOW
//#define CONFIG_BT_ATT_ENFORCE_FLOW 1
#endif

/* Number of buffers available for ATT prepare write, setting
	  this to 0 disables GATT long/reliable writes. */
#ifndef CONFIG_BT_ATT_PREPARE_COUNT
#define CONFIG_BT_ATT_PREPARE_COUNT 0
#endif

/* Number of ATT PDUs that can be at a single moment queued for
	  transmission. If the application tries to send more than this
	  amount the calls will block until an existing queued PDU gets
	  sent. */
#ifndef CONFIG_BT_ATT_TX_MAX
#define CONFIG_BT_ATT_TX_MAX 2
#endif

/* This option enables support for GATT Caching. When enabled the stack
	  will register Client Supported Features and Database Hash
	  characteristics which can be used by clients to detect if anything has
	  changed on the GATT database. */
#ifndef CONFIG_BT_GATT_CACHING
//#warning "CHECK CONFIG_BT_GATT_CACHING default config"
//#define CONFIG_BT_GATT_CACHING 1
#endif

/* When enable this option blocks notification and indications to client
	 to conform to the following statement from the Bluetooth 5.1
	 specification:
	 '...the server shall not send notifications and indications to such
	 a client until it becomes change-aware."
	 In case the service cannot deal with sudden errors (-EAGAIN) then it
	 shall not use this option. */
#ifndef CONFIG_BT_GATT_ENFORCE_CHANGE_UNAWARE
#ifdef CONFIG_BT_GATT_CACHING
//#define CONFIG_BT_GATT_ENFORCE_CHANGE_UNAWARE 1
#endif
#endif

/* This option enables support for the GATT Client role. */
#ifndef CONFIG_BT_GATT_CLIENT
//#define CONFIG_BT_GATT_CLIENT 1
#endif

/* This option enables support for the GATT Read Multiple Characteristic
	  Values procedure. */
#ifndef CONFIG_BT_GATT_READ_MULTIPLE
#define CONFIG_BT_GATT_READ_MULTIPLE 1
#endif

/* Peripheral connection parameter update timeout in milliseconds. */
#ifndef CONFIG_BT_CONN_PARAM_UPDATE_TIMEOUT
#define CONFIG_BT_CONN_PARAM_UPDATE_TIMEOUT 5000
#endif

/* Maximum number of paired Bluetooth devices. The minimum (and
	  default) number is 1. */
#ifndef CONFIG_BT_MAX_PAIRED
#ifndef CONFIG_BT_SMP
#define CONFIG_BT_MAX_PAIRED 0
#else
#define CONFIG_BT_MAX_PAIRED 1
#endif
#endif

/* Configure peripheral preferred connection parameters */
#ifndef CONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS
#ifdef CONFIG_BT_PERIPHERAL
#define CONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS 1
#endif
#endif

#ifdef CONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS

/* Peripheral preferred minimum connection interval in 1.25ms units */
#ifndef CONFIG_BT_PERIPHERAL_PREF_MIN_INT
#define CONFIG_BT_PERIPHERAL_PREF_MIN_INT 24 
#endif

/* Peripheral preferred maximum connection interval in 1.25ms units */
#ifndef CONFIG_BT_PERIPHERAL_PREF_MAX_INT
#define CONFIG_BT_PERIPHERAL_PREF_MAX_INT 40
#endif

/* Peripheral preferred slave latency in Connection Intervals */
#ifndef CONFIG_BT_PERIPHERAL_PREF_SLAVE_LATENCY
#define CONFIG_BT_PERIPHERAL_PREF_SLAVE_LATENCY 0
#endif

/* Peripheral preferred supervision timeout in 10ms units */
#ifndef CONFIG_BT_PERIPHERAL_PREF_TIMEOUT
#define CONFIG_BT_PERIPHERAL_PREF_TIMEOUT 42
#endif

#endif //CONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS

/* Timeout for pending LE Create Connection command in seconds */
#ifndef CONFIG_BT_CREATE_CONN_TIMEOUT
#define CONFIG_BT_CREATE_CONN_TIMEOUT 3
#endif

#endif //CONFIG_BT_CONN

#ifdef CONFIG_BT_OBSERVER

/* Scan interval used for background scanning in 0.625 ms units */
#ifndef CONFIG_BT_BACKGROUND_SCAN_INTERVAL
#define CONFIG_BT_BACKGROUND_SCAN_INTERVAL 2048
#endif

/* Scan window used for background scanning in 0.625 ms units */
#ifndef CONFIG_BT_BACKGROUND_SCAN_WINDOW
#define CONFIG_BT_BACKGROUND_SCAN_WINDOW 18
#endif

#endif //CONFIG_BT_OBSERVER

/* Enable this if you want to perform active scanning using the local
	  identity address as the scanner address. By default the stack will
	  always use a non-resolvable private address (NRPA) in order to avoid
	  disclosing local identity information. However, if the use case
	  requires disclosing it then enable this option. */
#ifndef CONFIG_BT_SCAN_WITH_IDENTITY
//#define CONFIG_BT_SCAN_WITH_IDENTITY 1
#endif

/* Enable this if you want to perform active scanning using the local
	  identity address as the scanner address. By default the stack will
	  always use a non-resolvable private address (NRPA) in order to avoid
	  disclosing local identity information. However, if the use case
	  requires disclosing it then enable this option. */
#ifndef CONFIG_BT_DEVICE_NAME_DYNAMIC
#define CONFIG_BT_DEVICE_NAME_DYNAMIC 1
#endif

/* Bluetooth device name storage size. Storage can be up to 248 bytes
	  long (excluding NULL termination). */
#ifndef CONFIG_BT_DEVICE_NAME_MAX
#ifdef CONFIG_BT_DEVICE_NAME_DYNAMIC
#define CONFIG_BT_DEVICE_NAME_MAX 28
#endif
#endif

/* Enabling this option allows remote GATT clients to write to device
	  name GAP characteristic. */
#ifndef CONFIG_BT_DEVICE_NAME_GATT_WRITABLE
#if defined(CONFIG_BT_DEVICE_NAME_DYNAMIC) && defined(CONFIG_BT_CONN)
#define CONFIG_BT_DEVICE_NAME_GATT_WRITABLE 28
#endif
#endif

/* Bluetooth device name. Name can be up to 248 bytes long (excluding
	  NULL termination). Can be empty string. */
#ifndef CONFIG_BT_DEVICE_NAME
#define CONFIG_BT_DEVICE_NAME "YoC Test"
#endif

/* Bluetooth device appearance. For the list of possible values please
	  consult the following link:
	  https://www.bluetooth.com/specifications/assigned-numbers */
#ifndef CONFIG_BT_DEVICE_APPEARANCE
#define CONFIG_BT_DEVICE_APPEARANCE 0
#endif

#ifndef CONFIG_BT_ID_MAX
#define CONFIG_BT_ID_MAX 1
#endif

#endif //CONFIG_BT_HCI_HOST

/* Enable ECDH key generation support */
#ifndef CONFIG_BT_ECC
//#define CONFIG_BT_ECC 1
#endif

#ifndef CONFIG_BT_TINYCRYPT_ECC
//#define CONFIG_BT_TINYCRYPT_ECC 1
#endif

#ifdef CONFIG_BT_DEBUG

#ifndef CONFIG_BT_DEBUG_LOG
//#define CONFIG_BT_DEBUG_LOG 1
#endif

#ifndef CONFIG_BT_DEBUG_SETTINGS
//#define CONFIG_BT_DEBUG_SETTINGS 1
#endif

#ifndef CONFIG_BT_DEBUG_HCI_CORE
//#define CONFIG_BT_DEBUG_HCI_CORE 1
#endif

#ifndef CONFIG_BT_DEBUG_CONN
//#define CONFIG_BT_DEBUG_CONN 1
#endif

#ifndef CONFIG_BT_DEBUG_KEYS
//#define CONFIG_BT_DEBUG_KEYS 1
#endif

#ifndef CONFIG_BT_DEBUG_L2CAP
//#define CONFIG_BT_DEBUG_L2CAP 1
#endif

#ifndef CONFIG_BT_DEBUG_SMP
//#define CONFIG_BT_DEBUG_SMP 1
#endif

#ifndef CONFIG_BT_SMP_SELFTEST
//#define CONFIG_BT_SMP_SELFTEST 1
#endif

#ifndef CONFIG_BT_DEBUG_ATT
//#define CONFIG_BT_DEBUG_ATT 1
#endif

#ifndef CONFIG_BT_DEBUG_GATT
//#define CONFIG_BT_DEBUG_GATT 1
#endif

#endif //CONFIG_BT_DEBUG

#ifndef CONFIG_BT_SHELL
//#define CONFIG_BT_SHELL 1
#endif

#endif //CONFIG_BT_HCI

#endif //CONFIG_BT

#endif //_BLE_DEFAULT_CONFIG_H_
