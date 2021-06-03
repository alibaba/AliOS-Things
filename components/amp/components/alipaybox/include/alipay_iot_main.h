#ifndef __ALIPAY_IOT_MAIN_H__
#define __ALIPAY_IOT_MAIN_H__
#include "alipay_iot_api.h"
#include "ant_os.h"

#ifdef USE_SERVER_XP

#include  "XpInformationTransferServerPB.pb-c.h"
#include  "XpInformationTransferTerminalPB.pb-c.h"

#else
// L2 AM7 TP1 M1EN TP21 --> A3 proto
#include "alipayBoxA3UpData.pb-c.h"
#include "alipayBoxA3DownData.pb-c.h"
#include "BoxA2StoreHomeDownDataModel.pb-c.h"
#include "BoxContentDownModel.pb-c.h"
#include "BoxContentUpModel.pb-c.h"
#endif

#ifdef USE_SERVER_XP

typedef Com__Alipay__Sdaxconnectplatform__Iot__Schema__Infotransfer__XpInformationTransferServerRequest iot_pb_up_data_t;
#define iot_pb_up_data_init                 com__alipay__sdaxconnectplatform__iot__schema__infotransfer__xp_information_transfer_server_request__init
#define iot_pb_up_data_get_packed_size      com__alipay__sdaxconnectplatform__iot__schema__infotransfer__xp_information_transfer_server_request__get_packed_size
#define iot_pb_up_data_pack                 com__alipay__sdaxconnectplatform__iot__schema__infotransfer__xp_information_transfer_server_request__pack
#define iot_pb_up_data_unpack               com__alipay__sdaxconnectplatform__iot__schema__infotransfer__xp_information_transfer_server_request__unpack
#define iot_pb_up_data_free_unpacked        com__alipay__sdaxconnectplatform__iot__schema__infotransfer__xp_information_transfer_server_request__free_unpacked

typedef Com__Alipay__Sdaxconnectplatform__Iot__Schema__Infotransfer__XpInformationTransferTerminalRequest iot_pb_down_data_t;                  
#define iot_pb_down_data_get_packed_size    com__alipay__sdaxconnectplatform__iot__schema__infotransfer__xp_information_transfer_terminal_request__get_packed_size
#define iot_pb_down_data_pack               com__alipay__sdaxconnectplatform__iot__schema__infotransfer__xp_information_transfer_terminal_request__pack
#define iot_pb_down_data_unpack             com__alipay__sdaxconnectplatform__iot__schema__infotransfer__xp_information_transfer_terminal_request__unpack
#define iot_pb_down_data_free_unpacked      com__alipay__sdaxconnectplatform__iot__schema__infotransfer__xp_information_transfer_terminal_request__free_unpacked



#else
typedef Com__Alipay__Iot__AlipayBoxA3UpData iot_pb_up_data_t;
#define iot_pb_up_data_init                 com__alipay__iot__alipay_box_a3_up_data__init
#define iot_pb_up_data_get_packed_size      com__alipay__iot__alipay_box_a3_up_data__get_packed_size
#define iot_pb_up_data_pack                 com__alipay__iot__alipay_box_a3_up_data__pack
#define iot_pb_up_data_unpack               com__alipay__iot__alipay_box_a3_up_data__unpack
#define iot_pb_up_data_free_unpacked        com__alipay__iot__alipay_box_a3_up_data__free_unpacked

typedef Com__Alipay__Iot__AlipayBoxA3DownData iot_pb_down_data_t;                  
#define iot_pb_down_data_get_packed_size    com__alipay__iot__alipay_box_a3_down_data__get_packed_size
#define iot_pb_down_data_pack               com__alipay__iot__alipay_box_a3_down_data__pack
#define iot_pb_down_data_unpack             com__alipay__iot__alipay_box_a3_down_data__unpack
#define iot_pb_down_data_free_unpacked      com__alipay__iot__alipay_box_a3_down_data__free_unpacked

#if defined(ALIPAY_BOX_L2)
 typedef Com__Alipay__Iot__ActivateL2Event  iot_pb_active_event_t;
 #define iot_pb_active_event_init           com__alipay__iot__activate_l2_event__init
 #define IOT_PB_EVENT_CODE_ACTIVE           COM__ALIPAY__IOT__EVENT_CODE__ACTIVATE_L2
#elif defined(ALIPAY_BOX_M1EN)
 typedef Com__Alipay__Iot__ActivateBizTidEvent iot_pb_active_event_t;
 #define iot_pb_active_event_init           com__alipay__iot__activate_biz_tid_event__init
 #define IOT_PB_EVENT_CODE_ACTIVE           COM__ALIPAY__IOT__EVENT_CODE__ACTIVATE_BIZTID
#else
 typedef Com__Alipay__Iot__ActivateA3Event  iot_pb_active_event_t;
 #define iot_pb_active_event_init           com__alipay__iot__activate_a3_event__init
 #define IOT_PB_EVENT_CODE_ACTIVE           COM__ALIPAY__IOT__EVENT_CODE__ACTIVATE_A3
#endif

#ifdef ALIPAY_BOX_UNI_BIND // M1EN TP21
#include "DeviceBindModel.pb-c.h"
#endif

#endif


#define IOTSDK_VER_1_2_1 1
#define IOTSDK_VER_1_2_2 2
#define IOTSDK_VER_1_2_3 3
#define IOTSDK_VER_1_2_4 4
#define IOTSDK_VER_1_2_5 5
#define IOTSDK_VER_1_2_6 6

#define CRASH_INFO_MAX_SIZE 1024

typedef struct{
  ant_char *trade_id;
  ant_char *money;
  ant_char *prefix;
  ant_char *suffix;
  ant_char *timestamp;
  ant_char *timeout;
  ant_bool  success;
  ant_s32   money_switch;
}iot_trade_info_t;

typedef struct{

  ant_s32    available_space;
  ant_s32    total_space;
  ant_char * parti_name;
  ant_char * files_hash;
}iot_media_info_t;

typedef struct{
  ant_char * file_id;
  ant_s64   create_time;
  ant_s32   play_type;
}iot_media_file_info_t;


#ifdef __cplusplus
extern "C" {
#endif
void  alipay_iot_event_handler(ant_msg *msg);
int   alipay_sdk_event_handler(ant_msg *msg);
void  alipay_sdk_login(int ota_check);
void  alipay_sdk_activate(void);
char *alipay_sdk_get_biztid(void);
char *alipay_sdk_get_device_id(void);
char *alipay_sdk_get_dynamic_id(void);
int   alipay_sdk_send_iotcms_data(void *data);
char *alipay_sdk_get_iotdid(void);
void  alipay_iot_send_trade_event(iot_trade_info_t * info);
void  alipay_sdk_crash_report(void);



#ifdef __cplusplus
}
#endif

#endif

