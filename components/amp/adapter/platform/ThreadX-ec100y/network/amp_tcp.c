/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ql_data_call.h"

#include "quec_include.h"
#include "wrappers_defs.h"

#include "amp_errno.h"

static struct in_addr ip4_addr = {0};

#define PLATFORM_LOG_D(format, ...)                                \
    do {                                                                   \
        printf("D: %d %s() | " format "\n", __LINE__, __FUNCTION__, \
               ##__VA_ARGS__);                                             \
    } while (0);

#define PLATFORM_LOG_E(format, ...)                                \
    do {                                                                   \
        printf("E: %d %s() | " format "\n", __LINE__, __FUNCTION__, \
               ##__VA_ARGS__);                                             \
    } while (0);

#ifndef CONFIG_NO_TCPIP

static uint64_t aliot_platform_time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}

uintptr_t HAL_TCP_Establish(_IN_ const char *host, _IN_ uint16_t port)
{
    struct addrinfo  hints;
    struct addrinfo *addrInfoList = NULL;
    struct addrinfo *cur          = NULL;
    struct ql_data_call_info info = {0};
    struct sockaddr_in	ip4_local_addr = {0};
    // char ip4_addr_str[16]         = {0};
    int              fd           = 0;
    int              rc           = -1;
    char             service[6];

    memset(&hints, 0, sizeof(hints));

    PLATFORM_LOG_D(
                "establish tcp connection with server(host=%s port=%u)", host, port);

    hints.ai_family   = AF_INET; // only IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sprintf(service, "%u", port);

    ql_get_data_call_info(1, 0, &info);
    ip4_addr = info.v4.addr.ip;

    ip4_local_addr.sin_family = AF_INET;
	ip4_local_addr.sin_port = htons(quec_soc_generate_port());
	ip4_local_addr.sin_addr = ip4_addr;

    if ((rc = getaddrinfo_with_pcid(host, service, &hints, &addrInfoList, 1)) != 0) {
        PLATFORM_LOG_E("getaddrinfo error: %d", rc);
        return (uintptr_t)-1;
    }
    for (cur = addrInfoList; cur != NULL; cur = cur->ai_next) {

        if (cur->ai_family != AF_INET) {
            PLATFORM_LOG_E("socket type error");
            rc = -1;
            continue;
        }

        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            PLATFORM_LOG_E("create socket error");
            rc = -1;
            continue;
        }

        rc = bind(fd, (struct sockaddr *)&ip4_local_addr, sizeof(ip4_local_addr));
        if(rc < 0)
        {
            printf("%s\r\n", "[TCP] *** bind fail ***\r\n");
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            rc = fd;
            break;
        }

        close(fd);
        PLATFORM_LOG_E("connect error");
        rc = -1;
    }

    if (-1 == rc) {
        PLATFORM_LOG_D("fail to establish tcp");
    } else {
        PLATFORM_LOG_D("success to establish tcp, fd=%d", rc);
    }
    freeaddrinfo(addrInfoList);

    return (uintptr_t)rc;
}


int32_t HAL_TCP_Destroy(uintptr_t fd)
{
    int rc;

    // Shutdown both send and receive operations.
    rc = shutdown((int)fd, 2);
    if (0 != rc) {
        PLATFORM_LOG_E("shutdown error");
        return -1;
    }

    rc = close((int)fd);
    if (0 != rc) {
        PLATFORM_LOG_E("closesocket error");
        return -1;
    }

    return 0;
}


int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len,
                      uint32_t timeout_ms)
{
    int      ret, err_code;
    uint32_t len_sent = 0;
    uint64_t t_end, t_left;
    fd_set   sets;

    if (fd >= FD_SETSIZE) {
        return -1;
    }

	if (timeout_ms == 0) {
		while (len_sent < len) {
			ret = send(fd, buf + len_sent, len - len_sent, 0);
			if (ret > 0) {
			    len_sent += ret;
			} else {
			    if ((EINTR == errno) || (EAGAIN == errno) || (EWOULDBLOCK == errno) ||
			        (EPROTOTYPE == errno) || (EALREADY == errno) || (EINPROGRESS == errno)) {
			        continue;
			    }
			    break;
			}
		}
		return len_sent;
	}

    t_end    = HAL_UptimeMs() + timeout_ms;
    err_code = 0;
    ret      = 1; // send one time if timeout_ms is value 0

    do {
        t_left = aliot_platform_time_left(t_end, HAL_UptimeMs());

        if (0 != t_left) {
            struct timeval timeout;

            FD_ZERO(&sets);
            FD_SET(fd, &sets);

            timeout.tv_sec  = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;
            ret = select(fd + 1, NULL, &sets, NULL, &timeout);
            if (ret > 0) {
                if (0 == FD_ISSET(fd, &sets)) {
                    PLATFORM_LOG_D("Should NOT arrive");
                    // If timeout in next loop, it will not sent any data
                    ret = 0;
                    continue;
                }
            } else if (0 == ret) {
                // PLATFORM_LOG_D("select-write timeout %lu", fd);
                break;
            } else {
                if (EINTR == errno) {
                    PLATFORM_LOG_D("EINTR be caught");
                    continue;
                }

                err_code = -1;
                PLATFORM_LOG_E("select-write fail");
                break;
            }
        }

        if (ret > 0) {
            ret = send(fd, buf + len_sent, len - len_sent, 0);
            if (ret > 0) {
                len_sent += ret;
            } else if (0 == ret) {
                PLATFORM_LOG_D("No data be sent");
            } else {
                if (EINTR == errno) {
                    PLATFORM_LOG_D("EINTR be caught");
                    continue;
                }

                err_code = -1;
                PLATFORM_LOG_E("send fail");
                break;
            }
        }
    } while ((len_sent < len) &&
             (aliot_platform_time_left(t_end, HAL_UptimeMs()) > 0));

    return err_code == 0 ? len_sent : err_code;
}

int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    int            ret, err_code;
    uint32_t       len_recv = 0;
    uint64_t       t_end, t_left;
    fd_set         sets;
    struct timeval timeout;

	if (timeout_ms == 0) {
		while (len_recv < len) {
			ret = recv(fd, buf + len_recv, len - len_recv, 0);
			if (ret > 0) {
			    len_recv += ret;
			} else {
			    if ((EINTR == errno) || (EAGAIN == errno) || (EWOULDBLOCK == errno) ||
			        (EPROTOTYPE == errno) || (EALREADY == errno) || (EINPROGRESS == errno)) {
			        continue;
			    }
			    break;
			}
		}
		return len_recv;
	}

    if (fd >= FD_SETSIZE) {
        PLATFORM_LOG_E("%s error: fd (%d) >= FD_SETSIZE (%d)", __func__, fd, FD_SETSIZE);
        return -1;
    }

    t_end    = HAL_UptimeMs() + timeout_ms;
    err_code = 0;

    do {
        t_left = aliot_platform_time_left(t_end, HAL_UptimeMs());
        if (0 == t_left) {
            break;
        }
        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        timeout.tv_sec  = t_left / 1000;
        timeout.tv_usec = (t_left % 1000) * 1000;

        ret = select(fd + 1, &sets, NULL, NULL, &timeout);
        if (ret > 0) {
            if (0 == FD_ISSET(fd, &sets)) {
                PLATFORM_LOG_D("%s No data for fd %d", __func__, fd);
                ret = 0;
                continue;
            }

            ret = recv(fd, buf + len_recv, len - len_recv, 0);
            if (ret > 0) {
                len_recv += ret;
            } else if (0 == ret) {
                PLATFORM_LOG_E("connection is closed");
                err_code = -1;
                break;
            } else {
                if ((EINTR == errno) || (EAGAIN == errno) || (EWOULDBLOCK == errno) ||
                    (EPROTOTYPE == errno) || (EALREADY == errno) || (EINPROGRESS == errno)) {
                    continue;
                }
                PLATFORM_LOG_E("recv fail (fd: %d), errno: %d, ret: %d", fd, errno, ret);
                err_code = -2;
                break;
            }
        } else if (0 == ret) {
            break;
        } else {
            if (EINTR == errno) {
                continue;
            }
            PLATFORM_LOG_E("select-recv (fd: %d) fail errno=%d", fd, errno);
            err_code = -2;
            break;
        }
    } while ((len_recv < len));
    return (0 != len_recv) ? len_recv : err_code;
}

#else
uintptr_t HAL_TCP_Establish(_IN_ const char *host, _IN_ uint16_t port)
{
    return 0;
}
int32_t HAL_TCP_Destroy(uintptr_t fd)
{
    return 0;
}
int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len,
                      uint32_t timeout_ms)
{
    return 0;
}
int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    return 0;
}
#endif
