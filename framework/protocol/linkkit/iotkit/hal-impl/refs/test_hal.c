#include "common.h"
#include "imports/iot_import_coap.h"
#include "imports/iot_import_dtls.h"

#define TEST_HOST "127.0.0.1"

#define TEST_UDP_HOST              TEST_HOST
#define TEST_UDP_PORT              (unsigned short)(8896)
#define TEST_UDP_CONNECTION_TIMES  (100)
#define TEST_UDP_READ_WRITE_TIMES  (100)

#define TEST_TCP_HOST              TEST_HOST
#define TEST_TCP_PORT              (unsigned short)(8897)
#define TEST_TCP_CONNECTION_TIMES  (100)
#define TEST_TCP_READ_WRITE_TIMES  (100)

#define TEST_SSL_HOST              TEST_HOST
#define TEST_SSL_PORT              (unsigned short)(8898)
#define TEST_SSL_CONNECTION_TIMES  (100)
#define TEST_SSL_READ_WRITE_TIMES  (100)

#define TEST_DTLS_HOST             TEST_HOST 
#define TEST_DTLS_PORT             (unsigned short)(8899)
#define TEST_DTLS_CONNECTION_TIMES (100)
#define TEST_DTLS_READ_WRITE_TIMES (100)

#define MAX_BUF_LEN                (1024)

static char rd_buf[MAX_BUF_LEN] = {0};
static char wr_buf[MAX_BUF_LEN] = {0};

static char ca_crt[] =
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIFizCCA3OgAwIBAgICEAAwDQYJKoZIhvcNAQELBQAwVzELMAkGA1UEBhMCQ04x\r\n" \
"CzAJBgNVBAgMAlpKMQswCQYDVQQHDAJIWjENMAsGA1UECgwEdGVzdDENMAsGA1UE\r\n" \
"CwwEdGVzdDEQMA4GA1UEAwwHcm9vdCBjYTAeFw0xNzExMDcwOTI5NDJaFw0yNzEx\r\n" \
"MDUwOTI5NDJaMFIxCzAJBgNVBAYTAkNOMQswCQYDVQQIDAJaSjENMAsGA1UECgwE\r\n" \
"dGVzdDENMAsGA1UECwwEdGVzdDEYMBYGA1UEAwwPaW50ZXJtZWRpYXRlIGNhMIIC\r\n" \
"IjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA2qMI26vKyhXNL8vEFZycY9ZK\r\n" \
"m1HLVl3LDnbs99mKTXyRhR3OxZKn+Y16A+sYshhofB6jQ1QXl81eejKcbJhHJFKh\r\n" \
"CKjZW4RvNXZsYgTqyREkxf+nTFsWxtdbGyb24FOShoswcG/wDKuko5hpdPnq/Va5\r\n" \
"NyBSiXAyV9EN1N9Nnfh96Zx7ZBF7Qs4oLoF8G/LAuEo4ubfbKsVzDEeXfM4q/mFW\r\n" \
"KH1/b+NGmvwc4nFiq7JfOtikty5MUo1KJ/CbxUeUJLKTHvPYZkj4JxXKFFPJtJdM\r\n" \
"gsL1i6RGOhGGneK3ESh/4+AcO5a0b6ZF+sn4w1ufNv5gXEIEnn/KEHVsk1uo498q\r\n" \
"bTtWbTewOWmPucwO9Bh4eDwWhORUv9y9qzKETM7ySeEr9g7E8mhOnq9Ue9OdZVbw\r\n" \
"NOAYP1NBBenFqudgfI5E5UJ0squzLRlZhq1OLx8x3usKw/Qt4v1Y/OkhB8ifieMx\r\n" \
"eI1Lhjr+xqmhHx9InnWwBX3ZOTTFgTTi6zIx+jwez6DvBpJ1HvUoQDztTYwHzzVw\r\n" \
"ZzodpIEZwB/vaMOLsXN085BpWA3rQNokz4b8+IIeBLEX24ywwRwToGj3nSZTq6+7\r\n" \
"U3litkC93LvHKdGOCBDl/0q6vMd8hNZV3ZgoP200W0DNQZPXUkn5Hcqpqewz0hH+\r\n" \
"BUkbC5yZwo2a8gwQxHcCAwEAAaNmMGQwHQYDVR0OBBYEFN1/+4yrprntjL09PB3h\r\n" \
"kxY1sEjoMB8GA1UdIwQYMBaAFCtepqJzVmI3k6+EUfcfqFpRSxz1MBIGA1UdEwEB\r\n" \
"/wQIMAYBAf8CAQAwDgYDVR0PAQH/BAQDAgGGMA0GCSqGSIb3DQEBCwUAA4ICAQB+\r\n" \
"f3vuBsc2/v2rekqUneFCxGaFOYT1wxAVeQTPuKAfndq+bq1wJVyB3vaq+ZR08B0F\r\n" \
"iJX8VHXX8AzWXfyP8ASaPRV7Z248sZSuMopQyfbnKo9o6v3XBDKxyhk0qwHZ2+Ru\r\n" \
"+/JvwZS4XE0yOcESamOF6n+eKS3Tu2rzzYU5RrVPmDGTZtobp+NhZdPyPV+imKmd\r\n" \
"2FVF9ia8Iy7XdideU6Vnct4QmiTRlTPKVAfwlA1mCYQj9PEyR5z8ZekW/J6/D5zO\r\n" \
"xH2aDp6yUfBLTPPo5lHCytiYw2cAvmJGsAsUYe4deh7zg8jpXYh+bJfV5ACUqL3N\r\n" \
"61dLDrX9q360+5CKkLfoDmKZecrz8IqyZMJWCoK5OLE3mvVf/Pydl1codV5m2gIO\r\n" \
"m+SDneLGHU9k8+A9NFk8uHdesOjoivQ82ttXerz7VJwe25GTxVY7NzomdVdMvDB2\r\n" \
"2QNZm2Qw7B8TBuWWNnMGf4FSK8/gJJvbj0MV3HpPyeF2MEmd7Zmc9sA79SyE0ptW\r\n" \
"kCl8XLtVtPRR+XJxsvaJBxKdsfwOz3P5+ALdoEIOrlDtairwhuShiNbyucauGiek\r\n" \
"SOUqX7hLaRNc57UlcgaYUSL/p8dlJWIQarefd5kpO/aGWt9cWTkpWhwXyPavQKFV\r\n" \
"RHwVkCdUPLhm29GTfC9X29ZQAEUQqeiY/a87qWj9qQ==\r\n" \
"-----END CERTIFICATE-----";

#ifdef TEST_HAL_UDP_ENABLED
/* 反复建立/断开连接 */
CASE(test_hal_udp, case_01)
{
    int  cnt = 100;
    void *sock = NULL;

    while(cnt--) {
        sock = HAL_UDP_create(TEST_UDP_HOST, TEST_UDP_PORT);
        ASSERT_GT((long)sock, 0);
        HAL_UDP_close(sock);
    }
}

/* 正常读/写数据 */
CASE(test_hal_udp, case_02)
{
    int ret = 0;
    void *sock = NULL;
    int i = 0;

    sock = HAL_UDP_create(TEST_UDP_HOST, TEST_UDP_PORT);
    ASSERT_GT((long)sock, 0);

    while(i++ < TEST_UDP_READ_WRITE_TIMES) {
        ret = HAL_UDP_write(sock, (unsigned char*)wr_buf, MAX_BUF_LEN);
        if (ret < 0) {
            HAL_UDP_close(sock);
            ASSERT_FAIL();
        }
        ASSERT_TRUE(ret == MAX_BUF_LEN);
        HAL_Printf("[%4d] HAL_UDP_Write: %d bytes\n", i, ret);

        ret = HAL_UDP_read(sock, (unsigned char*)rd_buf, MAX_BUF_LEN);
        if (ret < 0) {
            HAL_UDP_close(sock);
            ASSERT_FAIL();
        }
        ASSERT_TRUE(ret == MAX_BUF_LEN);
        ASSERT_TRUE(memcmp(rd_buf, wr_buf, MAX_BUF_LEN) == 0);
        HAL_Printf("[%4d] HAL_UDP_Read:  %d bytes\n", i, ret);
    }
    HAL_UDP_close(sock);
}

/* 正常读/写数据 timeout */
CASE(test_hal_udp, case_03)
{
    int ret = 0;
    void *sock = NULL;
    int timeout = 100;
    int i = 0;

    sock = HAL_UDP_create(TEST_UDP_HOST, TEST_UDP_PORT);
    ASSERT_GT((long)sock, 0);
    ASSERT_TRUE(sock > 0);

    while(i++ < TEST_UDP_READ_WRITE_TIMES){
        ret = HAL_UDP_write(sock, (unsigned char*)wr_buf, MAX_BUF_LEN);
        if (ret < 0) {
            HAL_UDP_close(sock);
            ASSERT_FAIL();
        }
        ASSERT_TRUE(ret == MAX_BUF_LEN);
        HAL_Printf("[%4d] HAL_UDP_Write:       %d bytes\n", i, ret);

        ret = HAL_UDP_readTimeout(sock, (unsigned char*)rd_buf, MAX_BUF_LEN, timeout);
        if (ret == -2)  { /* timeout */
            HAL_Printf("[%4d] HAL_UDP_readTimeout: timeout\n", i, ret);
            continue;
        }
        if (ret < 0) {
            HAL_UDP_close(sock);
            ASSERT_FAIL();
        }
        ASSERT_TRUE(memcmp(rd_buf, wr_buf, MAX_BUF_LEN) == 0);
        HAL_Printf("[%4d] HAL_UDP_readTimeout: %d bytes\n", i, ret);

        ret = HAL_UDP_readTimeout(sock, (unsigned char*)rd_buf, MAX_BUF_LEN, timeout);
        if (ret != -2) {
            HAL_UDP_close(sock);
            ASSERT_FAIL();
        }
        HAL_Printf("[%4d] HAL_UDP_readTimeout: timeout\n", i);
    }
    HAL_UDP_close(sock);
}

SUITE(test_hal_udp) = {
    ADD_CASE(test_hal_udp, case_01),
    ADD_CASE(test_hal_udp, case_02),
    ADD_CASE(test_hal_udp, case_03),
    ADD_CASE_NULL
};
#endif

#ifdef TEST_HAL_TCP_ENABLED
/* 反复建立/断开连接 */
CASE(test_hal_tcp, case_01)
{
    int       ret = 0;
    uintptr_t sock = -1;
    int       i = 0;

    while(i++ < TEST_TCP_CONNECTION_TIMES) {
        HAL_Printf("[%4d] HAL_TCP_Establish...\n", i);
        sock = HAL_TCP_Establish(TEST_TCP_HOST, TEST_TCP_PORT);
        ASSERT_TRUE(sock > 0);

        ret = HAL_TCP_Destroy(sock);
        ASSERT_EQ(0, ret);
    }
}


/* 正常读/写数据 */
CASE(test_hal_tcp, case_02)
{
    int       ret = 0;
    uintptr_t sock = -1;
    int       timeout = 100;
    int       i = 0;

    sock = HAL_TCP_Establish(TEST_TCP_HOST, TEST_TCP_PORT);
    ASSERT_TRUE(sock > 0);

    while(i++ < TEST_TCP_READ_WRITE_TIMES) {
        ret = HAL_TCP_Write(sock, wr_buf, MAX_BUF_LEN, timeout);
        if (ret < 0) {
            HAL_TCP_Destroy(sock);
            ASSERT_FAIL();
        }
        if (ret != MAX_BUF_LEN) {
            HAL_TCP_Destroy(sock);
            ASSERT_FAIL();
        }
        HAL_Printf("[%4d] HAL_TCP_Write: %d bytes\n", i, ret);

        ret = HAL_TCP_Read(sock, rd_buf, MAX_BUF_LEN, timeout);
        if(ret != MAX_BUF_LEN) {
            HAL_TCP_Destroy(sock);
            ASSERT_FAIL();
        }
        if(0 != memcmp(rd_buf, wr_buf, MAX_BUF_LEN)) {
            HAL_TCP_Destroy(sock);
            ASSERT_FAIL();
        }
        HAL_Printf("[%4d] HAL_TCP_Read:  %d bytes\n", i, ret);

        ret = HAL_TCP_Read(sock, rd_buf, MAX_BUF_LEN, timeout);
        if(ret == 0) {
            HAL_Printf("[%4d] HAL_TCP_Read:  timeout\n", i, ret);
            continue;
        }
    }
    ret = HAL_TCP_Destroy(sock);
}

SUITE(test_hal_tcp) = {
    ADD_CASE(test_hal_tcp, case_01),
    ADD_CASE(test_hal_tcp, case_02),
    ADD_CASE_NULL
};
#endif

#ifdef TEST_HAL_SSL_ENABLED
/* 反复建立连接 */
CASE(test_hal_ssl, case_01)
{
    uintptr_t sslsock = -1;
    int ret = -1;
    int i = 0;

    while(i++ < TEST_SSL_CONNECTION_TIMES) {
        HAL_Printf("[%4d] SSL Connetion...\n", i);
        sslsock = HAL_SSL_Establish(TEST_SSL_HOST, TEST_SSL_PORT, ca_crt, strlen(ca_crt)+1);
        ASSERT_NOT_NULL((void*)sslsock);

        ret = HAL_SSL_Destroy(sslsock);
        ASSERT_EQ(0, ret);
    }
}

/* 数据通信 */
CASE(test_hal_ssl, case_02)
{
    uintptr_t sslsock = -1;
    int ret = -1;
    int i = 0;

    sslsock = HAL_SSL_Establish(TEST_SSL_HOST, TEST_SSL_PORT, ca_crt, strlen(ca_crt)+1);
    ASSERT_NOT_NULL((void*)sslsock);
    HAL_Printf("HAL_SSL_Establish success!\n");

    while(i++ < TEST_SSL_READ_WRITE_TIMES) {
        ret = HAL_SSL_Write(sslsock, wr_buf, MAX_BUF_LEN, 1000);
        ASSERT_EQ(ret, MAX_BUF_LEN);
        HAL_Printf("[%4d] HAL_SSL_Write: %d bytes\n", i, ret);

        ret = HAL_SSL_Read(sslsock, rd_buf, MAX_BUF_LEN, 2000);
        ASSERT_EQ(ret, MAX_BUF_LEN);
        ASSERT_EQ(memcmp(rd_buf, wr_buf, MAX_BUF_LEN), 0);
        HAL_Printf("[%4d] HAL_SSL_Read:  %d bytes\n", i, ret);
    }

    ret = HAL_SSL_Destroy(sslsock);
    ASSERT_EQ(0, ret);
    HAL_Printf("HAL_SSL_Destroy success!\n");
}

SUITE(test_hal_ssl) = {
    ADD_CASE(test_hal_ssl, case_01),
    ADD_CASE(test_hal_ssl, case_02),
    ADD_CASE_NULL
};
#endif


#ifdef TEST_HAL_DTLS_ENABLED
/* dtls session create */
CASE(test_hal_dtls, case_01)
{
    int i = 0;
    DTLSContext *ctx = NULL;
    coap_dtls_options_t opt;

    memset(&opt, 0, sizeof(coap_dtls_options_t));
    opt.p_ca_cert_pem = (unsigned char*)ca_crt;
    opt.p_host = TEST_DTLS_HOST;
    opt.port = TEST_DTLS_PORT;

    while(i++ < TEST_DTLS_CONNECTION_TIMES) {
        HAL_Printf("[%4d] HAL_DTLSSession_create...\n", i);
        ctx = HAL_DTLSSession_create(&opt);
        ASSERT_NOT_NULL(ctx);
        HAL_DTLSSession_free(ctx);
    }
}

/* dtls session read/write */
CASE(test_hal_dtls, case_02)
{
    int ret = -1;
    unsigned int len = 0;
    int i = 0;
    DTLSContext *ctx = NULL;
    coap_dtls_options_t opt;

    memset(&opt, 0, sizeof(coap_dtls_options_t));
    opt.p_ca_cert_pem = (unsigned char*)ca_crt;
    opt.p_host = TEST_DTLS_HOST;
    opt.port = TEST_DTLS_PORT;

    ctx = HAL_DTLSSession_create(&opt);
    ASSERT_NOT_NULL(ctx);

    while(i++ < TEST_DTLS_READ_WRITE_TIMES) {
        len = MAX_BUF_LEN;
        ret = HAL_DTLSSession_write(ctx, (unsigned char*)wr_buf, &len);
        if(ret != 0 && len != MAX_BUF_LEN) {
            HAL_DTLSSession_free(ctx);
            ASSERT_FAIL();
        }
        HAL_Printf("[%4d] HAL_DTLSSession_write %d bytes...\n", i, len);

        len = MAX_BUF_LEN;
        ret = HAL_DTLSSession_read(ctx, (unsigned char*)rd_buf, &len, 1000);
        if(ret != 0 && len != MAX_BUF_LEN) {
            HAL_DTLSSession_free(ctx);
            ASSERT_FAIL();
        }
        HAL_Printf("[%4d] HAL_DTLSSession_read %d bytes...\n", i, len);

        if(memcmp(wr_buf, rd_buf, MAX_BUF_LEN) != 0) {
            HAL_DTLSSession_free(ctx);
            ASSERT_FAIL();
        }
    }

    HAL_DTLSSession_free(ctx);
}

SUITE(test_hal_dtls) = {
    ADD_CASE(test_hal_dtls, case_01),
    ADD_CASE(test_hal_dtls, case_02),
    ADD_CASE_NULL
};
#endif

void test_hal_entry()
{
#ifdef TEST_HAL_UDP_ENABLED
    ADD_SUITE(test_hal_udp);
#endif

#ifdef TEST_HAL_TCP_ENABLED
    ADD_SUITE(test_hal_tcp);
#endif

#ifdef TEST_HAL_SSL_ENABLED
    ADD_SUITE(test_hal_ssl);
#endif

#ifdef TEST_HAL_DTLS_ENABLED
    ADD_SUITE(test_hal_dtls);
#endif
}

