#ifndef __ALIPAY_BOX_SDK_API_H__
#define __ALIPAY_BOX_SDK_API_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ALIPAY_BOX_EVENT_SDK_INIT = 1000,
    ALIPAY_BOX_EVENT_SERVER_CONNECTED,
    ALIPAY_BOX_EVENT_SERVER_DISCONNECTED,
    ALIPAY_BOX_EVENT_LOGIN,
    ALIPAY_BOX_EVENT_BIND,
    ALIPAY_BOX_EVENT_UNBIND,
    ALIPAY_BOX_EVENT_TRADE,
    ALIPAY_BOX_EVENT_AUDIO,
    ALIPAY_BOX_EVENT_OTA,
    ALIPAY_BOX_EVENT_EXTA_DOWNLOAD,
    ALIPAY_BOX_EVENT_EXTA_DELETE,
    ALIPAY_BOX_EVENT_EXTA_CLEAR,
    ALIPAY_BOX_EVENT_STORE_HOME,
    ALIPAY_BOX_EVENT_VOICE_CODE,
    ALIPAY_BOX_EVENT_XP_LOGIN,
    ALIPAY_BOX_EVENT_XP_ACTIVE,
    ALIPAY_BOX_EVENT_SHADOW_GET_REPLY,
    ALIPAY_BOX_EVENT_SHADOW_PUSH,
    ALIPAY_BOX_EVENT_IOT_CREATE,
    ALIPAY_BOX_EVENT_XP_ONLINE,
    ALIPAY_BOX_EVENT_XP_OTA_NOTIFY,
    ALIPAY_BOX_EVENT_IS_DEFAULT_SET,
    ALIPAY_BOX_EVENT_END,
} alipay_box_event_enum;

typedef enum {
    ALIPAY_BOX_INIT_SUCCESS         =  0,
    ALIPAY_BOX_INIT_ERR_UNREGISTRED = -1,    
    ALIPAY_BOX_INIT_ERR_NETWROK     = -2,
    ALIPAY_BOX_INIT_ERR_TIME        = -3,
    ALIPAY_BOX_INIT_ERR_KV          = -4,
    ALIPAY_BOX_INIT_ERR_NOMEM       = -5,
    ALIPAY_BOX_INIT_ERR_ILLEGAL     = -6,
    ALIPAY_BOX_INIT_ERR_INTERNAL    = -99,    
} alipay_box_init_error_enum;

typedef enum {
    ALIPAY_BOX_MODE_NORMAL = 0,
    ALIPAY_BOX_MODE_DEBUG,
    ALIPAY_BOX_MODE_FACTORY,
} alipay_box_mode_enum;

#define  AUDO_DELETE_MODE_STAND   0
#define  AUDO_DELETE_MODE_CLEAR   1

#define ALIPAY_TTS_REQID_WELCOME           "com.alipay.tts.welcome"
#define ALIPAY_TTS_REQID_SHUTDOWN          "com.alipay.tts.shudown"
#define ALIPAY_TTS_REQID_BIND_OK           "com.alipay.tts.bind.ok"
#define ALIPAY_TTS_REQID_UNBIND_OK         "com.alipay.tts.unbind.ok"
#define ALIPAY_TTS_REQID_SCAN              "com.alipay.tts.scan"
#define ALIPAY_TTS_REQID_TAXI_METER_ON     "com.alipay.tts.taxi.meter.on"
#define ALIPAY_TTS_REQID_TAXI_METER_OFF    "com.alipay.tts.taxi.meter.off"

typedef struct {
    alipay_box_init_error_enum result;
} alipay_box_sdk_init_event_t;

typedef struct {
    int bind_status;
} alipay_box_login_event_t;

typedef struct {
    char *voice_code;
} alipay_box_vcode_event_t;

typedef struct {
    char *trade_id;
    char *money;
#ifdef ALIPAY_BOX_ISV
    char *prefix;
    char *suffix;
#else
    char *file_id;
#endif
    int money_switch;

} alipay_box_trade_event_t;


typedef struct {
    char *version; 
    char *digested;
    char *url;
    char *plan_id;
    char *aes_key;
    int  auto_reboot;    
    char * appid;
} alipay_box_ota_event_t;

typedef struct {
    char    *url;
    char    *md5;
    char    *file_id;
    char    *plan_id;
    int     start_time;
    int     end_time;
    char    *rules;     // for CUSTOM AUDIO
    int     play_type;
	int     is_default;
} alipay_box_exta_download_event_t;

typedef struct {
    int     mode;
    int     type;
    int     count;
    char    **file_ids;
} alipay_box_exta_delete_event_t;

typedef struct {
#define ARRIVE_STORE  1
#define ARRIVE_HOME   2
    int voice_type;
    char *file_id;
} alipay_box_store_home_event_t;



typedef struct{
    char * data;
 }alipay_box_shadow_event_t;

typedef struct{
    int  code;
 }alipay_box_xp_login_event_t;

 
typedef struct{
    int  code;
 }alipay_box_xp_active_event_t;


 typedef struct{
     int  code;
  }alipay_box_xp_online_event_t;


 typedef struct
 {
    char * did;
 }alipay_box_iot_create_event_t;
 

typedef union {
    alipay_box_sdk_init_event_t init;
    alipay_box_login_event_t    login;
    alipay_box_vcode_event_t    vcode;
    alipay_box_trade_event_t    trade;
    alipay_box_ota_event_t      ota;
    alipay_box_exta_download_event_t  exta_download;
    alipay_box_exta_delete_event_t  exta_delete;
    alipay_box_store_home_event_t   storehome;
    alipay_box_iot_create_event_t create;
    alipay_box_shadow_event_t    shadow;
    alipay_box_xp_login_event_t  xp_login;
    alipay_box_xp_active_event_t xp_active;
    alipay_box_xp_online_event_t xp_online;
} alipay_box_event_data_t;

typedef struct{ 
    alipay_box_event_enum   event_id;
    alipay_box_event_data_t *data;
} alipay_box_sdk_event_t;

typedef void (*alipay_box_sdk_event_callback)(alipay_box_sdk_event_t *event, void *user_data);

int alipay_box_generate_trade_audio(const char *money, char *buf, int *len);

int   alipay_box_generate_tts_data(const char * reqid,char * buf,int *len);

int  alipay_box_get_audio_path(const char * reqid,char * rname,int max_size);




#ifdef __cplusplus
}
#endif

#endif

