/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __GENIE_LPM_H__
#define __GENIE_LPM_H__

#define INTERRUPT_DELAY_TIME (50) //For debounce

#define DEFAULT_BOOTUP_DELAY_SLEEP_TIME (10000) //Unit:ms

#define PWR_STANDBY_BOOT_FLAG (0x1688)
#define PWR_BOOT_REASON (0x4000f034)

typedef enum _genie_lpm_status_e
{
    STATUS_WAKEUP,
    STATUS_SLEEP
} genie_lpm_status_e;

typedef enum _genie_lpm_wakeup_reason_e
{
    WAKEUP_BY_IO,
    WAKEUP_BY_TIMER,
    WAKEUP_IS_WAKEUP
} genie_lpm_wakeup_reason_e;

typedef void (*genie_lpm_cb_t)(genie_lpm_wakeup_reason_e reason, genie_lpm_status_e status, void* arg);

typedef struct _genie_lpm_wakeup_io_config_s {
   uint8_t port;
   uint8_t io_pol;
}genie_lpm_wakeup_io_config_t;


#define GENIE_WAKEUP_PIN(_port, _pol)  \
{                      \
	.port   = (_port), \
	.io_pol = (_pol),   \
}

typedef struct _genie_lpm_wakeup_io_s {
   uint8_t io_list_size;
   genie_lpm_wakeup_io_config_t* io_config;
} genie_lpm_wakeup_io_t;

typedef struct _genie_lpm_io_status_s {
   uint8_t port;
   uint8_t trigger_flag;
   uint8_t status;
}_genie_lpm_io_status_t;

typedef struct _genie_lpm_io_status_list_s {
   _genie_lpm_io_status_t *io_status;
   uint8_t size;
}_genie_lpm_io_status_list_t;


typedef struct _genie_lpm_conf_s
{
    uint8_t lpm_wakeup_io;       //0:then not wakeup by GPIO
    genie_lpm_wakeup_io_t  lpm_wakeup_io_config;   //wakeup IO list config
    uint8_t is_auto_enable;      //1:auto enter sleep mode when bootup
    uint32_t delay_sleep_time;   //if auto enter sleep,delay some time then enter sleep mode
    uint16_t sleep_ms;           //sleep time
    uint16_t wakeup_ms;          //wakeup time
    genie_lpm_cb_t genie_lpm_cb; //User callback
} genie_lpm_conf_t;

typedef struct _genie_lpm_ctx_s
{
    genie_lpm_conf_t p_config;
    uint8_t status;
    uint8_t is_mesh_init;
    uint8_t has_disabled;
    aos_timer_t wakeup_timer;
    aos_timer_t io_wakeup_timer;
    aos_mutex_t mutex;
    long long last_interrupt_time;
} genie_lpm_ctx_t;


void genie_lpm_init(genie_lpm_conf_t *lpm_conf);
int genie_lpm_start(void);
void genie_lpm_io_wakeup_handler(void *arg);
int genie_lpm_enable(bool force);
int genie_lpm_disable(void);
int genie_lpm_deep_sleep(void);
bool genie_lpm_get_wakeup_io_status(void);

#endif
