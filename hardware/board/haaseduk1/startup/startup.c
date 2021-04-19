#include "aos/init.h"
#include "board.h"
#include "k_config.h"
#include "stdio.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
// #include "aos/yloop.h"
#ifdef AOS_COMP_YUNIT
#include "yts.h"
#endif
#include "ota_port.h"

// #include "apps.h"
#include "hal_trace.h"
#include "led.h"

/*
 * If enable this macro for local test, we don't use wifi service to trigger
 * connection, remember to mask -DALIOS_WIFI_SERVICE and set -DLWIP_SUPPORT=1 in
 * SDK.
 */
#if LWIP_SUPPORT
#define ALIOS_BES_APP
#endif

#define BES_SDK          0
#define BES_WIFI_ONLY    1
#define BES_WIFI_BT      2
#define BES_WIFI_BT_MINI 3

/*
 * main task stask size(byte)
 */
#define OS_MAIN_TASK_STACK (32 * 1024)
#define OS_MAIN_TASK_PRI   32

/*  For user config
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 0;
*/
static kinit_t  kinit = {0, NULL, 1};
static ktask_t *g_main_task;
static ktask_t *g_speech_task       = NULL;
static ktask_t *g_dsp_task          = NULL;
uint8_t         mesh_open_enalbe    = 1;
uint32_t        g_wifi_factory_test = 0;

extern void     soc_peripheral_init();
extern void     aos_init_done_hook(void);
extern void     ble_nosignal_uart_bridge_loop(void);
extern uint8_t  ble_nosignal_start_flag;
extern int32_t  tg_bt_hal_vendor_hci_set_epta(int32_t state);
extern uint8_t  epta_hci_set_flag;
extern void     a7_heartbeat_reboot_enable(int enable);
extern uint32_t __aos_lastword_start[];

#ifndef AOS_BINS
extern int application_start(int argc, char **argv);
#endif

#ifdef ENABLE_LATTICE
#include "ali_led.h"
#include "ali_sensor.h"
void ali_lattice_cli_init();
#endif

#ifdef CONFIG_ALI_DISPLAY
void disp_init(void);
#endif

static void haas_board_init(void)
{
#if CONFIG_A7_DSP_ENABLE
    int init = BES_WIFI_BT;
#else
    int init = BES_WIFI_BT_MINI;
#endif
    int release_version = 1;
    int ret             = 0;

#if 1 // def CONFIG_GENIE_DEBUG disable watchdog
    release_version = 0;
#endif

    platform_init_step0(release_version);
    printf("%s platform_init_step0 done\n", __func__);
#if CONFIG_A7_DSP_ENABLE
    analog_aud_codec_set_dev(1); // haas use spk-L
#endif
    platform_init_step1(init);
    printf("%s platform_init_step1 done\n", __func__);

    ASSERT(DEBUG_LASTWORD_RAM_ADDR == (uint32_t)__aos_lastword_start,
           "DEBUG_LASTWORD_RAM_ADDR(0x%x) is not equel to "
           "__aos_lastword_start(0x%x)",
           DEBUG_LASTWORD_RAM_ADDR, (uint32_t)__aos_lastword_start)

    led_switch(1, LED_ON);
    led_switch(2, LED_ON);
    led_switch(3, LED_ON);

#if CONFIG_A7_DSP_ENABLE
#ifndef CONFIG_GENIE_DEBUG
    a7_heartbeat_reboot_enable(1);
#endif
#endif

    if (init == BES_SDK) {
        krhino_task_sleep(1000);
    }
}

static void a7_dsp_init(void)
{
    if (!app_enter_factory_wifi_test()) {
        printf("now a7_dsp_boot\n");
        a7_dsp_boot();
    } else {
        printf("app_enter_factory_wifi_test, no a7_dsp_boot\n");
    }
}

static void factory_rf_test(void)
{
    kinit_t kinit = {0, NULL, 1};
    printf("wait factory test now...\n");
    board_stduart_init();
    aos_components_init(&kinit);
#ifdef AOS_VENDOR_CLI
    vendor_cli_register_init();
#endif
    while (1) {
        aos_msleep(1000);
    }
}

static void aos_main_task_entry(void)
{
#ifdef AOS_COMP_YUNIT
    char *parm[4] = {"yts", "kv", "ramfs", "vfs"};
#endif

    /* user code start */
    soc_peripheral_init();

    if (app_enter_factory_wifi_test()) {
        printf("***************************************************************"
               "*****\n");
        printf("Begin wifi factory test ...\n");
        printf("***************************************************************"
               "*****\n");
        g_wifi_factory_test = 1;
    }

    // ch395_device_dereset();

#ifdef AOS_COMP_CPLUSPLUS
    cpp_init();
#endif

#ifdef AOS_COMP_OSAL_POSIX
    posix_init();
#endif

#ifdef AOS_COMP_YUNIT
    yts_run(sizeof(parm) / sizeof(parm[0]), &parm[0]);
#endif

#ifdef ENABLE_LATTICE
    ali_lattice_cli_init();
#endif

#ifdef CONFIG_ALI_DISPLAY
    disp_init();
#endif

    aos_init_done_hook();

#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif

#ifndef AOS_BINS
    if (app_enter_factory_wifi_test() == false) {
        aos_maintask();
    } else {
        factory_rf_test();
    }
#endif
}

int main(void)
{
    int times = 0;
    /*irq initialized is approved here.But irq triggering is forbidden, which
    will enter CPU scheduling. Put them in sys_init which will be called after
    aos_start. Irq for task schedule should be enabled here, such as PendSV for
    cortex-M4.
    */
    haas_board_init(); // including aos_heap_set();  flash_partition_init();

    /*kernel init, malloc can use after this!*/
    // krhino_init();

    /*main task to run */
    krhino_task_dyn_create(&g_main_task, "main_task", 0, OS_MAIN_TASK_PRI, 0,
                           OS_MAIN_TASK_STACK,
                           (task_entry_t)aos_main_task_entry, 1);

    /*kernel start schedule!*/
    // krhino_start();

    while (1) {
        krhino_task_sleep(100);

        if (times == 10) {
            led_switch(1, LED_OFF);
            led_switch(2, LED_OFF);
            led_switch(3, LED_OFF);
            times ++;
        } else {
            if (times < 11) {
                times++;
            }
        }
#ifdef AOS_VENDOR_CLI
        /*only work when enter ble nosignal*/
        if (ble_nosignal_start_flag) {
            ble_nosignal_uart_bridge_loop();
        }
#endif
        if (epta_hci_set_flag) {
            printf("%d epta_hci_set_flag\n", epta_hci_set_flag);
            tg_bt_hal_vendor_hci_set_epta(epta_hci_set_flag);
            epta_hci_set_flag = 0;
        }
    }

    /*never run here*/
    return 0;
}
