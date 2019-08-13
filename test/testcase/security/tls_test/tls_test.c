/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yunit/yts.h"
#include "yunit/yunit.h"

#include <aos/errno.h>

#include <network/network.h>

#include <mbedtls/mbedtls_ssl.h>

const char *mbedtls_real_ca_pem = "-----BEGIN CERTIFICATE-----\n"   \
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

const char *mbedtls_fake_ca_pem = "-----BEGIN CERTIFICATE-----\n" \
                                  "MIIDhzCCAm+gAwIBAgIBADANBgkqhkiG9w0BAQUFADA7MQswCQYDVQQGEwJOTDER\n" \
                                  "MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\n" \
                                  "MTEwMjEyMTQ0NDAwWhcNMjEwMjEyMTQ0NDAwWjA7MQswCQYDVQQGEwJOTDERMA8G\n" \
                                  "A1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwggEiMA0G\n" \
                                  "CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDA3zf8F7vglp0/ht6WMn1EpRagzSHx\n" \
                                  "mdTs6st8GFgIlKXsm8WL3xoemTiZhx57wI053zhdcHgH057Zk+i5clHFzqMwUqny\n" \
                                  "50BwFMtEonILwuVA+T7lpg6z+exKY8C4KQB0nFc7qKUEkHHxvYPZP9al4jwqj+8n\n" \
                                  "YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\n" \
                                  "R7EUQIN9Z/SG9jGr8XmksrUuEvmEF/Bibyc+E1ixVA0hmnM3oTDPb5Lc9un8rNsu\n" \
                                  "KNF+AksjoBXyOGVkCeoMbo4bF6BxyLObyavpw/LPh5aPgAIynplYb6LVAgMBAAGj\n" \
                                  "gZUwgZIwDAYDVR0TBAUwAwEB/zAdBgNVHQ4EFgQUtFrkpbPe0lL2udWmlQ/rPrzH\n" \
                                  "/f8wYwYDVR0jBFwwWoAUtFrkpbPe0lL2udWmlQ/rPrzH/f+hP6Q9MDsxCzAJBgNV\n" \
                                  "BAYTAk5MMREwDwYDVQQKEwhQb2xhclNTTDEZMBcGA1UEAxMQUG9sYXJTU0wgVGVz\n" \
                                  "dCBDQYIBADANBgkqhkiG9w0BAQUFAAOCAQEAuP1U2ABUkIslsCfdlc2i94QHHYeJ\n" \
                                  "SsR4EdgHtdciUI5I62J6Mom+Y0dT/7a+8S6MVMCZP6C5NyNyXw1GWY/YR82XTJ8H\n" \
                                  "DBJiCTok5DbZ6SzaONBzdWHXwWwmi5vg1dxn7YxrM9d0IjxM27WNKs4sDQhZBQkF\n" \
                                  "pjmfs2cb4oPl4Y9T9meTx/lvdkRYEug61Jfn6cA+qHpyPYdTH+UshITnmp5/Ztkf\n" \
                                  "m/UTSLBNFNHesiTZeH31NcxYGdHSme9Nc/gfidRa0FLOCfWxRlFqAI47zG9jAQCZ\n" \
                                  "7Z2mCGDNMhjQc+BYcdnl0lPXjdDK6V0qCg1dVewhUBcW5gZKzV7e9+DpVA==\n"     \
                                  "-----END CERTIFICATE-----\n";

const char *server_name = "alink.tcp.aliyun.com";
const int server_port = 443;

static char alink_req_data[] = {
    0x57, 0x53, 0x46, 0x31, 0x00, 0x00, 0x00, 0x11,
    0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03
};

static void *sock_fd = NULL;
static void *ssl = NULL;

static void *network_socket_create(const char *net_addr, int port)
{
    int ret;
    int tcp_fd;
    struct hostent *host;
    struct sockaddr_in saddr;
    int opt_val = 1;

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

    ret = setsockopt(tcp_fd, SOL_SOCKET, SO_RCVTIMEO, &opt_val, sizeof(opt_val));
    if ( ret != 0 ) {
        printf("mbedtls error net socket failed!\n");
        goto _err;
    }
    do {
        set_errno(0);
        ret = connect(tcp_fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
        if (ret < 0 && errno != EINTR) {
            printf("socket connect fail - errno: %d\n", errno);
            goto _err;
        }
    } while (ret < 0);

    return (void *)tcp_fd;

_err:
    close(tcp_fd);

    return NULL;
}

static void network_socket_destroy(void *tcp_fd)
{
    if (tcp_fd == NULL) {
        return;
    }

    close((int)tcp_fd);

    return;
}

static void test_tls_ssl_connect(void)
{
    int ret;
    void *sock_fd = NULL;
    void *ssl = NULL;

    sock_fd = network_socket_create(server_name, server_port);
    YUNIT_ASSERT((int)sock_fd >= 0);

    /*
     * testcase #1
     */
    ssl = mbedtls_ssl_connect((void *) - 1,
                              mbedtls_real_ca_pem, strlen(mbedtls_real_ca_pem));
    YUNIT_ASSERT(ssl == NULL);

    /*
     * testcase #2
     */
    ssl = mbedtls_ssl_connect(
              sock_fd, NULL, strlen(mbedtls_real_ca_pem));
    YUNIT_ASSERT(ssl == NULL);

    /*
     * testcase #3
     */
    ssl = mbedtls_ssl_connect(sock_fd, mbedtls_real_ca_pem, 0);
    YUNIT_ASSERT(ssl == NULL);

    /*
     * testcase #4
     */
    ssl = mbedtls_ssl_connect(sock_fd,
                              mbedtls_fake_ca_pem, strlen(mbedtls_fake_ca_pem));
    YUNIT_ASSERT(ssl == NULL);

    network_socket_destroy(sock_fd);

    /*
     * testcase #5
     */
    sock_fd = network_socket_create(server_name, server_port);
    YUNIT_ASSERT((int)sock_fd >= 0);

    ssl = mbedtls_ssl_connect(sock_fd,
                              mbedtls_real_ca_pem, strlen(mbedtls_real_ca_pem));
    YUNIT_ASSERT(ssl != NULL);

    ret = mbedtls_ssl_close(ssl);
    YUNIT_ASSERT(ret == 0);

    network_socket_destroy(sock_fd);

    return;
}

static void test_tls_ssl_send(void)
{
    int ret = 0;
    void *sock_fd = NULL;
    void *ssl = NULL;

    sock_fd = network_socket_create(server_name, server_port);
    YUNIT_ASSERT((int)sock_fd >= 0);

    ssl = mbedtls_ssl_connect(sock_fd,
                              mbedtls_real_ca_pem, strlen(mbedtls_real_ca_pem));
    YUNIT_ASSERT(ssl != NULL);

    /*
     * testcase #1
     */
    ret = mbedtls_ssl_send(NULL, alink_req_data,
                           (int)sizeof(alink_req_data));
    YUNIT_ASSERT(ret < 0);

    /*
     * testcase #2
     */
    ret = mbedtls_ssl_send(ssl, NULL,
                           (int)sizeof(alink_req_data));
    YUNIT_ASSERT(ret < 0);

    /*
     * testcase #3
     */
    ret = mbedtls_ssl_send(ssl, alink_req_data, 0);
    YUNIT_ASSERT(ret < 0);

    /*
     * testcase #4
     */
    ret = mbedtls_ssl_send(ssl, alink_req_data,
                           (int)sizeof(alink_req_data));
    YUNIT_ASSERT(ret > 0);

    ret = mbedtls_ssl_close(ssl);
    YUNIT_ASSERT(ret == 0);

    network_socket_destroy(sock_fd);

    return;
}

static void test_tls_ssl_recv(void)
{
    int ret = 0;
    char buf[128];

    sock_fd = network_socket_create(server_name, server_port);
    YUNIT_ASSERT((int)sock_fd >= 0);
    ssl = mbedtls_ssl_connect(sock_fd,
                              mbedtls_real_ca_pem, strlen(mbedtls_real_ca_pem));
    YUNIT_ASSERT(ssl != NULL);

    ret = mbedtls_ssl_send(ssl, alink_req_data,
                           (int)sizeof(alink_req_data));
    YUNIT_ASSERT(ret > 0);

    /*
     * testcase #1
     */
    ret = mbedtls_ssl_recv(NULL, buf, 128);
    YUNIT_ASSERT(ret < 0);

    /*
     * testcase #2
     */
    ret = mbedtls_ssl_recv(ssl, NULL, 128);
    YUNIT_ASSERT(ret < 0);

    /*
     * testcase #3
     */
    ret = mbedtls_ssl_recv(ssl, buf, 0);
    YUNIT_ASSERT(ret < 0);

    /*
     * testcase #4
     */
    ret = mbedtls_ssl_recv(ssl, buf, 128);
    YUNIT_ASSERT(ret >= 0 || ret == -EAGAIN);

    ret = mbedtls_ssl_close(ssl);
    YUNIT_ASSERT(ret == 0);

    network_socket_destroy(sock_fd);

    return;
}

static void test_tls_ssl_close(void)
{
    int ret = 0;

    sock_fd = network_socket_create(server_name, server_port);
    YUNIT_ASSERT((int)sock_fd >= 0);
    ssl = mbedtls_ssl_connect(sock_fd,
                              mbedtls_real_ca_pem, strlen(mbedtls_real_ca_pem));
    YUNIT_ASSERT(ssl != NULL);

    /*
     * testcase #1
     */
    ret = mbedtls_ssl_close(NULL);
    YUNIT_ASSERT(ret == 0);

    /*
     * testcase #2
     */
    ret = mbedtls_ssl_close(ssl);
    YUNIT_ASSERT(ret == 0);

    network_socket_destroy(sock_fd);

    return;
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
    return;
}

static void teardown(void)
{
    return;
}

static yunit_test_case_t aos_tls_testcases[] = {
    { "tls_ssl_connect", test_tls_ssl_connect},
    { "tls_ssl_send", test_tls_ssl_send},
    { "tls_ssl_recv", test_tls_ssl_recv},
    { "tls_ssl_close", test_tls_ssl_close},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "tls", init, cleanup, setup, teardown, aos_tls_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_tls(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_tls);
