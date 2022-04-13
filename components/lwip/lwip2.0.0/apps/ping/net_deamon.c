#include "stdlib.h"
#include "lwip/opt.h"

#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"
#include "lwip/prot/ip.h"
#include "lwip/prot/ip4.h"
#include "lwip/sockets.h"

#include "vfsdev/wifi_dev.h"
#include "ulog/ulog.h"

#define PRINT_TAG "net_deamon"

#ifndef POLL_NET_STATUS_PERIOD_MS
#define POLL_NET_STATUS_PERIOD_MS 3000
#endif

#ifndef POLL_NET_STATUS_PERIOD_FAST_MS
#define POLL_NET_STATUS_PERIOD_FAST_MS 3000
#endif

#ifndef POLL_PING_RCV_TIMEO
#define POLL_PING_RCV_TIMEO 3000
#endif

#ifndef POLL_PING_COUNT
#define POLL_PING_COUNT 5
#endif

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping delay - in milliseconds */
#ifndef POLL_PING_DELAY
#define POLL_PING_DELAY     1000
#endif

#define NET_DEAMON_TASK_STACK_SIZE  4096

static volatile int ping_seq_num = 0;
static volatile int daemon_started = 0;
static volatile int daemon_exit = 0;
static volatile u32_t ping_time = 0;
static volatile u32_t elapse_time = 0;
static volatile u32_t ping_success_count = 0;
static volatile u32_t ping_fail_count = 0;
static volatile float avg_elapse_time_ms = 0.0;

void get_gw_addr(ip4_addr_t *gw_addr);
/** Prepare a echo ICMP request */
static void
ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
  size_t i;
  size_t data_len = len - sizeof(struct icmp_echo_hdr);

  ICMPH_TYPE_SET(iecho, ICMP_ECHO);
  ICMPH_CODE_SET(iecho, 0);
  iecho->chksum = 0;
  iecho->id     = PING_ID;
  iecho->seqno  = lwip_htons(++ping_seq_num);

  /* fill the additional data buffer with some data */
  for(i = 0; i < data_len; i++) {
    ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
  }

  iecho->chksum = inet_chksum(iecho, len);
}

/* Ping using the socket ip */
static err_t
ping_send(int s, const ip_addr_t *addr)
{
  int err;
  struct icmp_echo_hdr *iecho;
  struct sockaddr_storage to;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + 5;
  LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

  iecho = (struct icmp_echo_hdr *)mem_malloc((mem_size_t)ping_size);
  if (!iecho) {
    return ERR_MEM;
  }

  ping_prepare_echo(iecho, (u16_t)ping_size);

  if(IP_IS_V4(addr)) {
    struct sockaddr_in *to4 = (struct sockaddr_in*)&to;
    to4->sin_len    = sizeof(to4);
    to4->sin_family = AF_INET;
    inet_addr_from_ipaddr(&to4->sin_addr, ip_2_ip4(addr));
  }

  err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));
  ping_time = sys_now();
  mem_free(iecho);
  return (err ? ERR_OK : ERR_VAL);
}

static int
ping_recv(int s)
{
  char buf[64];
  int len;
  struct sockaddr_storage from;
  int fromlen = sizeof(from);

  while((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0) {
    if (len >= (int)(sizeof(struct ip_hdr)+sizeof(struct icmp_echo_hdr))) {
      ip_addr_t fromaddr;
      memset(&fromaddr, 0, sizeof(fromaddr));

      if(from.ss_family == AF_INET) {
        struct sockaddr_in *from4 = (struct sockaddr_in*)&from;
        inet_addr_to_ipaddr(ip_2_ip4(&fromaddr), &from4->sin_addr);
        IP_SET_TYPE_VAL(fromaddr, IPADDR_TYPE_V4);
      }

      if (IP_IS_V4_VAL(fromaddr)) {
        struct ip_hdr *iphdr;
        struct icmp_echo_hdr *iecho;

        iphdr = (struct ip_hdr *)buf;
        iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
        if ((iecho->id == PING_ID) && (iecho->seqno == lwip_htons(ping_seq_num))) {
          /* do some ping result processing */
          elapse_time = aos_now_ms() -  ping_time;
          return 0;
        } else {
          LWIP_DEBUGF( PING_DEBUG, ("ping: drop\n"));
        }
      }
    }
    fromlen = sizeof(from);
  }

  if (len == 0) {
      LWIP_DEBUGF( PING_DEBUG, ("ping: recv - timeout\n"));
  }

  return -1;
}

void net_daemon_thread(void *arg)
{
  int s;
  int ret;
  int count;
  int success_count;
  ip4_addr_t *target = (ip4_addr_t *)arg;
  ip4_addr_t *now_addr = NULL;
  ip_addr_t ping_target;
  struct timeval timeout;
  int ping_recv_timeout = POLL_PING_RCV_TIMEO;

  if (!target) {
      LWIP_DEBUGF( PING_DEBUG, ("Invalid arg!"));
      return;
  }

  now_addr = (ip4_addr_t *)malloc(sizeof(ip4_addr_t));
  if (!now_addr) {
     return;
  }

  timeout.tv_sec = ping_recv_timeout/1000;
  timeout.tv_usec = (ping_recv_timeout%1000)*1000;

  ping_target = *target;

  free(target);
  target = NULL;

  if (daemon_started) {
      LWIP_DEBUGF( PING_DEBUG, ("Already started!"));
      free(now_addr);
      return;
  }

  daemon_started = 1;

  s = lwip_socket(AF_INET,  SOCK_RAW, IP_PROTO_ICMP);
  if (s == -1) {
    LWIP_DEBUGF( PING_DEBUG, ("ping: create socket failed\n"));
    goto exit;
  }

  ret = lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  LWIP_ASSERT("setting receive timeout failed", ret == 0);
  LWIP_UNUSED_ARG(ret);

  do {
    count = POLL_PING_COUNT;
    success_count = 0;
    ping_seq_num = 0;

    get_gw_addr(now_addr);
    ping_target = *now_addr;

    while (1) {
      if (daemon_exit) {
         goto exit;
      }

      if(count <= 0) {
          LOGW(PRINT_TAG, "ping: send finished\n");
          break;
      }

      if(count > 0) {
          count --;
      }

      if (ping_send(s, &ping_target) == ERR_OK) {
          if (ping_recv(s) == ERR_OK) {
              success_count++;
              break;
          }
      }

      sys_msleep(POLL_PING_DELAY);
    }

    if (success_count == 0) {
        // reboot netif and exit
        LOGW(PRINT_TAG, "==> Attention! Fail to ping router! <===\n");
        ping_fail_count++;
        sys_msleep(POLL_NET_STATUS_PERIOD_FAST_MS);
    } else {
        LOGI(PRINT_TAG, "==> ping router success! %"U32_F" ms <===\n", elapse_time);
        avg_elapse_time_ms = (avg_elapse_time_ms * ping_success_count + elapse_time) / (ping_success_count + 1);
        ping_success_count++;

        sys_msleep(POLL_NET_STATUS_PERIOD_MS);
    }
  } while (1);

exit:
    if (s != -1) {
      lwip_close(s);
    }
    daemon_started = 0;
    daemon_exit = 0;
    aos_task_exit(0);
    free(now_addr);
}

void start_net_deamon(void)
{
#if 0
    int ret;
    wifi_ip_stat_t ip_stat;
    ip4_addr_t *gw_addr = NULL;

    if (daemon_started) {
        LWIP_DEBUGF( PING_DEBUG, ("Netde Already started!\n"));
        return;
    }

    gw_addr = (ip4_addr_t *)malloc(sizeof(ip4_addr_t));
    if (!gw_addr)
        return;

    memset(&ip_stat, 0, sizeof(ip_stat));
    ret = wifi_get_ip_stat(NULL, &ip_stat, STATION);
    if (ret != 0) {
        LWIP_DEBUGF( PING_DEBUG, ("%s get ip fail\r\n", __func__));
        free(gw_addr);
        return;
    }

    gw_addr->addr = ipaddr_addr(ip_stat.gate);
    if (IPADDR_NONE == gw_addr->addr) {
        LWIP_DEBUGF( PING_DEBUG, ("Convert ipaddr addr failed!\n"));
        free(gw_addr);
        return;
    }

    if ( 0 != aos_task_new("net_deamon", net_daemon_thread,
                           gw_addr, NET_DEAMON_TASK_STACK_SIZE)) {
        free(gw_addr);
    }
#endif
}

void get_gw_addr(ip4_addr_t *gw_addr)
{
    int ret = 0;
    wifi_ip_stat_t ip_stat = {0};
    //ret = wifi_get_ip_stat(NULL, &ip_stat, STATION);
    if (ret != 0) {
        LWIP_DEBUGF( PING_DEBUG, ("%s get ip fail\r\n", __func__));
        return;
    }
    gw_addr->addr = ipaddr_addr(ip_stat.gate);
    if (IPADDR_NONE == gw_addr->addr) {
        LWIP_DEBUGF( PING_DEBUG, ("Convert ipaddr addr failed!\n"));
        return;
    }
    return;
}
void stop_net_deamon(void)
{
    daemon_exit = 1;
}

u32_t get_poll_ping_success_count(uint32_t *value, int clear)
{
    if (clear){
        ping_success_count = 0;
    }
    *value = ping_success_count;
    return ping_success_count;
}

u32_t get_poll_ping_fail_count(uint32_t *value, int clear)
{
    if (clear){
        ping_fail_count = 0;
    }
    *value = ping_fail_count;
    return 0;
}

void get_poll_ping_avg_elapse_ms(float *value, int clear)
{
    if (clear){
        avg_elapse_time_ms = 0.0;
    }
    //LOGI(PRINT_TAG, "get avg_ping = %lf\n",avg_elapse_time_ms);
    *value = avg_elapse_time_ms;
}

void get_poll_last_ping_elapse_ms(uint32_t *value)
{
    *value = elapse_time;
}

