/**
 * File : aos.c
 */

#include "cmsis.h"
#include "stdarg.h"
#include "aos/init.h"
#include "aos/kernel.h"
//#include "uagent.h"
#include "ulog/ulog.h"
#include "ulog_config.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/hal/gpio.h"
#include "aos/hal/uart.h"
#if(AOS_COMP_WIFI > 0)
#include "aos/hal/wifi.h"
#endif
#include "hal_cmu.h"
#include "app_utils.h"
#include "ota_port.h"
#include "hal_uart.h"
#include "hal_trace.h"

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

extern uart_dev_t uart_0;

#define WRAP_P_BUF_LEN 2048
uint8_t _wrap_p_buf[WRAP_P_BUF_LEN];
int max_p_size = 0;
int g_dirver_trace_flag = 1;

#if AOS_COMP_CLI
static uint8_t char2data(const char ch)
{
    if ((ch >= '0') && (ch <= '9')) {
        return (uint8_t)(ch - '0');
    }
    if ((ch >= 'a') && (ch <= 'f')) {
        return (uint8_t)(ch - 'a' + 10);
    }
    if ((ch >= 'A') && (ch <= 'F')) {
        return (uint8_t)(ch - 'A' + 10);
    }

    return 0;
}
static void str2mac(const char *sz_mac, uint8_t *pmac)
{
    const char *ptemp = sz_mac;
    for (int i = 0; i < 6; ++i) {
        pmac[i] = char2data(*ptemp++) * 16;
        pmac[i] += char2data(*ptemp++);
        ptemp++;
    }
}
extern int factory_section_set_wifi_address(uint8_t *wifi_addr);
extern uint8_t *factory_section_get_wifi_address(void);
extern int factory_section_set_bt_address(uint8_t *bt_addr);
extern uint8_t *factory_section_get_bt_address(void);

static void handle_aos_mac_cmd(int argc, char **argv)
{
    int ret = 0;
    const char *rtype = argc > 1 ? argv[1] : "";
    const char *mac   = argc > 2 ? argv[2] : NULL;
    uint8_t pmac[6];

    if (mac != NULL) {
        str2mac(mac, pmac);
        printf("will set mac %02x:%02x:%02x:%02x:%02x:%02x\n",
                pmac[0], pmac[1], pmac[2], pmac[3], pmac[4], pmac[5]);
    }

    if (strcmp(rtype, "WIFI") == 0) {
        if (mac != NULL) {
            ret = factory_section_set_wifi_address(pmac);
            if (ret == 0) {
                printf("set WIFI mac success!\n");
            } else {
                printf("set WIFI mac fail!\n");
            }
        } else {
            uint8_t *_mac = factory_section_get_wifi_address();
            if (_mac == NULL) {
                printf("get WIFI mac fail\n");
            } else {
                printf("WIFI mac is %02x:%02x:%02x:%02x:%02x:%02x\n",
                        _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
            }
        }
    } else if (strcmp(rtype, "BT") == 0) {
        if (mac != NULL) {
            ret = factory_section_set_bt_address(pmac);
            if (ret == 0) {
                printf("set BT mac success!\n");
            } else {
                printf("set BT mac fail!\n");
            }
        } else {
            uint8_t *_mac = factory_section_get_bt_address();
            if (_mac == NULL) {
                printf("get BT mac fail\n");
            } else {
                printf("BT mac is %02x:%02x:%02x:%02x:%02x:%02x\n",
                        _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
            }
        }
    } else {
        printf("Usage: aos_mac [WIFI/BT] [XX:XX:XX:XX:XX:XX]");
    }
}

ALIOS_CLI_CMD_REGISTER(handle_aos_mac_cmd, aos_mac, "aos_mac [WIFI/BT] [mac]")
#endif


int __wrap_printf(const char *fmt, ...)
{
    int i;
    int len = 0;
    int crlf = 0;
    char *p = NULL;
    va_list args;
    uint32_t lock = int_lock();
    va_start(args, fmt);
    if (strcmp(fmt, "%s") == 0) {
        p = va_arg(args, char *);
        len = strlen(p);
    } else if (strstr(fmt, "%") == NULL) {
        p = fmt;
        len = strlen(p);
    } else {
        p = _wrap_p_buf;
        len = vsnprintf(_wrap_p_buf, WRAP_P_BUF_LEN-1, fmt, args);
        max_p_size = (len > max_p_size) ? len : max_p_size;
    }

    hal_uart_send(&uart_0, p, len, 300000);

    va_end(args);
    int_unlock(lock);
    return len;
}

#ifdef AOS_COMP_PWRMGMT
int pwrmgmt_init();
#endif

/* use for printk */
int alios_debug_print(const char *buf, int size)
{
    hal_trace_output_block((const unsigned char *)buf, size);
    /*
    uint32_t i;
    for (i = 0; i < size; i++) {
        hal_uart_blocked_putc(0, buf[i]);
    } */

    return size;
}

void alios_cli_panic_hook()
{
    hal_watchdog_disable();
    hal_panic_uart_open();
}

char uart_input_read(void)
{
    return hal_uart_blocked_getc(0);
}


/*  check pc available  0:available  other:not available */
extern uint32_t __sram_text_start__[];
extern uint32_t __sram_text_end__[];
extern uint32_t __flashx_text_start__[];
extern uint32_t __flashx_text_end__[];
extern uint32_t __psramux_text_start__[];
extern uint32_t __psramux_text_end__[];

int alios_debug_pc_check(char *pc)
{
    if ( (((uint32_t)pc > (uint32_t)__sram_text_start__) &&
          ((uint32_t)pc < (uint32_t)__sram_text_end__)) ||
         (((uint32_t)pc > (uint32_t)__flashx_text_start__) &&
          ((uint32_t)pc < (uint32_t)__flashx_text_end__)) ||
         (((uint32_t)pc > (uint32_t)__psramux_text_start__) &&
          ((uint32_t)pc < (uint32_t)__psramux_text_end__)) ) {
        return 0;
    } else {
        return -1;
    }
}

#if AOS_COMP_CLI
void alios_debug_pc_show(int argc, char **argv)
{
    cli_printf("----- PC Addr ------\r\n");
    cli_printf("addr 1 : 0x%08x ~ 0x%08x\r\n", (uint32_t)__sram_text_start__, (uint32_t)__sram_text_end__);
    cli_printf("addr 2 : 0x%08x ~ 0x%08x\r\n", (uint32_t)__flashx_text_start__, (uint32_t)__flashx_text_end__);
    cli_printf("addr 3 : 0x%08x ~ 0x%08x\r\n", (uint32_t)__psramux_text_start__, (uint32_t)__psramux_text_end__);
}
#endif

#ifdef AOS_COMP_LITTLEFS
#include "littlefs.h"

int32_t littlefs_fetch_cfg_param(struct littlefs_cfg_param * cfg_param)
{
    cfg_param->block_size = 4096;
#ifdef CFG_HW_ALI_MODULE
    cfg_param->block_count = 520;
#else
    cfg_param->block_count = 1198;
#endif
    cfg_param->prog_size = 1024;
    cfg_param->read_size = 256;
    cfg_param->block_cycles = 1000;
    cfg_param->cache_size = 1024;
    cfg_param->lookahead_size = 16;
    return 0;
}
#endif /* AOS_COMP_LITTLEFS */

#if CONFIG_A7_DSP_ENABLE
#include "a7_cmd.h"
#include "aud_dump.h"
void mcu_first_handshake(void)
{
    transq_msg_onoff(1);
    A7_CMD_T a7_cmd;
    a7_cmd.type = A7_CMD_TYPE_HANDSHAKE;
    a7_cmd.p1 = (tg_ota_get_odm_type()==ALI_C5A01)?48000:16000;
    a7_cmd.p2 = (tg_ota_get_odm_type()==ALI_C5A01)?48:64;
    mcu_cmd_send(&a7_cmd);
    printf("%s A7_CMD_TYPE_HANDSHAKE to a7: mic_samplerate %d, period_ms %d\n", __FUNCTION__, a7_cmd.p1, a7_cmd.p2);
}
#endif

int WEAK do_ulog(const unsigned char s, const char *mod, const char *f, const unsigned long l, const char *fmt, va_list args)
{
    return 0;
}
int aos_printf_hook(const char *tag, const char *fmt, enum PRINTF_FLAG_T flag, va_list ap)
{
    int ret = 0;

    if (!g_dirver_trace_flag) {
        return 1;
    }

    if (fmt) {
        if (flag == 0) {
            uint32_t lock = int_lock();
            ret = vprintf(fmt, ap);
            int_unlock(lock);
        } else {
            ret = do_ulog(LOG_INFO, tag?tag:"SDK_TRACE", ULOG_TAG, fmt, ap);
        }
    } else {
        ret = check_pass_pop_out(ulog_session_std, LOG_ERR) ? 1 : -1;
    }

    return ret;
}

void aos_set_driver_trace_flag(int flag)
{
    g_dirver_trace_flag = flag;
}

void hal_watchdog_disable(void)
{
}

void hal_watchdog_reset(int timeout)
{
#ifndef CONFIG_GENIE_DEBUG
	app_wdt_ping();
#endif
}

void hal_reset_cpu(void)
{
    hal_reboot();
}



void soc_peripheral_init()
{
    app_sysfreq_req(APP_SYSFREQ_USER_APP_15, APP_SYSFREQ_390M);
    printf("sys freq calc : %u \n", hal_sys_timer_calc_cpu_freq(5, 0));
}

void aos_trace_notify(enum HAL_TRACE_STATE_T state)
{
    if (state == HAL_TRACE_STATE_CRASH_END)
        abort();
}

#if AOS_COMP_CLI
#ifdef CUSTOM_CLI_ENABLED
static uint8_t char2data(const char ch)
{
    if ((ch >= '0') && (ch <= '9')) {
        return (uint8_t)(ch - '0');
    }
    if ((ch >= 'a') && (ch <= 'f')) {
        return (uint8_t)(ch - 'a' + 10);
    }
    if ((ch >= 'A') && (ch <= 'F')) {
        return (uint8_t)(ch - 'A' + 10);
    }

    return 0;
}
static void str2mac(const char * sz_mac, uint8_t * pmac)
{
    const char * ptemp = sz_mac;
    for (int i = 0; i < 6; ++i)
    {
        pmac[i] = char2data(*ptemp++) * 16;
        pmac[i] += char2data(*ptemp++);
        ptemp++;
    }
}
extern int factory_section_set_wifi_address(uint8_t *wifi_addr);
extern uint8_t* factory_section_get_wifi_address(void);
extern int factory_section_set_bt_address(uint8_t *bt_addr);
extern uint8_t* factory_section_get_bt_address(void);

static void handle_aos_mac_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int ret = 0;
    const char *rtype = argc > 1 ? argv[1] : "";
    const char *mac   = argc > 2 ? argv[2] : NULL;
    uint8_t pmac[6];

    if(mac != NULL) {
        str2mac(mac, pmac);
        printf("will set mac %02x:%02x:%02x:%02x:%02x:%02x\n",
                pmac[0], pmac[1], pmac[2], pmac[3], pmac[4], pmac[5]);
    }

    if (strcmp(rtype, "WIFI") == 0) {
        if(mac != NULL) {
            ret = factory_section_set_wifi_address(pmac);
            if(ret == 0) {
                printf("set WIFI mac success!\n");
            } else {
                printf("set WIFI mac fail!\n");
            }
        } else {
            uint8_t *_mac = factory_section_get_wifi_address();
            if(_mac == NULL) {
                printf("get WIFI mac fail\n");
            } else {
                printf("WIFI mac is %02x:%02x:%02x:%02x:%02x:%02x\n",
                        _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
            }
        }
    } else if (strcmp(rtype, "BT") == 0) {
        if(mac != NULL) {
            ret = factory_section_set_bt_address(pmac);
            if(ret == 0) {
                printf("set BT mac success!\n");
            } else {
                printf("set BT mac fail!\n");
            }
        } else {
            uint8_t *_mac = factory_section_get_bt_address();
            if(_mac == NULL) {
                printf("get BT mac fail\n");
            } else {
                printf("BT mac is %02x:%02x:%02x:%02x:%02x:%02x\n",
                        _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
            }
        }
    } else {
        printf("Usage: aos_mac [WIFI/BT] [XX:XX:XX:XX:XX:XX]");
    }
}

static struct cli_command aos_mac_cmd = {
    .name     = "aos_mac",
    .help     = "aos_mac [WIFI/BT] [mac]",
    .function = handle_aos_mac_cmd
};

void aos_custom_cli_register()
{
    aos_cli_register_command(&aos_mac_cmd);
}
#endif
#endif

void aos_init_done_hook(void)
{
#ifdef SWD_ENABLE_AS_DEFAULT
    // enable swd as default
    hal_iomux_set_jtag();
    krhino_idle_hook_onoff(0);
#endif

#if CONFIG_A7_DSP_ENABLE
    //heartbeat_init();
    bes_kv_init();
#endif
    hal_trace_register_hook(aos_printf_hook);

#if 0//def CONFIG_A7_DSP_ENABLE
    // for dsp audiodump
    aud_dump_init();
    mcu_audiodump_register(_AUDIO_DUMP_SRC_BEFORE_ALG_, aud_dump_cb_register, aud_dump_cb_unregister, 3, 256, 3000);
    mcu_audiodump_register(_AUDIO_DUMP_SRC_INTER_ALG_, aud_dump_cb_register, aud_dump_cb_unregister, 1, 16, 10000);
    mcu_audiodump_register(_AUDIO_DUMP_SRC_AFTER_ALG_, aud_dump_cb_register, aud_dump_cb_unregister, 1, 16, 10000);
#endif

}

int uart_puts(const char *str)
{
    return hal_trace_output(str, strlen(str));
}

// tmp for build, TBD
uint32_t get_random(void)
{
    static uint32_t dns_txid;
    return (++dns_txid);
}

void sys_thread_exit(void)
{
#if LWIP_NETCONN_SEM_PER_THREAD
    netconn_thread_cleanup();
#endif

    osThreadExitPub();
}

#if CONFIG_A7_DSP_ENABLE
#include "hal_aud.h"
void a7_dsp_reboot()
{
    printf("%s", __FUNCTION__);
    //sendTrace("crashReport","dspreboot",NULL,NULL,NULL);
    transq_msg_onoff(0);
    transq_msg_flush();

    af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);

    hal_cmu_dsp_stop_cpu();

    transq_msg_reinit();
    heartbeat_init();

    a7_dsp_boot();
}
void a7_dsp_recover_report()
{
    printf("%s", __FUNCTION__);
    //sendTrace("crashReport","dspfinish",NULL,NULL,NULL);
}
#endif
