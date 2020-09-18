/**
 * File : aos.c
 */

#include "cmsis.h"
#include "stdarg.h"
#include "aos/init.h"
#include "aos/kernel.h"
#include "uagent/uagent.h"
#include "ulog/ulog.h"
#include "ulog_config.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/hal/gpio.h"
#include "aos/hal/uart.h"
#include "hal/wifi.h"

#include "hal_cmu.h"
#include "app_utils.h"
#include "ota_port.h"
#include "hal_uart.h"
#include "hal_trace.h"

uart_dev_t uart_0;

#define WRAP_P_BUF_LEN 2048
uint8_t _wrap_p_buf[WRAP_P_BUF_LEN];
int max_p_size = 0;

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

int alios_debug_print(const char *buf, int size)
{
    hal_trace_output_block((const unsigned char *)buf, size);
    return size;
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

static void pc_show(int argc, char **argv)
{
    printf("----- PC Addr ------\r\n");
    printf("addr 1 : 0x%08x ~ 0x%08x\r\n", (uint32_t)__sram_text_start__, (uint32_t)__sram_text_end__);
    printf("addr 2 : 0x%08x ~ 0x%08x\r\n", (uint32_t)__flashx_text_start__, (uint32_t)__flashx_text_end__);
    printf("addr 3 : 0x%08x ~ 0x%08x\r\n", (uint32_t)__psramux_text_start__, (uint32_t)__psramux_text_end__);
}

void alios_debug_pc_show(void)
{
    pc_show(0, NULL);
}

#ifdef AOS_COMP_LITTLEFS
#include "fs/littlefs.h"

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

void mcu_first_handshake(void)
{

}

int WEAK do_ulog(const unsigned char s, const char *mod, const char *f, const unsigned long l, const char *fmt, va_list args)
{
    return 0;
}
int aos_printf_hook(const char *tag, const char *fmt, enum PRINTF_FLAG_T flag, va_list ap)
{
    int ret = 0;

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

extern hal_wifi_module_t aos_wifi_haas1000;

void hal_init(void)
{
    uart_0.port = 0;
    uart_0.config.baud_rate = 1500000;
    uart_0.config.data_width = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.mode = MODE_TX_RX;
    uart_0.config.parity = NO_PARITY;
    uart_0.config.stop_bits = STOP_BITS_1;

    hal_uart_init(&uart_0);

#if AOS_NET_WITH_WIFI
    hal_wifi_register_module(&aos_wifi_haas1000);
    if(0 != hal_wifi_set_module_base(&aos_wifi_haas1000, "rtos", "AOS+TG", "websoc+http+srtp", NULL,
                              "TmallGenie", "TGCloud")) {
        printf("set module base information failed\n");
    }
    if (0 != hal_wifi_init()) {
        printf("hal_wifi_init fail ===!!!!\r\n");
    }
#endif
}

void soc_peripheral_init(int wifi_init)
{
    app_sysfreq_req(APP_SYSFREQ_USER_APP_15, APP_SYSFREQ_390M);
    printf("sys freq calc : %d, wifi init %d \n", hal_sys_timer_calc_cpu_freq(5, 0), wifi_init);
    hal_init();
}

void aos_trace_notify(enum HAL_TRACE_STATE_T state)
{
    if (state == HAL_TRACE_STATE_CRASH_END)
        abort();
}

extern void heartbeat_init(void);
void aos_init_done_hook(void)
{
#ifdef SWD_ENABLE_AS_DEFAULT
    // enable swd as default
    hal_iomux_set_jtag();
    krhino_idle_hook_onoff(0);
#endif

    hal_trace_register_hook(aos_printf_hook);
    hal_trace_app_register(aos_trace_notify, NULL);
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

void lwip_pkt_print(char* note_ptr, struct pbuf *pbuf, struct netif* netif)
{
    // stub
}
