#include "rda_sys_wrapper.h"
#include "rda_def.h"
#include "rda59xx_daemon.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "maclib_task.h"
#include "rda59xx_lwip.h"
#include "dhcps.h"

//#define WIFISTACK_DEBUG
#ifdef WIFISTACK_DEBUG
#define WIFISTACK_PRINT(fmt, ...) do {\
            printf(fmt, ##__VA_ARGS__);\
    } while (0)
#else
#define WIFISTACK_PRINT(fmt, ...)
#endif

//debug info
//dbg level, 0 ~ close 1 ~ error 2 ~ info 3 ~ debug
//dump, 0 ~ close 1 ~ open
r_u32 rda_wland_dbg_level = 2;
r_u32 rda_wpa_dbg_level = 0;
r_u32 rda_maclib_dbg_level = 0;
r_u32 rda_wland_dump = 0;
r_u32 rda_wpa_dump = 0;
r_u32 rda_hut_dump = 0;

#define SCAN_TIMES          5
#define RECONN_TIMES        3
#define DAEMON_MAILQ_SIZE   10

r_void *daemon_queue = NULL;
r_u32 module_state = STATE_DSIABLE;
static rda59xx_sta_info r_sta_info;
static rda59xx_bss_info r_bss_info;
static rda59xx_ap_info r_ap_info;
static wifi_event_cb_t wifi_event_cb = NULL;
sys_sem_t lwip_sta_netif_linked;
sys_sem_t lwip_sta_netif_has_addr;
sys_sem_t lwip_ap_netif_linked;
struct netif lwip_sta_netif;
struct netif lwip_ap_netif;

r_s32 rda59xx_send_wland_msg(rda_msg *msg, r_u32 wait_time)
{
    r_s32 res;
    r_void *msg_sem = NULL;
    
    msg_sem = rda_sem_create(0);
    msg->arg3 = (r_u32)msg_sem;
    res = rda_queue_send(wland_queue, (r_u32)msg, 1000);
    res = rda_sem_wait(msg_sem, wait_time);
    res = rda_sem_delete(msg_sem);

    return res;
}


static r_s32 rda59xx_send_daemon_msg(rda_msg *msg, r_u32 wait_time)
{
    r_s32 res;
    r_void *msg_sem = NULL;
    
    msg_sem = rda_sem_create(0);
    msg->arg3 = (r_u32)msg_sem;
    res = rda_queue_send(daemon_queue, (r_u32)msg, 1000);
    res = rda_sem_wait(msg_sem, wait_time);
    res = rda_sem_delete(msg_sem);

    return res;
}

r_void rda59xx_get_macaddr(r_u8 *macaddr, r_u32 mode)
{
    macaddr[0] = 0xD6;
    macaddr[1] = 0x71;
    macaddr[2] = 0x36;
    macaddr[3] = 0x60;
    macaddr[4] = 0xD8;
    macaddr[5] = 0xF0;
    
    if(mode == 1){
        if(macaddr[0] & 0x04)
           macaddr[0] &= 0xFB;
        else
           macaddr[0] |= 0x04;
    }
    return;
}

r_u32 rda59xx_get_module_state()
{  
    return module_state;
}


r_void rda59xx_sta_get_bss_info(rda59xx_bss_info *bss_info)
{
    r_memcpy(bss_info, &r_bss_info, sizeof(rda59xx_bss_info));
    return;
}

static r_s32 rda59xx_sta_init(struct netif *netif)
{
    static r_u32 init_flag = 0;
    if(init_flag == 1)
        return R_NOERR;
    
    sys_sem_new(&lwip_sta_netif_linked, 0);
    sys_sem_new(&lwip_sta_netif_has_addr, 0);
    r_memset(&lwip_sta_netif, 0, sizeof(struct netif));
    if (!netif_add(&lwip_sta_netif,
#if LWIP_IPV4
                0, 0, 0,
#endif
                NULL, rda59xx_sta_netif_init, tcpip_input)) {
            return ERR_DEVICE;
    }
    init_flag = 1;
    return R_NOERR;
}

static r_s32 rda59xx_ap_init(struct netif *netif)
{
    static r_u32 init_flag = 0;
    if(init_flag == 1)
        return R_NOERR;
    
    sys_sem_new(&lwip_ap_netif_linked, 0);
    r_memset(&lwip_ap_netif, 0, sizeof(struct netif));
    if (!netif_add(&lwip_ap_netif,
#if LWIP_IPV4
                0, 0, 0,
#endif
                NULL, rda59xx_ap_netif_init, tcpip_input)) {
            return ERR_DEVICE;
    }

    init_flag = 1;
    return R_NOERR;
}

static r_s32 rda59xx_sta_disconnect_internal()
{
    r_s32 res = R_NOERR;
    rda_msg msg;
    r_void *msg_sem = NULL;

    if(r_sta_info.dhcp) {
        dhcp_release(&lwip_sta_netif);
        dhcp_stop(&lwip_sta_netif);
    }

    netif_set_down(&lwip_sta_netif);
    
    msg_sem = rda_sem_create(0);
    msg.type = RDA59XX_WLAND_STA_DISCONNECT;
    res = rda59xx_send_wland_msg(&msg, 1000);
    if(res != R_NOERR){
        WIFISTACK_PRINT("Send disconnect cmd failed!\r\n");
        return R_ERR;
    }
    rda_sem_wait(msg_sem, 2000);
    rda_sem_delete(msg_sem);
    return res;
}

r_s32 rda59xx_sta_disconnect()
{
    rda_msg msg;
    msg.type = DAEMON_STA_DISCONNECT;
    rda59xx_send_daemon_msg(&msg, RDA_WAIT_FOREVER); 
    wifi_event_cb(EVENT_STA_DISCONNECTTED, NULL);
    return 0;
}

static r_s32 rda59xx_sta_connect_internal(rda59xx_sta_info *sta_info)
{
    r_s32 res = R_NOERR, res_t = R_NOERR;
    r_u8 reconn = 0, scan_times = 0, scan_res = 0, msg_type = 0;
    r_s32 index = 0;
    rda_msg msg;
    rda59xx_scan_info r_scan_info;
    rda59xx_scan_result ap;

    r_memcpy(&r_sta_info, sta_info, sizeof(rda59xx_sta_info));
    
    //scan
    r_memset(&r_scan_info, 0, sizeof(rda59xx_scan_info));
    r_memcpy(&(r_scan_info.SSID), &(r_sta_info.ssid), r_strlen(r_sta_info.ssid));
    r_scan_info.channel = r_sta_info.channel;
    r_scan_info.scan_mode = 1;
    r_scan_info.SSID_len = r_strlen(r_sta_info.ssid);
    rda59xx_del_scan_all_result();
    while (scan_times++ < SCAN_TIMES) {
        scan_res = rda59xx_scan_internal(&r_scan_info);
        index = rda59xx_get_scan_result_special(&ap, r_sta_info.ssid, r_sta_info.bssid, r_sta_info.channel);
        if(index == 0) {
           break;
        }
    }

    if(index == R_ERR) {
        WIFISTACK_PRINT("Can't find the special AP\r\n");
        return R_ERR;
    }
    //connect
    while(1){
        msg.type = RDA59XX_WLAND_STA_CONNECT;
        msg.arg1 = (r_u32)(&r_sta_info);
        msg.arg2 = (r_u32)&ap;
        res = rda_queue_send(wland_queue, (r_u32)&msg, 2000);
        if(res != R_NOERR){
            WIFISTACK_PRINT("Send connect cmd failed!\r\n");
            return ERR_SEND_MSG;
        }

        if (!netif_is_link_up(&lwip_sta_netif)) {
            res_t = sys_arch_sem_wait(&lwip_sta_netif_linked, 60000);
            if (res_t == SYS_ARCH_TIMEOUT) {
                res = ERR_CONNECTION;
                goto reconn;
            }

            if(!netif_is_link_up(&lwip_sta_netif)){
                res = ERR_AUTH;
                goto reconn;
            }
        }

        netif_set_up(&lwip_sta_netif);
        
        if (r_sta_info.dhcp) {
            res_t = dhcp_start(&lwip_sta_netif);
            if (res_t) {
                res = ERR_DHCP;
                goto reconn;
            }
        }else{
            netif_set_addr(&lwip_sta_netif, &(r_sta_info.ip), &(r_sta_info.netmask), &(r_sta_info.gateway));
        }
        
        if (!rda59xx_get_netif_ip(&lwip_sta_netif)) {
            res_t = sys_arch_sem_wait(&lwip_sta_netif_has_addr, 20000);
            if (res_t == SYS_ARCH_TIMEOUT) {
                res = ERR_DHCP;
            }
        }
reconn:   
        if(res == R_NOERR){
            WIFISTACK_PRINT("Connect successful!\r\n");
            rda59xx_set_data_rate(0, 0);
            r_memcpy(&r_bss_info, &ap, 6+33+1+1+1);//bssid, ssid, channel, secure type, RSSI
            r_bss_info.ipaddr = lwip_sta_netif.ip_addr.addr;
            r_bss_info.mask = lwip_sta_netif.netmask.addr;
            r_bss_info.gateway = lwip_sta_netif.gw.addr;
            r_memcpy(&r_bss_info.dns1, dns_getserver(0), sizeof(r_u32));
            r_memcpy(&r_bss_info.dns2, dns_getserver(1), sizeof(r_u32));
            wifi_event_cb(EVENT_STA_CONNECTTED, NULL);
            wifi_event_cb(EVENT_STA_GOT_IP, NULL);
            break;
        }else{
            rda59xx_sta_disconnect_internal();
            if(++reconn > RECONN_TIMES){  
                r_memset(&r_sta_info, 0, sizeof(rda59xx_sta_info));
                wifi_event_cb(EVENT_STA_CONNECT_FAIL, NULL);
            }else{
                rda_msleep(100);
                continue;
            }       
        }
    }
    return res;
}

r_s32 rda59xx_sta_connect(rda59xx_sta_info *sta_info)
{
    rda_msg msg;
    msg.type = DAEMON_STA_CONNECT;
    msg.arg1 = (r_u32)sta_info;
    rda59xx_send_daemon_msg(&msg, RDA_WAIT_FOREVER);
    return 0;        
}

r_s32 rda59xx_sta_get_ip(r_u32 ip_addr)
{
    if(module_state & STATE_STA)
        r_memcpy(ip_addr, &r_bss_info.ipaddr, sizeof(r_u32));
    return 0;        
}


static r_s32 rda59xx_ap_enable_internal(rda59xx_ap_info *ap_info)
{
    rda_msg msg;
    r_s32 res = R_NOERR;
    
    r_memcpy(&r_ap_info, ap_info, sizeof(rda59xx_ap_info));    
    res = rda59xx_ap_init(&lwip_ap_netif);
    netif_set_addr(&lwip_ap_netif, (ip_addr_t *)&(r_ap_info.ip), (ip_addr_t *)&(r_ap_info.netmask), (ip_addr_t *)&(r_ap_info.gateway));    

    msg.type = RDA59XX_WLAND_AP_START;
    msg.arg1 = (r_u32)&r_ap_info;
    rda_queue_send(wland_queue, (r_u32)&msg, RDA_WAIT_FOREVER);
    if (!netif_is_link_up(&lwip_ap_netif)) {
        res = sys_arch_sem_wait(&lwip_ap_netif_linked, 6000);
        if (res == SYS_ARCH_TIMEOUT) {
            return ERR_CONNECTION;
        }
    }
    netif_set_up(&lwip_ap_netif);
    dhcps_set_addr_pool(1, (ip_addr_t *)&(r_ap_info.dhcps), (ip_addr_t *)&(r_ap_info.dhcpe));
    dhcps_init(&lwip_ap_netif);

    return res;
}

r_s32 rda59xx_ap_enable(rda59xx_ap_info *ap_info)
{
    rda_msg msg;
    msg.type = DAEMON_AP_ENABLE;
    msg.arg1 = (r_u32)ap_info;
    rda59xx_send_daemon_msg(&msg, RDA_WAIT_FOREVER);

    return 0;        
}

static r_s32 rda59xx_ap_disable_internal()
{
    rda_msg msg;
    r_s32 res = R_NOERR;
        
    netif_set_down(&lwip_ap_netif);
    
    msg.type = RDA59XX_WLAND_AP_STOP;
    rda59xx_send_wland_msg(&msg, 1000);
    dhcps_deinit();

    return res;
}

r_s32 rda59xx_ap_disable()
{
    rda_msg msg;
    msg.type = DAEMON_AP_DISABLE;
    rda59xx_send_daemon_msg(&msg, RDA_WAIT_FOREVER);
    return 0;        
}

r_s32 rda59xx_scan(rda59xx_scan_info *scan_info)
{
    r_u32 scan_num;
    rda59xx_scan_info r_scan_info;
    r_memcpy(&r_scan_info, scan_info, sizeof(rda59xx_scan_info));
    rda_msg msg;
    msg.type = DAEMON_SCAN;
    msg.arg1 = (r_u32)&r_scan_info;
    rda59xx_send_daemon_msg(&msg, RDA_WAIT_FOREVER);
    scan_num = rda59xx_get_scan_num();
    return scan_num;
}

r_s32 rda59xx_sniffer_enable(sniffer_handler_t handler)
{
    rda_msg msg;
    msg.type = DAEMON_SNIFFER_ENABLE;
    msg.arg1 = (r_u32)handler;
    rda59xx_send_daemon_msg(&msg, RDA_WAIT_FOREVER);
    return 0;
}

r_s32 rda59xx_sniffer_disable()
{
    rda_msg msg;
    msg.type = DAEMON_SNIFFER_DISABLE;
    rda59xx_send_daemon_msg(&msg, RDA_WAIT_FOREVER);
    return 0;
}

static r_void rda59xx_daemon(r_void *arg)
{
    rda_msg msg;
    r_s32 res;

    while(1){
        rda_queue_recv(daemon_queue, (r_u32)&msg, RDA_WAIT_FOREVER);
        switch(msg.type)
        {   
            case DAEMON_SNIFFER_ENABLE:
                res = rda59xx_sniffer_enable_internal((sniffer_handler_t)(msg.arg1));
                rda_sem_release((r_void *)msg.arg3);
                module_state |= STATE_SNIFFER;
                break;
            case DAEMON_SNIFFER_DISABLE:
                res = rda59xx_sniffer_disable_internal();
                rda_sem_release((r_void *)msg.arg3);
                module_state &= ~(STATE_SNIFFER);
                break;
            case DAEMON_SCAN:
                res = rda59xx_scan_internal((rda59xx_scan_info *)msg.arg1);
                rda_sem_release((r_void *)msg.arg3);
                break;
            case DAEMON_STA_CONNECT:
                if(module_state & STATE_AP){
                    rda59xx_ap_disable_internal();
                    module_state &= ~(STATE_AP);
                    r_memset(&r_ap_info, 0, sizeof(rda59xx_ap_info));
                }    
                res = rda59xx_sta_connect_internal((rda59xx_sta_info*)msg.arg1);
                rda_sem_release((r_void *)msg.arg3);
                break;
            case DAEMON_STA_DISCONNECT:
                r_memset(&r_sta_info, 0, sizeof(rda59xx_sta_info));
                module_state &= ~(STATE_STA);
                res = rda59xx_sta_disconnect_internal();
                rda_sem_release((r_void *)msg.arg3);
                break;
            case DAEMON_STA_RECONNECT:
                res = rda59xx_sta_disconnect_internal();
                r_memset(&r_bss_info, 0, sizeof(rda59xx_bss_info));
                res = rda59xx_sta_connect_internal(&r_sta_info);
                if(res != R_NOERR){
                    msg.type = DAEMON_STA_RECONNECT;
                    res = rda_queue_send(daemon_queue, (r_u32)&msg, 1000);
                }
                break;
            case DAEMON_AP_ENABLE:
                res = rda59xx_ap_enable_internal((rda59xx_ap_info*)msg.arg1);
                rda_sem_release((r_void *)msg.arg3);
                break;
            case DAEMON_AP_DISABLE:
                r_memset(&r_ap_info, 0, sizeof(rda59xx_ap_info));
                res = rda59xx_ap_disable_internal();
                rda_sem_release((r_void *)msg.arg3);
                break;   
            default:
                r_printf("unknown cmd\r\n");
                break;
        }
    }
}

r_s32 rda59xx_wifi_init()
{
    static r_u32 init_flag = 0;
    if(init_flag == 1)
        return R_NOERR;
    daemon_queue = rda_queue_create(DAEMON_MAILQ_SIZE, sizeof(rda_msg));
    module_state = STATE_INIT;
    rda59xx_set_cb_queue(daemon_queue);
    rda59xx_sta_init(&lwip_sta_netif);
    tcpip_init(NULL, NULL);
    maclib_init();

    rda_thread_new("maclib_thread", maclib_task, NULL, 512*8, AOS_DEFAULT_APP_PRI);
    rda_thread_new("wland_thread", rda59xx_wland_task, NULL, 512*8, AOS_DEFAULT_APP_PRI);
    rda_thread_new("daemon_thread", rda59xx_daemon, NULL, 512*5, AOS_DEFAULT_APP_PRI);
    rda_msleep(100);//wait for maclib_task running
    wland_sta_init();
    rda59xx_filter_multicast(0);
    rda59xx_set_channel(6);
    init_flag = 1;
    return R_NOERR;
}

r_s32 rda59xx_wifi_set_event_cb(wifi_event_cb_t cb)
{
    wifi_event_cb = cb;
    return R_NOERR;
}


