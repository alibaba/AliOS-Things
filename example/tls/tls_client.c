/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <aos/network.h>
#include <aos/aos.h>
#include <netmgr.h>

#include <aos/mbedtls_ssl.h>
#include "mbedtls/net_sockets.h"

struct cookie {
    int flag;
};

const char *tls_test_ca_pem = "-----BEGIN CERTIFICATE-----\n"   \
"MIIDtzCCAp+gAwIBAgIJAOxbLdldR1+SMA0GCSqGSIb3DQEBBQUAMHIxCzAJBgNV\n"\
"BAYTAkNOMREwDwYDVQQIDAh6aGVqaWFuZzERMA8GA1UEBwwIaGFuZ3pob3UxEDAO\n"\
"BgNVBAoMB2FsaWJhYmExDjAMBgNVBAsMBXl1bm9zMRswGQYDVQQDDBIqLnNtYXJ0\n"\
"LmFsaXl1bi5jb20wHhcNMTQwOTE3MDI0OTM0WhcNMjQwOTE0MDI0OTM0WjByMQsw\n"\
"CQYDVQQGEwJDTjERMA8GA1UECAwIemhlamlhbmcxETAPBgNVBAcMCGhhbmd6aG91\n"\
"MRAwDgYDVQQKDAdhbGliYWJhMQ4wDAYDVQQLDAV5dW5vczEbMBkGA1UEAwwSKi5z\n"\
"bWFydC5hbGl5dW4uY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA\n"\
"pwFEj4onz7YZ0ESpG7BNZmuK4KJgGMlFHnEL3AT1YtvB7DGePTNsW9hX3WYsaqy7\n"\
"+4PyzJgfNJY3WQr6uPv/EMwqlYMO0F8kg9AmFepuicHh5JvCeTJciG7OH/8qHhEB\n"\
"3b3w35un5YxUXuffw3SiFTj+vnFdc3Yj9pBv++0nsDvl6l8TSkJMnRLY8lRzzi1T\n"\
"rbdsDeNXLnfeThElMPFeI1h+s7amt2ktBGnv6HAg7a9OehUI8uTpFZ7559Yf8Dpm\n"\
"MDijYc6LLLSE6OO5C7im0pg8IRu6oZo1F5raK5gbRU/QI7K58IuIo+k4+clcvtko\n"\
"Ck4RkwdvC8cc0u5mJ8mXJwIDAQABo1AwTjAdBgNVHQ4EFgQUw6RWDo81JEoy+Vnf\n"\
"vMTvRsLkZ30wHwYDVR0jBBgwFoAUw6RWDo81JEoy+VnfvMTvRsLkZ30wDAYDVR0T\n"\
"BAUwAwEB/zANBgkqhkiG9w0BAQUFAAOCAQEAS2UQtfguKHnlxH9jpfJUyGKFoaTT\n"\
"8/XhPW3CW9c++zDrgNq920iQ42Yl9zB58iJ/v0w6n9quTtIta6QD72ssEJtWc2v2\n"\
"rwu14WJB5tGhBRagMvtF7p/56KYxib0p3fqjaE3Neer5r8Mgb6oI13tHbf0WT4JM\n"\
"GJCLsDoz4ZwvemLISeonZVSVIezs0BDU/TeEK2kIeUDB14FR6fY/U4ovS/v+han8\n"\
"NLhWorEpB1p2sgnSPgSVc6ZPHHyjIQOcWdn56vnOf41rLF/zqjD0Sn7YgceAd5OT\n"\
"rJ/t7PbiC/sn8SR7+0ATOMh0vRSA9HuuvoDz0adMhoFnba2FwiENfsLlhw==\n"    \
"-----END CERTIFICATE-----\n";

const char *server_name = "alink.tcp.aliyun.com";
const int server_port = 443;

unsigned char alink_req_data[] = {
    0x57, 0x53, 0x46, 0x31, 0x00, 0x00, 0x00, 0x11,
    0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03
};

extern int dtls_client_sample(void);

#if !defined(STM32_USE_SPI_WIFI)
void *network_socket_create(const char *net_addr, int port)
{
    int ret;
    int tcp_fd;
    struct hostent *host;
    struct sockaddr_in saddr;
    int opt_val = 1;

    set_errno(0);
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_fd < 0) {
        printf("creat socket fail - errno: %d\n", errno);
        return NULL;
    }

    host = gethostbyname(net_addr);
    if (host == NULL) {
        printf("get host by name fail - errno: %d\n", errno);
        goto _err;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = *(unsigned long *)(host->h_addr);

    setsockopt(tcp_fd, SOL_SOCKET, SO_RCVTIMEO, &opt_val, sizeof(opt_val));

    do {
        set_errno(0);
        ret = connect(tcp_fd, (struct sockaddr*)&saddr, sizeof(saddr));
        if (ret < 0 && errno != EINTR) {
            printf("socket connect fail - errno: %d\n", errno);
            goto _err;
        }
    } while (ret < 0);

    return (void *)tcp_fd;

_err:
    close(tcp_fd);

    return (void *)-1;
}

void network_socket_destroy(void *tcp_fd)
{
    if (tcp_fd == NULL) {
        return;
    }

    close((int)tcp_fd);

    return;
}

#else /* STM32_USE_SPI_WIFI */

void *network_socket_create(mbedtls_net_context *ctx, const char *net_addr, int port)
{
    int ret;
    char server_port[8];

    mbedtls_net_init(ctx);

    memset(server_port, 0, 8);
    sprintf(server_port, "%d", port);

    ret = mbedtls_net_connect(ctx,
              net_addr, server_port, MBEDTLS_NET_PROTO_TCP);
    if (ret != 0) {
        printf("net connect fail - %d\n", ret);
        return (void *)-1;
    }

    return (void *)ctx->fd;
}

void network_socket_destroy(mbedtls_net_context *ctx)
{
    mbedtls_net_free(ctx);

    return;
}

#endif /* STM32_USE_SPI_WIFI */

static void tls_client_sample(void)
{
    int ret = 0;
    char buf[128];
    void *tcp_fd = NULL;
    void *ssl = NULL;
#if defined(STM32_USE_SPI_WIFI)
    mbedtls_net_context server_fd;
#endif

#if !defined(STM32_USE_SPI_WIFI)
    tcp_fd = network_socket_create(server_name, server_port);
#else
    tcp_fd = network_socket_create(&server_fd, server_name, server_port);
#endif
    if ((int)tcp_fd < 0) {
        printf("http client connect fail\n");
        return;
    } else {
        printf("network socket create ok\n");
    }

    ssl = mbedtls_ssl_connect(tcp_fd,
              tls_test_ca_pem, strlen(tls_test_ca_pem));
    if (ssl == NULL) {
        printf("ssl connect fail\n");
        goto _out;
    } else {
        printf("mbedtls ssl connect ok\n");
    }

    ret = mbedtls_ssl_send(ssl, (char *)alink_req_data,
                          (int)sizeof(alink_req_data));
    if (ret < 0) {
        printf("ssl send fail\n");
        goto _out;
    } else {
        printf("mbedtls ssl send ok\n");
    }

    do {
        ret = mbedtls_ssl_recv(ssl, buf, 128);
        if (ret < 0 && ret != -EAGAIN) {
            printf("ssl recv fail\n");
            goto _out;
        } else {
            printf("mbedtls ssl recv ok\n");
        }
    } while(ret < 0);

_out:
    if (ssl != NULL) {
        mbedtls_ssl_close(ssl);
    }

    if (tcp_fd != NULL) {
#if !defined(STM32_USE_SPI_WIFI)
        network_socket_destroy(tcp_fd);
#else
        network_socket_destroy(&server_fd);
#endif
    }

    return;
}

static void app_delayed_action(void *arg)
{
    printf("===========> TLS Client Sample start.\n");
    tls_client_sample();
    printf("<=========== TLS Client Sample End.\n\n");

#if defined(DTLS_ENABLED)
    printf("===========> DTLS Client Sample start.\n");
    dtls_client_sample();
    printf("<=========== DTLS Client Sample End.\n\n");
#endif
}

static void handle_event(input_event_t *event, void *arg)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    aos_post_delayed_action(1000, app_delayed_action, arg);
}

int application_start(void)
{
    struct cookie *cookie = aos_malloc(sizeof(*cookie));
    bzero(cookie, sizeof(*cookie));

    aos_register_event_filter(EV_WIFI, handle_event, cookie);

    netmgr_init();
#if !defined(STM32_USE_SPI_WIFI)
    netmgr_start(true);
#else
    netmgr_start(false);
#endif

    aos_loop_run();
    /* never return */

    return 0;
}

