#ifndef __ALIPAY_BOX_SDK_H__
#define __ALIPAY_BOX_SDK_H__
#include "ant_kal.h"
#include "alipay_box_sdk_api.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int connected;
} ant_network_event_t;

typedef struct{
    ant_u32 interval;
    ant_u32 cnt;
    ant_u32 mode;
}ant_idle_event_t;

typedef union {
    ant_network_event_t network;
    ant_key_event       key;
    ant_idle_event_t    idle;
} alipay_box_event_u;

typedef struct {
    ant_event_e        event_id;
    alipay_box_event_u *data;
} alipay_box_event_t;

typedef void (* alipay_box_audio_ctr_api)(void);
typedef void (*alipay_box_event_callback_t)(alipay_box_event_t *event, void *usr_data);
typedef int (* alipay_box_get_voice_pkg_size_api)(void);
typedef int (* alipay_box_voice_pkg_default_check_api)(void);
typedef char * (* alipay_box_get_voice_pkg_data_api)(void);
typedef int  (*alipay_box_shutdown_callback_t)(void);
//int alipay_box_init(alipay_box_event_callback_t cb, alipay_box_shutdown_callback_t shutdown_cb, void *usr_data);
int alipay_sdk_init(alipay_box_event_callback_t box_cb,alipay_box_sdk_event_callback sdk_cb,alipay_box_shutdown_callback_t shutdown_cb,
                      void *box_userdata,void *sdk_userdata,char * default_version,alipay_box_mode_enum mode);

void notify_sdk_init_event(alipay_box_init_error_enum result);
void notify_sdk_event(alipay_box_sdk_event_t *event);
void notify_box_event(alipay_box_event_t *event);

#ifdef __cplusplus
}
#endif

#endif

