#ifndef __ALIPAY_IOT_API_H__
#define __ALIPAY_IOT_API_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ALIPAY_IOT_EVENT_SERVER_CONNECTED,       // device is connected with alipay iot server
    ALIPAY_IOT_EVENT_SERVER_DISCONNECTED,    // device is disconnected from alipay iot server
    
    ALIPAY_IOT_EVENT_LOGIN,                  // device login to alipay iot server
    ALIPAY_IOT_EVENT_ACTIVATED,              // device is activated by alipay iot server
    
    ALIPAY_IOT_EVENT_TRADE,                  // device receive a trade event, @event_param is @alipay_iot_trade_param_t
    ALIPAY_IOT_EVENT_AUDIO,                  // device receive a downloadable audio event, @event_param is @alipay_iot_audio_download_param_t
    ALIPAY_IOT_EVENT_OTA,                    // device receive ota message, @event_param is alipay_iot_ota_param_t
    ALIPAY_IOT_VOICE_CODE,                   // device receive a voice code, @event_param is alipay_iot_voice_code_param_t
    
    ALIPAY_IOT_EVENT_STORE_HOME,             // device receive a arrive home or store event, @event_param is @alipay_iot_store_home_param_t
    ALIPAY_IOT_EVENT_EXTA_DOWNLOAD,          // device receive a ext audio download event, @event_param is @alipay_iot_exta_download_param_t
    ALIPAY_IOT_EVENT_EXTA_DELETE,            // device receive a ext audio delete event, @event_param is @alipay_iot_exta_delete_param_t
#ifdef USE_SERVER_XP
    ALIPAY_IOT_EVENT_XP_LOGIN_REPLY,
    ALIPAY_IOT_EVENT_XP_ACTIVE_REPLY,
    ALIPAY_IOT_EVENT_XP_SHADOW_REPLY,
    ALIPAY_IOT_EVENT_XP_SHADOW_PUSH,
    ALIPAY_IOT_EVENT_XP_ONLINE,
    ALIPAY_IOT_EVENT_XP_OTA_NOTIFY,
#endif    
	ALIPAY_IOT_EVENT_DEFAULT_VOICE_SET,
    ALIPAY_IOT_EVENT_NUM
} ALIPAY_IOT_EVENT_ENUM;

typedef enum {
    ALIPAY_IOT_UNBIND = 0,
    ALIPAY_IOT_BIND   = 1
} ALIPAY_IOT_BIND_STATUS_ENUM;

typedef struct {
    int bind_status;
} alipay_iot_login_param_t;

typedef struct {
    int bind_status;
} alipay_iot_activated_param_t;
#ifdef USE_SERVER_XP

typedef struct{
    ant_char * data;
 }alipay_iot_shadow_params_t;

 typedef struct{
    ant_char * data;
 }alipay_iot_online_params_t;
 

typedef struct{
    ant_s32  code;
 }alipay_iot_xp_login_params_t;

typedef struct{
    ant_s32  code;
 }alipay_iot_xp_active_params_t;

#endif
 
typedef struct {
    char *trade_id;
    char *money;
#ifdef ALIPAY_BOX_ISV
    char *prefix;
    char *suffix;
#else
    char *file_id;
#endif
    ant_s32 money_switch;
} alipay_iot_trade_param_t;

typedef struct {
    char *digested;
    char *url;
    char *aes_key;
} alipay_iot_audio_download_param_t;

typedef struct {
    char *code;
} alipay_iot_voice_code_param_t;

typedef struct {
#define ARRIVE_STORE  1
#define ARRIVE_HOME   2
    int voice_type;
    char *file_id;
} alipay_iot_store_home_param_t;

typedef struct {
    char * target_id;
    char * version; 
    char * digested;
    char * url;
    char * plan_id;
    char * aes_key;
    int    auto_reboot;
} alipay_iot_ota_param_t;

typedef struct {
    char    *url;
    char    *md5;
    char    *file_id;
    char    *plan_id;
    int     start_time;
    int     end_time;
	char    * voicepac_start_time;
	char    * voicepac_stop_time;
    char    *rules;     // for CUSTOM AUDIO
    int     play_type;
	int     is_default;
} alipay_iot_exta_download_param_t;

typedef struct {
    int     mode;
    int     type;
    int     count;
    char    **file_ids;
} alipay_iot_exta_delete_param_t;

typedef void (* alipay_iot_event_callback_t)(ALIPAY_IOT_EVENT_ENUM event, void *event_param, void *user_param);

int  alipay_iot_init(void);
void alipay_iot_free_event_param(ALIPAY_IOT_EVENT_ENUM event, void *event_param);
#ifdef __cplusplus
}
#endif

#endif

