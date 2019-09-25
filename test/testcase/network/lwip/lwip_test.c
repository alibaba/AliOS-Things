/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <network/network.h>

#define TCP_SERVER_IP "134.102.218.18"
#define TCP_SERVER_PORT 80
#define UDP_SERVER_IP TCP_SERVER_IP
#define UDP_SERVER_PORT 5683

#define RECV_BUFFER_LEN 1512

#define TAG "lwip_test"

static char *tcp_client_payload = "GET / HTTP/1.1 \r\n\
Host: coap.me\r\n\
Connection: keep-alive\r\n\
Upgrade-Insecure-Requests: 1\r\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.103 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept-Language: zh-CN,zh;q=0.9\r\n\
\r\n";

static void test_lwip_socket(void)
{
    int fd = 0, ret, readlen = 0, total_len = 0;
    struct sockaddr_in addr;
    struct timeval timeout;
    char *recv_buf = NULL;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    YUNIT_ASSERT(fd >= 0);
    if (fd < 0) return;

    timeout.tv_sec = 30;
    timeout.tv_usec = 0;
    ret = setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                      sizeof(timeout));
    YUNIT_ASSERT(ret == 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(TCP_SERVER_IP);
    ret = connect(fd, (struct sockadd *)&addr, sizeof(addr));
    YUNIT_ASSERT(ret == 0);
    if (ret != 0) return;

    ret = send(fd, (void *)tcp_client_payload, strlen(tcp_client_payload), 0);
    YUNIT_ASSERT(ret > 0);
    if (ret <= 0) return;

    recv_buf = (char *)aos_malloc(RECV_BUFFER_LEN);
    if (recv_buf != NULL) {
        int readlen = 0;
        memset(recv_buf, 0, RECV_BUFFER_LEN);

        do{
            char *rsp = "HTTP/1.1";
            char *rsp_ok = "HTTP/1.1 200 OK";

            /* Just read the pbuf, and do nothing with it. */
            readlen = read(fd, recv_buf, RECV_BUFFER_LEN - 1);
            YUNIT_ASSERT(readlen >= 0);

            if (readlen < 0 && errno != EINTR && errno != EWOULDBLOCK 
                && errno != EALREADY && errno != EINPROGRESS){
                LOGE(TAG, "recv failed, errno = %d.\r\n", errno);
                goto err;
            }

            if (readlen == 0){
                LOGI(TAG, "socket is closed\r\n");
                break;
            }
            total_len += readlen;

            if (strncmp(recv_buf, rsp, strlen(rsp)) == 0) {
                if (strncmp(recv_buf, rsp_ok, strlen(rsp_ok)) == 0) {
                    LOGI(TAG, "recv http 200 OK\r\n");
                    break;
                } else {
                    LOGE(TAG, "recv http erorr\r\n");
                    goto err;
                }
            }
        } while (1);
    }

err:
    ret = close(fd);
    YUNIT_ASSERT(ret == 0);

    if (recv_buf) aos_free(recv_buf);
}

static void test_lwip_socket_adv(void)
{
    int fd, ret, stype = -1;
    struct sockaddr_in addr;
    struct sockaddr addr2;
    socklen_t slen = sizeof(addr2);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    YUNIT_ASSERT(fd >= 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(TCP_SERVER_IP);
    ret = connect(fd, (struct sockadd *)&addr, sizeof(addr));
    YUNIT_ASSERT(ret == 0);
    if (ret != 0) return;

    memset(&addr2, 0, sizeof(addr2));
    ret = getpeername(fd, &addr2, &slen);
    YUNIT_ASSERT(ret == 0);

    slen = sizeof(addr2);
    memset(&addr2, 0, sizeof(addr2));
    ret = getsockname(fd, &addr2, &slen);
    YUNIT_ASSERT(ret == 0);

    ret = getsockopt(fd, SOL_SOCKET, SO_TYPE, &stype, sizeof(stype));
    YUNIT_ASSERT(ret == 0);

    uint32_t mode = 1;
    ret = ioctl(fd, FIONBIO, &mode);
    YUNIT_ASSERT(ret != -1);

    ret = -1;
    ret = fcntl(fd, F_GETFL, 0);
    YUNIT_ASSERT(ret >= 0);

    ret = shutdown(fd, SHUT_RDWR);
    YUNIT_ASSERT(ret == 0);

    ret = eventfd(0, 0);
    YUNIT_ASSERT(ret != -1);
    if (ret != -1) close(ret);
}

#define UDP_TEST_STR "hello world"
static void test_lwip_socket_udp(void)
{
    int fd, ret;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    YUNIT_ASSERT(fd >= 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_port = htons((short)atoi(UDP_SERVER_PORT));
    addr.sin_addr.s_addr = inet_addr(UDP_SERVER_IP);
    addr.sin_family = AF_INET;

    ret = sendto(fd, UDP_TEST_STR, strlen(UDP_TEST_STR), 0, 
                 (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    YUNIT_ASSERT(ret != -1);

    fd_set read_fds;
    struct timeval tv;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    tv.tv_sec  = 1;
    tv.tv_usec = 0;
    ret = select(fd + 1, &read_fds, NULL, NULL, &tv);
    YUNIT_ASSERT(ret != -1);

    uint8_t buf[32];
    struct sockaddr fromaddr;
    socklen_t slen;
    if (ret > 0) {
        slen = sizeof(fromaddr);
        ret = recvfrom(fd, buf, sizeof(buf), MSG_DONTWAIT, &fromaddr, &slen);
        YUNIT_ASSERT(ret != -1);
    }

    struct msghdr msg;
    struct iovec iov;
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    msg.msg_name = &addr;
    msg.msg_namelen = sizeof(addr);
    msg.msg_iovlen = 1;
    msg.msg_iov = &iov;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    ret = sendmsg(fd, &msg, 0);
    YUNIT_ASSERT(ret != -1);

    ret = close(fd);
    YUNIT_ASSERT(ret == 0);
}

#define TCP_SERVER_PORT 32000
static void test_lwip_socket_server(void)
{
    int fd, ret;
    struct sockaddr_in daddr;

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    YUNIT_ASSERT(fd >= 0);

    daddr.sin_addr.s_addr = htonl(INADDR_ANY);
    daddr.sin_family = AF_INET;
    daddr.sin_port = htons(TCP_SERVER_PORT);

    ret = bind(fd, (struct sockaddr *)&daddr, sizeof(daddr));
    YUNIT_ASSERT(ret == 0);

    ret = listen(fd, 1);
    YUNIT_ASSERT(ret == 0);

    struct timeval timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;
    ret = setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                      sizeof(timeout));
    YUNIT_ASSERT(ret == 0);

    struct sockaddr saddr;
    socklen_t slen = sizeof(saddr);
    ret = accept(fd, &saddr, &slen);
    YUNIT_ASSERT((ret != -1) || ((ret == -1) && (errno == EAGAIN)));

    ret = close(fd);
    YUNIT_ASSERT(ret == 0);
}

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_lwip_testcases[] = {
    { "basic", test_lwip_socket },
    { "advanced", test_lwip_socket_adv },
    { "udp", test_lwip_socket_udp },
    { "server", test_lwip_socket_server },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "lwip", init, cleanup, setup, teardown, aos_lwip_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_lwip(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_lwip);
