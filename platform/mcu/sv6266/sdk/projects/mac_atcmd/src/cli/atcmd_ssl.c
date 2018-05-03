#include <string.h>
#include <stdio.h>
#include <unistd.h>
//#include "sys_status_api.h"
#ifndef MBED_EN
#include "ssl_api.h"
#endif
#include "soc_types.h"
#include "atcmd.h"
#include "atcmdlib.h"
#include "netstack_def.h"
#include "error.h"

#define SUPPORT_SSL_NUM 4
#ifdef MBED_EN
int At_SSLTest(stParam *param)
{
    char *pIp = 0, *pport = 0, *psec;
    s32 i = 0;
    u16 port;
    s8 status = 0;
    u8 test_type = 0;
    u8 max_argc = 0;
    u8 sec = 0;
    
    if (get_wifi_status() != 1)
        return ERROR_WIFI_CONNECTION;

    test_type = atoi(param->argv[0]);

    if(test_type == 0) {
        max_argc = 1;
    } else if(test_type == 1){
        max_argc = 4;
    } else {
        printf("unknown type: %d\n", test_type);
        return ERROR_INVALID_PARAMETER;
    }
    if (param->argc < max_argc) {
        printf("unknown argc: %d %d\n", param->argc, max_argc);
        return ERROR_INVALID_PARAMETER;
    }

    if(test_type == 0) {
        ssl_mbed_test();
    } else if(test_type == 1) {
        pIp = param->argv[1];
        pport = param->argv[2];
        port = atoi (pport);
        psec = param->argv[3];
        sec = atoi (psec);
        if (port == 0) {
            printf("unknown port: %d\n", port);
            return ERROR_INVALID_PARAMETER;
        }
        ssl_mbed_thoughtput_test(pIp, port, sec);
    }
    return ERROR_SUCCESS;
}
#else
ssl_t *gatsslctx[SUPPORT_SSL_NUM] = {NULL};
u8 ssl_init =false;
s32 close_sk = -1;

static void init_list()
{
    u8 i = 0;
    for(i = 0; i < SUPPORT_SSL_NUM; i++) {
        gatsslctx[i] = NULL;
    }
}

static s8 have_empty_list()
{
    u8 i = 0;
    for(i = 0; i < SUPPORT_SSL_NUM; i++) {
        if(gatsslctx[i] == NULL)
            return true;
    }
    return false;
}

static ssl_t **get_empty_list()
{
    u8 i = 0;
    for(i = 0; i < SUPPORT_SSL_NUM; i++) {
        if(gatsslctx[i] == NULL)
            return &gatsslctx[i];
    }
    return NULL;
}

static void recv_ssl_tcp_task(void *arg)
{
    s32 i = 0;
    int sel_result = 0;
    fd_set r_set;
    struct timeval timeout = { .tv_sec = 1, .tv_usec = 0};
    s32 len = 0;
    s32 max_sd = -1;
    u8 buf[4096];
    while(ssl_init) {
        FD_ZERO(&r_set);
        for(i = 0; i < SUPPORT_SSL_NUM; i++) {
            if(gatsslctx[i] != NULL && gatsslctx[i]->socket != -1) {
                FD_SET(gatsslctx[i]->socket, &r_set);
                if(gatsslctx[i]->socket > max_sd)
                    max_sd =  gatsslctx[i]->socket;
            }
        }
        if(max_sd == -1) {
            OS_MsDelay(100);
            continue;
        }
        if(close_sk != -1) {
            SSL_disconnect(gatsslctx[close_sk]);
            gatsslctx[close_sk] = NULL;
            close_sk = -1;
            continue;
        }
        sel_result = select((max_sd+1), &r_set, NULL, NULL, &timeout);
        if (sel_result < 0) {
            printf("select fail close task & socket\n");
            goto TERMINATE;
        }
        else if (sel_result == 0) {
            //FD_SET(gatsslctx[0]->socket, &r_set);
        } else {
            if(close_sk != -1) {
                SSL_disconnect(gatsslctx[close_sk]);
                gatsslctx[close_sk] = NULL;
                continue;
            }
            for(i = 0; i < SUPPORT_SSL_NUM; i++) {
                if(gatsslctx[i] != NULL ) {
                    if(FD_ISSET(gatsslctx[i]->socket, &r_set)) {
                        len = SSL_read(gatsslctx[i], buf, 1400);
                        if(len < 0) {
                            printf("SSL_recv fail\n");
                            goto TERMINATE;
                        } else {
                            if(len > 0) {
                                buf[len] = '\0';
                                printf("[%d] len: %d buf:%s\n",i, len, buf);
                            }
                        }
                    }
                }
            }
        }
    }
TERMINATE:
    printf("--exit task --\n");
    for(i = 0; i < SUPPORT_SSL_NUM; i++) {
        if(gatsslctx[i] != NULL ) {
            SSL_disconnect(gatsslctx[i]);
            gatsslctx[i] = NULL;
        }
    }
    OS_TaskDelete(NULL);
}//Do the SSL handshake
/*---------------------------------------------------------------------------*/
int At_SSLConnect(stParam *param)
{
    char *pIp = 0, *pport = 0;
    s32 i = 0;
    u16 port;
    s8 status = 0;
    ssl_t **empty_ssl;

    if (get_wifi_status() != 1)
        return ERROR_WIFI_CONNECTION;

    if (have_empty_list() == false) {
        printf("SSL connection full\n");
        return ERROR_TCP_CONNECTION;
    }

    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }
    
    pIp = param->argv[0];
    pport = param->argv[1];

    port = atoi (pport);
    if (port == 0) {
        return ERROR_INVALID_PARAMETER;
    }

    if(ssl_init == false) {
        ssl_init = true;
        SSL_init();
        init_list();
        if (OS_TaskCreate(recv_ssl_tcp_task, "tcp receTask", 4096, NULL, OS_TASK_PRIO3, NULL) != 1)
        {
            printf("Failed to create OS_TaskCreate\n");
            return ERROR_NOT_IMPLEMENT;
        }
    }

    empty_ssl = get_empty_list();
    if(empty_ssl != NULL) {
        status = SSL_connect(empty_ssl, NULL, pIp, port);
        if(status < 0) {
            *empty_ssl = NULL;
            printf("SSL_connect fail\n");
            return ERROR_TCP_CONNECTION;
        }
        for(i = 0; i < SUPPORT_SSL_NUM; i++) {
            if(gatsslctx[i] != NULL)
                printf("\n--[%d] socket: %d--\n",i, gatsslctx[i]->socket);
        }
    } else {
        printf("SSL connection full and not create res\n");
    }
    return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/
int At_SSLSend(stParam *param)
{
    char *sk_num;
    s32 sd_num = -1;
    s32 i = 0;
    s8 status = 0;
    printf(" - %d-\n", param->argc);
    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }

    sk_num = param->argv[0];
    sd_num = atoi (sk_num);
    printf(" - 2 %d-\n", sd_num);
    if (get_wifi_status() != 1) {
        return ERROR_WIFI_CONNECTION;
    }
    
    for(i = 0; i < SUPPORT_SSL_NUM; i++) {
        printf("-[%d] sk: %d-\n", i, sd_num);
        if(gatsslctx[i] != NULL &&  gatsslctx[i]->socket == sd_num) {
            status = SSL_write(gatsslctx[i], param->argv[1], strlen(param->argv[1]));
            if(status < 0) {
                SSL_disconnect(gatsslctx[i]);
                gatsslctx[i] = NULL;
            }
            return ERROR_SUCCESS;
        }
    }
    
    return ERROR_INVALID_PARAMETER;
}

/*---------------------------------------------------------------------------*/
int At_SSLDisconnect(stParam *param)
{
    char *sk_num;
    s32 sd_num = -1;
    s32 i = 0;
    
    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

    sk_num = param->argv[0];
    sd_num = atoi (param->argv[0]);
    
    for(i = 0; i < SUPPORT_SSL_NUM; i++) {
        if(gatsslctx[i] != NULL &&  gatsslctx[i]->socket == sd_num) {
            close_sk = i;
            printf("AT+SSLDISCONNECT=OK\n");
            return ERROR_SUCCESS;
        }
    }
    
    return ERROR_INVALID_PARAMETER;
}
#endif

