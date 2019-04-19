#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "itls/config.h"
#include "itls/net_sockets.h"

#define MBEDTLS_NET_PRINT(_f, ...)  \
            printf("%s %d: "_f,  __FUNCTION__, __LINE__, ##__VA_ARGS__)

#if defined(MBEDTLS_NET_ALT)

#include <fcntl.h>
#include <unistd.h>
#include <aos/errno.h>

#include <aos/network.h>


#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
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

int mbedtls_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto)
{
    int ret;
    struct addrinfo hints, *addr_list, *cur;

    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    if ((ret = getaddrinfo(host, port, &hints, &addr_list)) != 0) {
        MBEDTLS_NET_PRINT("getaddrinfo fail- errno: %d, %d\n", errno, ret);
        return MBEDTLS_ERR_NET_UNKNOWN_HOST;
    }

    ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;

    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        ctx->fd = (int)socket(cur->ai_family,
                              cur->ai_socktype, cur->ai_protocol);
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
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
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
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
    u_long n = 1;
    return ( ioctlsocket( ctx->fd, FIONBIO, &n ) );
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
    int fd = ((mbedtls_net_context *) ctx)->fd;

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
    int fd = ((mbedtls_net_context *) ctx)->fd;

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
                             uint32_t timeout )
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd = ((mbedtls_net_context *) ctx)->fd;

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

#endif /* MBEDTLS_NET_ALT */
