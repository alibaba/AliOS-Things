/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APPS_H__
#define __APPS_H__

#include "app_status_ind.h"

#define STACK_READY_BT  0x01
#define STACK_READY_BLE 0x02

#ifdef __cplusplus
extern "C" {
#endif

enum {
    BES_SDK,
    BES_WIFI_ONLY,
    BES_WIFI_BT,
    BES_WIFI_BT_MINI, // cut off nouse tasks
};

#include "plat_types.h"

int app_init(int boot_type);

int app_deinit(int deinit_case);

int app_shutdown(void);

int app_reset(void);

int app_status_battery_report(uint8_t level);

int app_voice_report( APP_STATUS_INDICATION_T status,uint8_t device_id);
int app_voice_report_generic(APP_STATUS_INDICATION_T status, uint8_t device_id, uint8_t isMerging);

bool app_enter_factory_wifi_test(void);

/*FixME*/
void app_status_set_num(const char* p);

////////////10 second tiemr///////////////
#define APP_FAST_PAIRING_TIMEOUT_IN_SECOND  120

#define APP_PAIR_TIMER_ID       0
#define APP_POWEROFF_TIMER_ID   1
#define APP_FASTPAIR_LASTING_TIMER_ID   2

void app_stop_10_second_timer(uint8_t timer_id);
void app_start_10_second_timer(uint8_t timer_id);

void app_notify_stack_ready(uint8_t ready_flag);

void app_start_postponed_reset(void);

bool app_is_power_off_in_progress(void);

#define CHIP_ID_C     1
#define CHIP_ID_D     2

void app_disconnect_all_bt_connections(void);
bool app_is_stack_ready(void);

////////////////////


#ifdef __cplusplus
}
#endif
#endif//__FMDEC_H__
