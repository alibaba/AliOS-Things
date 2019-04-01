/*
 * iperf, Copyright (c) 2014, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE
 * file for complete information.
 */
 
//#include <os_wrapper.h>
#include "iperf_config.h"
//#include <rtos.h>
//#include <log.h>

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/tcp.h"
#include "lwip/stats.h"

#include "iperf.h"
#include "iperf_api.h"
#include "units.h"
#include "iperf_locale.h"
#include "net.h"
//#include "netstack.h"
//#include <host_apis.h>
#include "soc_types.h"

//#if (IPERF3_ENABLE == 1)
static int run(struct iperf_test *test);
static bool iperf_running_flag = false;
static struct iperf_test *g_iperf_test1 = NULL, *g_iperf_test2 = NULL;
static struct iperf_test *g_iperf_test3 = NULL, *g_iperf_test4 = NULL;
static OsMutex iperf_api_mutex = NULL;
#if 0
void iperf_peer_update(u8* mac)
{
    netstack_ip_addr_t ipaddr;
    netstack_find_ip_in_arp_table(mac,&ipaddr);
    //LOG_PRINTF("%s ip=%s,val=0x%x\r\n",__func__,netstack_inet_ntoa(ipaddr),netstack_ipaddr_addr(netstack_inet_ntoa(ipaddr)));
    if(g_iperf_test1)
    {
        LOG_PRINTF("g_iperf_test1 hostname=%s, Server:%s\r\n",
            (g_iperf_test1->server_hostname!=NULL)?g_iperf_test1->server_hostname:" ",netstack_inet_ntoa(ipaddr));
        if(netstack_ipaddr_addr(g_iperf_test1->server_hostname) == netstack_ipaddr_addr(netstack_inet_ntoa(ipaddr)))
        {
            g_iperf_test1->state = IPERF_DONE;
        }
        else if(g_iperf_test2)
        {
            LOG_PRINTF("g_iperf_test2 hostname=%s, Server:%s\r\n",
                (g_iperf_test2->server_hostname!=NULL)?g_iperf_test2->server_hostname:" ",netstack_inet_ntoa(ipaddr));
            if(netstack_ipaddr_addr(g_iperf_test2->server_hostname) == netstack_ipaddr_addr(netstack_inet_ntoa(ipaddr)))
            {
                g_iperf_test2->state = IPERF_DONE;
            }
            else if(g_iperf_test3)
            {
                LOG_PRINTF("g_iperf_test3 hostname=%s, Server:%s\r\n",
                    (g_iperf_test3->server_hostname!=NULL)?g_iperf_test3->server_hostname:" ",netstack_inet_ntoa(ipaddr));
                if(netstack_ipaddr_addr(g_iperf_test3->server_hostname) == netstack_ipaddr_addr(netstack_inet_ntoa(ipaddr)))
                {
                    g_iperf_test3->state = IPERF_DONE;
                }
                else if(g_iperf_test4)
                {
                    LOG_PRINTF("g_iperf_test4 hostname=%s, Server:%s\r\n",
                        (g_iperf_test4->server_hostname!=NULL)?g_iperf_test4->server_hostname:" ",netstack_inet_ntoa(ipaddr));
                    if(netstack_ipaddr_addr(g_iperf_test4->server_hostname) == netstack_ipaddr_addr(netstack_inet_ntoa(ipaddr)))
                    {
                        g_iperf_test4->state = IPERF_DONE;
                    }    
                }

            }
        }
    }

}
#endif
#if 0
void iperf_event_handler(u32 evt_id, void *data, s32 len)
{
    switch (evt_id) {
        case SOC_EVT_STA_STATUS://ap mode :: station add/remove event
        {
            struct cfg_set_sta *cfg_sta =data;
            
            if(cfg_sta->sta_oper == CFG_STA_DEL)
            {
                iperf_peer_update((u8*)&(cfg_sta->wsid_info.addr.addr[0]));
            }
            break;
        }
        case SOC_EVT_LEAVE_RESULT: //STA mode
        {
            if(g_iperf_test1)
                g_iperf_test1->state = IPERF_DONE;

            if(g_iperf_test2)
                g_iperf_test2->state = IPERF_DONE;

            if(g_iperf_test3)
                g_iperf_test3->state = IPERF_DONE;

            if(g_iperf_test4)
                g_iperf_test4->state = IPERF_DONE;

            break;
        }
        default:
            break;
    }
}
#endif
void iperf_api_mutex_lock (bool lock)
{
    if(lock)
        OS_MutexLock(iperf_api_mutex);
    else
        OS_MutexUnLock(iperf_api_mutex);
}

bool iperf_get_runflag()
{
    return iperf_running_flag;
}

void iperf_clear_runflag()
{
    iperf_api_mutex_lock(1);

    iperf_running_flag = false;

    iperf_api_mutex_lock(0);
}

void iperf_list()
{
    int i=0;
    iperf_api_mutex_lock(1);

    if (g_iperf_test1){
        PRINTF("iperf task%d: %s, server port: %d \r\n", ++i,
            g_iperf_test1->role == 'c' ? "client" : "server",
            g_iperf_test1->server_port);
    }
    if (g_iperf_test2){
        PRINTF("iperf task%d: %s, server port: %d \r\n", ++i,
            g_iperf_test2->role == 'c' ? "client" : "server",
            g_iperf_test2->server_port);
    }

    if (g_iperf_test3){
        PRINTF("iperf task%d: %s, server port: %d \r\n", ++i,
            g_iperf_test3->role == 'c' ? "client" : "server",
            g_iperf_test3->server_port);
    }

    if (g_iperf_test4){
        PRINTF("iperf task%d: %s, server port: %d \r\n", ++i,
            g_iperf_test4->role == 'c' ? "client" : "server",
            g_iperf_test4->server_port);
    }

    if (!g_iperf_test1 && !g_iperf_test2 && !g_iperf_test3 && !g_iperf_test4){
        PRINTF("No iperf thread running. \r\n");
    }
    iperf_api_mutex_lock(0);
}

#if 0
void iperf_query()
{
    #define MIN(a, b) ((a > b) ? b : a)
    
    struct stats_mem *tcp_pcb_mem = stats_m_get(MEMP_TCP_PCB);
    struct stats_mem *tcp_pcb_listen_mem = stats_m_get(MEMP_TCP_PCB_LISTEN);
    struct stats_mem *udp_pcb_mem = stats_m_get(MEMP_UDP_PCB);
    struct stats_mem *netconn_mem = stats_m_get(MEMP_NETCONN);

    u32_t tcp_pcb_free_n = tcp_pcb_mem->avail - tcp_pcb_mem->used;
    u32_t tcp_pcb_listen_free_n = tcp_pcb_listen_mem->avail - tcp_pcb_listen_mem->used;
    u32_t udp_pcb_free_n = udp_pcb_mem->avail - udp_pcb_mem->used;
    u32_t netconn_free_n = netconn_mem->avail - netconn_mem->used;

    #if 0
    PRINTF("\r\nMEMP_TCP_PCB\r\n\t"); 
    PRINTF("avail: %"U32_F"\r\n\t", (u32_t)tcp_pcb_mem->avail); 
    PRINTF("used: %"U32_F"\r\n\t", (u32_t)tcp_pcb_mem->used); 
    PRINTF("max: %"U32_F"\r\n\t", (u32_t)tcp_pcb_mem->max); 
    PRINTF("err: %"U32_F"\r\n", (u32_t)tcp_pcb_mem->err);
    
    PRINTF("\r\nMEMP_UDP_PCB\r\n\t"); 
    PRINTF("avail: %"U32_F"\r\n\t", (u32_t)udp_pcb_mem->avail); 
    PRINTF("used: %"U32_F"\r\n\t", (u32_t)udp_pcb_mem->used); 
    PRINTF("max: %"U32_F"\r\n\t", (u32_t)udp_pcb_mem->max); 
    PRINTF("err: %"U32_F"\r\n", (u32_t)udp_pcb_mem->err);

    PRINTF("\r\nMEMP_NETCONN\r\n\t"); 
    PRINTF("avail: %"U32_F"\r\n\t", (u32_t)netconn_mem->avail); 
    PRINTF("used: %"U32_F"\r\n\t", (u32_t)netconn_mem->used); 
    PRINTF("max: %"U32_F"\r\n\t", (u32_t)netconn_mem->max); 
    PRINTF("err: %"U32_F"\r\n\t", (u32_t)netconn_mem->err);
    #endif
    PRINTF("iperf3 max streams: \r\n\t");
    
    if ((tcp_pcb_free_n > 1) && (netconn_free_n > 1))
    {
        PRINTF("TCP TX : %d\r\n\t", MIN(tcp_pcb_free_n - 1, netconn_free_n - 1)); 
    }
    else
    {
        PRINTF("TCP TX : %d\r\n\t", 0); 
    }
    
    if ((udp_pcb_free_n > 0) && (netconn_free_n > 1))
    {
        PRINTF("UDP TX : %d\r\n\t", MIN(udp_pcb_free_n, netconn_free_n - 1)); 
    }
    else
    {
        PRINTF("UDP TX : %d\r\n\t", 0); 
    }
    
    if (tcp_pcb_listen_free_n > 0)
    {
        if ((tcp_pcb_free_n > 2) && (netconn_free_n > 2))
        {
            PRINTF("TCP RX : %d\r\n\t", MIN(tcp_pcb_free_n - 2, netconn_free_n - 2));
        }
        else
        {
            PRINTF("TCP RX : %d\r\n\t", 0); 
        }
        
        if ((udp_pcb_free_n > 1) && (netconn_free_n > 3))
        {
            PRINTF("UDP RX : %d\r\n\t", MIN(udp_pcb_free_n - 1, netconn_free_n - 3)); 
        }
        else
        {
            PRINTF("UDP RX : %d\r\n\t", 0); 
        }
    }
    else
    {
        PRINTF("TCP RX : %d\r\n\t", 0); 
        PRINTF("UDP RX : %d\r\n\t", 0); 
    }

    #if 0
    PRINTF("STA mode \r\n\t"); 
    PRINTF("Client mode :\r\n\t"); 
    PRINTF("TCP TX : %d\r\n\t", netconn_mem->avail - 1); 
    PRINTF("UDP TX : %d\r\n\t", netconn_mem->avail - 2); 

    PRINTF("Server mode :\r\n\t"); 
    PRINTF("TCP RX : %d\r\n\t", netconn_mem->avail - 2); 
    PRINTF("UDP RX : %d\r\n\t", netconn_mem->avail - 3); 

    PRINTF("AP mode \r\n\t"); 
    PRINTF("Client mode :\r\n\t"); 
    PRINTF("TCP TX : %d\r\n\t", netconn_mem->avail - 2); 
    PRINTF("UDP TX : %d\r\n\t", netconn_mem->avail - 2); 

    PRINTF("Server mode :\r\n\t"); 
    PRINTF("TCP RX : %d\r\n\t", netconn_mem->avail - 3); 
    PRINTF("UDP RX : %d\r\n\t", netconn_mem->avail - 4); 
    #endif
    
}
#endif
/*check iperf3 thread running via server_port
   only one instance permitted on a #server_port
   if you need two running instances, please add option -p <server_port> when run iperf3
   Totally, maximum instance is 2.
*/
bool iperf_add_test (struct iperf_test *test)
{
    bool result = true;

    iperf_api_mutex_lock(1);
#if 1
    if (g_iperf_test1 && g_iperf_test2 && g_iperf_test3 && g_iperf_test4){
        PRINTF("ERROR -- there have already been four iperf threads running! \r\n");
        result = false;
    }
    else{    
        if (g_iperf_test1 && g_iperf_test1->server_port == test->server_port)
            result = false;
        else if (g_iperf_test2 && g_iperf_test2->server_port == test->server_port)
            result = false;
        else if (g_iperf_test3 && g_iperf_test3->server_port == test->server_port)
            result = false;
        else if (g_iperf_test4 && g_iperf_test4->server_port == test->server_port)
            result = false;

        if (result){
            if (!g_iperf_test1)
                g_iperf_test1 = test;
            else{
                if (!g_iperf_test2)
                    g_iperf_test2 = test;
                else{
                    if (!g_iperf_test3)
                        g_iperf_test3 = test;                    
                    else
                        g_iperf_test4 = test;                    
                }
            }
            iperf_running_flag = true;
        }else{
            PRINTF("ERROR -- there has already been an iperf thread with server port %d! \r\n", test->server_port);
            PRINTF("Please use option '-p <port>' to designate another server port.\r\n");
        }
    }
#else
    if (g_iperf_test1 && g_iperf_test2){
        PRINTF("ERROR -- there have already been two iperf threads running! \r\n");
        result = false;
    }
    else{    
        if (g_iperf_test1 && g_iperf_test1->server_port == test->server_port)
            result = false;
        else if (g_iperf_test2 && g_iperf_test2->server_port == test->server_port)
            result = false;

        if (result){
            if (!g_iperf_test1)
                g_iperf_test1 = test;
            else
                g_iperf_test2 = test;

            iperf_running_flag = true;
        }else{
            PRINTF("ERROR -- there has already been an iperf thread with server port %d! \r\n", test->server_port);
            PRINTF("Please use option '-p <port>' to designate another server port.\r\n");
        }
    }
#endif    
    iperf_api_mutex_lock(0);
    
    return result;
}

void iperf_del_test (struct iperf_test *test)
{
    iperf_api_mutex_lock(1);
    
    if (g_iperf_test1 == test)
        g_iperf_test1 = NULL;
    else if (g_iperf_test2 == test)
        g_iperf_test2 = NULL;
    else if (g_iperf_test3 == test)
        g_iperf_test3 = NULL;
    else if (g_iperf_test4 == test)
        g_iperf_test4 = NULL;

    if (!g_iperf_test1 && !g_iperf_test2 && !g_iperf_test3 && !g_iperf_test4)
        iperf_running_flag = false;

    iperf_api_mutex_lock(0);
}

int iperf_init()
{
    iperf_running_flag = false;
    g_iperf_test1 = NULL;
    g_iperf_test2 = NULL;
    g_iperf_test3 = NULL;
    g_iperf_test4 = NULL;    
    //ssv6xxx_wifi_reg_evt_cb(iperf_event_handler);

    return OS_MutexInit(&iperf_api_mutex);
}


#ifdef IPERF_DEBUG_MEM
extern int iperf_mallocs;
extern int iperf_frees;
extern int iperf_msize;
extern int iperf_max_mallocs;
extern int iperf_max_msize;
#endif

#ifdef __SSV_UNIX_SIM__
extern int netmgr_send_arp_unicast (u8 *dst_mac);
#endif

/**************************************************************************/
void
net_app_iperf3(int argc, char **argv)
{
    struct iperf_test *test;
    int retval;

#if 0
    if (argc == 3 && STRCMP(argv[1], "arp") == 0)
    {
        u8 mac[6]={0}; //={0xd8,0xfc,0x93,0x2c,0xc9,0xff};
        int dst_mac[6], i;
        retval = sscanf(argv[2], "%02x:%02x:%02x:%02x:%02x:%02x", //sscanf doesn't work on GP15B platform
                &dst_mac[0],&dst_mac[1],&dst_mac[2],
                &dst_mac[3],&dst_mac[4],&dst_mac[5]);
        
        if (retval != 6){
            PRINTF("sscanf get MAC address: %02x:%02x:%02x:%02x:%02x:%02x \r\n",
                dst_mac[0],dst_mac[1],dst_mac[2],
                dst_mac[3],dst_mac[4],dst_mac[5]);
            PRINTF("Invalid MAC address [%s], retval:%d \r\n", argv[2], retval);
            return;
        }
        for(i=0;i<6;i++)
            mac[i]=(u8)dst_mac[i];
        retval = netmgr_send_arp_unicast(mac);
        PRINTF("arp unicast request return %s \r\n", retval ? "FAILURE" : "SUCCESS!");
        return ;
    }
#endif

#if IPERF3_UDP_TEST
    static int udp_speed_pkg_size = 1024;
    static int udp_speed_time = 60;
        
    if (argc == 5 && STRCMP(argv[1], "udp") == 0)
    {
        s32 fd = -1;
        struct sockaddr_in addr;
        s32 len = 0;
        u32 tick1,tick2,tick3;
        u32  send_count=0,send_total_count=0;
        u32  sleep_count=0;
        u8* sendBuf = NULL;
        u16 seq = 0;
        int p_total=0;

        udp_speed_time = ATOI(argv[2]);
        udp_speed_pkg_size = ATOI(argv[3]); 

        if (udp_speed_pkg_size > (60 * 1024))
        {
            PRINTF("udp speed pkg size > 60k \r\n");
            return;
        }
        else
        {
            sendBuf = (u8 *)OS_MemAlloc(udp_speed_pkg_size);
        }
        
        if (!sendBuf)
        {
            PRINTF("malloc %d failed\r\n", udp_speed_pkg_size);
            return;
        }

        addr.sin_family = AF_INET;
        addr.sin_len = sizeof(struct sockaddr_in);
        addr.sin_port = htons(4801);
        addr.sin_addr.s_addr = inet_addr(argv[4]);

        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd < 0)
        {
            OS_MemFree(sendBuf);
            return;
        } 

        tick3 = tick2 = tick1 = OS_GetSysTick();
        for(;((tick2-tick1)*OS_TICK_RATE_MS) < (u32)(udp_speed_time*1000);)
        {
            memcpy(sendBuf, &seq,sizeof(seq));
            len = sendto(fd, (u8*)sendBuf, udp_speed_pkg_size, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr));
            if (len <=0)
            {
                PRINTF("sendto err\r\n ");
                break;
            }
            sleep_count += udp_speed_pkg_size;
            seq ++;
            send_count++;
            send_total_count ++;
            tick2 = OS_GetSysTick();
            if ((tick2 - tick3) * OS_TICK_RATE_MS >= 1000)
            {
                tick3 = tick2;
                PRINTF("send speed=%dk \r\n ", (int)((send_count * len)>>10));
                p_total = p_total + ((send_count * len)>>10);
                send_count =0 ;
            }
            
            if (sleep_count >= 50 * 1024)
            {
                OS_MsDelay(20);
                sleep_count = 0;
            }
        }
        if ((u32)((tick2 - tick1) * OS_TICK_RATE_MS) > (u32)1000)
        {
            PRINTF("send end,tol time = %d(ms),tol pkg =%d, tol data=%d(k),speed=%d(k/s)\r\n"
                , (int)(tick2 -tick1)*OS_TICK_RATE_MS
                , (int)send_total_count
                , (int)((udp_speed_pkg_size>>10) * send_total_count) 
                , (int)(((udp_speed_pkg_size>>10) * send_total_count ) /((tick2 - tick1)*OS_TICK_RATE_MS/1000)));
        }
        else        
        {
            PRINTF("send end,time is < 1s\r\n");
        }

        shutdown(fd,SHUT_RDWR);
        closesocket(fd);
        OS_MemFree(sendBuf);
        PRINTF("iperf udp test end\r\n");
        return;
    }
#endif

    if (!iperf_api_mutex && iperf_init()){
        PRINTF("iperf init failed! \r\n");
        return;
    }

    PRINTF("\r\n");
    if (argc == 2 && STRCMP(argv[1], "stop") == 0)
    {
        if (iperf_get_runflag()){
            iperf_clear_runflag();
            PRINTF("The iperf threads stopped! \r\n");
            sys_msleep(2000);

            #ifdef IPERF_DEBUG_MEM
            PRINTF("iperf mallocs:%d, frees:%d, delta:%d, delta size:%d\r\n", iperf_mallocs, iperf_frees, iperf_mallocs - iperf_frees, iperf_msize);
            PRINTF("iperf max mallocs:%d, max size:%d \r\n", iperf_max_mallocs, iperf_max_msize);
            iperf_msize = iperf_mallocs = iperf_frees = iperf_max_mallocs = iperf_max_msize = 0;
            #endif
        }
        else{
            PRINTF("No iperf thread running. \r\n");
        }
        return;
    }else if (argc == 2 && STRCMP(argv[1], "list") == 0){
        iperf_list();
        return;
    }else if (argc == 2 && STRCMP(argv[1], "query") == 0){
        //iperf_query();
        return;
    }else if (argc == 2 && STRCMP(argv[1], "diag") == 0){
        float float_var = 1.4;
        double double_var = 1.4;
        unsigned long long u64_var = 0x500000004;
        char str[30];

        PRINTF("float_var = %f \r\n \r\n", float_var);
        PRINTF("double_var = %g \r\n \r\n", double_var);
        PRINTF("u64_var = %llu \r\n", u64_var);

        sprintf( str, "%lld", u64_var);
        PRINTF("u64_var=%s after sprintf \r\n", str);
        return;
    }
    test = iperf_new_test();
    if (!test)
    {
        PRINTF("create new test error - %s \r\n", iperf_strerror(IENEWTEST));
        return;
    }
    
    iperf_defaults(test);	/* sets defaults */
    retval = iperf_parse_arguments(test, argc, argv);
    if (retval < 0) {
        PRINTF("parameter error - %s \r\n", iperf_strerror(test->i_errno));
        PRINTF("\r\n");

        iperf_free_test(test);
        usage_long();
        return;
    }
    else if (retval == 100)
    {        
        iperf_free_test(test);
        return;
    }

    if ((test->protocol->id == Pudp) && (test->num_streams > 1))
    {
        PRINTF("Notes: iperf3 win32 version not support udp rx more than one stream\r\n");	
    }
    
    if (iperf_add_test(test)){
        if (run(test) < 0)
            PRINTF("error - %s \r\n", iperf_strerror(test->i_errno));
    }else{
        PRINTF("Please run command \"iperf3 list\" to show the running iperf threads. \r\n");	
        PRINTF("Please run command \"iperf3 stop\" to stop the running threads. \r\n");	
    }
	
    iperf_del_test(test);
    iperf_free_test(test);
}

/**************************************************************************/
static int
run(struct iperf_test *test)
{
    int consecutive_errors;

    switch (test->role) {
#if HAVE_SERVER
        case 's':
    	    consecutive_errors = 0;
            for (; iperf_get_runflag(); ) {
    		    if (iperf_run_server(test) < 0) {
    		        PRINTF("error - %s \r\n", iperf_strerror(test->i_errno));
                    PRINTF("\r\n");
    		        ++consecutive_errors;
    		        if (consecutive_errors >= 5) {
    		            PRINTF("too many errors, exiting\r\n");
    			        break;
    		        }
                } else
    		        consecutive_errors = 0;
                
                iperf_reset_test(test);
            }
            break;
#endif
#if HAVE_CLIENT
    	case 'c':
    	    if (iperf_run_client(test) < 0)
    		    PRINTF("error - %s \r\n", iperf_strerror(test->i_errno));
            break;
#endif
        default:
            iperf_usage();
            break;
    }

    return 0;
}
//#endif

typedef struct _stParam
{
	char*	argv[24];
	int	argc;
} stParam;

void iperf3(void *args)
{
    stParam *param;
    param = (stParam*)(args);
    net_app_iperf3(param->argc, param->argv);

    OS_TaskDelete(NULL);
}