/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 */
#include "stdio.h"
#include "string.h"
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "aos/cli.h"

#include "http.h"

#include "network/network.h"

#include "ulog/ulog.h"

#define TAG "httpupload"

#define HTTPUPLOAD_DEBUG 1

#if HTTPCLIENT_DEBUG
#define ERR(fmt,arg...)   LOGE(TAG, "[HTTPUpload]: "fmt,##arg)
#define WARN(fmt,arg...)   LOGW(TAG, "[HTTUpload]: "fmt,##arg)
#define DBG(fmt,arg...)   LOGI(TAG, "[HTTPUpload]: "fmt,##arg)
#else
#define DBG(fmt,arg...)
#define WARN(fmt,arg...)   LOGW(TAG, "[HTTPUpload]: "fmt,##arg)
#define ERR(fmt,arg...)    LOGE(TAG, "[HTTPUpload]: "fmt,##arg)
#endif

#define HTTP_DATA_SIZE    1500

#define MAX_FILENAME_LEN  8
#define MAX_FILEPATH_LEN  48

#define DEFAULT_FILENAME "%d"

static void handle_http_upload_file_command(int argc, char **argv);

static int http_upload_file(char* url, char* src_path);

uint32_t http_now(void)
{
    return aos_now_ms();
}

void *http_malloc(uint32_t size)
{   
    return aos_malloc(size);
}

void http_free(void *ptr)
{   
    aos_free(ptr);
}

void http_msleep(uint32_t ms)
{   
    aos_msleep(ms);
}

static void handle_http_help_command()
{
    printf("Usage: http [options]\n");
    printf("       http [-h]\n");
    printf("HTTP file upload specific:\n");
    printf("  -f,        upload file name\n");
    printf("  -p,        destination path\n");
    printf("Miscellaneous:\n" );
    printf("  -h,        print this message and quit\n");
    printf("Example:\n");
    printf("http -f /data/1.txt -p http://192.168.0.1/upload\n");
}

static void handle_http_upload_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int i;

    if ( argc < 2 ) {
        ERR("%s:%d Invalid command", __func__, __LINE__);
        return;
    }

    for(i = 0; i < argc; i++) {
        if ( strcmp( (char *) argv[i], "-h" ) == 0 ) {
            handle_http_help_command();
            break;
        }
        else if ( strcmp( (char *) argv[i], "-f" ) == 0 ) {
            handle_http_upload_file_command(argc, argv);
            break;
        }
    }

}

static void handle_http_upload_file_command(int argc, char **argv)
{
    int i;
    char* url = NULL;
    char* src_path = NULL;

    if ( argc < 2 ) {
        ERR("%s:%d Invalid command", __func__, __LINE__);
    }
 
    for(i = 0; i < argc; i++) {
        if ( strcmp( (char *) argv[i], "-f" ) == 0 ) {
            i++;
            if(i >= argc) {
               ERR("%s:%d Invalid command", __func__, __LINE__);
               return ;
            }
            src_path = argv[i];
        }
        else if ( strcmp( (char *) argv[i], "-p" ) == 0 ) {
           i++;
           if(i >= argc) {
               ERR("%s:%d Invalid command", __func__, __LINE__);
               return ;
           }
           url = argv[i]; 
        }
    }

    if((url != NULL)&&(src_path != NULL)) {
        http_upload_file(url, src_path);
    }
    else {
        ERR("%s:%d Invalid command", __func__, __LINE__);
    }
}
#define BUF_SIZE (1024*1024)
static int http_upload_file(char* url, char* src_path)
{
    int ret;
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf = NULL;

    buf = (char*)http_malloc(BUF_SIZE);
    if (buf == NULL) {
        LOGE(TAG, "Malloc failed.");
        return -1;
    }
    memset(buf, 0, BUF_SIZE);
    client_data.response_buf = buf;  //Sets a buffer to store the result.
    client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
    ret = httpclient_formdata_addfile(&client_data, "form-data", "uploadFile", "application/octet-stream", src_path);
    httpclient_post(&client, url, &client_data);
    http_free(client_data.response_buf);
    return ret;
}

static struct cli_command http_commands[] = {
    {"http", "http [-s|-f|-h]", handle_http_upload_command},
};

int httpclient_cli_register()
{
    if( 0 == aos_cli_register_commands(http_commands,
            sizeof(http_commands) / sizeof(struct cli_command))) {
        return 0;
    }
    else {
        return -1;
    }
}

