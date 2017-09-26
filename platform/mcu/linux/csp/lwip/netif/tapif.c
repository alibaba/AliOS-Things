/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
//#include <sys/uio.h>
#include <sys/socket.h>

#include "lwip/opt.h"
#include "lwip/tcpip.h"

#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/ip.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"

#if defined(LWIP_DEBUG) && defined(LWIP_TCPDUMP)
#include "netif/tcpdump.h"
#endif /* LWIP_DEBUG && LWIP_TCPDUMP */

#include "netif/tapif.h"

#define IFCONFIG_BIN "/sbin/ifconfig "

#if defined(LWIP_UNIX_LINUX)
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include <semaphore.h>
#include <cpu_event.h>
#include <pthread.h>
#include <signal.h>
/*
 * Creating a tap interface requires special privileges. If the interfaces
 * is created in advance with `tunctl -u <user>` it can be opened as a regular
 * user. The network must already be configured. If DEVTAP_IF is defined it
 * will be opened instead of creating a new tap device.
 *
 * You can also use PRECONFIGURED_TAPIF environment variable to do so.
 */
#ifndef DEVTAP_DEFAULT_IF
#define DEVTAP_DEFAULT_IF "tap0"
#endif
#ifndef DEVTAP
#define DEVTAP "/dev/net/tun"
#endif
#define NETMASK_ARGS "netmask %d.%d.%d.%d"
#define IFCONFIG_ARGS "tap0 inet %d.%d.%d.%d " NETMASK_ARGS
#elif defined(LWIP_UNIX_OPENBSD)
#define DEVTAP "/dev/tun0"
#define NETMASK_ARGS "netmask %d.%d.%d.%d"
#define IFCONFIG_ARGS "tun0 inet %d.%d.%d.%d " NETMASK_ARGS " link0"
#else /* others */
#define DEVTAP "/dev/tap0"
#define NETMASK_ARGS "netmask %d.%d.%d.%d"
#define IFCONFIG_ARGS "tap0 inet %d.%d.%d.%d " NETMASK_ARGS
#endif

/* Define those to better describe your network interface. */
#define IFNAME0 't'
#define IFNAME1 'p'

#ifndef TAPIF_DEBUG
#define TAPIF_DEBUG LWIP_DBG_OFF
#endif

struct tapif
{
    /* Add whatever per-interface state that is needed here. */
    int fd;
};

/* Forward declarations. */
static void *tapif_thread(void *arg);

/* For select thread, the driver layer of lwip */
static pthread_t tid;

/* For pthread sem signal */
static sem_t pthread_sem;

/* For sigmask, ignore SYSALARM signal */
static sigset_t sigset;

/*-----------------------------------------------------------------------------------*/
static void low_level_init(struct netif *netif)
{
    struct tapif *tapif;
#if LWIP_IPV4
    int ret;
    char buf[1024];
#endif /* LWIP_IPV4 */
    char *preconfigured_tapif = getenv("PRECONFIGURED_TAPIF");

    tapif = (struct tapif *)netif->state;

    /* Obtain MAC address from network interface. */

    /* (We just fake an address...) */
    netif->hwaddr[0] = 0x02;
    netif->hwaddr[1] = 0x12;
    netif->hwaddr[2] = 0x34;
    netif->hwaddr[3] = 0x56;
    netif->hwaddr[4] = 0x78;
    netif->hwaddr[5] = 0xab;
    netif->hwaddr_len = 6;

    /* device capabilities */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

    tapif->fd = open(DEVTAP, O_RDWR);
    LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_init: fd %d\n", tapif->fd));
    if (tapif->fd == -1) {
#ifdef LWIP_UNIX_LINUX
        perror("tapif_init: try running \"modprobe tun\" or rebuilding your kernel with CONFIG_TUN; cannot open "DEVTAP);
#else /* LWIP_UNIX_LINUX */
        perror("tapif_init: cannot open "DEVTAP);
#endif /* LWIP_UNIX_LINUX */
        exit(1);
    }

#ifdef LWIP_UNIX_LINUX
    {
      struct ifreq ifr;
      memset(&ifr, 0, sizeof(ifr));

      if (preconfigured_tapif) {
        strncpy(ifr.ifr_name, preconfigured_tapif, sizeof(ifr.ifr_name));
      } else {
        strncpy(ifr.ifr_name, DEVTAP_DEFAULT_IF, sizeof(ifr.ifr_name));
      }
      ifr.ifr_name[sizeof(ifr.ifr_name)-1] = 0; /* ensure \0 termination */

      ifr.ifr_flags = IFF_TAP|IFF_NO_PI;
      if (ioctl(tapif->fd, TUNSETIFF, (void *) &ifr) < 0) {
          perror("tapif_init: "DEVTAP" ioctl TUNSETIFF");
          exit(1);
      }
    }
#endif /* LWIP_UNIX_LINUX */

    netif_set_link_up(netif);

    if (preconfigured_tapif == NULL) {
#if LWIP_IPV4
        snprintf(buf, 1024, IFCONFIG_BIN IFCONFIG_ARGS,
                 ip4_addr1(netif_ip4_gw(netif)),
                 ip4_addr2(netif_ip4_gw(netif)),
                 ip4_addr3(netif_ip4_gw(netif)),
                 ip4_addr4(netif_ip4_gw(netif))
#ifdef NETMASK_ARGS
                 ,
                 ip4_addr1(netif_ip4_netmask(netif)),
                 ip4_addr2(netif_ip4_netmask(netif)),
                 ip4_addr3(netif_ip4_netmask(netif)),
                 ip4_addr4(netif_ip4_netmask(netif))
#endif /* NETMASK_ARGS */
        );

    LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_init: system(\"%s\");\n", buf));
    ret = system(buf);
    if (ret < 0) {
        perror("ifconfig failed");
        exit(1);
    }
    if (ret != 0) {
        printf("ifconfig returned %d\n", ret);
    }
#else /* LWIP_IPV4 */
    perror("todo: support IPv6 support for non-preconfigured tapif");
    exit(1);
#endif /* LWIP_IPV4 */
    }

    /* it has to be pthread, as it needs to access linux functions */
    pthread_create(&tid, NULL, tapif_thread, netif);
}

/*-----------------------------------------------------------------------------------*/
/*
 * low_level_output():
 *
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 */
/*-----------------------------------------------------------------------------------*/

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct tapif *tapif = (struct tapif *)netif->state;
    char buf[1514];
    ssize_t written;

    /* initiate transfer(); */
    pbuf_copy_partial(p, buf, p->tot_len, 0);

    /* signal that packet should be sent(); */
    written = write(tapif->fd, buf, p->tot_len);
    if (written == -1) {
        MIB2_STATS_NETIF_INC(netif, ifoutdiscards);
        perror("tapif: write");
    }
    else {
        MIB2_STATS_NETIF_ADD(netif, ifoutoctets, written);
    }
    return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_input():
 *
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 */
/*-----------------------------------------------------------------------------------*/
static struct pbuf* low_level_input(struct netif *netif, void *buf, int len)
{
    struct pbuf *p;

    MIB2_STATS_NETIF_ADD(netif, ifinoctets, len);

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (p != NULL) {
        pbuf_take(p, buf, len);
        /* acknowledge that packet has been read(); */
    } else {
        /* drop packet(); */
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
        LWIP_DEBUGF(NETIF_DEBUG, ("tapif_input: could not allocate pbuf\n"));
    }

    return p;
}

/*-----------------------------------------------------------------------------------*/
/*
 * tapif_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */
/*-----------------------------------------------------------------------------------*/
struct netif_buf {
    struct netif *netif;
    int len;
    char buf[0];
};

static void tapif_input(struct netif_buf *nbuf)
{
    struct netif *netif = nbuf->netif;
    void *buf = nbuf->buf;
    int len = nbuf->len;
    struct pbuf *p = low_level_input(netif, buf, len);

    if (p == NULL) {
#if LINK_STATS
        LINK_STATS_INC(link.recv);
#endif /* LINK_STATS */
        LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_input: low_level_input returned NULL\n"));
        return;
    }

    LOCK_TCPIP_CORE();
    if (netif->input(p, netif) != ERR_OK) {
        LWIP_DEBUGF(NETIF_DEBUG, ("tapif_input: netif input error\n"));
        pbuf_free(p);
    }
    UNLOCK_TCPIP_CORE();

    cpu_event_free(nbuf);
}

/*-----------------------------------------------------------------------------------*/
/*
 * tapif_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */
/*-----------------------------------------------------------------------------------*/
err_t tapif_init(struct netif *netif)
{
    struct tapif *tapif = (struct tapif *)mem_malloc(sizeof(struct tapif));

    if (tapif == NULL) {
        LWIP_DEBUGF(NETIF_DEBUG, ("tapif_init: out of memory for tapif\n"));
        return ERR_MEM;
    }
    netif->state = tapif;
    MIB2_INIT_NETIF(netif, snmp_ifType_other, 100000000);

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
#if LWIP_IPV4
    netif->output = etharp_output;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = low_level_output;
    netif->mtu = 1500;

    low_level_init(netif);

    return ERR_OK;
}


/*-----------------------------------------------------------------------------------*/
static void *tapif_thread(void *arg)
{
    struct netif *netif;
    struct tapif *tapif;
    fd_set fdset;
    int ret;

    ret = sem_init(&pthread_sem, 0, 0);

    netif = (struct netif *)arg;
    tapif = (struct tapif *)netif->state;

    sigaddset(&sigset, SIGALRM);
    ret = pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    if(ret != 0) {
        perror("sigmask failed");
        return NULL;
    }

    while(1) {
        FD_ZERO(&fdset);
        FD_SET(tapif->fd, &fdset);

        /* Wait for a packet to arrive. */
        ret = select(tapif->fd + 1, &fdset, NULL, NULL, NULL);
        if(ret == 1) {
            struct netif_buf *nbuf = cpu_event_malloc(sizeof(*nbuf) + 2048);
            nbuf->len = 2048;
            nbuf->netif = netif;

            /* Obtain the size of the packet and put it into the "len"
             variable. */
            nbuf->len = read(tapif->fd, nbuf->buf, nbuf->len);
            if (nbuf->len < 0) {
                perror("tapif read eror");
                continue;
            }

            /* Handle incoming packet. */
            cpu_call_handler((cpu_event_handler)tapif_input, nbuf);
        } else if(ret == -1) {
            perror("tapif_thread: select");
        }
    }

    return NULL;
}

