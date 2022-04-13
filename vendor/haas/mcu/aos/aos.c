/**
 * File : aos.c
 */

#include "cmsis.h"
#include "stdarg.h"
#include "aos/init.h"
#include "aos/kernel.h"
#include "uagent/uagent.h"
#include "ulog/ulog.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/hal/gpio.h"
#include "aos/hal/uart.h"
#include "hal/wifi.h"
#include "hal_iomux.h"
#include "hal_timer.h"
#include "hal_uart.h"

#include "transq_msg.h"
#include "hal_cmu.h"
#include "app_utils.h"
//#include "tg_ota.h"
#include "beswifi.h"
#include "hal_trace.h"
#include "k_arch.h"
#include "k_proc.h"

#define HAL_TEST_SUPPORT
#ifdef HAL_TEST_SUPPORT
#define _HAL_TEST_ADC (1<<0)
#define _HAL_TEST_PWM (1<<1)
#define _HAL_TEST_SPI (1<<2)
#define _HAL_TEST_I2C (1<<3)
#define _HAL_TEST_UART (1<<4)

extern unsigned int _ram_start[];
extern unsigned int _ram_end[];
extern unsigned int _ramx_start[];
extern unsigned int _ramx_end[];
extern unsigned int _psram_start[];
extern unsigned int _psram_end[];
extern unsigned int _ramcp_start[];
extern unsigned int _ramcp_end[];
extern unsigned int _ramcpx_start[];
extern unsigned int _ramcpx_end[];
extern unsigned int _ramext_start[];
extern unsigned int _ramext_end[];
extern unsigned int _flash_start[];
extern unsigned int _flash_end[];
extern unsigned int _flashx_start[];
extern unsigned int _flashx_end[];

uart_dev_t uart_0;

void do_hal_test(int test_suite)
{
    if (test_suite&_HAL_TEST_PWM) {
        _hal_pwm_test();
    }
    if (test_suite&_HAL_TEST_ADC) {
        _hal_adc_test();
    }
    if (test_suite&_HAL_TEST_SPI) {
        _hal_spi_test();
    }
    if (test_suite&_HAL_TEST_I2C) {
        _hal_i2c_test();
    }
    if (test_suite&_HAL_TEST_UART) {
        _hal_uart_test();
    }
}
#endif

//#define VFS_TEST_SUPPORT
#ifdef VFS_TEST_SUPPORT
//#define _VFS_TEST_ADC (1<<0)
#define _VFS_TEST_PWM (1<<1)
#define _VFS_TEST_UART (1<<2)
#define _VFS_TEST_I2C (1<<3)
#define _VFS_TEST_RTC (1<<4)

void do_vfs_test(int test_suite)
{
    printf("%s %d\r\n",__FUNCTION__,test_suite);
    //if (test_suite&_VFS_TEST_ADC) {
    //    _vfs_adc_test();
    //}
    if (test_suite&_VFS_TEST_PWM) {
        _vfs_pwm_test();
    }
    if (test_suite&_VFS_TEST_UART) {
        _vfs_uart_test();
    }
    if (test_suite&_VFS_TEST_I2C) {
        _vfs_i2c_test();
    }
    if (test_suite&_VFS_TEST_RTC) {
        _vfs_rtc_test();
    }
}
#endif

#define WRAP_P_BUF_LEN 2048
uint8_t _wrap_p_buf[WRAP_P_BUF_LEN];
int max_p_size = 0;
int __wrap_printf(const char *fmt, ...)
{
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
        max_p_size = (len>max_p_size) ? len : max_p_size;
    }

    if ((len>2) && (p[len-2]!='\r') && (p[len-1]=='\n')) {
        len -= 1;
        crlf = 1;
    }
    hal_trace_output_block((const unsigned char *)p, len);
    if (crlf) {
        hal_trace_output_block("\r\n", 2);
    }

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

/* for aos_printf in rhino.c */
int hal_uart_print(const char *buf, int size)
{
    uart_dev_t uart_stdio;

    char *outbuf = (char *)buf;
    int   wbytes = size;
    int   i      = 0;

    if (buf == NULL) {
        return 0;
    }

    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = 0;

    while(wbytes-- > 0) {
        if (*(outbuf + i) == '\n') {
            hal_uart_send(&uart_stdio, "\r", 1, AOS_WAIT_FOREVER);
        }
        hal_uart_send(&uart_stdio, outbuf + i, 1, AOS_WAIT_FOREVER);
        i++;
    }
    return size;
}

char uart_input_read(void)
{
    return hal_uart_blocked_getc(0);
}

void alios_cli_panic_hook(void)
{
    hal_panic_uart_open();
}

uint32_t __intlock_userspace_save(void)
{
    uint32_t old_basepri = __get_BASEPRI();
    __set_BASEPRI(((IRQ_PRIORITY_HIGHPLUSPLUS << (8 - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL));
    return old_basepri;
}

void __intlock_userspace_restore(uint32_t pri)
{
    __set_BASEPRI(pri);
}

//#define MCU_PSRAMUHS_SIZE 0x000000 // sync with target.mk

/*  check pc available  0:available  other:not available */
extern uint32_t __sram_text_start__[];
extern uint32_t __sram_text_end__[];
extern uint32_t __flashx_text_start__[];
extern uint32_t __flashx_text_end__[];
#if (MCU_PSRAMUHS_SIZE)
extern uint32_t __psramux_text_start__[];
extern uint32_t __psramux_text_end__[];
#endif

int alios_debug_pc_check(char *pc)
{
    if ( (((uint32_t)pc > (uint32_t)__sram_text_start__) &&
          ((uint32_t)pc < (uint32_t)__sram_text_end__))
    || (((uint32_t)pc > (uint32_t)__flashx_text_start__) &&
          ((uint32_t)pc < (uint32_t)__flashx_text_end__))
#if (MCU_PSRAMUHS_SIZE)
    || (((uint32_t)pc > (uint32_t)__psramux_text_start__) &&
          ((uint32_t)pc < (uint32_t)__psramux_text_end__))
#endif
          ) {
        return 0;
    } else {
        return -1;
    }
}

void alios_debug_pc_show(int argc, char **argv)
{
    cli_printf("----- [kernel] PC Addr ------\r\n");
    cli_printf("addr 1 : 0x%08x ~ 0x%08x\r\n", (uint32_t)__sram_text_start__, (uint32_t)__sram_text_end__);
    cli_printf("addr 2 : 0x%08x ~ 0x%08x\r\n", (uint32_t)__flashx_text_start__, (uint32_t)__flashx_text_end__);
#if (MCU_PSRAMUHS_SIZE)
    cli_printf("addr 3 : 0x%08x ~ 0x%08x\r\n", (uint32_t)__psramux_text_start__, (uint32_t)__psramux_text_end__);
#endif

#if (RHINO_CONFIG_USER_SPACE > 0)
    uint32_t     pid;
    app_t       *app;
    preamble_t  *preamble;
    uintptr_t    addr_start, addr_end;

    extern app_info_t g_app;

    for (pid = 1; pid <= MAX_APP_BINS; pid++) {
        app = &(g_app.procs[pid]);
        if (app->pid == 0) {
            break; //pid find end
        }
        preamble   = app->preamble;
        addr_start = (uintptr_t)(preamble->flash_start);
        addr_end   = (uintptr_t)(preamble->flash_end);

        cli_printf("\r\n-----[APP:%s - pid:%d] PC Addr ------\r\n", preamble->app_name, pid);
        cli_printf("addr: 0x%08x ~ 0x%08x\r\n", (uint32_t)addr_start, (uint32_t)addr_end);
    }
#endif
}

#ifdef AOS_COMP_LITTLEFS
#include "fs/littlefs.h"

int32_t littlefs_fetch_cfg_param(struct littlefs_cfg_param * cfg_param)
{
    cfg_param->block_size = 4096;
#ifdef CFG_HW_ALI_MODULE
    cfg_param->block_count = 520;
#else
    cfg_param->block_count = 1192;
#endif
    cfg_param->prog_size = 1024;
    cfg_param->read_size = 256;
    cfg_param->block_cycles = 1000;
    cfg_param->cache_size = 1024;
    cfg_param->lookahead_size = 16;
    return 0;
}
#endif /* AOS_COMP_LITTLEFS */

#ifdef __A7_DSP_ENABLE__
#include "a7_cmd.h"
#include "aud_dump.h"
void mcu_first_handshake(void)
{
    transq_msg_onoff(1);
    A7_CMD_T a7_cmd;
    a7_cmd.type = A7_CMD_TYPE_HANDSHAKE;
    a7_cmd.p1 = (ota_adapt_get_odm_type()==ALI_C5A01)?48000:16000;
    a7_cmd.p2 = (ota_adapt_get_odm_type()==ALI_C5A01)?48:64;
    mcu_cmd_send(&a7_cmd);
    printf("%s A7_CMD_TYPE_HANDSHAKE to a7: mic_samplerate %d, period_ms %d\n", __FUNCTION__, a7_cmd.p1, a7_cmd.p2);
}
#endif

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

void aos_uart_init(void)
{
    uart_0.port = 0;
    uart_0.config.baud_rate = 1500000;
    uart_0.config.data_width = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.mode = MODE_TX_RX;
    uart_0.config.parity = NO_PARITY;
    uart_0.config.stop_bits = STOP_BITS_1;

    hal_uart_init(&uart_0);
}

/* If the address form addr to addr +len overrides the kernel's address space,
 * return true, otherwise return false */
bool hal_addr_overlap_kernel(void *addr, size_t len)
{
    char *addr_start = addr;
    char *addr_end   = addr + len;

    if (!((addr_start < (char *)&_ram_end) || (addr_end > (char *)&_ram_start))) {
        return true;
    }

    if (!((addr_start < (char *)&_ramx_end) || (addr_end > (char *)&_ramx_start))) {
        return true;
    }

    if (!((addr_start < (char *)&_psram_end) || (addr_end > (char *)&_psram_start))) {
        return true;
    }

    if (!((addr_start < (char *)&_ramcp_end) || (addr_end > (char *)&_ramcp_start))) {
        return true;
    }

    if (!((addr_start < (char *)&_ramcpx_end) || (addr_end > (char *)&_ramcpx_start))) {
        return true;
    }

    if (!((addr_start < (char *)&_ramext_end) || (addr_end > (char *)&_ramext_start))) {
        return true;
    }

    if (!((addr_start < (char *)&_flash_end) || (addr_end > (char *)&_flash_start))) {
        return true;
    }

    if (!((addr_start < &_flashx_end) || (addr_end > &_flashx_start))) {
        return true;
    }

    return false;
}

void soc_peripheral_init(int wifi_init)
{
    app_sysfreq_req(APP_SYSFREQ_USER_APP_15, APP_SYSFREQ_390M);
    aos_uart_init();
    printf("sys freq calc : %d, wifi init %d \n", hal_sys_timer_calc_cpu_freq(5, 0), wifi_init);
}

void aos_trace_notify(enum HAL_TRACE_STATE_T state)
{
    if (state == HAL_TRACE_STATE_CRASH_END)
        abort();
}

extern void heartbeat_init(void);
extern int aos_printf_hook(const char *tag, const char *fmt, enum PRINTF_FLAG_T flag, va_list ap);
void aos_init_done_hook(void)
{
#ifdef SWD_ENABLE_AS_DEFAULT
    // enable swd as default
    hal_iomux_set_jtag();
    krhino_idle_hook_onoff(0);
#endif

#ifdef __A7_DSP_ENABLE__
    //heartbeat_init();
    bes_kv_init();
#endif
    hal_trace_register_hook(aos_printf_hook);
    hal_trace_app_register(aos_trace_notify, NULL);

#if 0//def __A7_DSP_ENABLE__
    // for dsp audiodump
    aud_dump_init();
    mcu_audiodump_register(_AUDIO_DUMP_SRC_BEFORE_ALG_, aud_dump_cb_register, aud_dump_cb_unregister, 3, 256, 3000);
    mcu_audiodump_register(_AUDIO_DUMP_SRC_INTER_ALG_, aud_dump_cb_register, aud_dump_cb_unregister, 1, 16, 10000);
    mcu_audiodump_register(_AUDIO_DUMP_SRC_AFTER_ALG_, aud_dump_cb_register, aud_dump_cb_unregister, 1, 16, 10000);
#endif

#ifdef HAL_TEST_SUPPORT
    /*do_hal_test(_HAL_TEST_UART);*/
    /*do_hal_test(_HAL_TEST_PWM);*/
    /*do_hal_test(_HAL_TEST_I2C);*/
    /*do_hal_test(_HAL_TEST_SPI);*/
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

#ifdef __A7_DSP_ENABLE__
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