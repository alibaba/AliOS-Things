/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <aos/aos.h>
#include <aos/kernel.h>

#include <hal/soc/soc.h>

#include <yunit.h>
#include <yts.h>
#include <hal/base.h>
#include <hal/wifi.h>


static void test_timer_cb(void *arg)
{
    int *pc = arg;
    (*pc) ++;
}

static void test_timer_case(void)
{
    int counter = 0, old_counter;
    timer_dev_t t;
    t.config.period = 50000;
    t.config.reload_mode = TIMER_RELOAD_AUTO;
    t.config.cb = &test_timer_cb;
    t.config.arg = &counter;

    hal_timer_init(&t);
    aos_msleep(1000);
    YUNIT_ASSERT(counter == 0);

    hal_timer_start(&t);
    check_cond_wait(counter > 3, 2);

    hal_timer_stop(&t);
    aos_msleep(1000);

    old_counter = counter;
    aos_msleep(1000);
    YUNIT_ASSERT(counter == old_counter);
    if (counter != old_counter)
        printf("%s %d %d\n", __func__, counter, old_counter);
}


static uint8_t fixmac[6] = {0xd8,0x96,0xe0,0x03,0x04,0x01};

static int wifi_init(hal_wifi_module_t *m)
{
    printf("wifi init success!!\n");
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    (void)m;
    printf("wifi_get_mac_addr!!\n");
    memcpy(mac, fixmac, 6);
};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    (void)init_para;

    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    (void)init_para_adv;
 
    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    (void)wifi_type;

    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{
   
}

static void start_scan_adv(hal_wifi_module_t *m)
{
}

static int power_off(hal_wifi_module_t *m)
{
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{

    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    return 0;
}

static void start_monitor(hal_wifi_module_t *m)
{

}

static void stop_monitor(hal_wifi_module_t *m)
{

}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{

}


static hal_wifi_module_t sim_aos_wifi_module = {
    .base.name           = "sim_aos_wifi_module",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  start_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
};

static void test_wifi_case(void)
{
    uint8_t mac[6];
    hal_wifi_module_t *tmp;

    printf("start wifi test case\n");

    tmp = hal_wifi_get_default_module();
    (void)tmp;

    hal_wifi_register_module(&sim_aos_wifi_module);
    hal_wifi_init();
    hal_wifi_get_mac_addr(&sim_aos_wifi_module, mac);
    hal_wifi_start(&sim_aos_wifi_module, NULL);
    hal_wifi_start_adv(&sim_aos_wifi_module, NULL);
    hal_wifi_get_ip_stat(&sim_aos_wifi_module, NULL, SOFT_AP);
    hal_wifi_get_link_stat(&sim_aos_wifi_module, NULL);
    hal_wifi_start_scan(&sim_aos_wifi_module);
    hal_wifi_start_scan_adv(&sim_aos_wifi_module);
    hal_wifi_power_off(&sim_aos_wifi_module);
    hal_wifi_power_on(&sim_aos_wifi_module);
    hal_wifi_suspend(&sim_aos_wifi_module);
    hal_wifi_suspend_station(&sim_aos_wifi_module);
    hal_wifi_suspend_soft_ap(&sim_aos_wifi_module);
    hal_wifi_set_channel(&sim_aos_wifi_module, 0);
    hal_wifi_start_wifi_monitor(&sim_aos_wifi_module);
    hal_wifi_stop_wifi_monitor(&sim_aos_wifi_module);
    hal_wifi_register_monitor_cb(&sim_aos_wifi_module, NULL);
    printf("first mac addr is 0x%x\n", mac[0]);
}

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_wifi_hal_testcases[] = {
    { "timer", test_timer_case },
    { "wifi",  test_wifi_case },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "hal", init, cleanup, setup, teardown, aos_wifi_hal_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_wifi_hal(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_wifi_hal);
