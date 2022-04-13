/**
 * @file event_type_code.h
 * Definitions of event codes.
 *
 * @version   V1.0
 * @date      2019-11-08
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef AOS_EVENT_TYPE_CODE_API_H
#define AOS_EVENT_TYPE_CODE_API_H

/** @addtogroup aos_yloop event_type_code
 *  Definitions of event codes.
 *
 *  @{
 */

/* remote procedure call */
#define EV_RPC 0x0100

/* YunIO event */
#define EV_YUNIO 0x0101
#define CODE_YUNIO_CMD_START 1
#define CODE_YUNIO_CMD_RETRY 2
#define CODE_YUNIO_CMD_DISCONNECT 3
#define CODE_YUNIO_ON_CONNECTED 4
#define CODE_YUNIO_ON_DISCONNECTED 5
#define CODE_YUNIO_ON_CONNECT_FAILED 6
#define CODE_YUNIO_ON_READ_ERROR 7
#define CODE_YUNIO_ON_WRITE_ERROR 8
#define CODE_YUNIO_ON_HEARTBIT_ERROR 9
#define CODE_YUNIO_PUBLISH_SUCCESS 10
#define CODE_YUNIO_PUBLISH_TIMEOUT 11

/** OTA event define */
#define EV_OTA 0x0102
#define CODE_OTA_ON_RESULT 1

/** register device service */
#define EV_DEVICE_REGISTER 0x0103
#define CODE_REGISTER_ON_RESULT 1
#define VALUE_REGISTER_SUCCESS 0

/** id2 device activate */
#define EV_ACTIVATE_ID2 0x0104
#define CODE_ACTIVATE_ID2_ON_RESULT 1
#define VALUE_ACTIVATE_ID2_SUCCESS 0
#define VALUE_ACTIVATE_ID2_FAILURE 1

#define EV_DDA 0x0105
#define CODE_DDA_ON_CONNECTED 1

#define EV_BZ_COMBO 0x0106
#define CODE_COMBO_AP_INFO_READY 1

#define EV_BZ_AUTH  0x0107
#define EV_BZ_TRANS 0x0108
#define EV_BZ_EXT   0x0109
#define EV_BZ_GAP   0x0110

/** General key define */
#define EV_KEY 0x0201
#define CODE_RECORD 1
#define CODE_VOLUME 2 /* Reserve */
#define CODE_VOLUME_INC 3
#define CODE_VOLUME_DEC 4
#define CODE_PLAY_PAUSE 5
#define CODE_MUTE 6
#define CODE_CHANNEL 7
#define CODE_NEXT 8
#define CODE_RECORD_PRE 9
#define CODE_RESET 0x1001
#define CODE_STATUS 11
#define CODE_ELINK 12
#define CODE_BOOT 13
/** General key value */
#define VALUE_KEY_UP 0
#define VALUE_KEY_DOWN 1
#define VALUE_KEY_CLICK 2
#define VALUE_KEY_LTCLICK 3
#define VALUE_KEY_LLTCLICK 4
#define VALUE_KEY_DBCLICK 5
/** General channel value */
#define VALUE_SYS_CHN_CONNECTED 0
#define VALUE_SYS_CHN_CONNECTING 1
#define VALUE_SYS_CHN_DISCONNECTED 2

/** Reserve event */
#define EV_KNOD 0x0203

/** SD card event */
#define EV_SD 0x0204
#define CODE_SD_PLUG 1
#define VALUE_SD_OUT 0
#define VALUE_SD_IN 1

/** LAN Driver event */
#define EV_LAN 0x0205
#define VALUE_LAN_OUT 0
#define VALUE_LAN_IN 1

/** Net event define */
#define EV_NET 0x0206
#define CODE_NET_DHCP_START 1
#define CODE_NET_DHCP_RESULT 2
#define CODE_NET_IP_STATIC 3
#define CODE_NET_STATUS 4
#define VALUE_NET_LAN_OK 0x01
#define VALUE_NET_WIFI_OK 0x02

/** Usb driver event */
#define EV_USB 0x0207
#define VALUE_USB_OUT 0
#define VALUE_USB_IN 1

/** PM event */
#define EV_PM 0x0208
#define CODE_PM_ENTER_INFORM 1
#define CODE_PM_ENTER 2
#define CODE_PM_DONE 3
#define CODE_PM_QUIT 4
#define CODE_PM_SHUTDOWN_INFORM 5
#define CODE_PM_STARTUP_INFORM 6
#define CODE_PM_LOWPOWER_INFORM 7
#define CODE_PM_POWERRECOVER_INFORM 8
#define CODE_PM_START_HEARTBEAT_INFORM 9
#define CODE_PM_STOP_HEARTBEAT_INFORM 10
#define VALUE_PM_IDLE_INFORM 1
#define VALUE_PM_POWERBUTTON_INFORM 2

/** File system event */
#define EV_FS 0x0209
#define CODE_FSYS_SD_LOAD 1
#define CODE_FSYS_UDISK_LOAD 2
#define CODE_FSYS_FLASH_LOAD 3
#define CODE_FSYS_SD_SPACE 4
#define CODE_FSYS_UDISK_SPACE 5
#define CODE_FSYS_FLASH_SPACE 6

/** Bluetooth */
#define EV_BT 0x020A
#define CODE_BT_DEVICE_CONNECT 1
#define CODE_BT_DEVICE_DISCONNECT 2
#define CODE_BT_MODE_ON 3
#define CODE_BT_MODE_OFF 4

/** DLNA */
#define EV_DLNA 0x020B
#define CODE_DLNA_DEVICE_CONNECT 1
#define CODE_DLNA_DEVICE_DISCONNECT 2
#define CODE_DLNA_MODE_ON 3
#define CODE_DLNA_MODE_OFF 4

/** AIRPLAY */
#define EV_AIRPLAY 0x020C
#define CODE_AIRPLAY_DEVICE_CONNECT 1
#define CODE_AIRPLAY_DEVICE_DISCONNECT 2
#define CODE_AIRPLAY_MODE_ON 3
#define CODE_AIRPLAY_MODE_OFF 4

/** AT */
#define EV_AT 0x020D
#define CODE_AT_IF_READY 1
#define CODE_AT_IF_DISAPPEAR 2

/** @} */

#endif /* AOS_EVENT_TYPE_CODE_API_H */
