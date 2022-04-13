/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __ME_API__H__
#define __ME_API__H__

#include "bluetooth.h"

typedef uint8_t btif_event_type_t;

#if defined(BTIF_HCI_HOST_FLOW_CONTROL) && defined (__IAG_BLE_INCLUDE__)
#define BTIF_BTEVENT_CONNECT_CPMPLETE       1
#endif /*  */

#define BTIF_BTEVENT_INQUIRY_RESULT       1

#define BTIF_BTEVENT_INQUIRY_COMPLETE     2

#define BTIF_BTEVENT_INQUIRY_CANCELED     3

#define BTIF_BTEVENT_LINK_CONNECT_IND     4

#define BTIF_BTEVENT_SCO_CONNECT_IND      5

#define BTIF_BTEVENT_LINK_DISCONNECT      6

#define BTIF_BTEVENT_LINK_CONNECT_CNF     7

#define BTIF_BTEVENT_LINK_CON_RESTRICT    8

#define BTIF_BTEVENT_MODE_CHANGE          9

#define BTIF_BTEVENT_ACCESSIBLE_CHANGE   10

#define BTIF_BTEVENT_AUTHENTICATED       11

#define BTIF_BTEVENT_ENCRYPTION_CHANGE   12

#define BTIF_BTEVENT_SECURITY_CHANGE     13

#define BTIF_BTEVENT_ROLE_CHANGE         14

#define BTIF_BTEVENT_SCO_DISCONNECT      15

#define BTIF_BTEVENT_SCO_CONNECT_CNF     16

#define BTIF_BTEVENT_SIMPLE_PAIRING_COMPLETE       17

#define BTIF_BTEVENT_REMOTE_FEATURES               18

#define BTIF_BTEVENT_REM_HOST_FEATURES             19

#define BTIF_BTEVENT_LINK_SUPERV_TIMEOUT_CHANGED   20

#define BTIF_BTEVENT_SET_SNIFF_SUBRATING_PARMS_CNF 21

#define BTIF_BTEVENT_SNIFF_SUBRATE_INFO            22

#define BTIF_BTEVENT_SET_INQUIRY_MODE_CNF          23

#define BTIF_BTEVENT_SET_INQ_TX_PWR_LVL_CNF        24

#define BTIF_BTEVENT_SET_EXT_INQUIRY_RESP_CNF      25

#define BTIF_BTEVENT_SET_ERR_DATA_REPORTING_CNF    26

#define BTIF_BTEVENT_KEY_PRESSED                   27

#define BTIF_BTEVENT_QOS_SETUP_COMPLETE            28

#ifdef __TWS_RECONNECT_USE_BLE__
#define BTIF_BTEVENT_TWS_BLE_ADV_REPORT_EVENT      29
#endif /*  */

/**  an ACL connection has received an internal data transmit
 *  request while it is in hold, park or sniff mode. The data will still be
 *  passed to the radio in park and sniff modes. However, hold mode will
 *  block data transmit. It may be necessary to return the ACL to active
 *  mode to restore normal data transfer.
 */
#define BTIF_BTEVENT_ACL_DATA_NOT_ACTIVE 99

/*
Indicate that an ACL connection is sending or receiving data
while it is in active mode. Then, keep resetting the sniff timer.
*/
#define BTIF_BTEVENT_ACL_DATA_ACTIVE 98
/** Indicates that the HCI failed to initialize.
 */
#define BTIF_BTEVENT_HCI_INIT_ERROR      100

#define BTIF_BTEVENT_HCI_INITIALIZED     101
/** Indicates that a fatal error has occurred in the radio or the HCI transport.
 */
#define BTIF_BTEVENT_HCI_FATAL_ERROR     102

/** Indicates that the HCI has been deinitialized.
 */
#define BTIF_BTEVENT_HCI_DEINITIALIZED   103

/** Indicates that the HCI cannot be initialized.
 */
#define BTIF_BTEVENT_HCI_FAILED          104

#define BTIF_BTEVENT_HCI_COMMAND_SENT    105

/** Indicates the name of a remote device or cancellation of a name request.
 */
#define BTIF_BTEVENT_NAME_RESULT         30

#define BTIF_BTEVENT_SCO_DATA_IND        31

/** Outgoing SCO data has been sent and the packet is free for re-use by
 *  the application.
 */
#define BTIF_BTEVENT_SCO_DATA_CNF        32

#define BTIF_BTEVENT_LINK_CONNECT_REQ    33

/** Incoming link accept complete.  */
#define BTIF_BTEVENT_LINK_ACCEPT_RSP     34

/** Incoming link reject complete. . */
#define BTEVENT_LINK_REJECT_RSP     35

#define BTIF_BTEVENT_COMMAND_COMPLETE    36

#define BTIF_BTEVENT_SCO_CONNECT_REQ     37

/** Set Audio/Voice settings complete.  */
#define BTIF_BTEVENT_SCO_VSET_COMPLETE   38

/** SCO link connection process started. */
#define BTIF_BTEVENT_SCO_STARTED         39

/** Select Device operation complete, "p.select" is valid. */
#define BTIF_BTEVENT_DEVICE_SELECTED     40

/** The eSCO connection has changed. "p.scoConnect" is valid.
 */
#define BTIF_BTEVENT_SCO_CONN_CHNG       41

/* Group: Security-related events. */

/** Indicates access request is successful. "p.secToken" is valid. */
#define BTIF_BTEVENT_ACCESS_APPROVED     50

/** Indicates access request failed. "p.secToken" is valid. */
#define BTIF_BTEVENT_ACCESS_DENIED       51

/** Request authorization when "errCode" is BEC_NO_ERROR.
 *  "p.remDev" is valid.
 */
#define BTIF_BTEVENT_AUTHORIZATION_REQ   52

/** Request a Pin for pairing when "errCode" is BEC_NO_ERROR.
 *  "p.pinReq" is valid. If p.pinReq.pinLen is > 0 then SEC_SetPin()
 *  must be called in response to this event with a pin length >=
 *  p.pinReq.pinLen.
 */
#define BTIF_BTEVENT_PIN_REQ             53

/** Pairing operation is complete.
 */
#define BTIF_BTEVENT_PAIRING_COMPLETE    54

/** Authentication operation complete. "p.remDev" is valid. */
#define BTIF_BTEVENT_AUTHENTICATE_CNF    55

/** Encryption operation complete. "p.remDev" is valid. */
#define BTIF_BTEVENT_ENCRYPT_COMPLETE    56

/** Security mode 3 operation complete. "p.secMode" is valid. */
#define BTIF_BTIF_BTEVENT_SECURITY3_COMPLETE  57

/** A link key is returned. "p.bdLinkKey" is valid.  */
#define BTIF_BTEVENT_RETURN_LINK_KEYS    58

/** Out of Band data has been received from the host controller. */
#define BTIF_BTEVENT_LOCAL_OOB_DATA      59

/** Request a Pass Key for simple pairing when "errCode" is BEC_NO_ERROR. The
 *  application should call SEC_SetPassKey() to provide the passkey or reject
 *  the request, and optionally save the link key.
 */
#define BTIF_BTEVENT_PASS_KEY_REQ        60

/** Request a User Confirmation for simple pairing when "errCode" is
 *  BEC_NO_ERROR.
 */
#define BTIF_BTEVENT_CONFIRM_NUMERIC_REQ            61

#define BTIF_BTEVENT_DISPLAY_NUMERIC_IND            62

#define BTIF_BTEVENT_CONN_PACKET_TYPE_CHNG          63

#define BTIF_SDEVENT_QUERY_RSP                      70

#define BTIF_SDEVENT_QUERY_ERR                      71

#define BTIF_SDEVENT_QUERY_FAILED                   72

#define BTIF_BTEVENT_SELECT_DEVICE_REQ              80

#define BTIF_BTEVENT_DEVICE_ADDED                   81

#define BTIF_BTEVENT_DEVICE_DELETED                 82

#define BTIF_BTEVENT_MAX_SLOT_CHANGED               83

#define BTIF_BTEVENT_SNIFFER_CONTROL_DONE           84

#define BTIF_BTEVENT_LINK_POLICY_CHANGED            85

#define BTIF_BTEVENT_DEFAULT_LINK_POLICY_CHANGED    86

#define BTIF_BTEVENT_BES_AUD_CONNECTED              200

#define BTIF_BTEVENT_BES_AUD_DISCONNECTED           201

#define BTIF_STACK_LINK_DISCONNECT_COMPLETE         220

typedef uint32_t btif_event_mask_t;

#define BTIF_BEM_NO_EVENTS                    0x00000000
#define BTIF_BEM_INQUIRY_RESULT               0x00000001
#define BTIF_BEM_INQUIRY_COMPLETE             0x00000002
#define BTIF_BEM_INQUIRY_CANCELED             0x00000004
#define BTIF_BEM_LINK_CONNECT_IND             0x00000008
#define BTIF_BEM_SCO_CONNECT_IND              0x00000010
#define BTIF_BEM_LINK_DISCONNECT              0x00000020
#define BTIF_BEM_LINK_CONNECT_CNF             0x00000040
#define BTIF_BEM_LINK_CON_RESTRICT            0x00000080
#define BTIF_BEM_MODE_CHANGE                  0x00000100
#define BTIF_BEM_ACCESSIBLE_CHANGE            0x00000200
#define BTIF_BEM_AUTHENTICATED                0x00000400
#define BTIF_BEM_ENCRYPTION_CHANGE            0x00000800
#define BTIF_BEM_SECURITY_CHANGE              0x00001000
#define BTIF_BEM_ROLE_CHANGE                  0x00002000
#define BTIF_BEM_SCO_DISCONNECT               0x00004000
#define BTIF_BEM_SCO_CONNECT_CNF              0x00008000
#define BTIF_BEM_SIMPLE_PAIRING_COMPLETE      0x00010000
#define BTIF_BEM_REMOTE_FEATURES              0x00020000
#define BTIF_BEM_REM_HOST_FEATURES            0x00040000
#define BTIF_BEM_LINK_SUPERV_TIMEOUT_CHANGED  0x00080000
#define BTIF_BEM_SET_SNIFF_SUBR_PARMS         0x00100000
#define BTIF_BEM_SNIFF_SUBRATE_INFO           0x00200000
#define BTIF_BEM_SET_INQ_MODE                 0x00400000
#define BTIF_BEM_SET_INQ_RSP_TX_PWR           0x00800000
#define BTIF_BEM_SET_EXT_INQ_RESP             0x01000000
#define BTIF_BEM_SET_ERR_DATA_REP             0x02000000
#define BTIF_BEM_KEY_PRESSED                  0x04000000
#define BTIF_BEM_CONN_PACKET_TYPE_CHNG        0x08000000
#define BTIF_BEM_QOS_SETUP_COMPLETE           0x10000000
#define BTIF_BEM_MAX_SLOT_CHANGED             0x20000000
#define BTIF_BEM_SNIFFER_CONTROL_DONE         0x40000000
#define BTIF_BEM_LINK_POLICY_CHANGED             0x80000000
#define BTIF_BEM_ALL_EVENTS                   0xffffffff

typedef uint8_t btif_error_code_t;

#define BTIF_BEC_NO_ERROR             0x00
#define  BTIF_BEC_UNKNOWN_HCI_CMD      0x01
#define  BTIF_BEC_NO_CONNECTION        0x02
#define  BTIF_BEC_HARDWARE_FAILURE     0x03
#define  BTIF_BEC_PAGE_TIMEOUT         0x04
#define  BTIF_BEC_AUTHENTICATE_FAILURE 0x05
#define  BTIF_BEC_MISSING_KEY          0x06
#define  BTIF_BEC_MEMORY_FULL          0x07
#define BTIF_BEC_CONNECTION_TIMEOUT   0x08
#define BTIF_BEC_MAX_CONNECTIONS      0x09
#define BTIF_BEC_MAX_SCO_CONNECTIONS  0x0a
#define BTIF_BEC_ACL_ALREADY_EXISTS   0x0b
#define BTIF_BEC_COMMAND_DISALLOWED   0x0c
#define BTIF_BEC_LIMITED_RESOURCE     0x0d
#define BTIF_BEC_SECURITY_ERROR       0x0e
#define BTIF_BEC_PERSONAL_DEVICE      0x0f
#define BTIF_BEC_HOST_TIMEOUT         0x10
#define BTIF_BEC_UNSUPPORTED_FEATURE  0x11
#define BTIF_BEC_INVALID_HCI_PARM     0x12
#define BTIF_BEC_USER_TERMINATED      0x13
#define BTIF_BEC_LOW_RESOURCES        0x14
#define BTIF_BEC_POWER_OFF            0x15
#define BTIF_BEC_LOCAL_TERMINATED     0x16
#define BTIF_BEC_REPEATED_ATTEMPTS    0x17
#define BTIF_BEC_PAIRING_NOT_ALLOWED  0x18
#define BTIF_BEC_UNKNOWN_LMP_PDU      0x19
#define BTIF_BEC_UNSUPPORTED_REMOTE   0x1a
#define  BTIF_BEC_SCO_OFFSET_REJECT    0x1b
#define BTIF_BEC_SCO_INTERVAL_REJECT  0x1c
#define BTIF_BEC_SCO_AIR_MODE_REJECT  0x1d
#define BTIF_BEC_INVALID_LMP_PARM     0x1e
#define BTIF_BEC_UNSPECIFIED_ERR      0x1f
#define BTIF_BEC_UNSUPPORTED_LMP_PARM 0x20
#define BTIF_BEC_ROLE_CHG_NOT_ALLOWED 0x21
#define BTIF_BEC_LMP_RESPONSE_TIMEOUT 0x22
#define BTIF_BEC_LMP_TRANS_COLLISION  0x23
#define BTIF_BEC_LMP_PDU_NOT_ALLOWED  0x24
#define BTIF_BEC_ENCRYP_MODE_NOT_ACC  0x25
#define BTIF_BEC_UNIT_KEY_USED        0x26
#define BTIF_BEC_QOS_NOT_SUPPORTED    0x27
#define BTIF_BEC_INSTANT_PASSED       0x28
#define BTIF_BEC_PAIR_UNITKEY_NO_SUPP 0x29
#define BTIF_BEC_NOT_FOUND            0xf1
#define BTIF_BEC_REQUEST_CANCELLED    0xf2

#define BTIF_BEC_INVALID_SDP_PDU      0xd1
#define BTIF_BEC_SDP_DISCONNECT       0xd2
#define BTIF_BEC_SDP_NO_RESOURCES     0xd3
#define BTIF_BEC_SDP_INTERNAL_ERR     0xd4

#define BTIF_BEC_STORE_LINK_KEY_ERR   0xe0

typedef uint32_t btif_iac_t;

#define BTIF_BT_IAC_GIAC 0x9E8B33   /* General/Unlimited Inquiry Access Code */
#define BTIF_BT_IAC_LIAC 0x9E8B00   /* Limited Dedicated Inquiry Access Code */

typedef uint8_t btif_link_type_t;

#define BTIF_BLT_SCO   0x00
#define BTIF_BLT_ACL   0x01
#define BTIF_BLT_ESCO  0x02

typedef U16 btif_acl_packet;

#define BTIF_BAPT_NO_2_DH1  0x0002
#define BTIF_BAPT_NO_3_DH1  0x0004
#define BTIF_BAPT_DM1       0x0008
#define BTIF_BAPT_DH1       0x0010
#define BTIF_BAPT_NO_2_DH3  0x0100
#define BTIF_BAPT_NO_3_DH3  0x0200
#define BTIF_BAPT_DM3       0x0400
#define BTIF_BAPT_DH3       0x0800
#define BTIF_BAPT_NO_2_DH5  0x1000
#define BTIF_BAPT_NO_3_DH5  0x2000
#define BTIF_BAPT_DM5       0x4000
#define BTIF_BAPT_DH5       0x8000

#define BTIF_2M_PACKET     (BTIF_BAPT_DM1|BTIF_BAPT_DH1|BTIF_BAPT_NO_3_DH1|BTIF_BAPT_NO_3_DH3|BTIF_BAPT_DM3|BTIF_BAPT_DH3|BTIF_BAPT_NO_3_DH5)
#define BTIF_3M_PACKET     (BTIF_BAPT_DM1|BTIF_BAPT_DH1|BTIF_BAPT_DM3|BTIF_BAPT_DH3|BTIF_BAPT_DM5|BTIF_BAPT_DH5)
#define BTIF_1_SLOT_PACKET (BTIF_BAPT_DM1|BTIF_BAPT_DH1|BTIF_BAPT_NO_3_DH1|BTIF_BAPT_NO_2_DH3|BTIF_BAPT_NO_3_DH3|BTIF_BAPT_NO_2_DH5|BTIF_BAPT_NO_3_DH5)
#define BTIF_3_SLOT_PACKET (BTIF_BAPT_DM1|BTIF_BAPT_NO_3_DH3|BTIF_BAPT_NO_2_DH5|BTIF_BAPT_NO_3_DH5)

typedef uint16_t btif_link_policy_t;

#define BTIF_BLP_DISABLE_ALL         0x0000
#define BTIF_BLP_MASTER_SLAVE_SWITCH 0x0001
#define BTIF_BLP_HOLD_MODE           0x0002
#define BTIF_BLP_SNIFF_MODE          0x0004
#define BTIF_BLP_PARK_MODE           0x0008
#define BTIF_BLP_SCATTER_MODE        0x0010

/* Mask must be updated if new policy values are added */
#define BLP_MASK                0xfff0  /* Disables ScatterNet bit */
#define BLP_SCATTER_MASK        0xffe0  /* Enables ScatterNet bit */

typedef uint8_t btif_accessible_mode_t;

#define BTIF_BAM_NOT_ACCESSIBLE     0x00    /* Non-discoverable or connectable */
#define BTIF_BAM_GENERAL_ACCESSIBLE 0x03    /* General discoverable and connectable */
#define BTIF_BAM_LIMITED_ACCESSIBLE 0x13    /* Limited discoverable and connectable */
#define BTIF_BAM_CONNECTABLE_ONLY   0x02    /* Connectable but not discoverable */
#define BTIF_BAM_DISCOVERABLE_ONLY  0x01    /* Discoverable but not connectable */
#define BTIF_BT_DEFAULT_ACCESS_MODE_PAIR       BTIF_BAM_GENERAL_ACCESSIBLE
#define BTIF_BAM_INVALID_ACCESS_MODE    0xFF

/* End of BtAccessibleMode */

typedef uint8_t btif_oob_data_present_t;

#define BTIF_OOB_DATA_NOT_PRESENT  0    /* No Out of Band Data is present */
#define BTIF_OOB_DATA_PRESENT      1    /* Out of Band Data is present    */

typedef uint8_t btif_auth_requirements_t;

#define BTIF_MITM_PROTECT_NOT_REQUIRED  0x00    /* No Man in the Middle protection  */
#define BTIF_MITM_PROTECT_REQUIRED      0x01    /* Man in the Middle protection req */

typedef uint8_t btif_link_mode_t;

#define BTIF_BLM_ACTIVE_MODE     0x00
#define BTIF_BLM_HOLD_MODE       0x01
#define BTIF_BLM_SNIFF_MODE      0x02
#define BTIF_BLM_PARK_MODE       0x03
#define BTIF_BLM_SCATTER_MODE    0x04

typedef uint8_t btif_link_key_type_t;

#define BTIF_COMBINATION_KEY          0x00
#define BTIF_LOCAL_UNIT_KEY           0x01
#define BTIF_REMOTE_UNIT_KEY          0x02
#define BTIF_DEBUG_COMBINATION_KEY    0x03
#define BTIF_UNAUTH_COMBINATION_KEY   0x04
#define BTIF_AUTH_COMBINATION_KEY     0x05
#define BTIF_CHANGED_COMBINATION_KEY  0x06
#define BTIF_IBRT_STORE_ADDR_ONLY     0x07

typedef uint8_t btif_connection_role_t;

#define BTIF_BCR_MASTER   0x00
#define BTIF_BCR_SLAVE    0x01
#define BTIF_BCR_ANY      0x02
#define BTIF_BCR_UNKNOWN  0x03
#define BTIF_BCR_PSLAVE   0x04
#define BTIF_BCR_PMASTER  0x05

typedef uint8_t btif_stack_state_t;

/* The stack has completed initialization of the radio hardware. */
#define BTIF_BTSS_NOT_INITIALIZED 0

/* The stack is initialized. */
#define BTIF_BTSS_INITIALIZED     1

/* The stack has encountered an error while initializing the radio hardware. */
#define BTIF_BTSS_INITIALIZE_ERR  2

/* The stack is deinitializing. */
#define BTIF_BTSS_DEINITIALIZE    3

/* End of BtStackState */
typedef void btif_remote_device_t;


typedef void btif_BtDeviceContext;


/* Forward declaration of the callback parameters */
typedef void btif_event_t;

typedef void (*btif_callback) (const void *);
typedef void (*ibrt_cmd_status_callback)(const uint8_t *para);
typedef void (*ibrt_cmd_complete_callback)(const uint8_t *para);
typedef uint8_t (*btif_callback_ext) (const void *);
typedef uint8_t (*btif_callback_ext2) (void);
typedef void (*btif_callback_ext3) (void);

typedef uint8_t btif_inquiry_mode_t;

#define BTIF_INQ_MODE_NORMAL    0   /* Normal Inquiry Response format           */
#define BTIF_INQ_MODE_RSSI      1   /* RSSI Inquiry Response format             */
#define BTIF_INQ_INVALID_RSSI   127   /* RSSI Inquiry Response format             */
#define BTIF_INQ_MODE_EXTENDED  2   /* Extended or RSSI Inquiry Response format */

typedef uint8_t btif_rem_dev_state_t;

#define BTIF_BDS_DISCONNECTED  0x00
#define BTIF_BDS_OUT_CON       0x01 /* Starting an out going connection */
#define BTIF_BDS_IN_CON        0x02 /* In process of incoming connection */
#define BTIF_BDS_CONNECTED     0x03 /* Connected */
#define BTIF_BDS_OUT_DISC      0x04 /* Starting an out going disconnect */
#define BTIF_BDS_OUT_DISC2     0x05 /* Disconnect status received */
#define BTIF_BDS_OUT_CON2      0x06 /* In SCO, used when connection request has
                                       been sent */

typedef uint8_t btif_eir_data_type_t;

#define BTIF_EIR_FLAGS                0x01
#define BTIF_EIR_SRV_CLASS_16_PART    0x02
#define BTIF_EIR_SRV_CLASS_16_WHOLE   0x03
#define BTIF_EIR_SRV_CLASS_32_PART    0x04
#define BTIF_EIR_SRV_CLASS_32_WHOLE   0x05
#define BTIF_EIR_SRV_CLASS_128_PART   0x06
#define BTIF_EIR_SRV_CLASS_128_WHOLE  0x07
#define BTIF_EIR_REM_NAME_PART        0x08
#define BTIF_EIR_REM_NAME_WHOLE       0x09
#define BTIF_EIR_TX_POWER             0x0A
#define BTIF_EIR_MAN_SPECIFIC         0xFF


#define CONN_AUTO_ACCEPT_DISABLE                      0x01

#define CONN_AUTO_ACCEPT_ENABLE_WITH_MSS_DISABLE      0x02

#define CONN_AUTO_ACCEPT_ENABLE_WITH_MSS_ENABLE       0x03
/*---------------------------------------------------------------------------
 * btif_class_of_device_t type
 *
 *     Bit pattern representing the class of device along with the
 *     supported services. There can be more than one supported service.
 *     Service classes can be ORed together. The Device Class is composed
 *     of a major device class plus a minor device class. ORing together
 *     each service class plus one major device class plus one minor device
 *     class creates the class of device value. The minor device class is
 *     interpreted in the context of the major device class.
 */

typedef uint32_t btif_class_of_device_t;

/* Group: Major Service Classes. Can be ORed together */
#define BTIF_COD_LIMITED_DISCOVERABLE_MODE 0x00002000
#define BTIF_COD_POSITIONING               0x00010000
#define BTIF_COD_NETWORKING                0x00020000
#define BTIF_COD_RENDERING                 0x00040000
#define BTIF_COD_CAPTURING                 0x00080000
#define BTIF_COD_OBJECT_TRANSFER           0x00100000
#define BTIF_COD_AUDIO                     0x00200000
#define BTIF_COD_TELEPHONY                 0x00400000
#define BTIF_COD_INFORMATION               0x00800000

/* Group: Major Device Classes (Select one) */
#define BTIF_COD_MAJOR_MISCELLANEOUS       0x00000000
#define BTIF_COD_MAJOR_COMPUTER            0x00000100
#define BTIF_COD_MAJOR_PHONE               0x00000200
#define BTIF_COD_MAJOR_LAN_ACCESS_POINT    0x00000300
#define BTIF_COD_MAJOR_AUDIO               0x00000400
#define BTIF_COD_MAJOR_PERIPHERAL          0x00000500
#define BTIF_COD_MAJOR_IMAGING             0x00000600
#define BTIF_COD_MAJOR_UNCLASSIFIED        0x00001F00

/* Group: Minor Device Class - Computer Major class */
#define BTIF_COD_MINOR_COMP_UNCLASSIFIED   0x00000000
#define BTIF_COD_MINOR_COMP_DESKTOP        0x00000004
#define BTIF_COD_MINOR_COMP_SERVER         0x00000008
#define BTIF_COD_MINOR_COMP_LAPTOP         0x0000000C
#define BTIF_COD_MINOR_COMP_HANDHELD       0x00000010
#define BTIF_COD_MINOR_COMP_PALM           0x00000014
#define BTIF_COD_MINOR_COMP_WEARABLE       0x00000018

/* Group: Minor Device Class - Phone Major class */
#define BTIF_COD_MINOR_PHONE_UNCLASSIFIED  0x00000000
#define BTIF_COD_MINOR_PHONE_CELLULAR      0x00000004
#define BTIF_COD_MINOR_PHONE_CORDLESS      0x00000008
#define BTIF_COD_MINOR_PHONE_SMART         0x0000000C
#define BTIF_COD_MINOR_PHONE_MODEM         0x00000010
#define BTIF_COD_MINOR_PHONE_ISDN          0x00000014

/* Group: Minor Device Class - LAN Access Point Major class */
#define BTIF_COD_MINOR_LAN_0               0x00000000   /* fully available */
#define BTIF_COD_MINOR_LAN_17              0x00000020   /* 1-17% utilized */
#define BTIF_COD_MINOR_LAN_33              0x00000040   /* 17-33% utilized */
#define BTIF_COD_MINOR_LAN_50              0x00000060   /* 33-50% utilized */
#define BTIF_COD_MINOR_LAN_67              0x00000080   /* 50-67% utilized */
#define BTIF_COD_MINOR_LAN_83              0x000000A0   /* 67-83% utilized */
#define BTIF_COD_MINOR_LAN_99              0x000000C0   /* 83-99% utilized */
#define BTIF_COD_MINOR_LAN_NO_SERVICE      0x000000E0   /* 100% utilized */

/* Group: Minor Device Class - Audio Major class */
#define BTIF_COD_MINOR_AUDIO_UNCLASSIFIED  0x00000000
#define BTIF_COD_MINOR_AUDIO_HEADSET       0x00000004
#define BTIF_COD_MINOR_AUDIO_HANDSFREE     0x00000008
#define BTIF_COD_MINOR_AUDIO_MICROPHONE    0x00000010
#define BTIF_COD_MINOR_AUDIO_LOUDSPEAKER   0x00000014
#define BTIF_COD_MINOR_AUDIO_HEADPHONES    0x00000018
#define BTIF_COD_MINOR_AUDIO_PORTABLEAUDIO 0x0000001C
#define BTIF_COD_MINOR_AUDIO_CARAUDIO      0x00000020
#define BTIF_COD_MINOR_AUDIO_SETTOPBOX     0x00000024
#define BTIF_COD_MINOR_AUDIO_HIFIAUDIO     0x00000028
#define BTIF_COD_MINOR_AUDIO_VCR           0x0000002C
#define BTIF_COD_MINOR_AUDIO_VIDEOCAMERA   0x00000030
#define BTIF_COD_MINOR_AUDIO_CAMCORDER     0x00000034
#define BTIF_COD_MINOR_AUDIO_VIDEOMONITOR  0x00000038
#define BTIF_COD_MINOR_AUDIO_VIDEOSPEAKER  0x0000003C
#define BTIF_COD_MINOR_AUDIO_CONFERENCING  0x00000040
#define BTIF_COD_MINOR_AUDIO_GAMING        0x00000048

/* Group: Minor Device Class - Peripheral Major class */
#define BTIF_COD_MINOR_PERIPH_KEYBOARD     0x00000040
#define BTIF_COD_MINOR_PERIPH_POINTING     0x00000080
#define BTIF_COD_MINOR_PERIPH_COMBOKEY     0x000000C0
/* Group: Minor Device Class - ORed with Peripheral Minor Device class */
#define BTIF_COD_MINOR_PERIPH_UNCLASSIFIED 0x00000000
#define BTIF_COD_MINOR_PERIPH_JOYSTICK     0x00000004
#define BTIF_COD_MINOR_PERIPH_GAMEPAD      0x00000008
#define BTIF_COD_MINOR_PERIPH_REMOTECTRL   0x0000000C
#define BTIF_COD_MINOR_PERIPH_SENSING      0x00000010
#define BTIF_COD_MINOR_PERIPH_DIGITIZER    0x00000014
#define BTIF_COD_MINOR_PERIPH_CARD_RDR     0x00000018

/* Group: Minor Device Class - Imaging Major class */
#define BTIF_COD_MINOR_IMAGE_UNCLASSIFIED  0x00000000
#define BTIF_COD_MINOR_IMAGE_DISPLAY       0x00000010
#define BTIF_COD_MINOR_IMAGE_CAMERA        0x00000020
#define BTIF_COD_MINOR_IMAGE_SCANNER       0x00000040
#define BTIF_COD_MINOR_IMAGE_PRINTER       0x00000080

/* Group: Masks used to isolate the class of device components */
#define BTIF_COD_SERVICE_MASK              0x00ffC000   /* Less LIAC bit */
#define BTIF_COD_MAJOR_MASK                0x00001F00
#define BTIF_COD_MINOR_MASK                0x000000FC
#define BTIF_COD_LIMITED_DISC_MASK         0x00002000   /* LIAC bit */

/* End of btif_class_of_device_t */

//when report_format=0,data_format will set the interval(unit:slot)
//when report_format=1,data_format will set the packet total num
//when report_format=2,data_format has no meaning
#define FLAG_RESET_DATA    2
#define FLAG_START_DATA    1
#define FLAG_GET_DATA        0

#define REP_FORMAT_TIME     0
#define REP_FORMAT_PACKET  1
#define REP_FORMAT_STOP_GET  2


//Vendor defined error code
#define  BTIF_EVT_ERROR_SCO_UNKNOWN       (0xE8)
#define  BTIF_EVT_ERROR_ACL_UNKNOWN       (0xE9)


typedef struct {
    list_entry_t node;
    uint8_t opType;
} btif_operation_t;

typedef struct {

    /* the mandatory sniff interval range for controllers is between 0x0006 and 0x0540.
     * The value is expressed in 0.625 ms increments (0x0006 = 3.75 ms).
     *
     * The actual interval selected by the radio will be returned in
     * a BTEVENT_MODE_CHANGE event.
     */

    uint16_t maxInterval;

    /* Minimum acceptable interval between each consecutive sniff period.
     * Must be an even number between 0x0002 and 0xFFFE, and be less than
     * "maxInterval". Like maxInterval this value is expressed in
     * 0.625 ms increments.
     */
    uint16_t minInterval;

    /* The number of master-to-slave transmission slots during which
     * a device should listen for traffic (sniff attempt).
     * Expressed in 0.625 ms increments. May be between 0x0001 and 0x7FFF.
     */
    uint16_t attempt;

    /* The amount of time before a sniff timeout occurs. Expressed in
     * 1.25 ms increments. May be between 0x0000 and 0x7FFF, but the mandatory
     * range for controllers is 0x0000 to 0x0028.
     */
    uint16_t timeout;
} btif_sniff_info_t;

typedef struct {

    /*  May be any number between 0x0000 and 0xFFFE.  The value is expressed in 0.625 ms
     * increments.
     */
    uint16_t maxLatency;

    /*  any number between 0x0000 and 0xFFFE.  The value is expressed in
     * 0.625 ms increments.
     */
    uint16_t minRemoteTimeout;

    /* Minimum base sniff subrate timeout that the local device may use.
     * May be any number between 0x0000 and 0xFFFE.  The value is expressed in
     * 0.625 ms increments.
     */
    uint16_t minLocalTimeout;
} btif_sniff_subrate_parms_t;

typedef struct {
    uint16_t inqInterval;            /* Inquiry scan interval */
    uint16_t inqWindow;              /* Inquiry scan Window */
    uint16_t pageInterval;           /* Page scan interval */
    uint16_t pageWindow;             /* Page scan window */
} btif_access_mode_info_t;

typedef struct {
    list_entry_t node;          /* For internal stack use only. */
    btif_callback callback;  /* Pointer to callback function */
    btif_event_mask_t emask; /* For internal stack use only. */
} btif_handler;

/* Internal types required for BtRemoteDevice structure */
typedef uint8_t btif_auth_state_t;

typedef uint8_t btif_encrypt_state_t;

typedef uint8_t btif_authorize_state_t;

typedef uint8_t btif_sec_access_state_t;

typedef uint8_t btif_link_rx_state_t;

typedef uint8_t btif_op_type_t;

typedef struct {

    /* Reserved */
    uint8_t flags;

    /* Service Type: 0 = No Traffic, 1 = Best Effort, 2 Guaranteed */
    uint8_t serviceType;

    /* Token Rate in octets per second */
    uint32_t tokenRate;

    /* Peak Bandwidth in octets per second */
    uint32_t peakBandwidth;

    /* Latency in microseconds */
    uint32_t latency;

    /* Delay Variation in microseconds */
    uint32_t delayVariation;
} btif_qos_info_t;

typedef struct {
    bt_bdaddr_t bdAddr;
    BOOL trusted;
    uint8_t linkKey[16];
    btif_link_key_type_t keyType;

    uint8_t pinLen;
} btif_device_record_t;

typedef struct {
    btif_operation_t op;
    uint8_t len;
    uint8_t data[31];
} btif_write_adv_data_op_t;

typedef struct {
    uint16_t interval_min;
    uint16_t interval_max;
    uint8_t adv_type;
    uint8_t own_addr_type;
    uint8_t peer_addr_type;
    bt_bdaddr_t bd_addr;
    uint8_t adv_chanmap;
    uint8_t adv_filter_policy;
} btif_adv_para_struct_t;

typedef struct {
    btif_operation_t op;
    btif_adv_para_struct_t adv_para;
} btif_write_adv_para_op_t;

typedef struct {
    btif_operation_t op;
    uint8_t en;
} btif_write_adv_en_op_t;

#define BLE_ADV_REPORT_MAX_LEN 31

typedef struct {
    uint8_t type;
    uint8_t addr_type;
    bt_bdaddr_t addr;
    uint8_t data_len;
    uint8_t data[BLE_ADV_REPORT_MAX_LEN];
    int8_t rssi;
} btif_ble_adv_report;

typedef struct {
    uint8_t scan_type;
    uint16_t scan_interval;
    uint16_t scan_window;
    uint8_t own_addr_type;
    uint8_t scan_filter_policy;
} __attribute__ ((packed)) btif_scan_para_struct_t;

typedef struct {
    btif_operation_t op;
    btif_scan_para_struct_t scan_para;
} btif_write_scan_para_op_t;

typedef struct {
    btif_operation_t op;
    uint8_t scan_en;
    uint32_t filter_duplicat;
} btif_write_scan_en_op_t;

typedef struct
{
    bt_bdaddr_t bdAddr;           /* Device Address */
    uint8_t     linkKey[16];
} btif_dev_linkkey;

typedef struct
{
    U8   psRepMode;
    U8   psMode;
    U16  clockOffset;
} bt_page_scanInfo_t;

typedef struct
{
    bt_bdaddr_t bdAddr;           /* Device Address */
    bt_page_scanInfo_t  psi;              /* Page scan info used for connecting */
    U8              psPeriodMode;
    U32     classOfDevice;

    /* RSSI in dBm (-127 to +20). Only valid when controller reports RSSI with
     * in inquiry results (also see ME_SetInquiryMode). Otherwise it will be
     * set to BT_INVALID_RSSI.
     */
    S8              rssi;

    /* Extended Inquiry response.  Only valid when controller reports an
     * extended inquiry (also see ME_SetInquiryMode).  Otherwise it will be
     * set to all 0's.
     */
    U8              extInqResp[240];

    /* Describes the format of the current inquiry result */
    U8   inqMode;

} bt_Inquiry_result_t;

typedef struct
{
    /* Event causing callback. Always valid.*/
    uint8_t   eType;

    /* Error code. See BtEventType for guidance on whether errCode is valid. */
    uint8_t   errCode;

    /* Pointer to handler. Only valid for events directed to BtHandlers. */
    btif_handler   *handler;

    /* Parameters */
    union
    {
        btif_accessible_mode_t   aMode;      /* New access mode */
        void    *remDev;     /* Pointer to remote device */
        bt_bdaddr_t bdAddr;     /* Device Address */
        void    *meToken;    /* Me command token */
        U8                 pMode;      /* Simple Pairing Mode */

#ifdef __TWS_RECONNECT_USE_BLE__
        struct
        {
            U8          *data;
            U16         len;
        } twsBleReport;
#endif
        bt_Inquiry_result_t    inqResult;

        void  *secToken;   /* Security Token */
        void    *token;

        /* Information for BTEVENT_PAIRING_COMPLETE */
        struct
        {
            void *remDev;
            btif_link_key_type_t   keyType;
        } pairingInfo;

        /* Information for BTEVENT_PIN_REQ */
        struct
        {
            void *remDev;

            /* If > 0 then the pin length returned in SEC_SetPin must be >= pinLen */
            U8              pinLen;
        } pinReq;

        /* Information for BTEVENT_SET_INQUIRY_MODE_CNF */
        btif_inquiry_mode_t      inqMode;

        /* Information for BTEVENT_SET_INQ_TX_PWR_LVL_CNF */
        S8       inqTxPwr;

        /* Information for BTEVENT_REMOTE_FEATURES */
        struct
        {
            bt_bdaddr_t  bdAddr;
            U8      features[8];
        } remoteFeatures;

        /* Information for BTEVENT_REM_HOST_FEATURES */
        struct
        {
            bt_bdaddr_t bdAddr;
            U8      features[8];
        } remHostFeatures;

        /* Information for BTEVENT_LINK_SUPERV_TIMEOUT_CHANGED */
        struct
        {
            void *remDev;
            U16             timeout;
        } linkSupervision;

        /* Information for BTEVENT_MAX_SLOT_CHANGED */
        struct
        {
            U16             connHandle;
            U8              maxSlot;
        } maxSlotChanged;

        /* Information for BTEVENT_QOS_SETUP_COMPLETE */
        struct
        {

            /* Reserved */
            U8 flags;

            /* Service Type: 0 = No Traffic, 1 = Best Effort, 2 Guaranteed */
            U8  serviceType;

            /* Token Rate in octets per second */
            U32 tokenRate;

            /* Peak Bandwidth in octets per second */
            U32 peakBandwidth;

            /* Latency in microseconds */
            U32 latency;

            /* Delay Variation in microseconds */
            U32 delayVariation;
        } qos;

        /* Result for BTEVENT_SET_SNIFF_SUBRATING_PARMS_CNF */
        btif_sniff_subrate_parms_t *sniffSubrateParms;

        /* Information for BTEVENT_SNIFF_SUBRATE_INFO */
        struct
        {
            /* Pointer to the remote device */
            void *remDev;

            /* Maximum latency for data being transmitted from the local
             * device to the remote device.
             */
            U16 maxTxLatency;

            /* Maximum latency for data being received by the local
             * device from the remote device.
             */
            U16 maxRxLatency;

            /* The base sniff subrate timeout in baseband slots that the
             * remote device shall use.
             */
            U16 minRemoteTimeout;

            /* The base sniff subrate timeout in baseband slots that the
             * local device will use.
             */
            U16 minLocalTimeout;
        } sniffSubrateInfo;

        /* Result for BTEVENT_LOCAL_OOB_DATA */
        struct
        {
            /* Simple Pairing Hash C from the local device */
            U8 hashC[16];

            /* Simple Pairing Randomizer R the local device */
            U8 randR[16];
        } oobData;

        /* Information for BTEVENT_CONFIRM_NUMERIC_REQ, BTEVENT_PASS_KEY_REQ,
         * and BTEVENT_DISPLAY_NUMERIC_IND
         */
        struct
        {
            void *remDev;      /* Pointer to the remote device */
            U32             numeric;     /* Numeric value received from Secure
                                          * Simple Pairing (not valid for
                                          * BTEVENT_PASS_KEY_REQ
                                          */

            U8   bondingMode;  /* Bonding has been requested */
        } userIoReq;

        /* Result for BTEVENT_ENCRYPTION_CHANGE event. */
        struct
        {
            void *remDev;
            U8              mode; /* New encryption mode (uses the
                                   * BtEncryptMode type) */
        } encrypt;

        /* Result for BTEVENT_KEY_PRESSED */
        struct
        {
            bt_bdaddr_t bdAddr; /* Address of the device sending the keypress */
            U8  parm;   /* The value of the keypress parameter */
        } keyPress;

        struct
        {
            void *remDev;

            /* If disconnection was successful, contains BEC_NO_ERROR.
             * errCode will contain the disconnect reason.
             *
             * Unsuccessful disconnections will contain an error code
             * as generated by the radio. In this case, errCode can be
             * ignored.
             */
            btif_error_code_t status;
        } disconnect;

        /* Result for the BTEVENT_SCO_DATA_CNF event */
        struct
        {
            void *remDev;     /* Pointer to remote device */
            void   *scoCon;     /* SCO connection */
            void       *scoPacket;  /* SCO Packet Handled */
        } scoPacketHandled;

        /* Result for the BTEVENT_SCO_CONNECT_CNF && BTEVENT_SCO_CONNECT_IND
           events.
         */
        struct
        {
            void *remDev;     /* Pointer to remote device */
            U16             scoHandle;  /* SCO Connection handle for HCI */
            void   *scoCon;     /* SCO connection */
            U8      scoLinkType;/* SCO link type */
            void   *scoTxParms; /* Pointer to eSCO TX parameters */
            void   *scoRxParms; /* Pointer to eSCO RX parameters */
        } scoConnect;

        /* Result for the BTEVENT_SCO_DATA_IND event */
        struct
        {
            void *remDev;     /* Pointer to remote device */
            U16     scoHandle;  /* SCO Connection handle for HCI */
            void   *scoCon;     /* SCO connection. */
            U8              len;        /* SCO data len */
            U8             *ptr;        /* SCO data ptr */
            U8   errFlags;   /* Erroneous Data Reporting */
        } scoDataInd;

        /* Result for the BTEVENT_SECURITY_CHANGE and
         * BTEVENT_SECURITY3_COMPLETE events
         */
        struct
        {
            U8    mode;    /* New security mode (uses the BtSecurityMode
                            * type). */
            BOOL  encrypt; /* Indicate if encryption set or not */
        } secMode;

        /* Results for the BTEVENT_MODE_CHANGE event */
        struct
        {
            void *remDev;
            uint8_t      curMode;
            U16             interval;
        } modeChange;

        /* Results for BTEVENT_ROLE_CHANGE */
        struct
        {
            void    *remDev;
            uint8_t   newRole;    /* New role */
        } roleChange;

        /* Results for BTEVENT_ACL_DATA_ACTIVE */
        struct
        {
            void    *remDev;
            uint16_t   dataLen;    /* ACL data length */
        } aclDataActive;
        
        /* Results for BTEVENT_ACL_DATA_NOT_ACTIVE */
        struct
        {
            void    *remDev;
            uint16_t   dataLen;    /* ACL data length */
        } aclDataNotActive;
        struct
        {
            uint8_t status;
            bt_bdaddr_t bdAddr; /* Address of the device sending the keypress */
        }name_rsp;
        struct
        {
            btif_remote_device_t *conn;
        }conn_req;
    } edata;
} event_t;

typedef struct
{
    event_t  *evt;
    btif_event_mask_t mask;
} me_event_t;
typedef bool (*ibrt_io_capbility_callback)(void *bdaddr);
typedef void (*ibrt_disconnect_callback)(const void *event);
typedef void (*btif_cmgr_callback) (void *handler, uint8_t event, bt_status_t status);
typedef void (*btif_global_handle)(const btif_event_t *Event);

#ifdef __cplusplus
extern "C" {
#endif                          /*  */
    bt_status_t btif_me_get_tws_slave_mobile_rssi(uint16_t ConnHandle);

    void btif_me_set_sniffer_env(uint8_t sniffer_acitve, uint8_t sniffer_role,
                                 uint8_t * monitored_addr, uint8_t * sniffer_addr);
    BOOL btif_me_get_remote_device_initiator(btif_remote_device_t * rdev);
    uint16_t btif_me_get_remote_device_hci_handle(btif_remote_device_t * rdev);
    btif_remote_device_t* btif_me_get_remote_device_by_handle(uint16_t hci_handle);
    btif_remote_device_t* btif_me_get_remote_device_by_bdaddr(bt_bdaddr_t *bdaddr);
    uint8_t btif_me_get_remote_device_op_optype(btif_remote_device_t * rdev);
    btif_connection_role_t btif_me_get_remote_device_role(btif_remote_device_t * rdev);
    void btif_me_set_remote_device_role(btif_remote_device_t * rdev, uint8_t role);
    BOOL *btif_me_get_remote_device_new_link_key(btif_remote_device_t * rdev);
    void *btif_me_get_remote_device_parms(btif_remote_device_t * rdev);
    bt_status_t btif_me_exchange_bt_addr(uint16_t connHandle);
    bool btif_me_role_switch_pending(uint16_t handle);
    bool btif_me_is_tws_role_switch_pending();
    bool btif_me_is_exechange_bt_addr_pending();
    bool is_btif_me_current_role_bcr_master(btif_remote_device_t * device);
    bt_status_t btif_me_inquiry(uint32_t lap, uint8_t len, uint8_t maxResp);
    bt_status_t btif_me_ble_add_dev_to_whitelist(uint8_t addr_type, bt_bdaddr_t * addr);
    bt_status_t btif_me_ble_clear_whitelist(void);
    bt_status_t btif_me_ble_set_private_address(bt_bdaddr_t * addr);
    bt_status_t btif_me_ble_set_adv_data(uint8_t len, uint8_t * data);
    bt_status_t btif_me_ble_set_scan_rsp_data(U8 len, U8 * data);
    bt_status_t btif_me_ble_set_adv_parameters(btif_adv_para_struct_t * para);
    bt_status_t btif_me_ble_set_adv_en(uint8_t en);
    bt_status_t btif_me_ble_set_scan_parameter(btif_scan_para_struct_t * para);
    bt_status_t btif_me_ble_set_scan_en(uint8_t scan_en, uint8_t filter_duplicate);
    bt_status_t btif_me_ble_receive_adv_report(void (*cb)(const btif_ble_adv_report* report));
    bt_status_t btif_sec_find_device_record(const bt_bdaddr_t * bdAddr,
                                            btif_device_record_t * record);
    
    uint8_t btif_sec_set_io_capabilities(uint8_t ioCap);
    uint8_t btif_sec_set_authrequirements(uint8_t authRequirements);
    uint8_t btif_me_get_callback_event_type(const btif_event_t * event);
    uint8_t btif_me_get_callback_event_rem_dev_role(const btif_event_t * event);
    bt_bdaddr_t *btif_me_get_callback_event_rem_dev_bd_addr(const btif_event_t * event);
    bt_bdaddr_t *btif_me_get_callback_event_disconnect_rem_dev_bd_addr(const btif_event_t *
                                                                       event);
    btif_remote_device_t *btif_me_get_callback_event_disconnect_rem_dev(const btif_event_t *
                                                                           event);
    uint8_t btif_me_get_callback_event_disconnect_rem_dev_disc_reason_saved(const btif_event_t *
                                                                            event);
    uint8_t btif_me_get_callback_event_disconnect_rem_dev_disc_reason(const btif_event_t *
                                                                      event);

    uint8_t btif_me_get_pendCons(void);

    uint8_t btif_me_get_activeCons(void);

    void btif_me_set_pendCons(uint8_t pendCons);

    void btif_me_set_activeCons(uint8_t activeCons );

    btif_remote_device_t *btif_me_get_callback_event_rem_dev(const btif_event_t * event);

    btif_BtDeviceContext* btif_me_get_bt_device_context(const btif_event_t * event);

    uint16_t btif_me_get_scohdl_by_connhdl(uint16_t conn_handle);
    btif_remote_device_t *btif_me_get_callback_event_sco_connect_rem_dev(const btif_event_t *
                                                                            event);
    uint8_t btif_me_get_callback_event_role_change_new_role(const btif_event_t * event);
    bt_bdaddr_t *btif_me_get_callback_event_inq_result_bd_addr(const btif_event_t * event);
    uint8_t *btif_me_get_callback_event_inq_result_bd_addr_addr(const btif_event_t * event);
    uint8_t btif_me_get_callback_event_inq_result_inq_mode(const btif_event_t * event);
    uint8_t btif_me_get_callback_event_rssi(const btif_event_t *event);
    uint8_t *btif_me_get_callback_event_inq_result_ext_inq_resp(const btif_event_t * event);
    uint32_t btif_me_get_callback_event_inq_result_classofdevice(const btif_event_t *event);
    uint8_t btif_me_get_callback_event_err_code(const btif_event_t * event);
    uint8_t btif_me_get_callback_event_a_mode(const btif_event_t * event);
    uint16_t btif_me_get_callback_event_max_slot_changed_connHandle(const btif_event_t * event);
    uint8_t btif_me_get_callback_event_max_slot_changed_max_slot(const btif_event_t * event);
    uint8_t btif_me_get_callback_event_mode_change_curMode(const btif_event_t * event);
    uint16_t btif_me_get_callback_event_mode_change_interval(const btif_event_t * event);

    uint16_t btif_me_get_callback_event_remote_dev_name(const btif_event_t * event, uint8_t** ppName);

    bt_status_t btif_me_get_remote_device_name(const bt_bdaddr_t * bdAddr, btif_global_handle handler);

    uint8_t btif_me_get_ext_inq_data(uint8_t * eir, btif_eir_data_type_t type,
                                     uint8_t * outBuffer, uint8_t Length);
    bt_status_t btif_me_cancel_inquiry(void);
    bt_status_t btif_sec_delete_device_record(const bt_bdaddr_t * bdAddr);
    bt_status_t btif_me_cancel_create_link(btif_handler * handler,
                                           btif_remote_device_t * rdev);
    void btif_me_set_handler(void *handler, btif_callback cb);
    uint8_t btif_me_set_connection_role(uint8_t role);
    bt_status_t btif_me_disconnect_link(btif_handler * handler,
                                        btif_remote_device_t * rdev);
    bt_status_t btif_me_set_link_policy(btif_remote_device_t * rdev,
                                        btif_link_policy_t policy);
    bt_status_t btif_me_set_link_lowlayer_monitor(btif_remote_device_t * rdev, uint8_t control_flag,uint8_t report_format,
                                                          uint32_t data_format,uint8_t report_unit);   
    bt_status_t btif_me_set_connection_qos_info(btif_remote_device_t * rdev,
                                                btif_qos_info_t * qosInfo);
    void btif_me_fake_tws_disconnect(uint16_t hci_handle, uint8_t reason);
    void btif_me_fake_mobile_disconnect(uint16_t hci_handle, uint8_t reason);
    void btif_me_fake_tws_connect(uint8_t status, uint16_t hci_handle, bt_bdaddr_t * bdAddr);
    void btif_me_reset_bt_controller(void);
    void btif_me_fake_mobile_connect(uint8_t status, uint16_t hci_handle, bt_bdaddr_t *bdAddr);

    bt_status_t btif_me_set_lbrt_enable(uint16_t connHandle, uint8_t enable);
    bt_status_t btif_me_set_accessible_mode(btif_accessible_mode_t mode,
                                            const btif_access_mode_info_t * info);
    bt_status_t btif_me_write_page_timeout(uint16_t timeout);
    bool btif_me_is_op_in_progress(btif_remote_device_t * rdev);
    bt_status_t btif_me_switch_role(btif_remote_device_t * rdev);
    bt_status_t btif_me_register_global_handler(void *handler);
    void *btif_me_register_accept_handler(void *handler);
    bt_status_t btif_me_set_event_mask(void *handler, btif_event_mask_t mask);
    void *btif_me_get_bt_handler(void);
    bt_status_t btif_me_set_bt_address(uint8_t * btAddr);
    bt_status_t btif_me_set_local_device_name(const U8 *name, U8 length);
    bt_status_t btif_me_set_ble_bd_address(uint8_t * btAddr);
    bt_status_t btif_sec_add_device_record(btif_device_record_t * record);
    bt_status_t btif_enum_device_record(U16 dev_id, btif_device_record_t *record);
    bt_bdaddr_t *btif_me_get_remote_device_bdaddr(btif_remote_device_t * rdev);
    btif_rem_dev_state_t btif_me_get_remote_device_state(btif_remote_device_t * rdev);
    btif_link_mode_t btif_me_get_remote_device_mode(btif_remote_device_t * rdev);
    btif_authorize_state_t btif_me_get_remote_device_auth_state(btif_remote_device_t * rdev);
    bt_status_t btif_me_write_link_superv_timeout(uint16_t handle, uint16_t slots);
    btif_link_mode_t btif_me_get_current_mode(btif_remote_device_t * rdev);
    btif_connection_role_t btif_me_get_current_role(btif_remote_device_t * rdev);

    bt_status_t btif_me_start_sniff(btif_remote_device_t * rdev, btif_sniff_info_t* info);

    bt_status_t btif_me_stop_sniff(btif_remote_device_t * rdev);

    bt_status_t btif_me_accept_incoming_link(const btif_event_t *event,btif_remote_device_t * rdev, btif_connection_role_t role);
    
    bt_status_t btif_me_reject_incoming_link(btif_remote_device_t * rdev, btif_error_code_t reason);

    bt_status_t btif_me_start_tws_role_switch(uint16_t slaveConnHandle, uint16_t mobileConnHandle);
    bt_status_t btif_me_set_sco_tx_silence(uint16_t connHandle, uint8_t silence_on);
    void btif_me_increase_active_counts(void);
    bool btif_me_is_sending_data_to_peer_dev_pending(void);
    btif_handler *btif_me_get_me_handler(void);
    bt_status_t btif_me_force_disconnect_link_with_reason(btif_handler * handler,
                                                          btif_remote_device_t * rdev,
                                                          uint8_t reason, BOOL forceDisconnect);

    void btif_me_sec_set_io_cap_rsp_reject_ext(BOOL(*ext_fn) (void *));
    void btif_me_write_bt_sleep_enable(uint8_t sleep_en);
    void btif_me_write_bt_page_scan_type(uint8_t scan_type);
    void btif_me_write_bt_inquiry_scan_type(uint8_t scan_type);

#if IS_USE_INTERNAL_ACL_DATA_PATH
    bt_status_t btif_me_send_data_to_peer_dev(uint16_t connHandle, uint8_t dataLen, uint8_t * data);
#endif                          /*  */
    void btif_bt_call_back(const btif_event_t * event);
    void btif_me_init_handler(btif_handler * handler);
    bt_status_t btif_me_dbg_sniffer_interface(uint16_t connHandle, uint8_t subCode);
    bt_status_t btif_me_switch_sco(uint16_t scohandle);
    uint8_t *btif_me_get_remote_device_version(btif_remote_device_t * rdev);
    void *btif_me_get_cmgr_handler();
    bt_status_t btif_bind_cmgr_handler(void *cmgr_handler, bt_bdaddr_t * bdAddr,btif_cmgr_callback Callback);
    bt_status_t btif_create_acl_to_slave(void *cmgr_handler, bt_bdaddr_t * bdAddr,btif_cmgr_callback Callback);
    bt_status_t btif_register_cmgr_handle(void *cmgr_handler,btif_cmgr_callback Callback);
    void btif_me_update_cmgr_info(void *cmgr_handler,btif_BtDeviceContext* bt_device_context,btif_remote_device_t * rdev);
    void btif_me_unregister_globa_handler(btif_handler * handler);
    void btif_me_set_inquiry_mode(uint8_t mode);
    void btif_me_inquiry_result_setup(uint8_t *inquiry_buff, bool rssi,
                                    bool extended_mode);
    btif_remote_device_t *btif_me_enumerate_remote_devices(uint32_t devid);

    uint8_t btif_me_get_remote_sevice_encrypt_state(btif_remote_device_t* rdev);

   uint8_t btif_me_get_remote_device_disc_reason_saved(btif_remote_device_t * device);


   uint8_t btif_me_get_remote_device_disc_reason(btif_remote_device_t * device);

   void  btif_me_event_report(me_event_t *event);
   void btif_me_init_peer_headset_addr(uint8_t *p_remote_addr);
   bt_bdaddr_t * btif_me_get_peer_headset_addr(void);
   uint8_t btif_me_get_remote_device_link_mode(btif_remote_device_t* rdev);
   uint8_t btif_me_get_remote_device_bt_role(btif_remote_device_t* rdev);
   bt_status_t btif_me_change_packet_type(btif_remote_device_t *rdev, btif_acl_packet packetTypes);
   void btif_me_cobuf_state_dump(void);
   void btif_me_hcibuff_state_dump(void);

#if defined(IBRT)
    // ibrt fast ack toggle
    #define IBRT_FAST_ACK_OFF                    (0)
    #define IBRT_FAST_ACK_ON                     (1)
    // ibrt fast direction 
    #define IBRT_FAST_ACK_TX                     0
    #define IBRT_FAST_ACK_RX                     1
    // ibrt slave flow control
    #define IBRT_SLAVE_FLOW_CTRL_ON              IBRT_FAST_ACK_OFF
    #define IBRT_SLAVE_FLOW_CTRL_OFF             IBRT_FAST_ACK_ON

    bt_status_t btif_me_enable_fastack(uint16_t conhdl, uint8_t direction, uint8_t enable);
    bt_status_t btif_me_start_ibrt(U16 slaveConnHandle, U16 mobileConnHandle);
    bt_status_t btif_me_stop_ibrt(uint8_t enable,uint8_t reason);
    bt_status_t btif_me_suspend_ibrt(void);
    bt_status_t btif_me_ibrt_mode_init(bool enable);
    bt_status_t btif_me_ibrt_role_switch(uint8_t switch_op);

    void btif_me_set_ibrt_mobile_hci_handle(uint16_t hci_handle);
    uint16_t btif_me_get_ibrt_mobile_hci_handle(void);

    void btif_me_set_devctx_state(uint8_t ctx_idx, uint16_t state);
    void btif_me_set_devctx_link(uint8_t ctx_idx, btif_remote_device_t * rm_dev);
    bt_bdaddr_t*  btif_me_get_devctx_btaddr(uint8_t ctx_idx);
    btif_remote_device_t* btif_me_get_remote_device(uint8_t dev_index);
    void btif_me_free_tws_outgoing_dev(uint8_t *peer_tws_addr);
    btif_remote_device_t*  btif_me_get_devctx_remote_device(uint8_t ctx_idx);
    btif_remote_device_t * btif_me_avdev_ctx_get_conn_remote_device(uint8_t dev_idx);

    uint32_t btif_me_save_record_ctx(uint8_t *ctx_buffer, uint8_t *addr);
    uint32_t btif_me_set_record_ctx(uint8_t *ctx_buffer,  uint8_t *addr);
    uint32_t btif_me_save_me_ctx(uint8_t *ctx_buffer, uint16_t dev_id);
    uint32_t btif_me_set_me_ctx(uint8_t *ctx_buffer, uint16_t dev_id);
    uint8_t btif_me_get_mobile_avdev_index(uint16_t mobile_handle);
    uint8_t btif_me_get_free_avdev_index(void);
    uint32_t btif_me_save_avdev_ctx(uint8_t*  ctx_buffer, uint16_t dev_id);
    uint32_t btif_me_set_avdev_ctx(uint8_t *ctx_buffer, uint16_t dev_id, uint8_t rm_devtbl_idx);
    void btif_me_conn_auto_accept_enable(uint8_t *condition);
    uint8_t btif_me_get_callback_event_encty_mode(const btif_event_t * event);
    btif_dev_linkkey *btif_me_get_callback_link_key(const btif_event_t *event);
    void btif_me_register_ibrt_disconnect_callback(btif_callback cb);
    bt_status_t btif_me_set_tws_poll_interval(uint16_t conn_handle, uint16_t poll_interval);
    bt_status_t btif_me_resume_ibrt(uint8_t enable);
    void btif_me_write_scan_activity_specific(uint16_t opcode, uint16_t scan_interval, uint16_t scan_window);
    void btif_me_write_dbg_sniffer(const uint8_t subcode, const uint16_t connhandle);
    void btif_me_ibrt_simu_hci_event_disallow(uint8_t opcode1, uint8_t opcode2);
    void btif_me_register_conn_req_pre_treatment_callback(btif_callback cb);
    void btif_me_register_cmd_status_callback(btif_callback cb);
    void btif_me_register_cmd_complete_callback(btif_callback cb);
    bt_status_t btif_me_ibrt_conn_connected(bt_bdaddr_t *bt_addr, uint16_t conhdl);
    bt_status_t btif_me_ibrt_conn_disconnected(bt_bdaddr_t *bt_addr, uint16_t conhdl, uint8_t status, uint8_t reason);
    void btif_me_register_ibrt_io_capbility_callback(ibrt_io_capbility_callback cb);
    bt_status_t btif_me_auth_req(uint16_t conn_handle);
#endif
#ifdef __cplusplus
}
#endif                          /*  */
#endif                          /* __ME_H */
