#include "rda59xx_daemon.h"
#include "rda_sys_wrapper.h"
#include "rda_def.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/inet.h"
#include "lwip/tcpip.h"
#include "maclib_task.h"
#include "rda59xx_lwip.h"
#include "dhcps.h"
#include "rda5981_sys_data.h"
#include "trng_api.h"

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
r_u32 rda_wland_dbg_level = 0;
r_u32 rda_wpa_dbg_level = 0;
r_u32 rda_maclib_dbg_level = 0;
r_u32 rda_wland_dump = 0;
r_u32 rda_wpa_dump = 0;
r_u32 rda_hut_dump = 0;

#define SCAN_TIMES          10
#define RECONN_TIMES        3
#define DAEMON_MAILQ_SIZE   10

#define AOS_MACLIB_APP_PRI  16

static bool rda_lwip_tcpip_init_flag = false;
static sys_sem_t rda_lwip_tcpip_inited;
r_void *daemon_queue = NULL;
r_u32 module_state = STATE_DSIABLE;
unsigned char g_airkiss_connect_flag = 0;
static rda59xx_sta_info r_sta_info;
static rda59xx_bss_info r_bss_info;
static rda59xx_ap_info r_ap_info;
static wifi_event_cb_t wifi_event_cb = NULL;
sys_sem_t lwip_sta_netif_linked;
sys_sem_t lwip_sta_netif_has_addr;
sys_sem_t lwip_ap_netif_linked;
struct netif lwip_sta_netif;
struct netif lwip_ap_netif;

extern int sniffer_cb(void *data, unsigned short data_len);
extern unsigned int filter_backup;
static void rda59xx_lwip_tcpip_init_irq(void *eh)
{
    sys_sem_signal(&rda_lwip_tcpip_inited);
    rda_lwip_tcpip_init_flag = true;
}

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
#ifdef DELETE_HFILOP_CODE 
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
#else
    extern unsigned char *hfilop_layer_get_mac(void);
    memcpy((char *)macaddr, hfilop_layer_get_mac(), 6);
    if(mode == 1)
        macaddr[5] ^= 0x01;
#endif
    return;
	}



r_void rda59xx_set_macaddr(r_u8 *macaddr, r_u32 mode)
{
    rda5981_write_sys_data(macaddr, 6, RDA5981_SYS_DATA_FLAG_MAC);
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
    int netif_no = 0;
    if(init_flag == 1)
        return R_NOERR;
    //rda_netif.netif_no = 0;
    sys_sem_new(&lwip_sta_netif_linked, 0);
    sys_sem_new(&lwip_sta_netif_has_addr, 0);

    if(rda_lwip_tcpip_init_flag == false){
        sys_sem_new(&rda_lwip_tcpip_inited, 0);
        tcpip_init(rda59xx_lwip_tcpip_init_irq, NULL);
        sys_arch_sem_wait(&rda_lwip_tcpip_inited, 0);
    }
   
    r_memset(&lwip_sta_netif, 0, sizeof(struct netif));
    if (!netif_add(&lwip_sta_netif,
#if LWIP_IPV4
                0, 0, 0,
#endif
        (void *)&netif_no, rda59xx_sta_netif_init, tcpip_input)) {
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
    int netif_no = 1;
    //rda_netif.netif_no = 1;
    sys_sem_new(&lwip_ap_netif_linked, 0);

    if(rda_lwip_tcpip_init_flag == false){
        sys_sem_new(&rda_lwip_tcpip_inited, 0);
        tcpip_init(rda59xx_lwip_tcpip_init_irq, NULL);
        sys_arch_sem_wait(&rda_lwip_tcpip_inited, 0);
    }

    r_memset(&lwip_ap_netif, 0, sizeof(struct netif));
    if (!netif_add(&lwip_ap_netif,
#if LWIP_IPV4
                0, 0, 0,
#endif
                (void *)&netif_no, rda59xx_ap_netif_init, tcpip_input)) {
            return ERR_DEVICE;
    }

    init_flag = 1;
    return R_NOERR;
}

static r_s32 rda59xx_sta_disconnect_internal()
{
    r_s32 res = R_NOERR;
    rda_msg msg;

    if(r_sta_info.dhcp) {
        dhcp_release(&lwip_sta_netif);
        dhcp_stop(&lwip_sta_netif);
    }
    netif_set_down(&lwip_sta_netif);
    
    msg.type = RDA59XX_WLAND_STA_DISCONNECT;
    res = rda59xx_send_wland_msg(&msg, RDA_WAIT_FOREVER);
    if(res != R_NOERR){
        WIFISTACK_PRINT("Send disconnect cmd failed!\r\n");
        return R_ERR;
    }

#if LWIP_IPV4
    char *ip_zero = "0.0.0.0";
    ip4_addr_t ip_init;
    inet_aton(ip_zero, &ip_init);
    netif_set_ipaddr(&lwip_sta_netif, &ip_init);
#endif

    sys_sem_free(&lwip_sta_netif_has_addr);
    sys_sem_new(&lwip_sta_netif_has_addr, 0);

    return res;
}

r_s32 rda59xx_sta_disconnect()
{
    rda_msg msg;
    msg.type = DAEMON_STA_DISCONNECT;
    msg.arg1 = DISCONNECT_ACTIVE;
    rda59xx_send_daemon_msg(&msg, RDA_WAIT_FOREVER); 
    wifi_event_cb(EVENT_STA_DISCONNECTTED, NULL);
    return 0;
}

static r_s32 rda59xx_sta_connect_internal(rda59xx_sta_info *sta_info)
{
    r_s32 res = R_NOERR, res_t = R_NOERR;
    r_u8 reconn = 0, scan_times = 0, scan_res = 0;
    r_s32 index = 0;
    rda_msg msg;
    rda59xx_scan_info r_scan_info;
    rda59xx_scan_result ap;
    r_u16 timeout_val = (g_airkiss_connect_flag) ? 10000 : 6000;

    WIFISTACK_PRINT("rda59xx_sta_connect_internal!\r\n");
    r_memcpy(&r_sta_info, sta_info, sizeof(rda59xx_sta_info));
    
    //scan
    r_memset(&r_scan_info, 0, sizeof(rda59xx_scan_info));
    r_memcpy(&(r_scan_info.SSID), &(r_sta_info.ssid), r_strlen(r_sta_info.ssid));
    r_scan_info.channel = r_sta_info.channel;
    r_scan_info.scan_mode = 1;
    r_scan_info.SSID_len = r_strlen(r_sta_info.ssid);
    rda59xx_del_scan_all_result();
    while (scan_times++ < SCAN_TIMES) {
        WIFISTACK_PRINT("scan_times=%d\r\n",scan_times);
        scan_res = rda59xx_scan_internal(&r_scan_info);
        index = rda59xx_get_scan_result_special(&ap, r_sta_info.ssid, r_sta_info.bssid, r_sta_info.channel);
        if(index != R_ERR) {
           WIFISTACK_PRINT("find the special AP\r\n");
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
            res_t = sys_arch_sem_wait(&lwip_sta_netif_linked, timeout_val);
            if (res_t == SYS_ARCH_TIMEOUT) {
                WIFISTACK_PRINT("sta linkup SYS_ARCH_TIMEOUT!\r\n");
                res = ERR_CONNECTION;
                goto reconn;
            }

            if(!netif_is_link_up(&lwip_sta_netif)){
                WIFISTACK_PRINT("sta linkup ERR_AUTH!\r\n");
                res = ERR_AUTH;
                goto reconn;
            }
        }

        netif_set_up(&lwip_sta_netif);

        if (r_sta_info.dhcp) {
            WIFISTACK_PRINT("dhcp\r\n");
            res_t = dhcp_start(&lwip_sta_netif);
            if (res_t) {
                WIFISTACK_PRINT("ERR_DHCP!\r\n");
                res = ERR_DHCP;
                goto reconn;
            }
        } else {
            netif_set_addr(&lwip_sta_netif, &(r_sta_info.ip), &(r_sta_info.netmask), &(r_sta_info.gateway));
        }
       
        if (!rda59xx_get_netif_ip(&lwip_sta_netif)) {
            res_t = sys_arch_sem_wait(&lwip_sta_netif_has_addr, 10000);
            if (res_t == SYS_ARCH_TIMEOUT) {
                WIFISTACK_PRINT("sta has addr SYS_ARCH_TIMEOUT!\r\n");
                res = ERR_DHCP;
            }
        }

        //rda59xx_add_dns_addr(&lwip_sta_netif);
reconn:   
        if(res == R_NOERR){
            WIFISTACK_PRINT("Connect successful!\r\n");
            rda59xx_set_data_rate(0, 0);
#ifndef DELETE_HFILOP_CODE
             r_memcpy(&r_bss_info, &ap, 6+33);//bssid, ssid, channel, secure type, RSSI
             r_bss_info.channel = ap.channel;
             r_bss_info.secure = ap.secure_type;
             r_bss_info.rssi = ap.RSSI;
#else
            r_memcpy(&r_bss_info, &ap, 6+33+1+1+1);//bssid, ssid, channel, secure type, RSSI
#endif
            r_bss_info.ipaddr = lwip_sta_netif.ip_addr.addr;
            r_bss_info.mask = lwip_sta_netif.netmask.addr;
            r_bss_info.gateway = lwip_sta_netif.gw.addr;
            r_memcpy(&r_bss_info.dns1, dns_getserver(0), sizeof(r_u32));
            r_memcpy(&r_bss_info.dns2, dns_getserver(1), sizeof(r_u32));
            WIFISTACK_PRINT("Sta got ip successful!\r\n");
            wifi_event_cb(EVENT_STA_CONNECTTED, NULL);
            wifi_event_cb(EVENT_STA_GOT_IP, NULL);
            break;
        }else{
            rda59xx_sta_disconnect_internal();
            if(++reconn > RECONN_TIMES){
                WIFISTACK_PRINT("reconn times=%d\r\n",reconn);
                //r_memset(&r_sta_info, 0, sizeof(rda59xx_sta_info));
                wifi_event_cb(EVENT_STA_CONNECT_FAIL, NULL);
                break;
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

    WIFISTACK_PRINT("rda59xx_sta_connect!\r\n");
    msg.type = DAEMON_STA_CONNECT;
    msg.arg1 = (r_u32)sta_info;
    rda59xx_send_daemon_msg(&msg, RDA_WAIT_FOREVER);
    return 0;        
}

#ifndef DELETE_HFILOP_CODE
r_s32 rda59xx_sta_connect_ex(rda59xx_sta_info *sta_info)
{
    rda_msg msg;
    msg.type = DAEMON_STA_CONNECT;
    msg.arg1 = (r_u32)sta_info;
    rda59xx_send_daemon_msg(&msg, 1000);
    return 0;        
}
#endif

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
            WIFISTACK_PRINT("SYS_ARCH_TIMEOUT,ret ERR_CONNECTION = -4\r\n");
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
    netif_set_link_down(&lwip_ap_netif);
    msg.type = RDA59XX_WLAND_AP_STOP;
    rda59xx_send_wland_msg(&msg, RDA_WAIT_FOREVER);
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
    rda59xx_send_daemon_msg(&msg, RDA_NO_WAIT);
    return 0;
}

static r_void rda59xx_daemon(r_void *arg)
{
    rda_msg msg;
    r_s32 res;
    r_u32 stop_reconnect = 0, monitor_restore = 0;

    while(1){
        rda_queue_recv(daemon_queue, (r_u32)&msg, RDA_WAIT_FOREVER);
        WIFISTACK_PRINT("daemon_q=%d,daemon_queue_msg_type=%d,module_state=0x%x\r\n",daemon_queue,msg.type,module_state);
        if((msg.type != DAEMON_SCAN) && (msg.type != DAEMON_STA_RECONNECT) && (module_state & STATE_STA_RC) && \
            !((msg.type == DAEMON_STA_DISCONNECT) && (msg.arg1 == DISCONNECT_PASSIVE))){
            WIFISTACK_PRINT("set stop_reconnect!\r\n");
            stop_reconnect = 1;
        }
        switch(msg.type)
        {
            case DAEMON_SNIFFER_ENABLE:
                WIFISTACK_PRINT("DAEMON_SNIFFER_ENABLE!\r\n");
                if(module_state & STATE_SNIFFER){
                    WIFISTACK_PRINT("SNIFFER has been enabled!\r\n");
                    rda_sem_release((r_void *)msg.arg3);
                    break;
                }
                res = rda59xx_sniffer_enable_internal((sniffer_handler_t)(msg.arg1));
                rda_sem_release((r_void *)msg.arg3);
                module_state |= STATE_SNIFFER;
                break;
            case DAEMON_SNIFFER_DISABLE:
                WIFISTACK_PRINT("DAEMON_SNIFFER_DISABLE!\r\n");
                if(!(module_state & STATE_SNIFFER)){
                    WIFISTACK_PRINT("SNIFFER has been disabled!\r\n");
                    //rda_sem_release((r_void *)msg.arg3);
                    break;
                }
                res = rda59xx_sniffer_disable_internal();
                //rda_sem_release((r_void *)msg.arg3);
                module_state &= ~(STATE_SNIFFER);
                break;
            case DAEMON_SCAN:
                WIFISTACK_PRINT("DAEMON_SCAN!\r\n");
                r_u32 monitor_restore_scan = 0;
                if(module_state & STATE_SNIFFER){
                    module_state &= ~(STATE_SNIFFER);
                    rda59xx_sniffer_disable_internal();
                    monitor_restore_scan = 1;
                }
                res = rda59xx_scan_internal((rda59xx_scan_info *)msg.arg1);
                if(monitor_restore_scan == 1){
                    rda59xx_sniffer_enable_internal(sniffer_cb);
                    rda59xx_sniffer_set_filter(1, 1, filter_backup);
                    module_state |= STATE_SNIFFER;
                    monitor_restore_scan = 0;
                }
                rda_sem_release((r_void *)msg.arg3);
                break;
            case DAEMON_STA_CONNECT:
                WIFISTACK_PRINT("DAEMON_STA_CONNECT!\r\n");
                if(module_state & STATE_STA){
                    WIFISTACK_PRINT("STA has been connected!\r\n");
                    rda_sem_release((r_void *)msg.arg3);
                    break;
                } 
/*
                if(module_state & STATE_AP){
                    rda59xx_ap_disable_internal();
                    module_state &= ~(STATE_AP);
                    r_memset(&r_ap_info, 0, sizeof(rda59xx_ap_info));
                }
*/
                res = rda59xx_sta_connect_internal((rda59xx_sta_info*)msg.arg1);
                if(res == R_NOERR)
                    module_state |= STATE_STA;
                rda_sem_release((r_void *)msg.arg3);
                break;
            case DAEMON_STA_DISCONNECT:
                WIFISTACK_PRINT("DAEMON_STA_DISCONNECT!\r\n");
                if(!(module_state & STATE_STA)){
                    WIFISTACK_PRINT("STA has been disconnected!\r\n");
                    if(msg.arg1 == DISCONNECT_ACTIVE)
                        rda_sem_release((r_void *)msg.arg3);
                    break;
                }
                r_memset(&r_bss_info, 0, sizeof(rda59xx_bss_info));
                module_state &= ~(STATE_STA);
                res = rda59xx_sta_disconnect_internal();
                if(msg.arg1 == DISCONNECT_ACTIVE){
                    r_memset(&r_sta_info, 0, sizeof(rda59xx_sta_info));
                    rda_sem_release((r_void *)msg.arg3);
                }else if(msg.arg1 == DISCONNECT_PASSIVE){
                    if(module_state & STATE_SNIFFER){
                        module_state &= ~(STATE_SNIFFER);
                        rda59xx_sniffer_disable_internal();
                        monitor_restore = 1;
                    }    
                    rda59xx_netif_down(0);
                    msg.type = DAEMON_STA_RECONNECT;
                    res = rda_queue_send(daemon_queue, (r_u32)&msg, 1000);
                    module_state |= STATE_STA_RC;
                }
                break;
            case DAEMON_STA_RECONNECT:
                WIFISTACK_PRINT("DAEMON_STA_RECONNECT!\r\n");
                if(stop_reconnect == 1){
                    WIFISTACK_PRINT("STOP RECONNECT!\r\n");
                    stop_reconnect = 0;
                    module_state &= ~(STATE_STA_RC);
                    //r_memset(&r_sta_info, 0, sizeof(rda59xx_sta_info));
                    break;
                }
                res = rda59xx_sta_connect_internal(&r_sta_info);
                if(res != R_NOERR){
                    rda_msleep(100);
                    //r_memset(&r_bss_info, 0, sizeof(rda59xx_bss_info));
                    //res = rda59xx_sta_disconnect_internal();
                    msg.type = DAEMON_STA_RECONNECT;
                    res = rda_queue_send(daemon_queue, (r_u32)&msg, 1000);
                }else{
                    module_state |= STATE_STA;
                    module_state &= ~(STATE_STA_RC);
                    if(monitor_restore == 1){
                        rda59xx_sniffer_enable_internal(sniffer_cb);
                        rda59xx_sniffer_set_filter(1, 1, filter_backup);
                        module_state |= STATE_SNIFFER;
                        monitor_restore = 0;
                    }
                }
                break;
            case DAEMON_AP_ENABLE:
                WIFISTACK_PRINT("DAEMON_AP_ENABLE!\r\n");
                if(module_state & STATE_AP){
                    WIFISTACK_PRINT("AP has been started!\r\n");
                    rda_sem_release((r_void *)msg.arg3);
                    break;
                }
                res = rda59xx_ap_enable_internal((rda59xx_ap_info*)msg.arg1);
                module_state |= STATE_AP;
                rda_sem_release((r_void *)msg.arg3);
                break;
            case DAEMON_AP_DISABLE:
                WIFISTACK_PRINT("DAEMON_AP_DISABLE!\r\n");
                if(!(module_state & STATE_AP)){
                    WIFISTACK_PRINT("AP has been stoped! shouldn't be running here!! \r\n");
                    aos_reboot();
                    rda_sem_release((r_void *)msg.arg3);
                    break;
                }
                r_memset(&r_ap_info, 0, sizeof(rda59xx_ap_info));
                res = rda59xx_ap_disable_internal();
                module_state &= ~(STATE_AP);
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
    static r_u32 init_flag = 0, thread_init_flag = 0;
    if(init_flag == 1)
        return R_NOERR;
    daemon_queue = rda_queue_create(DAEMON_MAILQ_SIZE, sizeof(rda_msg));
    WIFISTACK_PRINT("wifi_init,daemon_q=%d,dae_q is null=%d\r\n",daemon_queue,daemon_queue == NULL ? 1: 0);
    module_state = STATE_INIT;
    rda59xx_set_cb_queue(daemon_queue);
    rda59xx_sta_init(&lwip_sta_netif);
    //tcpip_init(NULL, NULL);
    //maclib_init();
    if(thread_init_flag == 0){
    rda_thread_new("maclib_thread", maclib_task, NULL, 512*8, AOS_MACLIB_APP_PRI);
    rda_thread_new("wland_thread", rda59xx_wland_task, NULL, 512*8, AOS_DEFAULT_APP_PRI);
    rda_thread_new("daemon_thread", rda59xx_daemon, NULL, 512*5, AOS_DEFAULT_APP_PRI);
        /* Allow the PHY task to detect the initial link state and set up the proper flags */
        rda_msleep(100);//wait for maclib_task running
        thread_init_flag = 1;
    }
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


