/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "be_ssdp.h"
#include <string.h>
#include <unistd.h>
#include "be_jse_task.h"
#include "be_log.h"
#include "be_port_osal.h"

#ifdef BE_OS_AOS
#if defined(WITH_LWIP)
#include <lwip/netdb.h>
#include <lwip/pbuf.h>
#include <lwip/sockets.h>
#include <lwip/tcp.h>
#include <lwip/udp.h>
#include "lwip/err.h"
#include "lwip/sys.h"
#else
#include <network/network.h>
#endif
#endif

#ifdef LINUXOSX
#include <arpa/inet.h>
#include <be_osal_net.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#ifdef USE_FREERTOS
#include <lwip/netdb.h>
#include <sys/socket.h>
#include "hal/system.h"
#endif

#include "websocket.h"

#define MODULE_TAG ("be_debuger")
#define CLI_REPLY printf

static char pLocalAddress[16] = {0};

#if defined(WITH_LWIP) || defined(LINUXOSX) || defined(USE_FREERTOS)

#define WEB_SERVER_PORT 9001
#define WEB_SERVER_PATH "/socket.io/?EIO=3&transport=websocket"

void connect_webserver(void* ip)
{
    printf("connect_webserver ip=%s\r\n", (char*)ip);

    if (NULL == ip) {
        return;
    }

    if (bone_websocket_getInstance() != NULL) {
        free(ip);
        return;
    }

    bone_websocket_client_t* client;

    client = calloc(1, sizeof(bone_websocket_client_t));
    if (client == NULL) {
        free(ip);
        return;
    }

    bone_websocket_init(client);

    strcpy(client->ip, ip);
    client->port = WEB_SERVER_PORT;
    strcpy(client->path, WEB_SERVER_PATH);

    printf("%s ip = %s \n", __FUNCTION__, (char*)ip);
    bone_websocket_connect(client);

    free(ip);
    ip = NULL;
}

static char target_ip[16] = {0};

void be_debuger_websocket_reconnect()
{
    if (target_ip[0]) {
        be_jse_task_timer_action(200, connect_webserver, strdup(target_ip),
                                 JSE_TIMER_ONCE);
    }
}

#define MULTICAST_IPV4_ADDR "239.255.255.250"

static int ssdp_sock = -1;

static int socket_add_ipv4_multicast_group(int sock, bool assign_source_if)
{
    struct ip_mreq imreq = {0};
    struct in_addr iaddr = {0};
    int err              = 0;
    /* Configure source interface */
    imreq.imr_interface.s_addr = IPADDR_ANY;

    /* Configure multicast address to listen to */
    err = inet_aton(MULTICAST_IPV4_ADDR, &imreq.imr_multiaddr.s_addr);
    if (err != 1) {
        be_debug(MODULE_TAG,
                 "Configured IPV4 multicast address '%s' is invalid.",
                 MULTICAST_IPV4_ADDR);
        goto err;
    }
    be_debug(MODULE_TAG, "Configured IPV4 Multicast address %s",
             inet_ntoa(imreq.imr_multiaddr));

#if defined(WITH_LWIP)
    if (!IP_MULTICAST(ntohl(imreq.imr_multiaddr.s_addr))) {
        be_debug(MODULE_TAG,
                 "Configured IPV4 multicast address '%s' is not a valid "
                 "multicast address. This will probably not work.",
                 MULTICAST_IPV4_ADDR);
    }
#endif

    if (assign_source_if) {
        /* Assign the IPv4 multicast source interface, via its IP
           (only necessary if this socket is IPV4 only) */
        err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &iaddr,
                         sizeof(struct in_addr));
        if (err < 0) {
            be_debug(MODULE_TAG, "Failed to set IP_MULTICAST_IF. Error %d",
                     errno);
            goto err;
        }
    }

    err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imreq,
                     sizeof(struct ip_mreq));
    if (err < 0) {
        be_debug(MODULE_TAG, "Failed to set IP_ADD_MEMBERSHIP. Error %d",
                 errno);
        goto err;
    }

    printf("%s %d\n\r", __FUNCTION__, __LINE__);
err:
    return err;
}

static int create_multicast_ipv4_socket()
{
    struct sockaddr_in saddr = {0};
    int sock                 = -1;
    int err                  = 0;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        be_debug(MODULE_TAG, "Failed to create socket. Error %d", errno);
        return -1;
    }

    be_debug(MODULE_TAG, "create_multicast_ipv4_socket =  %d", sock);
    /* Bind the socket to any address */
    saddr.sin_family      = PF_INET;
    saddr.sin_port        = htons(BE_DEBUGER_SSDP_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in));
    if (err < 0) {
        be_debug(MODULE_TAG, "Failed to bind socket. Error %d", errno);
        goto err;
    }

    /* Assign multicast TTL (set separately from normal interface TTL) */
    uint8_t ttl = 5;
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(uint8_t));
    if (err < 0) {
        be_debug(MODULE_TAG, "Failed to set IP_MULTICAST_TTL. Error %d", errno);
        goto err;
    }

#if MULTICAST_LOOPBACK
    /* select whether multicast traffic should be received by this device, too
       (if setsockopt() is not called, the default is no) */
    uint8_t loopback_val = MULTICAST_LOOPBACK;
    err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loopback_val,
                     sizeof(uint8_t));
    if (err < 0) {
        be_debug(MODULE_TAG, "Failed to set IP_MULTICAST_LOOP. Error %d",
                 errno);
        goto err;
    }
#endif

    /* this is also a listening socket, so add it to the multicast
       group for listening... */
    err = socket_add_ipv4_multicast_group(sock, true);
    if (err < 0) {
        goto err;
    }

    /* All set, socket is configured for sending and receiving */
    return sock;

err:
    close(sock);
    return -1;
}

static void on_ssdp_recv(int sock, void* arg)
{
    char recvbuf[128] = {0};
    struct sockaddr_in raddr; /* Large enough for both IPv4 or IPv6 */
    socklen_t socklen = sizeof(raddr);

    int len = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,
                       (struct sockaddr*)&raddr, &socklen);
    if (len < 0) {
        be_debug(MODULE_TAG, "multicast recvfrom failed: errno %d", errno);
        return;
    }

    char* urn_prefix = strstr(recvbuf, "urn:be-debuger");
    if (urn_prefix != NULL) {
        if (strstr(recvbuf, "NOTIFY * HTTP/1.1") != NULL) {
            char* urn_prefix_end = strstr(urn_prefix, "\r\n");
            int urn_prefix_len   = strlen("urn:be-debuger");
            if (urn_prefix_end != NULL &&
                (urn_prefix_end - urn_prefix) > urn_prefix_len) {
                char* st_ip = urn_prefix + urn_prefix_len + 1;
                if (pLocalAddress != NULL &&
                    strncmp(st_ip, pLocalAddress, strlen(pLocalAddress)) == 0) {
                    char* remote_address = (char*)calloc(1, 16);
#ifdef BE_OS_AOS
                    inet_ntoa_r(((struct sockaddr_in*)&raddr)->sin_addr.s_addr,
                                remote_address, 16);
#else
                    inet_ntop(AF_INET, &((struct sockaddr_in*)&raddr)->sin_addr,
                              remote_address, 16);
#endif
                    be_debug(MODULE_TAG, "prepare connect: %s\n",
                             remote_address);
                    strcpy(target_ip, remote_address);
                    hal_system_kv_set("WS_ADDRESS", remote_address,
                                      strlen(remote_address), 1);
                    be_jse_task_timer_action(100, connect_webserver,
                                             remote_address, JSE_TIMER_ONCE);
                }
            }
        } else if (strstr(recvbuf, "M-SEARCH * HTTP/1.1") != NULL) {
            const char* reply = "HTTP/1.1 200 OK\r\n\r\n";
            sendto(sock, reply, strlen(reply), 0,
                   (const struct sockaddr*)&raddr, socklen);
            be_debug(MODULE_TAG, "send m-search reply \n");
        }
    }
}

void ssdp_read(void* arg)
{
    fd_set readfds;
    struct timeval tv;
    int result;
    int sockfd;

    sockfd = ssdp_sock;

    /* int flags;
       flags = fcntl(sockfd, F_GETFL, 0);
       fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); */
    tv.tv_usec = 0;
    tv.tv_sec  = 1;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        result = select(sockfd + 1, &readfds, 0, 0, &tv);
        if (result > 0) {
            if (FD_ISSET(sockfd, &readfds)) {
                on_ssdp_recv(sockfd, NULL);
            }
        } else if (result < 0) {
            break;
        } else {
            /* be_debug("ssdp", "select timeout%d\n\r",sockfd); */
        }
    }
}

#endif

static int ssdp_started = 0;
int be_debuger_ssdp_start(char* localAddress)
{
    if (ssdp_started) return 0;

    ssdp_started = 1;

    if (localAddress) {
        strncpy(pLocalAddress, localAddress, 16);
    }

#if defined(WITH_LWIP) || defined(LINUXOSX) || defined(USE_FREERTOS)
    ssdp_sock = create_multicast_ipv4_socket();
    printf("ssdp_sock=%d\n\r", ssdp_sock);
    be_osal_create_task("ssdpTsk", ssdp_read, NULL, 1024 * 4, SSDP_TSK_PRIORITY,
                        NULL);

    char* remote_address = (char*)calloc(1, 16);
    uint32_t buffer_len  = 16;
    hal_system_kv_get("WS_ADDRESS", remote_address, &buffer_len);

    if (strlen(remote_address) != 0) {
        be_debug(MODULE_TAG, "be_debuger_ssdp_start remote_address: %s \n",
                 remote_address);
        connect_webserver(remote_address);
    }
#endif
    return 0;
}
