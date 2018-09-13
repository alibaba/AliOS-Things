/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <hal/wifi.h>
#include <string.h>

#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "espressif/c_types.h"
#include "espressif/esp_libc.h"
#include "espressif/esp_wifi.h"

#include "airkiss.h"

// airkiss 状态回调函数
typedef void (*airkiss_cb_fn)(AIR_KISS_STATE state, void *pdata);

void start_airkiss(airkiss_cb_fn airkiss_done);
static void start_scan(void);
static void udp_send_random(uint8_t num);
static void channel_change_action(void *arg);

// 当前监听的无线信道
uint8_t cur_channel = 1;
uint8_t wifi_ssid_crc;
uint8_t airkiss_random_num;
char wifi_ssid[32 + 1];          /* SSID got form airkiss */
char wifi_pwd[64 + 1];           /* password got form airkiss */

// 信道锁定标志
uint8_t airkiss_channel_locked = 0;

// Airkiss 过程中需要的 RAM 资源，完成 Airkiss 后可释放
airkiss_context_t *akcontexprt;

// 定义 Airkiss 库需要用到的一些标准函数，由对应的硬件平台提供，前三个为必要函数
const airkiss_config_t akconf = {
    (airkiss_memset_fn)&memset,
    (airkiss_memcpy_fn)&memcpy,
    (airkiss_memcmp_fn)&memcmp,
    (airkiss_printf_fn)&printf 
};

airkiss_cb_fn airkiss_cb = NULL;
hal_wifi_init_type_t type;

extern hal_wifi_module_t aos_wifi_esp8266;

uint8_t crc8_chk_value(uint8_t *str)
{
    uint8_t crc = 0;
    uint8_t i;

    while(*str != '\0')
    {
        crc ^= *str++;
        for(i = 0; i < 8; i++)
        {
            if(crc & 0x01)
                crc = (crc >> 1) ^ 0x8c;
            else 
                crc >>= 1;
        }
    }
    
    return crc;
}

// wifi hal 相关回调函数定义
static void wifi_connect_fail(hal_wifi_module_t *m, int err, void *arg)
{
    printf("wifi connect fail: errcode = %d\r\n", err);
}

static void wifi_ip_got(hal_wifi_module_t *m, hal_wifi_ip_stat_t *pnet, void *arg)
{
    // printf("wifi got ip: %s\r\n", pnet->ip);
    udp_send_random(airkiss_random_num);
}

static void wifi_stat_chg(hal_wifi_module_t *m, hal_wifi_event_t stat, void *arg)
{
    printf("wifi disconnect: NOTIFY_STATION_DOWN\r\n");
}

static void wifi_scan_adv_compeleted(hal_wifi_module_t *m, hal_wifi_scan_result_adv_t *result, void *arg)
{
    // TO DO
    return ;
}

static void wifi_para_chg(hal_wifi_module_t *m, hal_wifi_ap_info_adv_t *ap_info, char *key, int key_len, void *arg)
{
    // TO DO
    return ;
}

static void wifi_fatal_err(hal_wifi_module_t *m, void *arg)
{
    // TO DO
    return ;
}

static void udp_send_random(uint8_t num)
{
    uint8_t i;
    uint8_t *msg = &num;
    struct udp_pcb *ptel_pcb;
    struct pbuf *p;

    ptel_pcb = (struct udp_pcb *)udp_new();

    udp_bind(ptel_pcb, IP_ADDR_ANY, 1234);

    for (i = 0; i < 30; i++)
    {
        // Allocate packet buffer
        p = pbuf_alloc(PBUF_TRANSPORT, sizeof(uint8_t), PBUF_RAM);
        memcpy (p->payload, msg, sizeof(uint8_t));
        udp_sendto(ptel_pcb, p, IP_ADDR_BROADCAST, 10000);
        pbuf_free(p); //De-allocate packet buffer
        aos_msleep(100);  // some delay
    }

    airkiss_cb(AIRKISS_STATE_COMPLETE, NULL);   
}

static void wifi_scan_compeleted(hal_wifi_module_t *m, hal_wifi_scan_result_t *result, void *arg)
{
    int8_t i;
    
    for (i = 0; i < result->ap_num; i++)
    {       
        if (wifi_ssid_crc == crc8_chk_value(result->ap_list[i].ssid))
        {
            stpcpy(wifi_ssid, result->ap_list[i].ssid);
        }
    }   
    
    if (*wifi_ssid != '\0')
    {
        strncpy(type.wifi_ssid, wifi_ssid, sizeof(type.wifi_ssid) - 1);
        strncpy(type.wifi_key, wifi_pwd, sizeof(type.wifi_key) - 1);
        airkiss_cb(AIRKISS_STATE_GETTING_SSID_PSWD, &type);
        
        printf("T|pswd : %s\r\n", wifi_ssid);
        printf("T|ssid : %s\r\n", wifi_pwd);
    }
    else
    {
        start_scan();  // scan again
    }
        
}

// wifi 事件回调函数
const hal_wifi_event_cb_t wifi_event_cb = {
    &wifi_connect_fail,
    &wifi_ip_got,
    &wifi_stat_chg,
    &wifi_scan_compeleted,
    &wifi_scan_adv_compeleted,
    &wifi_para_chg,
    &wifi_fatal_err
};
    
// 用于切换信道的定时任务
static void channel_change_action(void *arg)
{
    if (!airkiss_channel_locked)
    {
        // 切换信道
        if (cur_channel >= 13)
            cur_channel = 1;
        else
            cur_channel++;
        
        hal_wifi_set_channel(&aos_wifi_esp8266, cur_channel);
        airkiss_change_channel(akcontexprt);
        aos_post_delayed_action(100, channel_change_action, NULL);
    }
}

static void airkiss_finish(void)
{
    int8_t err;
    uint8 buffer[256];
    airkiss_result_t result;
    err = airkiss_get_result(akcontexprt, &result);
    
    if (err == 0)
    {
        stpcpy(wifi_pwd, result.pwd);
        wifi_ssid_crc = result.reserved;
        airkiss_random_num = result.random;
    }
    else
    {
        printf("AIRKISS_STATUS_GETTING_PSWD_FAILED\r\n");
    }
    
    aos_free(akcontexprt);
    start_scan();
}

static void wifi_promiscuous_rx(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    int8_t ret;
    
    ret = airkiss_recv(akcontexprt, data, len);
    
    if (ret == AIRKISS_STATUS_CHANNEL_LOCKED)
    {
        airkiss_channel_locked = 1;
        airkiss_cb(AIRKISS_STATE_FIND_CHANNEL, NULL);
        printf("T|LOCK CHANNEL : %d\r\n", cur_channel);
    }
    else if (ret == AIRKISS_STATUS_COMPLETE)
    {
        hal_wifi_stop_wifi_monitor(&aos_wifi_esp8266);
        airkiss_finish();
    }
}

static void start_scan(void)
{
    wifi_set_opmode(STATION_MODE);
    hal_wifi_install_event(&aos_wifi_esp8266, &wifi_event_cb);
    hal_wifi_start_scan(&aos_wifi_esp8266);
}

void start_airkiss(airkiss_cb_fn airkiss_done)
{
    int8_t ret;
    
    airkiss_cb = airkiss_done;
    akcontexprt = (airkiss_context_t*)aos_malloc(sizeof(airkiss_context_t));
    
    // 初始化 Airkiss 流程，每次调用该接口，流程重新开始
    ret = airkiss_init(akcontexprt, &akconf);
    if (ret < 0)
    {
        printf("Airkiss init failed!\r\n");
        return;
    }
    
    // 开始抓包
    cur_channel = 1;
    airkiss_channel_locked = 0;
    hal_wifi_set_channel(&aos_wifi_esp8266, cur_channel);
    hal_wifi_register_monitor_cb(&aos_wifi_esp8266, wifi_promiscuous_rx);
    hal_wifi_start_wifi_monitor(&aos_wifi_esp8266);
    aos_post_delayed_action(100, channel_change_action, NULL);
    airkiss_cb(AIRKISS_STATE_WAIT, NULL);
}

