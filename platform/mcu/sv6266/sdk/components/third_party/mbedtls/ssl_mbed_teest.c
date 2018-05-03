/* FILE   : ssl.c
 * AUTHOR : leon
 * DATE   : Feb 24, 2015
 * DESC   :
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "system_cfg.h"
#include "sntp.h"
#include "osdep.h"
#include "lwip/sockets.h"

#define _snprintf snprintf

#define DEBUG_LEVEL 0

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#if DEBUG_LEVEL > 0
#include "mbedtls/debug.h"
#endif

char ssl_ip[64];
u16 ssl_port = 0;
u8 test_sec = 0;

#define MAX_TIMEOUT_HANDSHAKE 60000
#define MIN_TIMEOUT_HANDSHAKE 1000

struct connectdata
{
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
};

extern int32 psGetTime(psTime_t *t, void *userPtr);

static void
mbedtls_show_error_str(char* func, int errCode)
{
    char cMbedTlsErrBuf[512] = {0};
    mbedtls_strerror(errCode, cMbedTlsErrBuf, sizeof(cMbedTlsErrBuf));
    printf( "%s error %s\n", func, cMbedTlsErrBuf);
}

/*static void
my_debug(void *ctx, int level, const char *file, int line, const char *str)
    {
    ((void) level);
    fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
    fflush((FILE *) ctx);
    }*/

static int ssl_teardown(struct connectdata *conn)
{
    mbedtls_net_free( &conn->server_fd );
    mbedtls_ssl_free( &conn->ssl );
    mbedtls_ssl_config_free( &conn->conf );
    mbedtls_ctr_drbg_free( &conn->ctr_drbg );
    mbedtls_entropy_free( &conn->entropy );
    return 0;
}

static int ssl_handshake(struct connectdata *conn)
{
    int ret = -1;
    int flags = -1;
    mbedtls_ssl_conf_handshake_timeout( &conn->conf, MIN_TIMEOUT_HANDSHAKE, MAX_TIMEOUT_HANDSHAKE );
    mbedtls_ssl_set_bio( &conn->ssl, &conn->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );
    while( ( ret = mbedtls_ssl_handshake( &conn->ssl ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            mbedtls_show_error_str("mbedtls_ssl_handshake", ret);
            return -2;
        }
        printf("mbedtls_ssl_handshake retry\n");
    }

    return 0;
}

/*
 * PURPOSE : Create TCP socket with destination address and port
 * INPUT   : [ip]   - IP address of destination
 *           [port] - port of destination
 * OUTPUT  : None
 * RETURN  : Pointer context of this connection
 * DESCRIPT: None
 */
static int ssl_connect(struct connectdata *conn, char *ip, int port)
{
    char sPort[8] = {0};
    struct sockaddr_in addr;
    struct hostent *host = NULL;
    int err = 0;
    struct sockaddr_in server_address;
    int length = 0;

    length = sizeof(server_address);
    if(ip == NULL || port > 65535 || port < 0)
    {
        printf("IP NULL or invalid port");
        return -1;
    }

    snprintf(sPort, sizeof(sPort) - 1, "%d", port);
    sPort[strlen(sPort)] = 0;

    err = mbedtls_net_connect(&conn->server_fd, ip, sPort, MBEDTLS_NET_PROTO_TCP);
    if (err != 0)
    {
        printf("mbedtls_net_connect %s:%d failed - Error(%#.4X)", ip, port, (err < 0 ? (0 - err) : err));
        mbedtls_show_error_str("mbedtls_net_connect", err);
        return -2;
    }

    getsockname(conn->server_fd.fd, (struct sockaddr * )&server_address, (socklen_t *)&length);
    printf("Connecting to Port = %d \n", ntohs(server_address.sin_port));

    if (mbedtls_ssl_config_defaults(&conn->conf,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT) != 0)
    {
        printf("mbedtls_ssl_config_defaults failed");
        return -3;
    }
    mbedtls_ssl_conf_authmode(&conn->conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&conn->conf, mbedtls_ctr_drbg_random, &conn->ctr_drbg);
#if DEBUG_LEVEL > 0
    mbedtls_ssl_conf_dbg( &conn->conf, my_debug, stdout );
#endif
    if (mbedtls_ssl_setup(&conn->ssl, &conn->conf) != 0)
    {
        printf("mbedtls_ssl_setup failed");
        return -4;
    }
    if (ssl_handshake(conn) != 0)
    {
        printf("ssl_handshake failed");
        return -5;
    }
    return 0;
}
/*
 * PURPOSE : Get status of ssl request
 * INPUT   : [ssl] - SSL pointer
 * OUTPUT  : None
 * RETURN  : HTTP status
 * DESCRIPT: None
 */
int
ssl_request_stat_get(struct connectdata *conn, void* res, int res_len)
{
    int nReadSize = 0;
    int ret, i;
    int status = -1;
    struct timeval timeout;
    int state = 0, idx = 0;
    fd_set fdread;
    int content_length = 0, current_length = 0;
    unsigned char dataPointer[4096] = {0};

    for(i = 0; i < 20; i++)    // 3*20 = 60s
    {
        char* pCh = NULL;
        char* resBuff = NULL;
        FD_ZERO(&fdread);
        FD_SET(conn->server_fd.fd, &fdread);
        /* set a suitable timeout to play around with */
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        ret = select(conn->server_fd.fd + 1, &fdread, NULL, NULL, &timeout);
        if(ret == -1)
        {
            break;
        }
        else if(ret == 0)
        {
            continue;
        }


        nReadSize = mbedtls_ssl_read( &conn->ssl, dataPointer, sizeof(dataPointer) - 1);
        resBuff = (char*) dataPointer;
        if(nReadSize > 0) 
        {
            resBuff[nReadSize] = '\0';
            printf("%s", resBuff);
        }
        if( nReadSize == MBEDTLS_ERR_SSL_WANT_READ || nReadSize == MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            continue;
        }

        if(nReadSize == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
        {
            status = -2;
            printf("\n");
            break;
        }

        current_length = nReadSize;
    }
    return 0;
}
int
cinatic_entropy_func(void *data, unsigned char *output, size_t len )
{
    int i = 0;
    int length = len / sizeof(unsigned long);
    int remain = len % sizeof(unsigned long);
    int current_len = 0;

    memset(output, 0, len);
    for (i = 0; i < length; i++)
    {
        sprintf((char *)output + current_len, "%lu", OS_Random());
        current_len = strlen((char *)output);
    }

    for (i = 0; i < remain; i++)
    {
        sprintf((char *)output + current_len, "%1X", (unsigned int)(OS_Random() % 0x0F));
        current_len = strlen((char *)output);
    }
    return 0;
}
int ssl_init_conn(struct connectdata *conn)
{
    int ret = -1;
    const char *pers = "ssl_client";
    mbedtls_net_init( &(conn->server_fd) );
    mbedtls_ssl_init( &(conn->ssl) );
    mbedtls_ssl_config_init( &(conn->conf) );
    mbedtls_ctr_drbg_init( &(conn->ctr_drbg) );
    mbedtls_entropy_init( &(conn->entropy) );

    /*if( ( ret = mbedtls_ctr_drbg_seed( &conn->ctr_drbg, mbedtls_entropy_func, &conn->entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )*/

    if( ( ret = mbedtls_ctr_drbg_seed( &conn->ctr_drbg, cinatic_entropy_func, &conn->entropy,
                                       (const unsigned char *) pers,
                                       strlen( pers ) ) ) != 0 )
    {
        printf( " failed mbedtls_ctr_drbg_seed returned %d\n", ret );
        return -1;
    }

    return 0;
}
/*
 * PURPOSE : Perform an SSL request
 * INPUT   : [ip]   - server address
 *           [port] - server port
 *           [req]  - request
 *           [len]  - size of request
 * OUTPUT  : [res]  - response. Should be over 3KBs for safe
 * RETURN  : status of this request, -1 if failed
 * DESCRIPT: None
 */
int
thirdparty_https_client_request(char* ip, int port, char* req, int len, void* res)
{
    int http_code = 0, ret;
    struct connectdata conn;
    int err = 0;
    int i = 0;
    int connect_stat = 0;
    s32 btime = 0, etime = 0;
    if(ip == NULL || req == NULL)
        return -1;

    //mbedtls_debug_set_threshold(3);
    
    sntp_init();
    while(btime == 0) {
        btime = psGetTime(NULL, NULL);
        OS_MsDelay(100);
    }
    
    if( ssl_init_conn(&conn) != 0 )
    {
        printf("ssl_init_conn failed");
        ssl_teardown(&conn);
        return -2;
    }

    btime = os_tick2ms(OS_GetSysTick());
    printf("\nbtime: %d\n", (int)btime);
    /* Retry 3 times if connection is closed before we can read response */
    //for(i = 0; i < 3; i ++)
    //{
        err = ssl_connect(&conn, ip, port);
        if (err == 0)
        {
            connect_stat = 1;
            //break;
        }
        else
           return -5;
    //}
    etime = os_tick2ms(OS_GetSysTick());
    printf("\netime: %d diff time: %d\n", (int)etime, (int)(etime -btime));
#if 0
    while( (ret = mbedtls_ssl_write( &conn.ssl, (uint8_t*) req, len)) <= 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            printf( "mbedtls_ssl_write returned failed %d", ret );
            ssl_teardown(&conn);
            return -3;
        }
    }
    
    http_code = ssl_request_stat_get(&conn, res, 4096);
#endif
    ssl_teardown(&conn);

    if(http_code != 0)
        return http_code;
    return -4;
}

static void test_task(void *arg)
{
    char *send = "GET /utils/current_time HTTP/1.1\r\nHost: api.cinatic.com\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:56.0) Gecko/20100101 Firefox/56.0\r\nAccept: */*\r\nAccept-Language: zh-TW,zh;q=0.8,en-US;q=0.5,en;q=0.3\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n";

    thirdparty_https_client_request("52.54.79.224",443,send, strlen(send), NULL);
    
    OS_TaskDelete(NULL);
}

#define BUF_SIZE 8192
#define TEST_SEC 1
static void thoughtput_test(void *arg)
{
    u8 tmp_buf[BUF_SIZE];
    u32 count = 0;
    u32 i = 0;
    s32 ret = 0, err = 0, total = 0;
    s32 btime = 0, etime = 0;
    struct connectdata conn;
    
    sntp_init();
    while(btime == 0) {
        btime = psGetTime(NULL, NULL);
        OS_MsDelay(100);
    }
    
    if( ssl_init_conn(&conn) != 0 )
    {
        printf("ssl_init_conn failed\n");
        ssl_teardown(&conn);
        return;
    }
    printf("connect ip: %s port: %d test sec: %d\n", ssl_ip, ssl_port, test_sec);
    btime = os_tick2ms(OS_GetSysTick());
    err = ssl_connect(&conn, ssl_ip, ssl_port);
    if(err < 0) {
        printf("ssl_connect failed\n");
        goto EXIT;
    }
    etime = os_tick2ms(OS_GetSysTick());
    printf("connect time: %d\n", (int)(etime - btime));
    for(i = 0; i < BUF_SIZE; i++)
        tmp_buf[i] = '0' + ((i % 10) & 0Xff);
    tmp_buf[BUF_SIZE - 2] = 0XD;
    tmp_buf[BUF_SIZE - 1] = 0XA;
    btime = os_tick2ms(OS_GetSysTick());
    printf("\nbtime: %d\n", (int)(btime));
    while(1) {
        ret = mbedtls_ssl_write( &conn.ssl, tmp_buf, BUF_SIZE);
        if(  ret <= 0 ) {
            if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE ) {
                printf( "mbedtls_ssl_write returned failed %d", (int)ret );
                ssl_teardown(&conn);
                break;
            }
        } else {
            //printf("ret: %d\n", ret);
            count++;
            total += ret;
        }
        etime = os_tick2ms(OS_GetSysTick());
        if((etime - btime) >= (test_sec * 1000)) {
            printf("\netime: %d\n", (int)etime);
            break;
        }
    }
    //printf("mbedtls_ssl_write after\n");
    printf("[%d ]totoal size: %d bytes avg: %d Kb/s\n", (int)count, (int)total, (int)(total * 8 / test_sec / 1024));

    OS_MsDelay(1000);
    ssl_teardown(&conn);

EXIT:
    OS_TaskDelete(NULL);
}

void ssl_mbed_test()
{
    if (OS_TaskCreate(test_task, "test_task", 4096, NULL, OS_TASK_PRIO3, NULL) != 1)
    {
        printf("Failed to create test task\n");
    }
}

void ssl_mbed_thoughtput_test(char *ip, u16 port, u8 sec)
{
    strcpy(ssl_ip, ip);
    ssl_port = port;
    test_sec = sec;
    if (OS_TaskCreate(thoughtput_test, "thoughtput_test", 4096, NULL, OS_TASK_PRIO3, NULL) != 1)
    {
        printf("Failed to create test task\n");
    }
}
