/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _AVRCP_API_H
#define _AVRCP_API_H
#include "me_api.h"
#include "avctp_api.h"
#include "bluetooth.h"
#include "plat_types.h"

#define BTIF_AVRCP_MAX_CHAR_SETS                 10

#define BTIF_AVTP_HEADER_LEN 10

typedef btif_avctp_event_t btif_avrcp_event_t;
typedef void *btif_avrcp_chnl_handle_t;

/** The transport layer is connected and commands/responses can now
 *  be exchanged.
 */
#define BTIF_AVRCP_EVENT_CONNECT             BTIF_AVCTP_CONNECT_EVENT

/** The application will receive this event when a lower layer connection
 *  (L2CAP) has been disconnected.  Both the target and controller of the
 *  connection are notified.
 *
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define BTIF_AVRCP_EVENT_DISCONNECT           BTIF_AVCTP_DISCONNECT_EVENT

/** A remote device is attempting to connect the transport layer.
 *  Only the acceptor of the connection is notified.
 */
#define BTIF_AVRCP_EVENT_CONNECT_IND          BTIF_AVCTP_CONNECT_IND_EVENT

/* Group: Events for the exchange of basic AV/C commands that are not routed
 * to the panel subunit.
 */
#define BTIF_AVRCP_EVENT_COMMAND             BTIF_AVCTP_COMMAND_EVENT

/** A AV/C response was received from the remote device (target). This event
 *  is received for responses not routed to the panel subunit.
 */
#define BTIF_AVRCP_EVENT_RESPONSE            BTIF_AVCTP_RESPONSE_EVENT

/** The remote device (target) rejected the AV/C command.  This event is
 *  received for responses not routed to the panel subunit.
 */
#define BTIF_AVRCP_EVENT_REJECT               BTIF_AVCTP_REJECT_EVENT

/** A command (see AVRCP_SendCommand) or response (see AVRCP_SendResponse)
 * has been sent.
 */
#define BTIF_AVRCP_EVENT_TX_DONE             BTIF_AVCTP_TX_DONE_EVENT

/** Additional operand data has been received for the previous
 *  AVRCP_EVENT_COMMAND or AVRCP_EVENT_RESPONSE.
 */
#define BTIF_AVRCP_EVENT_OPERANDS             BTIF_AVCTP_OPERANDS_EVENT

/** An AV/C command has timed out.
 */
#define BTIF_AVRCP_EVENT_CMD_TIMEOUT         ( BTIF_AVCTP_LAST_EVENT + 1)

/* Group: Events for the exchange of standard AV/C panel subunit commands.
 */

#define BTIF_AVRCP_EVENT_PANEL_PRESS        ( BTIF_AVCTP_LAST_EVENT + 2)

/** The key corresponding to a panel operation has been held down on the
 * remote controller device for at least AVRCP_PANEL_PRESSHOLD_TIME.
 */
#define BTIF_AVRCP_EVENT_PANEL_HOLD       ( BTIF_AVCTP_LAST_EVENT + 3)

/** The key corresponding to a panel operation has been released on the
 * remote controller device.
 */
#define BTIF_AVRCP_EVENT_PANEL_RELEASE      ( BTIF_AVCTP_LAST_EVENT + 4)

/** A panel response has been received from the remote target device.
 * All fields of "p.panelCnf" are valid.
 */
#define BTIF_AVRCP_EVENT_PANEL_CNF          ( BTIF_AVCTP_LAST_EVENT + 5)

/* Group: Events for the exchange of Bluetooth specific advanced AV/C
 * commands routed through the panel subunit.
 */

/** Advanced status information has been received from the controller.
 */
#define BTIF_AVRCP_EVENT_ADV_INFO          ( BTIF_AVCTP_LAST_EVENT + 6)

/** An advanced request was received and requires a response by the
 *  application.
 */
#define BTIF_AVRCP_EVENT_ADV_REQ            ( BTIF_AVCTP_LAST_EVENT + 7)

/** The notification of a registered event has been received.
 */
#define BTIF_AVRCP_EVENT_ADV_NOTIFY        ( BTIF_AVCTP_LAST_EVENT + 8)

/** An Advanced command was sent successfully to the target and a response
 *  was received.
 */
#define BTIF_AVRCP_EVENT_ADV_RESPONSE      ( BTIF_AVCTP_LAST_EVENT + 9)

/** No response was received for an Advanced command and it has timed out.
 *  This event is received for advanced commands routed to the panel subunit.
 */
#define BTIF_AVRCP_EVENT_ADV_CMD_TIMEOUT    ( BTIF_AVCTP_LAST_EVENT + 10)

/** An Advanced PDU was sent successfully.
 */
#define BTIF_AVRCP_EVENT_ADV_TX_DONE        ( BTIF_AVCTP_LAST_EVENT + 11)

/* Group: Connection events for establishing and releasing the AVRCP browsing
 * channel.  The browsing channel is used for the exchange of AVCTP-based
 * commands/responses which contain Bluetooth specific media
 * operations.
 */
#define BTIF_AVRCP_EVENT_BROWSE_CONNECT     ( BTIF_AVCTP_LAST_EVENT + 12)

/** The application will receive this event when a lower layer connection
 *  (L2CAP) has been disconnected.  Both the target and controller of the
 *  connection are notified.
 */
#define BTIF_AVRCP_EVENT_BROWSE_DISCONNECT  ( BTIF_AVCTP_LAST_EVENT + 13)

/**  remote device is attempting to connect the browsing channel.
 */
#define BTIF_AVRCP_EVENT_BROWSE_CONNECT_IND ( BTIF_AVCTP_LAST_EVENT + 14)

/* Group: Events for the exchange of browsing channel commands.
 */
#define BTIF_AVRCP_EVENT_BROWSE_REQ        ( BTIF_AVCTP_LAST_EVENT + 15)

#define BTIF_AVRCP_EVENT_BROWSE_INFO       ( BTIF_AVCTP_LAST_EVENT + 16)

#define BTIF_AVRCP_EVENT_BROWSE_RESPONSE    ( BTIF_AVCTP_LAST_EVENT + 17)

#define BTIF_AVRCP_EVENT_BROWSE_CMD_TIMEOUT ( BTIF_AVCTP_LAST_EVENT + 18)

#define BTIF_AVRCP_EVENT_BROWSE_TX_DONE     ( BTIF_AVCTP_LAST_EVENT + 19)

#define BTIF_AVRCP_EVENT_LAST               ( BTIF_AVCTP_LAST_EVENT + 19)

typedef uint16_t avrcp_role_t;

/* Controller: Player/recorder features supported ("play" and "stop") */
#define BTIF_AVRCP_CT_CATEGORY_1       0x0001

/* Controller: Monitor/amplifier features supported ("volume up" and "volume down") */
#define BTIF_AVRCP_CT_CATEGORY_2       0x0002

/* Controller: Tuner features supported ("channel up" and "channel down") */
#define BTIF_AVRCP_CT_CATEGORY_3       0x0004

/* Controller: Menu features supported ("root menu", "up", "down", "left",
 * "right", and "select")
 */
#define BTIF_AVRCP_CT_CATEGORY_4       0x0008

/* Target: Player/recorder features supported ("play" and "stop") */
#define BTIF_AVRCP_TG_CATEGORY_1       0x0100

/* Target: Monitor/amplifier features supported ("volume up" and "volume down") */
#define BTIF_AVRCP_TG_CATEGORY_2       0x0200

/* Target: Tuner features supported ("channel up" and "channel down") */
#define BTIF_AVRCP_TG_CATEGORY_3       0x0400

/* Target: Menu features supported ("root menu", "up", "down", "left",
 * "right", and "select")
 */
#define BTIF_AVRCP_TG_CATEGORY_4       0x0800

/* Target: Player settings supported */
#define BTIF_AVRCP_TG_PLAYER_SETTINGS  0x1000

/* Target: Group navigation supported */
#define BTIF_AVRCP_TG_GROUP_NAV        0x2000

/* Target: Media browsing is supported */
#define BTIF_AVRCP_TG_BROWSING         0x4000

/* Target: Multiple media players */
#define BTIF_AVRCP_TG_MULTIPLE_PLAYERS 0x8000

typedef uint8_t avrcp_ctype_t;

#define BTIF_AVRCP_CTYPE_CONTROL              BTIF_ AVCTP_CTYPE_CONTROL
#define BTIF_AVRCP_CTYPE_STATUS                 BTIF_AVCTP_CTYPE_STATUS
#define BTIF_AVRCP_CTYPE_SPECIFIC_INQUIRY     BTIF_ AVCTP_CTYPE_SPECIFIC_INQUIRY
#define BTIF_AVRCP_CTYPE_NOTIFY                BTIF_AVCTP_CTYPE_NOTIFY
#define BTIF_AVRCP_CTYPE_GENERAL_INQUIRY       BTIF_AVCTP_CTYPE_GENERAL_INQUIRY

#define BTIF_AVRCP_CTYPE_BROWSING              BTIF_AVCTP_CTYPE_BROWSING

typedef avctp_response_t avrcp_response_t;

#define BTIF_AVRCP_RESPONSE_NOT_IMPLEMENTED    BTIF_AVCTP_RESPONSE_NOT_IMPLEMENTED
#define BTIF_AVRCP_RESPONSE_ACCEPTED           AVCTP_RESPONSE_ACCEPTED
#define BTIF_AVRCP_RESPONSE_REJECTED           AVCTP_RESPONSE_REJECTED
#define BTIF_AVRCP_RESPONSE_IN_TRANSITION      AVCTP_RESPONSE_IN_TRANSITION
#define BTIF_AVRCP_RESPONSE_IMPLEMENTED_STABLE AVCTP_RESPONSE_IMPLEMENTED_STABLE
#define BTIF_AVRCP_RESPONSE_CHANGED            AVCTP_RESPONSE_CHANGED
#define BTIF_AVRCP_RESPONSE_INTERIM            AVCTP_RESPONSE_INTERIM

#define BTIF_AVRCP_RESPONSE_BROWSING           AVCTP_RESPONSE_BROWSING

#define BTIF_AVRCP_RESPONSE_SKIPPED            0xF0

#define BTIF_AVRCP_RESPONSE_TIMEOUT            0xF1

typedef uint8_t avrcp_opcode_t;

#define BTIF_AVRCP_OPCODE_VENDOR_DEPENDENT     AVCTP_OPCODE_VENDOR_DEPENDENT
#define BTIF_AVRCP_OPCODE_UNIT_INFO            AVCTP_OPCODE_UNIT_INFO
#define BTIF_AVRCP_OPCODE_SUBUNIT_INFO         AVCTP_OPCODE_SUBUNIT_INFO
#define BTIF_AVRCP_OPCODE_PASS_THROUGH         AVCTP_OPCODE_PASS_THROUGH

typedef avctp_cmdframe_t avrcp_cmdframe_t;
typedef avctp_rspframe_t avrcp_rspframe_t;

typedef uint8_t avrcp_subunit_type_t;

#define BTIF_AVRCP_SUBUNIT_PANEL        0x09
#define BTIF_AVRCP_SUBUNIT_UNIT         0x1F

typedef U16 avrcp_panel_operation_t;

#define BTIF_AVRCP_POP_SELECT            0x0000
#define BTIF_AVRCP_POP_UP                0x0001
#define BTIF_AVRCP_POP_DOWN              0x0002
#define BTIF_AVRCP_POP_LEFT              0x0003
#define BTIF_AVRCP_POP_RIGHT             0x0004
#define BTIF_AVRCP_POP_RIGHT_UP          0x0005
#define BTIF_AVRCP_POP_RIGHT_DOWN        0x0006
#define BTIF_AVRCP_POP_LEFT_UP           0x0007
#define BTIF_AVRCP_POP_LEFT_DOWN         0x0008
#define BTIF_AVRCP_POP_ROOT_MENU         0x0009
#define BTIF_AVRCP_POP_SETUP_MENU        0x000A
#define BTIF_AVRCP_POP_CONTENTS_MENU     0x000B
#define BTIF_AVRCP_POP_FAVORITE_MENU     0x000C
#define BTIF_AVRCP_POP_EXIT              0x000D

#define BTIF_AVRCP_POP_0                 0x0020
#define BTIF_AVRCP_POP_1                 0x0021
#define BTIF_AVRCP_POP_2                 0x0022
#define BTIF_AVRCP_POP_3                 0x0023
#define BTIF_AVRCP_POP_4                 0x0024
#define BTIF_AVRCP_POP_5                 0x0025
#define BTIF_AVRCP_POP_6                 0x0026
#define BTIF_AVRCP_POP_7                 0x0027
#define BTIF_AVRCP_POP_8                 0x0028
#define BTIF_AVRCP_POP_9                 0x0029
#define BTIF_AVRCP_POP_DOT               0x002A
#define BTIF_AVRCP_POP_ENTER             0x002B
#define BTIF_AVRCP_POP_CLEAR             0x002C

#define BTIF_AVRCP_POP_CHANNEL_UP        0x0030
#define BTIF_AVRCP_POP_CHANNEL_DOWN      0x0031
#define BTIF_AVRCP_POP_PREVIOUS_CHANNEL  0x0032
#define BTIF_AVRCP_POP_SOUND_SELECT      0x0033
#define BTIF_AVRCP_POP_INPUT_SELECT      0x0034
#define BTIF_AVRCP_POP_DISPLAY_INFO      0x0035
#define BTIF_AVRCP_POP_HELP              0x0036
#define BTIF_AVRCP_POP_PAGE_UP           0x0037
#define BTIF_AVRCP_POP_PAGE_DOWN         0x0038

#define BTIF_AVRCP_POP_POWER             0x0040
#define BTIF_AVRCP_POP_VOLUME_UP         0x0041
#define BTIF_AVRCP_POP_VOLUME_DOWN       0x0042
#define BTIF_AVRCP_POP_MUTE              0x0043
#define BTIF_AVRCP_POP_PLAY              0x0044
#define BTIF_AVRCP_POP_STOP              0x0045
#define BTIF_AVRCP_POP_PAUSE             0x0046
#define BTIF_AVRCP_POP_RECORD            0x0047
#define BTIF_AVRCP_POP_REWIND            0x0048
#define BTIF_AVRCP_POP_FAST_FORWARD      0x0049
#define BTIF_AVRCP_POP_EJECT             0x004A
#define BTIF_AVRCP_POP_FORWARD           0x004B
#define BTIF_AVRCP_POP_BACKWARD          0x004C

#define BTIF_AVRCP_POP_ANGLE             0x0050
#define BTIF_AVRCP_POP_SUBPICTURE        0x0051

#define BTIF_AVRCP_POP_F1                0x0071
#define BTIF_AVRCP_POP_F2                0x0072
#define BTIF_AVRCP_POP_F3                0x0073
#define BTIF_AVRCP_POP_F4                0x0074
#define BTIF_AVRCP_POP_F5                0x0075

#define BTIF_AVRCP_POP_VENDOR_UNIQUE     0x007E

#define BTIF_AVRCP_POP_NEXT_GROUP        0x017E
#define BTIF_AVRCP_POP_PREV_GROUP        0x027E

#define BTIF_AVRCP_POP_RESERVED          0x007F

/*---------------------------------------------------------------------------
 * avrcp operation code
 */
typedef U8 btif_avrcp_operation_t;

#define BTIF_AVRCP_OP_GET_CAPABILITIES               0x10

#define BTIF_AVRCP_OP_LIST_PLAYER_SETTING_ATTRIBS    0x11

#define BTIF_AVRCP_OP_LIST_PLAYER_SETTING_VALUES     0x12

#define BTIF_AVRCP_OP_GET_PLAYER_SETTING_VALUE       0x13

#define BTIF_AVRCP_OP_SET_PLAYER_SETTING_VALUE       0x14

#define BTIF_AVRCP_OP_GET_PLAYER_SETTING_ATTR_TEXT   0x15

#define BTIF_AVRCP_OP_GET_PLAYER_SETTING_VALUE_TEXT  0x16

#define BTIF_AVRCP_OP_INFORM_DISP_CHAR_SET           0x17

#define BTIF_AVRCP_OP_INFORM_BATT_STATUS             0x18

#define BTIF_AVRCP_OP_GET_MEDIA_INFO                 0x20

#define BTIF_AVRCP_OP_GET_PLAY_STATUS                0x30

#define BTIF_AVRCP_OP_REGISTER_NOTIFY                0x31

#define BTIF_AVRCP_OP_REQUEST_CONT_RESP              0x40

#define BTIF_AVRCP_OP_ABORT_CONT_RESP                0x41

#define BTIF_AVRCP_OP_SET_ABSOLUTE_VOLUME            0x50

#define BTIF_AVRCP_OP_SET_ADDRESSED_PLAYER           0x60

#define BTIF_AVRCP_OP_SET_BROWSED_PLAYER             0x70

#define BTIF_AVRCP_OP_GET_FOLDER_ITEMS               0x71

#define BTIF_AVRCP_OP_CHANGE_PATH                    0x72

#define BTIF_AVRCP_OP_GET_ITEM_ATTRIBUTES            0x73

#define BTIF_AVRCP_OP_PLAY_ITEM                      0x74

#define BTIF_AVRCP_OP_SEARCH                         0x80

#define BTIF_AVRCP_OP_ADD_TO_NOW_PLAYING             0x90

#define BTIF_AVRCP_OP_GENERAL_REJECT                 0xA0

#define BTIF_AVRCP_OP_CUSTOM_CMD                        0xF0

typedef uint8_t avrcp_error_code_t;

#define BTIF_AVRCP_INVALID_CMD_ERR         0x00
#define BTIF_AVRCP_INVALID_PARM_ERR        0x01
#define BTIF_AVRCP_PARM_NOT_FOUND_ERR      0x02
#define BTIF_AVRCP_INTERNAL_ERROR_ERR      0x03
#define BTIF_AVRCP_NO_ERROR_ERR            0x04
#define BTIF_AVRCP_UIDS_CHANGED_ERR        0x05
#define BTIF_AVRCP_ERR_UNKNOWN_ERROR       0x06
#define BTIF_AVRCP_ERR_INVALID_DIRECTION   0x07
#define BTIF_AVRCP_ERR_NON_DIRECTORY       0x08
#define BTIF_AVRCP_ERR_DOES_NOT_EXIST      0x09
#define BTIF_AVRCP_ERR_INVALID_SCOPE       0x0A
#define BTIF_AVRCP_ERR_OUT_OF_BOUNDS       0x0B
#define BTIF_AVRCP_ERR_IS_DIRECTORY        0x0C
#define BTIF_AVRCP_ERR_MEDIA_IN_USE        0x0D
#define BTIF_AVRCP_ERR_NOW_PLAYING_FULL    0x0E
#define BTIF_AVRCP_ERR_NO_SEARCH_SUPPORT   0x0F
#define BTIF_AVRCP_ERR_SEARCH_IN_PROGRESS  0x10
#define BTIF_AVRCP_ERR_INVALID_PLAYER_ID   0x11
#define BTIF_AVRCP_ERR_NOT_BROWSABLE       0x12
#define BTIF_AVRCP_ERR_NOT_ADDRESSED       0x13
#define BTIF_AVRCP_ERR_NO_SEARCH_RESULTS   0x14
#define BTIF_AVRCP_ERR_NO_AVAIL_PLAYERS    0x15
#define BTIF_AVRCP_ERR_ADDR_PLAYER_CHANGED 0x16

/*---------------------------------------------------------------------------
 *  event Id
 */
typedef uint8_t avrcp_event_Id_t;

#define BTIF_AVRCP_EID_MEDIA_STATUS_CHANGED        0x01 /* Change in media status */

#define BTIF_AVRCP_EID_TRACK_CHANGED               0x02 /* Current track changed */

#define BTIF_AVRCP_EID_TRACK_END                   0x03 /* Reached end of track */

#define BTIF_AVRCP_EID_TRACK_START                 0x04 /* Reached track start */

#define BTIF_AVRCP_EID_PLAY_POS_CHANGED            0x05 /* Change in playback position.
                                                         * Returned after the specified
                                                         * playback notification change
                                                         * notification interval.
                                                         */

#define BTIF_AVRCP_EID_BATT_STATUS_CHANGED         0x06 /* Change in battery status   */
#define BTIF_AVRCP_EID_SYS_STATUS_CHANGED          0x07 /* Change in system status    */
#define BTIF_AVRCP_EID_APP_SETTING_CHANGED         0x08 /* Change in player
                                                         * application setting
                                                         */

#define BTIF_AVRCP_EID_NOW_PLAYING_CONTENT_CHANGED 0x09 /* Contents of the now playing
                                                         * list have changed
                                                         */

#define BTIF_AVRCP_EID_AVAILABLE_PLAYERS_CHANGED   0x0A /* The available players have
                                                         * changed
                                                         */

#define BTIF_AVRCP_EID_ADDRESSED_PLAYER_CHANGED    0x0B /* The addressed player has
                                                         * changed
                                                         */

#define BTIF_AVRCP_EID_UIDS_CHANGED                0x0C /* The UIDS have changed */

#define BTIF_AVRCP_EID_VOLUME_CHANGED              0x0D /* The volume was changed */

#define BTIF_AVRCP_EID_FLAG_INTERIM                0x80 /* Used Internally */

/*---------------------------------------------------------------------------
 * even mask
 */
typedef uint16_t avrcp_event_mask_t;

#define BTIF_AVRCP_ENABLE_PLAY_STATUS_CHANGED      0x0001   /* Change in playback
                                                             * status
                                                             */
#define BTIF_AVRCP_ENABLE_MEDIA_STATUS_CHANGED     0x0001   /* Alias */
#define BTIF_AVRCP_ENABLE_TRACK_CHANGED            0x0002   /* Current track changed */
#define BTIF_AVRCP_ENABLE_TRACK_END                0x0004   /* Reached end of track  */
#define BTIF_AVRCP_ENABLE_TRACK_START              0x0008   /* Reached track start   */
#define BTIF_AVRCP_ENABLE_PLAY_POS_CHANGED         0x0010   /* Change in playback
                                                             * position
                                                             */
#define BTIF_AVRCP_ENABLE_BATT_STATUS_CHANGED      0x0020   /* Change in battery
                                                             * status
                                                             */
#define BTIF_AVRCP_ENABLE_SYS_STATUS_CHANGED       0x0040   /* Change in system status */
#define BTIF_AVRCP_ENABLE_APP_SETTING_CHANGED      0x0080   /* Change in player
                                                             * application setting
                                                             */

#define BTIF_AVRCP_ENABLE_NOW_PLAYING_CHANGED      0x0100   /* Change in the now
                                                             * playing list
                                                             */
#define BTIF_AVRCP_ENABLE_AVAIL_PLAYERS_CHANGED    0x0200   /* Available players
                                                             * changed
                                                             */
#define BTIF_AVRCP_ENABLE_ADDRESSED_PLAYER_CHANGED 0x0400   /* Addressed player changed */
#define BTIF_AVRCP_ENABLE_UIDS_CHANGED             0x0800   /* UIDS changed */
#define BTIF_AVRCP_ENABLE_VOLUME_CHANGED           0x1000   /* Volume Changed */

typedef uint8_t btif_avrcp_capabilityId;

#define BTIF_AVRCP_CAPABILITY_COMPANY_ID        2
#define BTIF_AVRCP_CAPABILITY_EVENTS_SUPPORTED  3

/*---------------------------------------------------------------------------
 * player attrId  type
 */
typedef uint8_t avrcp_playerAttrId;

#define BTIF_AVRCP_PLAYER_EQ_STATUS        1    /* Player equalizer status */
#define BTIF_AVRCP_PLAYER_REPEAT_STATUS    2    /* Player repeat status */
#define BTIF_AVRCP_PLAYER_SHUFFLE_STATUS   3    /* Player Shuffle status */
#define BTIF_AVRCP_PLAYER_SCAN_STATUS      4    /* Player scan status */

/*---------------------------------------------------------------------------
 *  playerattrId mask type
 */
typedef uint16_t avrcp_player_attrId_mask_t;

#define BTIF_AVRCP_ENABLE_PLAYER_EQ_STATUS       0x0001
#define BTIF_AVRCP_ENABLE_PLAYER_REPEAT_STATUS   0x0002
#define BTIF_AVRCP_ENABLE_PLAYER_SHUFFLE_STATUS  0x0004
#define BTIF_AVRCP_ENABLE_PLAYER_SCAN_STATUS     0x0008
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_5  0x0010
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_6  0x0020
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_7  0x0040
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_8  0x0080
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_9  0x0100
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_10 0x0200
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_11 0x0400
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_12 0x0800
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_13 0x1000
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_14 0x2000
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_15 0x4000
#define BTIF_AVRCP_ENABLE_PLAYER_EXTENSION_ID_16 0x8000

#ifndef BTIF_AVRCP_MAX_FOLDER_DEPTH
#define BTIF_AVRCP_MAX_FOLDER_DEPTH 10
#endif

typedef uint8_t avrcp_eq_value_t;

#define BTIF_AVRCP_EQ_OFF  1
#define BTIF_AVRCP_EQ_ON   2

typedef uint8_t avrcp_repeat_value_t;

#define BTIF_AVRCP_REPEAT_OFF     1
#define BTIF_AVRCP_REPEAT_SINGLE  2
#define BTIF_AVRCP_REPEAT_ALL     3
#define BTIF_AVRCP_REPEAT_GROUP   4

typedef uint8_t avrcp_shuffle_value_t;

#define BTIF_AVRCP_SHUFFLE_OFF    1
#define BTIF_AVRCP_SHUFFLE_ALL    2
#define BTIF_AVRCP_SHUFFLE_GROUP  3

typedef uint8_t avrcp_scan_value_t;

#define BTIF_AVRCP_SCAN_OFF    1
#define BTIF_AVRCP_SCAN_ALL    2
#define BTIF_AVRCP_SCAN_GROUP  3

typedef uint8_t avrcp_ext_value_t;

typedef uint8_t avrcp_battery_status_t;

#define BTIF_AVRCP_BATT_STATUS_NORMAL      0
#define BTIF_AVRCP_BATT_STATUS_WARNING     1
#define BTIF_AVRCP_BATT_STATUS_CRITICAL    2
#define BTIF_AVRCP_BATT_STATUS_EXTERNAL    3
#define BTIF_AVRCP_BATT_STATUS_FULL_CHARGE 4

typedef uint8_t avrcp_system_status_t;

#define BTIF_AVRCP_SYS_POWER_ON   0
#define BTIF_AVRCP_SYS_POWER_OFF  1
#define BTIF_AVRCP_SYS_UNPLUGGED  2

typedef uint32_t avrcp_media_attrId_t;

#define BTIF_AVRCP_MEDIA_ATTR_TITLE       0x00000001
#define BTIF_AVRCP_MEDIA_ATTR_ARTIST      0x00000002
#define BTIF_AVRCP_MEDIA_ATTR_ALBUM       0x00000003
#define BTIF_AVRCP_MEDIA_ATTR_TRACK       0x00000004
#define BTIF_AVRCP_MEDIA_ATTR_NUM_TRACKS  0x00000005
#define BTIF_AVRCP_MEDIA_ATTR_GENRE       0x00000006
#define BTIF_AVRCP_MEDIA_ATTR_DURATION    0x00000007

typedef uint32_t avrcp_media_attrId_mask_t;

#define BTIF_AVRCP_ENABLE_MEDIA_ATTR_TITLE       0x01
#define BTIF_AVRCP_ENABLE_MEDIA_ATTR_ARTIST      0x02
#define BTIF_AVRCP_ENABLE_MEDIA_ATTR_ALBUM       0x04
#define BTIF_AVRCP_ENABLE_MEDIA_ATTR_TRACK       0x08
#define BTIF_AVRCP_ENABLE_MEDIA_ATTR_NUM_TRACKS  0x10
#define BTIF_AVRCP_ENABLE_MEDIA_ATTR_GENRE       0x20
#define BTIF_AVRCP_ENABLE_MEDIA_ATTR_DURATION    0x40

typedef uint8_t avrcp_media_status_t;

#define BTIF_AVRCP_MEDIA_STOPPED       0x00
#define BTIF_AVRCP_MEDIA_PLAYING       0x01
#define BTIF_AVRCP_MEDIA_PAUSED        0x02
#define BTIF_AVRCP_MEDIA_FWD_SEEK      0x03
#define BTIF_AVRCP_MEDIA_REV_SEEK      0x04
#define BTIF_AVRCP_MEDIA_ERROR         0xFF

typedef uint16_t avrcp_version_t;

/* Unable to determine the Hands Free Profile version that is supported */
#define BTIF_AVRCP_VERSION_UNKNOWN 0x0000

/* Supports Version 1.0 of the AVRCP Profile */
#define BTIF_AVRCP_VERSION_1_0     0x0100

/* Supports Version 1.3 of the AVRCP Profile */
#define BTIF_AVRCP_VERSION_1_3     0x0103

/* Supports Version 1.4 of the AVRCP Profile */
#define BTIF_AVRCP_VERSION_1_4     0x0104

typedef uint16_t avrcp_features_t;

/* Player/recorder features supported ("play" and "stop") */
#define BTIF_AVRCP_FEATURES_CATEGORY_1       0x0001

/* Monitor/amplifier features supported ("volume up" and "volume down") */
#define BTIF_AVRCP_FEATURES_CATEGORY_2       0x0002

/* Tuner features supported ("channel up" and "channel down") */
#define BTIF_AVRCP_FEATURES_CATEGORY_3       0x0004

/* Menu features supported ("root menu", "up", "down", "left", "right", and
 * "select")
 */
#define BTIF_AVRCP_FEATURES_CATEGORY_4       0x0008

/* Player settings supported */
#define BTIF_AVRCP_FEATURES_PLAYER_SETTINGS  0x0010

/* Group navigation supported */
#define BTIF_AVRCP_FEATURES_GROUP_NAV        0x0020

#define BTIF_AVRCP_FEATURES_BROWSING         0x0040

#define BTIF_AVRCP_FEATURES_MULTIPLE_PLAYERS 0x0080

typedef uint8_t avrcp_maj_player_type_t;

#define BTIF_AVRCP_MAJ_PLAYER_AUDIO      0x01
#define BTIF_AVRCP_MAJ_PLAYER_VIDEO      0x02
#define BTIF_AVRCP_MAJ_PLAYER_AUD_BCAST  0x04
#define BTIF_AVRCP_MAJ_PLAYER_VID_BCAST  0x08

typedef uint32_t avrcp_player_sub_type_t;

#define BTIF_AVRCP_PLAYER_SUB_AUDIO_BOOK  0x00000001
#define BTIF_AVRCP_PLAYER_SUB_PODCAST     0x00000002

typedef uint8_t avrcp_scope_t;

#define BTIF_AVRCP_SCOPE_MEDIA_PLAYER_LIST  0x00
#define BTIF_AVRCP_SCOPE_VIRTUAL_FILESYS    0x01
#define BTIF_AVRCP_SCOPE_SEARCH             0x02
#define BTIF_AVRCP_SCOPE_NOW_PLAYING        0x03

typedef uint8_t avrcp_chg_path_direction_t;

#define BTIF_AVRCP_DIR_UP                   0x00
#define BTIF_AVRCP_DIR_DOWN                 0x01

typedef uint8_t avrcp_plyr_featr_byte_t;

/* Byte 0 */

#define BTIF_AVRCP_BRWS_0_FEAT_SELECT              0x01
#define BTIF_AVRCP_BRWS_0_FEAT_UP                  0x02
#define BTIF_AVRCP_BRWS_0_FEAT_DOWN                0x04
#define BTIF_AVRCP_BRWS_0_FEAT_LEFT                0x08
#define BTIF_AVRCP_BRWS_0_FEAT_RIGHT               0x10
#define BTIF_AVRCP_BRWS_0_FEAT_RIGHT_UP            0x20
#define BTIF_AVRCP_BRWS_0_FEAT_RIGHT_DOWN          0x40
#define BTIF_AVRCP_BRWS_0_FEAT_LEFT_UP             0x80

/* Byte 1 */

#define BTIF_AVRCP_BRWS_1_FEAT_LEFT_DOWN           0x01
#define BTIF_AVRCP_BRWS_1_FEAT_ROOT_MENU           0x02
#define BTIF_AVRCP_BRWS_1_FEAT_SETUP_MENU          0x04
#define BTIF_AVRCP_BRWS_1_FEAT_CONTENTS_MENU       0x08
#define BTIF_AVRCP_BRWS_1_FEAT_FAVORITE_MENU       0x10
#define BTIF_AVRCP_BRWS_1_FEAT_EXIT                0x20
#define BTIF_AVRCP_BRWS_1_FEAT_0                   0x40
#define BTIF_AVRCP_BRWS_1_FEAT_1                   0x80

/* Byte 2 */

#define BTIF_AVRCP_BRWS_2_FEAT_2                   0x01
#define BTIF_AVRCP_BRWS_2_FEAT_3                   0x02
#define BTIF_AVRCP_BRWS_2_FEAT_4                   0x04
#define BTIF_AVRCP_BRWS_2_FEAT_5                   0x08
#define BTIF_AVRCP_BRWS_2_FEAT_6                   0x10
#define BTIF_AVRCP_BRWS_2_FEAT_7                   0x20
#define BTIF_AVRCP_BRWS_2_FEAT_8                   0x40
#define BTIF_AVRCP_BRWS_2_FEAT_9                   0x80

/* Byte 3 */

#define BTIF_AVRCP_BRWS_3_FEAT_DOT                 0x01
#define BTIF_AVRCP_BRWS_3_FEAT_ENTER               0x02
#define BTIF_AVRCP_BRWS_3_FEAT_CLEAR               0x04
#define BTIF_AVRCP_BRWS_3_FEAT_CHNL_UP             0x08
#define BTIF_AVRCP_BRWS_3_FEAT_CHNL_DOWN           0x10
#define BTIF_AVRCP_BRWS_3_FEAT_PREV_CHNL           0x20
#define BTIF_AVRCP_BRWS_3_FEAT_SOUND_SEL           0x40
#define BTIF_AVRCP_BRWS_3_FEAT_INPUT_SEL           0x80

/* Byte 4 */

#define BTIF_AVRCP_BRWS_3_FEAT_DISPLAY_INFO        0x01
#define BTIF_AVRCP_BRWS_4_FEAT_HELP                0x02
#define BTIF_AVRCP_BRWS_4_FEAT_PG_UP               0x04
#define BTIF_AVRCP_BRWS_4_FEAT_PG_DOWN             0x08
#define BTIF_AVRCP_BRWS_4_FEAT_POWER               0x10
#define BTIF_AVRCP_BRWS_4_FEAT_VOL_UP              0x20
#define BTIF_AVRCP_BRWS_4_FEAT_VOL_DOWN            0x40
#define BTIF_AVRCP_BRWS_4_FEAT_MUTE                0x80

/* Byte 5 */

#define BTIF_AVRCP_BRWS_5_FEAT_PLAY                0x01
#define BTIF_AVRCP_BRWS_5_FEAT_STOP                0x02
#define BTIF_AVRCP_BRWS_5_FEAT_PAUSE               0x04
#define BTIF_AVRCP_BRWS_5_FEAT_RECORD              0x08
#define BTIF_AVRCP_BRWS_5_FEAT_REWIND              0x10
#define BTIF_AVRCP_BRWS_5_FEAT_FF                  0x20
#define BTIF_AVRCP_BRWS_5_FEAT_EJECT               0x40
#define BTIF_AVRCP_BRWS_5_FEAT_FORWARD             0x80

/* Byte 6 */

#define BTIF_AVRCP_BRWS_6_FEAT_BACKWARD            0x01
#define BTIF_AVRCP_BRWS_6_FEAT_ANGLE               0x02
#define BTIF_AVRCP_BRWS_6_FEAT_SUBPIC              0x04
#define BTIF_AVRCP_BRWS_6_FEAT_F1                  0x08
#define BTIF_AVRCP_BRWS_6_FEAT_F2                  0x10
#define BTIF_AVRCP_BRWS_6_FEAT_F3                  0x20
#define BTIF_AVRCP_BRWS_6_FEAT_F4                  0x40
#define BTIF_AVRCP_BRWS_6_FEAT_F5                  0x80

/* Byte 7 */

#define BTIF_AVRCP_BRWS_7_FEAT_VENDOR_UNIQUE       0x01
#define BTIF_AVRCP_BRWS_7_FEAT_GROUP_NAV           0x02
#define BTIF_AVRCP_BRWS_7_FEAT_ADVANCED_CTRL       0x04
#define BTIF_AVRCP_BRWS_7_FEAT_BROWSING            0x08
#define BTIF_AVRCP_BRWS_7_FEAT_SEARCHING           0x10
#define BTIF_AVRCP_BRWS_7_FEAT_ADD_TO_PLAYING      0x20
#define BTIF_AVRCP_BRWS_7_FEAT_UIDS_UNIQUE         0x40
#define BTIF_AVRCP_BRWS_7_FEAT_BRWS_WHEN_ADDRESSED 0x80

/* Byte 8 */

#define BTIF_AVRCP_BRWS_8_SEARCH_WHEN_ADDRESSED    0x01
#define BTIF_AVRCP_BRWS_8_NOW_PLAYING              0x02
#define BTIF_AVRCP_BRWS_8_UID_PERSISTENT           0x04

typedef uint8_t avrcp_item_type_t;

#define BTIF_AVRCP_ITEM_MEDIA_PLAYER  0x01
#define BTIF_AVRCP_ITEM_FOLDER        0x02
#define BTIF_AVRCP_ITEM_MEDIA_ELEMENT 0x03

typedef uint8_t avrcp_folder_type_t;

#define BTIF_AVRCP_FOLDER_TYPE_MIXED     0x00
#define BTIF_AVRCP_FOLDER_TYPE_TITLES    0x01
#define BTIF_AVRCP_FOLDER_TYPE_ALBUMS    0x02
#define BTIF_AVRCP_FOLDER_TYPE_ARTISTS   0x03
#define BTIF_AVRCP_FOLDER_TYPE_GENRES    0x04
#define BTIF_AVRCP_FOLDER_TYPE_PLAYLISTS 0x05
#define BTIF_AVRCP_FOLDER_TYPE_YEARS     0x06

typedef uint8_t avrcp_is_playable_t;
typedef uint16_t avrcp_capabilityId_t;
typedef void avrcp_advanced_pdu_t;

#define BTIF_AVRCP_IS_NOT_PLAYABLE  0x00
#define BTIF_AVRCP_IS_PLAYABLE      0x01

typedef uint8_t avrcp_media_type_t;

#define BTIF_AVRCP_MEDIA_TYPE_AUDIO 0x00
#define BTIF_AVRCP_MEIDA_TYPE_VIDEO 0x01

#define BTIF_AVCTP_RESPONSE_INTERIM            0x0F

typedef struct {

    /* Index 0 == Off, Index 1 == On */
    const char *string[2];
    uint8_t len[2];
} avrcp_eq_string_t;

typedef struct {

    /* Index 0 == Off, Index 1 == Single, Index 2 == All, Index 3 == Group */
    const char *string[4];
    uint8_t len[4];
} avrcp_repeat_string_t;

typedef struct {

    /* Index 0 == Off, Index 1 == All, Index 2 == Group */
    const char *string[3];
    uint8_t len[3];
} avrcp_shuffle_string_t;

typedef struct {

    /* Index 0 == Off, Index 1 == All, Index 2 == Group */
    const char *string[3];
    uint8_t len[3];
} avrcp_scan_string_t;

#ifndef BTIF_AVRCP_MAX_PLAYER_STRINGS
#define BTIF_AVRCP_MAX_PLAYER_STRINGS  5
#endif
/*--------------------------------------------------------------------------
 * NUM  PLAYER SETTINGS constant
 */

#define BTIF_AVRCP_PANEL_STATE_NONE       0

#define BTIF_AVRCP_PANEL_STATE_C_IDLE     1
#define BTIF_AVRCP_PANEL_STATE_C_PRESS    2
#define BTIF_AVRCP_PANEL_STATE_C_PRESS_R  3
#define BTIF_AVRCP_PANEL_STATE_C_RELEASE  4
#define BTIF_AVRCP_PANEL_STATE_C_SKIP     5

#define BTIF_AVRCP_PANEL_STATE_T_IDLE     101
#define BTIF_AVRCP_PANEL_STATE_T_PRESS    102
#define BTIF_AVRCP_PANEL_STATE_T_HOLD     103

typedef struct {

    /* Number of extended player settings */
    uint8_t numStrings;
    const char *string[BTIF_AVRCP_MAX_PLAYER_STRINGS];  /* The player setting string */
    uint8_t len[BTIF_AVRCP_MAX_PLAYER_STRINGS]; /* The player setting string length */
} avrcp_ext_string_t;
typedef uint8_t avrcp_player_attrId_t;

typedef struct {
    avrcp_player_attrId_t attrId;   /* Media Player Attribute ID */
    union {
        avrcp_eq_value_t eq;
        avrcp_repeat_value_t repeat;
        avrcp_shuffle_value_t shuffle;
        avrcp_scan_value_t scan;
        avrcp_ext_value_t extValue;
        uint8_t value;          /* Used Internally */
    } setting;
} avrcp_player_setting_t;

typedef struct {
    const char *attrString;     /* Media Player Attribute Text */
    uint8_t attrLen;            /* Length of the Attribute Text */
    union {
        avrcp_eq_string_t eq;   /* Equalizer status */
        avrcp_repeat_string_t repeat;   /* Repeat mode status */
        avrcp_shuffle_string_t shuffle; /* Shuffle mode status */
        avrcp_scan_string_t scan;   /* Scan mode status */
        avrcp_ext_string_t ext; /* Extended settings status */
    } setting;
} avrcp_player_strings_t;

typedef struct {
    avrcp_media_attrId_t attrId;    /* Media element attribute ID */
    const char *string;         /* The media element */
    uint16_t length;            /* Length of the media element */
} avrcp_media_info_t;

typedef struct {
    uint8_t numCharSets;
    uint16_t charSets[BTIF_AVRCP_MAX_CHAR_SETS];
} avrcp_char_sets_t;

typedef struct {
    uint32_t length;

    uint32_t position;
    avrcp_media_status_t mediaStatus;
} avrcp_media_play_status_t;

typedef struct {
    uint16_t nameLen;
    const char *name;
} avrcp_folder_t;

typedef struct {

    /* Used Internally */
    list_entry_t node;

    /* Used Internally */
    uint16_t playerId;

    /* Major Player Type */
    avrcp_maj_player_type_t majorType;

    /* Player Sub Type */
    avrcp_player_sub_type_t subType;

    /* Current media play status */
    avrcp_media_status_t mediaStatus;

    /* Player feature bitmask */
    avrcp_plyr_featr_byte_t features[16];

    /* Displayable character set */
    uint16_t charSet;

    /* Player name length */
    uint16_t nameLen;

    /* Player name */
    const char *name;

    /* Folder depth */
    uint16_t folderDepth;

    /* Current folder */
    avrcp_folder_t folder[BTIF_AVRCP_MAX_FOLDER_DEPTH];

    /* Number of items in the current folder */
    uint32_t numItems;
} avrcp_media_player_item_t;
typedef uint16_t avrcp_event_mask_t;
typedef uint8_t avrcp_operation_t;

typedef struct _AvrcpFolderItem {

    /* The UID of this media element */
    uint8_t uid[8];

    /* Folder Type */
    avrcp_folder_type_t folderType;

    /* Is Playable Flag */
    avrcp_is_playable_t isPlayable;

    /* Character Set */
    uint16_t charSet;

    /* The name length of this media element */
    uint16_t nameLen;

    /* The name of this media element */
    uint8_t *name;
} avrcp_folder_item_t;

typedef struct {

    /* Attribute ID */
    avrcp_media_attrId_t attrId;

    /* Character Set */
    uint16_t charSet;

    /* Attribute Length */
    uint16_t valueLen;

    /* Attribute Name */
    uint8_t *value;
} avrcp_media_attribute_t;

typedef struct {

    /* The UID of this media element */
    uint8_t uid[8];

    /* Media type (audio or video) */
    avrcp_media_type_t mediaType;

    /* Character Set */
    uint16_t charSet;

    /* The name length of this media element */
    uint16_t nameLen;

    /* The name of this media element */
    uint8_t *name;

    /* Number of media attributes */
    uint8_t numAttrs;

    /* Media Attributes */
    avrcp_media_attribute_t attrs[BTIF_AVRCP_NUM_MEDIA_ATTRIBUTES];
} avrcp_media_element_item_t;

typedef uint8_t avrcp_Item_type_t;

typedef struct {

    /* Media Item Type */
    avrcp_Item_type_t itemType;

    /* The Media Item */
    union {
        avrcp_media_player_item_t player;
        avrcp_folder_item_t folder;
        avrcp_media_element_item_t element;
    } item;
} avrcp_media_item_t;

typedef struct {

    /* The most significant 32 bits of the track index information.  */
    uint32_t msU32;

    /* The least significant 32 bits of the track index information.  */
    uint32_t lsU32;
} avrcp_track_struct_t;

typedef struct {

    /* Transaction ID of the request */
    uint32_t transId;
    union {

        /* Contains an item to be played on the addressed media player.  This
         * is valid when "advOp" is set to AVRCP_OP_PLAY_ITEM or
         * AVRCP_OP_ADD_TO_NOW_PLAYING.
         */
        struct {
            avrcp_scope_t scope;
            uint8_t uid[8];
            uint16_t uidCounter;
        } item;
    } p;
} avrcp_adv_req_parms_t;

typedef union {

    /* The value of each player setting.  This is valid when "advOp" is set
     * to AVRCP_OP_SET_PLAYER_SETTING_VALUE.
     */
    avrcp_player_setting_t playerSetting;
    struct {

        /* Bitmask that describes which
         * attributes are being reported
         */
        avrcp_player_attrId_mask_t attrMask;

        /* The equalizer setting. */
        avrcp_eq_value_t eq;

        /* The repeat setting. */
        avrcp_repeat_value_t repeat;

        /* The shuffle setting. */
        avrcp_shuffle_value_t shuffle;

        /* The scan setting. */
        avrcp_scan_value_t scan;

#if BTIF_AVRCP_NUM_PLAYER_SETTINGS > 4
        /* Extended player setting. */
        avrcp_ext_value_t extValue[BTIF_AVRCP_NUM_PLAYER_SETTINGS - 4];

#endif                          /*  */
    } setting;

    /* The list of character sets displayable on the controller.
     * This is valid when "advOp" is set to AVRCP_OP_INFORM_DISP_CHAR_SET. The
     * default is 106 (UTF-8).
     */
    avrcp_char_sets_t charSet;

    /* The battery status of the controller.
     * This is valid when "advOp" is set to AVRCP_OP_INFORM_BATT_STATUS.
     */
    avrcp_battery_status_t battStatus;

#if BTIF_AVRCP_VERSION_1_3_ONLY == BTIF_DISABLED
    /* The Absolute Volume
     * This is valid when "advOp" is set to AVRCP_OP_SET_ABSOLUTE_VOLUME.
     */
    uint8_t volume;

    /* The addressed player
     * This is valid when "advOp" is set to AVRCP_OP_SET_ADDRESSED_PLAYER.
     */
    avrcp_media_player_item_t *addrPlayer;

#endif                          /* BTIF_AVRCP_VERSION_1_3_ONLY == BTIF_DISABLED */
} avrcp_adv_inf_parms_t;

typedef struct {
    avrcp_player_attrId_t attrId;
    uint16_t charSet;
    uint8_t length;
    const char *string;
} avrcp_player_attrib_t;

typedef struct {
    uint8_t valueId;
    uint16_t charSet;
    uint8_t length;
    const char *string;
} avrcp_player_settings_t;

typedef struct {
    U32 attrId;
    uint16_t charSet;
    uint16_t length;
    const char *string;
} avrcp_media_attr_t;
typedef uint16_t avrcp_player_attrId_mask_t;

typedef union {

    /* The capabilities of the target.
     * This is valid when "advOp" is set to AVRCP_OP_GET_CAPABILITIES.
     */
    struct {

        /* The type of capability. */
        uint8_t type;

        /* The capability info. */
        union {

            /* The list of company IDs.
             * (type == AVRCP_CAPABILITY_COMPANY_ID)
             */
            struct {

                /* The number of supported company IDs. */
                uint8_t numIds;

                /* An array of company IDs (3 bytes each). */
                uint8_t *ids;
            } companyId;

            /* A bitmask of the supported events.
             * (type == AVRCP_CAPABILITY_EVENTS_SUPPORTED)
             */
            uint16_t eventMask;
        } info;
    } capability;

    /* A bitmask that indicates the player settings supported by
     * the target device.
     * This is valid when "advOp" is set to
     * AVRCP_OP_LIST_PLAYER_SETTING_ATTRIBS.
     */
    uint16_t attrMask;

    /* The list of values for the requested attribute on the target
     * device.
     * This is valid when "advOp" is set to AVRCP_OP_LIST_PLAYER_SETTING_VALUES.
     */
    struct {

        /* The number of supported value IDs. */
        uint8_t numIds;

        /* An array of value ID (1 byte each). */
        uint8_t *ids;
    } attrValues;

    /* The text of each player setting attribute.
     * This is valid when "advOp" is set to
     * AVRCP_OP_GET_PLAYER_SETTING_ATTR_TEXT
     */
    struct {

        /* The number of attribute IDs returned. */
        uint8_t numIds;

        /* An array of attribute text information */
        avrcp_player_attrib_t txt[BTIF_AVRCP_NUM_PLAYER_SETTINGS];
    } attrStrings;

    /* The value of each player setting.
     * This is valid when "advOp" is set to AVRCP_OP_GET_PLAYER_SETTING_VALUE.
     */
    struct {

        /* Bitmask that describes which
         * attributes are being reported
         */
        avrcp_player_attrId_mask_t attrMask;

        /* The equalizer setting. */
        avrcp_eq_value_t eq;

        /* The repeat setting. */
        avrcp_repeat_value_t repeat;

        /* The shuffle setting. */
        avrcp_shuffle_value_t shuffle;

        /* The scan setting. */
        avrcp_scan_value_t scan;

#if BTIF_AVRCP_NUM_PLAYER_SETTINGS > 4
        /* Extended player setting. */
        avrcp_ext_value_t extValue[BTIF_AVRCP_NUM_PLAYER_SETTINGS - 4];

#endif                          /*  */
    } setting;

    /* The list of setting values for the requested attribute on the target.
     * This is valid when "advOp" is set to
     * AVRCP_OP_GET_PLAYER_SETTING_VALUE_TEXT.
     */
    struct {

        /* The number of settings returned */
        uint8_t numSettings;

        /* An array of value text information */
        avrcp_player_settings_t txt[BTIF_AVRCP_NUM_PLAYER_SETTINGS];
    } settingStrings;

    /* The list of element values for the current track on the
     * target.  This is valid when "advOp" is set to AVRCP_OP_GET_MEDIA_INFO.
     */
    struct {

        /* The number of elements returned */
        uint8_t numIds;

        /* An array of element value text information */
        avrcp_media_attr_t txt[BTIF_AVRCP_NUM_MEDIA_ATTRIBUTES];
    } element;

#if BTIF_AVRCP_VERSION_1_3_ONLY == BTIF_DISABLED
    /* The Absolute Volume
     * This is valid when "advOp" is set to AVRCP_OP_SET_ABSOLUTE_VOLUME.
     */
    uint8_t volume;

#endif                          /* BTIF_AVRCP_VERSION_1_3_ONLY == BTIF_DISABLED */
} avrcp_adv_rsp_parms_t;

typedef struct {
    uint32_t length;
    uint32_t position;
    uint8_t mediaStatus;
} adv_play_status_t;

/*---------------------------------------------------------------------------
 * AvrcpNotifyParms structure
 *
 * Defines the callback parameters for AVRCP_EVENT_ADV_RESPONSE when "advOp" is
 * set to AVRCP_OP_REGISTER_NOTIFY, and for the AVRCP_EVENT_ADV_NOTIFY event.
 */
typedef struct {

    /* Defines the event ID that was received */
    avrcp_event_Id_t event;
    union {

        /* Play status of the media.  Valid when the event ID is
         * AVRCP_EID_MEDIA_STATUS_CHANGED.
         */
        avrcp_media_status_t mediaStatus;

        /* The current track.  Valid when the event ID is
         * AVRCP_EID_TRACK_CHANGED.
         */
        avrcp_track_struct_t track;

        /* The position (ms) of the current track.  Valid when the event
         * ID is AVRCP_EID_PLAY_POS_CHANGED.
         */
        uint32_t position;

        /* The battery status of the target.  Valid when the event ID is
         * AVRCP_EID_BATT_STATUS_CHANGED.
         */
        avrcp_battery_status_t battStatus;

        /* The system status of the target.  Valid when the event ID is
         * AVRCP_EID_SYS_STATUS_CHANGED.
         */
        avrcp_system_status_t sysStatus;

        /* The player settings.  Valid when the event ID is
         * AVRCP_EID_APP_SETTING_CHANGED
         */
        struct {

            /* Bitmask that describes which
             * attributes are being reported
             */
            avrcp_player_attrId_mask_t attrMask;

            /* The equalizer setting. */
            avrcp_eq_value_t eq;

            /* The repeat setting. */
            avrcp_repeat_value_t repeat;

            /* The shuffle setting. */
            avrcp_shuffle_value_t shuffle;

            /* The scan setting. */
            avrcp_scan_value_t scan;

#if BTIF_AVRCP_NUM_PLAYER_SETTINGS > 4
            /* Extended player setting. */
            avrcp_ext_value_t extValue[BTIF_AVRCP_NUM_PLAYER_SETTINGS - 4];

#endif                          /*  */
        } setting;

#if BTIF_AVRCP_VERSION_1_3_ONLY == BTIF_DISABLED
        /* Absolute volume.  Valid when the event ID is
         * AVRCP_EID_VOLUME_CHANGED.
         */
        uint8_t volume;

        /* The addressed player.  Valid when the event ID is
         * AVRCP_EID_ADDRESSED_PLAYER_CHANGED.
         */
        struct {
            uint16_t playerId;
            uint16_t uidCounter;
        } addrPlayer;

        /* The UID counter.  Valid when the event ID is
         * AVRCP_EID_UIDS_CHANGED.
         */
        uint16_t uidCounter;

#endif                          /* BTIF_AVRCP_VERSION_1_3_ONLY == BTIF_DISABLED */
    } p;
} avrcp_adv_notify_parms_t;

typedef struct {
    uint8_t transId;
    union {

        /* Folder Item.  Valid when "advOp" is AVRCP_OP_GET_FOLDER_ITEMS */
        struct {
            avrcp_scope_t scope;    /* The requested scope */
            uint32_t start;     /* The starting item */
            uint32_t end;       /* The ending item */
            uint8_t numAttrs;   /* Number of attributes requested */
            uint8_t *attrList;  /* List of attributes requested */
        } folder;

        /* Path.  Valid when "advOp" is AVRCP_OP_CHANGE_PATH */
        struct {
            uint16_t uidCounter;    /* Current UID counter */
            uint8_t direction;  /* Direction to change in the path (up or down) */
            uint8_t *uid;       /* UID of folder to change to (down) */
        } chPath;

        /* Item Attributes.  Valid when "advOp" is AVRCP_OP_GET_ITEM_ATTRIBUTES */
        struct {
            avrcp_scope_t scope;    /* The requested scope */
            uint8_t *uid;       /* UID of the requested item */
            uint16_t uidCounter;    /* Current UID counter */
            uint8_t numAttrs;   /* Number of attributes requested */
            uint8_t *attrList;  /* List of attributes requested */
        } attr;

        /* Search. Valid when "advOp" is AVRCP_OP_SEARCH */
        struct {
            uint16_t charSet;   /* Character set of the search string */
            uint16_t len;       /* Length of the search string */
            uint8_t *str;       /* Search string */
        } search;
    } p;
} avrcp_browse_req_parms_t;

typedef union {

    /* The browsed player.  Valid when "advOp" is AVRCP_OP_SET_BROWSED_PLAYER
     */
    avrcp_media_player_item_t *brwsPlayer;
} avrcp_browse_inf_parms_t;

typedef union {

    /* Browsed Player. Valid when "advOp" is AVRCP_OP_SET_BROWSED_PLAYER */
    struct {
        uint16_t uidCounter;    /* Current UID counter */
        uint32_t numItems;      /* Number of items in the current path */
        uint16_t charSet;       /* Character set used by the player */
        uint8_t fDepth;         /* Number of folder length/name pairs to follow */
        uint8_t *list;          /* List of folder names */
    } brwsPlayer;

    /* Folder Items. Valid when "advOp" is AVRCP_OP_GET_FOLDER_ITEMS */
    struct {
        uint16_t uidCounter;    /* Current UID counter */
        uint32_t numItems;      /* Number of items in the current path */
        uint8_t *list;          /* List of items returned */
    } fldrItems;

    /* Change Path. Valid when "advOp" is AVRCP_OP_CHANGE_PATH */
    struct {
        uint32_t numItems;      /* Number of items in the current path */
    } chPath;

    /* Item Attributes. Valid when "advOp" is AVRCP_OP_GET_ITEM_ATTRIBUTES */
    struct {
        uint8_t numAttrs;       /* Number of attributes returned */
        uint8_t *list;          /* List of attributes returned */
    } itemAttrs;

    /* Search. Valid when "advOp" is AVRCP_OP_SEARCH */
    struct {
        uint16_t uidCounter;    /* Current UID counter */
        uint32_t numItems;      /* Number of items found in the search */
    } search;
} avrcp_browse_rsp_parms_t;

typedef struct {

    /* Operation to which the remote target responded */
    uint16_t operation;

    /* The press state of the key in the command to which
     * the target responded.
     */
    BOOL press;

    /* Response from the target. May indicate
     * an "extended" response code such as
     * AVRCP_RESPONSE_SKIPPED or AVRCP_RESPONSE_TIMEOUT.
     */
    uint8_t response;
} avrcp_panel_cnf_t;
typedef struct {

    /* Operation corresponding to the key pressed, held, or
     * released. AVRCP will only indicate a new operation
     * when the previous one has been _RELEASE'd.
     */
    uint16_t operation;
} avrcp_panel_ind_t;

/* Channel Packet */
typedef struct {
    list_entry_t node;
    U8 transId;
    U8 msgType;
    U8 msgHdrLen;
    U8 msgHdr[BTIF_AVTP_HEADER_LEN];
    U8 txIdSize;
    U16 txId;
    U16 txDataLen;
    U8 *txData;
    U32 context;
} avtp_packet_t;

typedef struct {
    list_entry_t node;          /* Used internally by AVCTP. */
    uint8_t transId;            /* Transaction ID */
    uint8_t ctype;              /* 4 bits */
    uint8_t subunitType;        /* 5 bits */
    uint8_t subunitId;          /* 3 bits */
    uint8_t opcode;             /* 8 bits */
    uint8_t headerLen;          /* Header length */
    uint8_t header[6];          /* Header information */
    uint16_t operandLen;         /* Length of buffer in "operands" */
    uint8_t *operands;          /* Buffer containing the command data */
    BOOL more;                  /* Indicates whether to expect additional
                                 * frames containing more operand data. */
    avtp_packet_t avtpPacket;   /* For sending over AVTP */

    // EvmTimer    timer;          /* Timer for the command */
} avctp_cmd_frame_t;

enum {
    BTIF_AVRCP_STATE_DISCONNECTED,
    BTIF_AVRCP_STATE_CONNECTED
};
typedef void avrcp_callback_parms_t;

typedef void (*btif_avrcp_callback)(btif_avrcp_chnl_handle_t chnl, const avrcp_callback_parms_t *parms);

static POSSIBLY_UNUSED inline btif_avctp_event_t btif_convert_avrcp_event(btif_avrcp_event_t event)
{
    return (btif_avctp_event_t) event;
}

#define BTIF_AVRCP_EVENT(stack_avrcp_event)   btif_convert_avrcp_event(stack_avrcp_event)
#define BTIF_AVRCP_ERROR_CODE(statck_error_code)  statck_error_code
typedef struct {
    bool free;
    btif_avrcp_chnl_handle_t avrcp_channel_handle;  //AvrcpChannel *
} btif_avrcp_channel_t;

#ifdef __cplusplus
extern "C" {

#endif

    void btif_avrcp_init(void);

    bt_status_t btif_avrcp_register(btif_avrcp_channel_t *channel, btif_avrcp_callback cb, avrcp_features_t feat);

    btif_avrcp_channel_t *btif_alloc_avrcp_channel(void);

    btif_avrcp_channel_t *btif_get_avrcp_channel(btif_avrcp_chnl_handle_t handle);

    btif_avctp_event_t btif_avrcp_get_callback_event(const avrcp_callback_parms_t * parms);

    bool btif_avrcp_adv_cmd_inuse(btif_avrcp_channel_t * channel);

    bool btif_avrcp_state_connected(btif_avrcp_channel_t * channel);

    void btif_avrcp_set_volume_cmd(void *cmd, uint8_t transid, int8_t volume);


    void btif_avrcp_send_custom_cmd_generic(btif_avrcp_channel_t * chnl, uint8_t * ptrData,
                                            uint32_t len);
    bt_status_t btif_avrcp_ct_accept_custom_cmd_rsp(btif_avrcp_channel_t * chnl, void *cmd_res,
                                                    uint8_t isAccept);
    bt_status_t btif_avrcp_connect(btif_avrcp_channel_t * channel, bt_bdaddr_t * addr);

    bt_status_t btif_avrcp_connect_rsp(btif_avrcp_channel_t * chnl, BOOL accept);

    bt_status_t btif_avrcp_disconnect(btif_avrcp_chnl_handle_t channel);

    bt_status_t btif_avrcp_ct_get_play_status(btif_avrcp_channel_t * channel,

                                              avrcp_advanced_pdu_t * cmd);
    avrcp_version_t btif_get_avrcp_version(btif_avrcp_channel_t * channel);

    bt_status_t btif_avrcp_ct_get_capabilities(btif_avrcp_channel_t * channel,
                                               avrcp_advanced_pdu_t * pdu,
                                               btif_avrcp_capabilityId capabilityId);

    void btif_set_avrcp_state(btif_avrcp_channel_t * channel, uint8_t avrcp_state);

    uint8_t btif_get_avrcp_state(btif_avrcp_channel_t * channel);

    btif_remote_device_t *btif_avrcp_get_remote_device(btif_avrcp_chnl_handle_t handle);

    uint8_t btif_avrcp_get_channel_panel_int_state(btif_avrcp_chnl_handle_t handle);

    uint8_t btif_avrcp_get_channel_avrcp_state(btif_avrcp_chnl_handle_t handle);

    bt_status_t btif_avrcp_ct_register_notification(btif_avrcp_channel_t * channel,

                                                    avrcp_advanced_pdu_t * pdu,
                                                    btif_avrcp_event_t eventId, uint32_t interval);
    bt_status_t btif_get_avrcp_cb_channel_state(const avrcp_callback_parms_t * parms);

    avrcp_error_code_t btif_get_avrcp_cb_channel_error_code(const avrcp_callback_parms_t * parms);

    btif_avrcp_operation_t btif_get_avrcp_cb_channel_advOp(const avrcp_callback_parms_t * parms);

    void *btif_get_avrcp_cb_txPdu(const avrcp_callback_parms_t * parms);

    btif_avrcp_operation_t btif_get_avrcp_cb_txPdu_Op(const avrcp_callback_parms_t * parms);

    uint8_t  btif_get_avrcp_pdu_ctype(void * adv_pdu);

    uint8_t btif_get_avrcp_cb_txPdu_transId(const avrcp_callback_parms_t * parms);

    avrcp_role_t btif_get_avrcp_cb_channel_role(btif_avrcp_channel_t * channel);

    avrcp_panel_cnf_t *btif_get_panel_cnf(const avrcp_callback_parms_t * parms);

    avrcp_role_t btif_get_avrcp_channel_role(btif_avrcp_channel_t * channel);

    adv_play_status_t *btif_get_avrcp_adv_rsp_play_status(const avrcp_callback_parms_t * parms);

    avrcp_adv_rsp_parms_t *btif_get_avrcp_adv_rsp(const avrcp_callback_parms_t * parms);

    avrcp_adv_notify_parms_t *btif_get_avrcp_adv_notify(const avrcp_callback_parms_t * parms);

    avrcp_event_mask_t btif_get_avrcp_adv_rem_event_mask(btif_avrcp_channel_t * channel);

    void btif_set_avrcp_adv_rem_event_mask(btif_avrcp_channel_t * channel, uint16_t mask);

    void btif_set_app_bt_device_avrcp_notify_rsp_ctype(avrcp_advanced_pdu_t * cmd, uint8_t type);

    uint8_t btif_get_app_bt_device_avrcp_notify_rsp_transid(avrcp_advanced_pdu_t * cmd);

    bt_status_t btif_avrcp_ct_get_media_Info(btif_avrcp_channel_t * channel,

                                             avrcp_advanced_pdu_t * cmd,
                                             avrcp_media_attrId_mask_t mediaMask);
    avctp_cmd_frame_t *btif_get_avrcp_cmd_frame(const avrcp_callback_parms_t * parms);

    void btif_avrcp_set_capabilities_rsp_cmd(avrcp_advanced_pdu_t * cmd_pdu, uint8_t transId,

                                             uint8_t type);

    void btif_avrcp_set_control_rsp_cmd(avrcp_advanced_pdu_t * cmd_pdu, uint8_t transId,
                                        uint8_t type);
    
    void btif_avrcp_set_control_rsp_cmd_witherror(avrcp_advanced_pdu_t * cmd_pdu, uint8_t transId, uint8_t type, uint8_t error);

    bt_status_t btif_avrcp_ct_get_capabilities_rsp(btif_avrcp_channel_t * channel,
                                                   avrcp_advanced_pdu_t * cmd_pdu,
                                                   avrcp_capabilityId_t capid, uint16_t mask);

    bt_status_t btif_avrcp_ct_accept_absolute_volume_rsp(btif_avrcp_channel_t * channel,
                                                         avrcp_advanced_pdu_t * cmd_pdu,
                                                         uint8_t volume);

    void btif_avrcp_set_notify_rsp_cmd(avrcp_advanced_pdu_t * cmd_pdu, uint8_t transId,
                                       uint8_t type);

    uint8_t btif_avrcp_get_ctl_trans_id(btif_avrcp_channel_t * channel);
    void btif_avrcp_set_ctl_trans_id(btif_avrcp_channel_t * channel, uint8_t trans_id);

    void btif_avrcp_set_notify_rsp_cmd_witherror(avrcp_advanced_pdu_t * cmd_pdu, uint8_t transId, uint8_t type, uint8_t error);

    void btif_avrcp_recheck_vol_ctrl_flag(uint8_t * flag);

    bt_status_t btif_avrcp_ct_get_absolute_volume_rsp(btif_avrcp_channel_t * channel,
                                                      avrcp_advanced_pdu_t * cmd_pdu,
                                                      uint8_t volume);

    bt_status_t btif_avrcp_ct_get_media_status_rsp(btif_avrcp_channel_t * channel,
                                                   avrcp_advanced_pdu_t * cmd_pdu, 
                                                   avrcp_media_status_t status);
    
    uint16_t btif_avrcp_get_cmgrhandler_remDev_hciHandle(btif_avrcp_channel_t * channel);

    avrcp_panel_cnf_t *btif_get_avrcp_panel_cnf(const avrcp_callback_parms_t * parms);

    avrcp_panel_ind_t *btif_get_avrcp_panel_ind(const avrcp_callback_parms_t * parms);

    bt_status_t btif_avrcp_set_panel_key(btif_avrcp_channel_t * channel, avrcp_panel_operation_t op,
                                         bool press);

    void btif_avrcp_set_channel_adv_event_mask(btif_avrcp_chnl_handle_t handle, uint16_t mask);



    bt_status_t btif_avrcp_ct_invalid_volume_rsp(btif_avrcp_channel_t * channel, avrcp_advanced_pdu_t * cmd_pdu);

#ifdef __cplusplus
}
#endif                          /*  */
#endif                          /*  */
