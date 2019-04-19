#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mbedtls/config.h"
#include "mbedtls/net_sockets.h"

#define MBEDTLS_NET_PRINT(_f, ...) \
    printf("%s %d: "_f, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#if defined(MBEDTLS_NET_ALT)
#if defined(STM32_USE_SPI_WIFI)

#include "stm32_wifi.h"
#include "mbedtls/net_sockets.h"

#define WIFI_WRITE_TIMEOUT 200
#define WIFI_READ_TIMEOUT 200
#define WIFI_PAYLOAD_SIZE ES_WIFI_PAYLOAD_SIZE
#define WIFI_READ_RETRY_TIME 5

void mbedtls_net_init(mbedtls_net_context *ctx)
{
    ctx->fd = -1;
}

int mbedtls_net_connect(mbedtls_net_context *ctx, const char *host,
                        const char *port, int proto)
{
    WIFI_Status_t   ret;
    WIFI_Protocol_t type;
    uint8_t         ip_addr[4];

    ret = WIFI_GetHostAddress((char *)host, ip_addr);
    if (ret != WIFI_STATUS_OK) {
        MBEDTLS_NET_PRINT("net_connect: get host addr fail - %d\n", ret);
        return MBEDTLS_ERR_NET_UNKNOWN_HOST;
    }

    type =
      proto == MBEDTLS_NET_PROTO_UDP ? WIFI_UDP_PROTOCOL : WIFI_TCP_PROTOCOL;
    ret = WIFI_OpenClientConnection(0, type, "", ip_addr, atoi(port), 0);
    if (ret != WIFI_STATUS_OK) {
        MBEDTLS_NET_PRINT("net_connect: open client fail - %d\n", ret);
        return MBEDTLS_ERR_NET_CONNECT_FAILED;
    }

    ctx->fd = 0;

    return ret;
}

int mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
    WIFI_Status_t ret;
    uint16_t      send_size;
    uint16_t      once_len;
    uint8_t *     pdata      = (uint8_t *)buf;
    uint16_t      send_total = 0;
    int           fd         = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        MBEDTLS_NET_PRINT("net_send: invalid socket fd\n");
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    }

    do {
        if (len > WIFI_PAYLOAD_SIZE) {
            MBEDTLS_NET_PRINT(
              "net_send: buffer length = %d, split data sending\n", len);
            once_len = WIFI_PAYLOAD_SIZE;
            len -= WIFI_PAYLOAD_SIZE;
        } else {
            once_len = len;
            len      = 0;
        }

        ret = WIFI_SendData((uint8_t)fd, pdata, once_len, &send_size,
                            WIFI_WRITE_TIMEOUT);
        if (ret != WIFI_STATUS_OK) {
            MBEDTLS_NET_PRINT("net_send: send data fail - %d\n", ret);
            return MBEDTLS_ERR_NET_SEND_FAILED;
        }
        pdata += once_len;
        send_total += send_size;
    } while (len > 0);

    return send_total;
}

int mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len)
{
    WIFI_Status_t ret;
    uint16_t      recv_size;
    int           fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        MBEDTLS_NET_PRINT("net_recv: invalid socket fd\n");
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    if (len > WIFI_PAYLOAD_SIZE) {
        len = WIFI_PAYLOAD_SIZE;
    }
    int err_count = 0;
    do {
        ret = WIFI_ReceiveData((uint8_t)fd, buf, (uint16_t)len, &recv_size,
                               WIFI_READ_TIMEOUT);
        if (ret != WIFI_STATUS_OK) {
            MBEDTLS_NET_PRINT("net_recv: receive data fail - %d\n", ret);
            return MBEDTLS_ERR_NET_RECV_FAILED;
        }

        // TODO, how to identify the connection is shutdown?
        if (recv_size == 0) {
            if (err_count == WIFI_READ_RETRY_TIME) {
                MBEDTLS_NET_PRINT("retry WIFI_ReceiveData %d times failed\n",
                                  err_count);
                return MBEDTLS_ERR_SSL_WANT_READ;
            } else {
                err_count++;
                MBEDTLS_NET_PRINT("retry WIFI_ReceiveData time %d\n",
                                  err_count);
            }
        }
    } while (ret == WIFI_STATUS_OK && recv_size == 0);

    return recv_size;
}

int mbedtls_net_recv_timeout(void *ctx, unsigned char *buf, size_t len,
                             uint32_t timeout)
{
    WIFI_Status_t ret;
    uint16_t      recv_size;
    int           fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        MBEDTLS_NET_PRINT("net_recv_timeout: invalid socket fd\n");
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    if (len > WIFI_PAYLOAD_SIZE) {
        len = WIFI_PAYLOAD_SIZE;
    }

    // TODO: STM32 WiFi module can't set mqtt default timeout 60000, will return
    // error, need to check with WiFi module, ignore param "timeout"
    int err_count = 0;
    do {
        ret = WIFI_ReceiveData((uint8_t)fd, buf, (uint16_t)len, &recv_size,
                               WIFI_READ_TIMEOUT);
        if (ret != WIFI_STATUS_OK) {
            MBEDTLS_NET_PRINT("net_recv_timeout: receive data fail - %d\n",
                              ret);
            return MBEDTLS_ERR_NET_RECV_FAILED;
        }

        // TODO, how to identify the connection is shutdown?
        if (recv_size == 0) {
            if (err_count == WIFI_READ_RETRY_TIME) {
                MBEDTLS_NET_PRINT("retry WIFI_ReceiveData %d times failed\n",
                                  err_count);
                return MBEDTLS_ERR_SSL_WANT_READ;
            } else {
                err_count++;
                MBEDTLS_NET_PRINT("retry WIFI_ReceiveData time %d\n",
                                  err_count);
            }
        }
    } while (ret == WIFI_STATUS_OK && recv_size == 0);

    return recv_size;
}

void mbedtls_net_free(mbedtls_net_context *ctx)
{
    WIFI_Status_t ret;

    if (ctx->fd == -1) {
        return;
    }

    ret = WIFI_CloseClientConnection((uint32_t)ctx->fd);
    if (ret != WIFI_STATUS_OK) {
        MBEDTLS_NET_PRINT("net_free: close client fail - %d\n", ret);
        return;
    }

    ctx->fd = -1;
}

#else /* STM32_USE_SPI_WIFI */

#include <fcntl.h>
#include <unistd.h>
#include <aos/errno.h>

#include <aos/network.h>


#if (defined(_WIN32) || defined(_WIN32_WCE)) && !defined(EFIX64) && \
  !defined(EFI32)
/*
 * Check if the requested operation would be blocking on a non-blocking socket
 * and thus 'failed' with a negative return value.
 */
static int net_would_block(const mbedtls_net_context *ctx)
{
    ((void)ctx);
    return (WSAGetLastError() == WSAEWOULDBLOCK);
}
#else
static int net_would_block(const mbedtls_net_context *ctx)
{
    /*
     * Never return 'WOULD BLOCK' on a non-blocking socket
     */
    if ((fcntl(ctx->fd, F_GETFL, 0) & O_NONBLOCK) != O_NONBLOCK) {
        return 0;
    }

    switch (errno) {
#if defined EAGAIN
        case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
#endif
            return 1;
    }

    return 0;
}
#endif

void mbedtls_net_init(mbedtls_net_context *ctx)
{
    ctx->fd = -1;
}

int mbedtls_net_connect(mbedtls_net_context *ctx, const char *host,
                        const char *port, int proto)
{
    int             ret;
    struct addrinfo hints, *addr_list, *cur;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype =
      proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol =
      proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    if ((ret = getaddrinfo(host, port, &hints, &addr_list)) != 0) {
        MBEDTLS_NET_PRINT("getaddrinfo fail- errno: %d, %d\n", errno, ret);
        return MBEDTLS_ERR_NET_UNKNOWN_HOST;
    }

    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;

    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        ctx->fd =
          (int)socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (ctx->fd < 0) {
            ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
            continue;
        }

        do {
            ret = connect(ctx->fd, cur->ai_addr, cur->ai_addrlen);
            if (ret == 0) {
                goto _out;
            } else {
                if (errno == EINTR) {
                    continue;
                }

                break;
            }
        } while (1);

        close(ctx->fd);
        ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
    }

_out:
    freeaddrinfo(addr_list);

    return ret;
}

int mbedtls_net_set_block(mbedtls_net_context *ctx)
{
#if (defined(_WIN32) || defined(_WIN32_WCE)) && !defined(EFIX64) && \
  !defined(EFI32)
    u_long n = 0;
    return (ioctlsocket(ctx->fd, FIONBIO, &n));
#else
    int flags;

    flags = fcntl(ctx->fd, F_GETFL, 0);
    flags &= ~O_NONBLOCK;

    return fcntl(ctx->fd, F_SETFL, flags);
#endif
}

int mbedtls_net_set_nonblock(mbedtls_net_context *ctx)
{
#if (defined(_WIN32) || defined(_WIN32_WCE)) && !defined(EFIX64) && \
  !defined(EFI32)
    u_long n = 1;
    return (ioctlsocket(ctx->fd, FIONBIO, &n));
#else
    int flags;

    flags = fcntl(ctx->fd, F_GETFL, 0);
    flags |= O_NONBLOCK;

    return fcntl(ctx->fd, F_SETFL, flags);
#endif
}

int mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len)
{
    int ret;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        MBEDTLS_NET_PRINT("invalid socket fd\n");
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    ret = (int)read(fd, buf, len);

    if (ret < 0) {
        if (net_would_block(ctx) != 0) {
            return (MBEDTLS_ERR_SSL_WANT_READ);
        }

        if (errno == EPIPE || errno == ECONNRESET) {
            MBEDTLS_NET_PRINT("net reset - errno: %d\n", errno);
            return (MBEDTLS_ERR_NET_CONN_RESET);
        }

        if (errno == EINTR) {
            return (MBEDTLS_ERR_SSL_WANT_READ);
        }

        MBEDTLS_NET_PRINT("net recv failed - errno: %d\n", errno);
        return (MBEDTLS_ERR_NET_RECV_FAILED);
    }

    return ret;
}

int mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
    int ret;
    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    ret = (int)write(fd, buf, len);

    if (ret < 0) {
        if (net_would_block(ctx) != 0) {
            return (MBEDTLS_ERR_SSL_WANT_WRITE);
        }

        if (errno == EPIPE || errno == ECONNRESET) {
            MBEDTLS_NET_PRINT("net reset - errno: %d\n", errno);
            return (MBEDTLS_ERR_NET_CONN_RESET);
        }

        if (errno == EINTR) {
            return (MBEDTLS_ERR_SSL_WANT_WRITE);
        }

        MBEDTLS_NET_PRINT("net send failed - errno: %d\n", errno);
        return (MBEDTLS_ERR_NET_SEND_FAILED);
    }

    return ret;
}

int mbedtls_net_recv_timeout(void *ctx, unsigned char *buf, size_t len,
                             uint32_t timeout)
{
    int            ret;
    struct timeval tv;
    fd_set         read_fds;
    int            fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0) {
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    }

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    ret = select(fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv);
    if (ret == 0) {
        return MBEDTLS_ERR_SSL_TIMEOUT;
    }

    if (ret < 0) {
        if (errno == EINTR) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }

        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return mbedtls_net_recv(ctx, buf, len);
}

void mbedtls_net_free(mbedtls_net_context *ctx)
{
    if (ctx->fd == -1) {
        return;
    }

    shutdown(ctx->fd, 2);
    close(ctx->fd);

    ctx->fd = -1;
}

#endif /* STM32_USE_SPI_WIFI */
#endif /* MBEDTLS_NET_ALT */
