#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "httpclient.h"
#include "aos/kernel.h"




#define REQ_BUF_SIZE 2048
static char req_buf[REQ_BUF_SIZE];

/* @brief http response buffer */
#define RSP_BUF_SIZE 2048
static char rsp_buf[RSP_BUF_SIZE];


#define HTTP_BUFF_SIZE              2048
#define HTTP_HEADER_SIZE            1024
#define HTTP_HEADER_COUNT           8
#define HTTP_REQUEST_PARAMS_LEN_MAX 2048
#define HTTP_SEND_RECV_TIMEOUT      10000
#define HTTP_REQUEST_TIMEOUT        30000
#define HTTP_DEFAULT_HEADER_NAME    "content-type"
#define HTTP_DEFAULT_HEADER_DATA    "application/json"
#define HTTP_CRLF                   "\r\n"

extern const mp_obj_type_t http_client_type;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    httpclient_t client ;
    httpclient_data_t client_data  ;
} http_client_obj_t;


typedef struct
{
    char *name;
    char *data;
} http_header_t;

typedef struct
{
    char *url;
    char *filepath;
    int method;
    http_header_t http_header[HTTP_HEADER_COUNT];
    uint32_t timeout;
    char *params;
    char *buffer;
    int params_len;
    int js_cb_ref;
} http_param_t;





void http_client_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    http_client_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t http_client_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    http_client_obj_t* http_client_obj = m_new_obj(http_client_obj_t);
    if (!http_client_obj) {
        mp_raise_OSError(ENOMEM);
    }

    http_client_obj->Base.type = &http_client_type;
    http_client_obj->ModuleName = "client";

    httpclient_t client  = {0};
    httpclient_data_t client_data = {0} ;

    http_client_obj->client = client ;
    http_client_obj->client_data = client_data ;

    // http_client_obj->i2c_handle.handle     = NULL;

    memset(req_buf, 0, sizeof(req_buf));
    http_client_obj->client_data.header_buf = req_buf;
    http_client_obj->client_data.header_buf_len = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    http_client_obj->client_data.response_buf = rsp_buf;
    http_client_obj->client_data.response_buf_len = sizeof(rsp_buf);


    return MP_OBJ_FROM_PTR(http_client_obj);
}

STATIC mp_obj_t obj_http_get(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *url = (char *)mp_obj_str_get_str(args[1]);
    ret = httpclient_get(&http_client_obj->client, url, &http_client_obj->client_data);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_get, 2, obj_http_get);


STATIC mp_obj_t obj_http_put(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *url = (char *)mp_obj_str_get_str(args[1]);
    ret = httpclient_put(&http_client_obj->client, url, &http_client_obj->client_data);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_put, 2, obj_http_put);

STATIC mp_obj_t obj_http_post(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *url = (char *)mp_obj_str_get_str(args[1]);
    ret = httpclient_post(&http_client_obj->client, url, &http_client_obj->client_data);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_post, 2, obj_http_post);


STATIC mp_obj_t http_set_header(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj  is NULL\n");
        return mp_const_none;
    }

    char *header = (char *)mp_obj_str_get_str(args[1]);
    http_client_obj->client.header = header ;
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_set_header, 2, http_set_header);


STATIC mp_obj_t http_set_data(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj  is NULL\n");
        return mp_const_none;
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    http_client_obj->client_data.post_buf = bufinfo.buf;
    http_client_obj->client_data.post_buf_len = mp_obj_get_int(args[2]);
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_set_data, 3, http_set_data);


STATIC mp_obj_t obj_http_header(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *url = (char *)mp_obj_str_get_str(args[1]);
    ret = httpclient_head(&http_client_obj->client, url, &http_client_obj->client_data);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_header, 2, obj_http_header);

STATIC mp_obj_t obj_http_conn(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *url = (char *)mp_obj_str_get_str(args[1]);
    ret = httpclient_conn(&http_client_obj->client, url);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_conn, 2, obj_http_conn);


STATIC mp_obj_t obj_http_recv(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *url = (char *)mp_obj_str_get_str(args[1]);
    ret = httpclient_recv(&http_client_obj->client,  &http_client_obj->client_data);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_recv, 1, obj_http_recv);



STATIC mp_obj_t obj_http_send(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *url = (char *)mp_obj_str_get_str(args[1]);
    int method = mp_obj_get_int(args[2]);
    ret = httpclient_send(&http_client_obj->client, url, method ,&http_client_obj->client_data);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_send, 3, obj_http_send);





STATIC mp_obj_t obj_http_get_res_header_value(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *url = (char *)mp_obj_str_get_str(args[1]);
    ret = httpclient_get_response_code(&http_client_obj->client);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_get_res_header_value, 2, obj_http_get_res_header_value);


STATIC mp_obj_t obj_http_get_res_code(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *url = (char *)mp_obj_str_get_str(args[1]);
    ret = httpclient_get_response_code(&http_client_obj->client);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_get_res_code, 1, obj_http_get_res_code);





STATIC mp_obj_t obj_http_get_add_text(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 6)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *content_disposition = (char *)mp_obj_str_get_str(args[1]);
    char *name = (char *)mp_obj_str_get_str(args[3]);
    char *content_type = (char *)mp_obj_str_get_str(args[2]);
    char *data = (char *)mp_obj_str_get_str(args[4]);
    int data_len = mp_obj_get_int(args[5]);


    ret = httpclient_formdata_addtext(&http_client_obj->client,content_disposition,content_type,name,data,data_len);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_add_text, 6, obj_http_get_add_text);


STATIC mp_obj_t obj_http_get_add_file(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *content_disposition = (char *)mp_obj_str_get_str(args[1]);
    char *name = (char *)mp_obj_str_get_str(args[2]);
    char *content_type = (char *)mp_obj_str_get_str(args[3]);
    char *file_path = (char *)mp_obj_str_get_str(args[4]);


    ret = httpclient_formdata_addfile(&http_client_obj->client,content_disposition,name,content_type,file_path);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_add_file, 5, obj_http_get_add_file);


STATIC mp_obj_t obj_http_delete(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char *url = (char *)mp_obj_str_get_str(args[1]);
    ret = httpclient_delete(&http_client_obj->client,url,&http_client_obj->client_data);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_delete, 2, obj_http_delete);



STATIC mp_obj_t obj_http_reset(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    httpclient_reset(&http_client_obj->client);
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_reset, 1, obj_http_reset);


STATIC mp_obj_t obj_http_prepare(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    int header_size= mp_obj_get_int(args[1]);
    int resp_size= mp_obj_get_int(args[2]);


    ret = httpclient_prepare(&http_client_obj->client,header_size,resp_size);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_prepare, 3, obj_http_prepare);


STATIC mp_obj_t obj_http_unprepare(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    ret = httpclient_unprepare(&http_client_obj->client);
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_unprapare, 1, obj_http_unprepare);



STATIC mp_obj_t obj_http_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    httpclient_clse(&http_client_obj->client);
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_client_close, 1, obj_http_close);


STATIC mp_obj_t obj_http_get_response(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }


    return mp_obj_new_str(http_client_obj->client_data.response_buf,strlen(http_client_obj->client_data.response_buf));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_get_response, 1, obj_http_get_response);


static void http_request_notify(void *pdata)
{
    http_param_t *msg = (http_param_t *)pdata;

}


/* create task for http download */
static void task_http_download_func(char * url,char *filepath)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    int num=0;
    int ret;

    // 字符串偶现丢失，不知原因，暂时写死，by梓同
    //char * req_url = param->url;
    //int fd = aos_open(param->filepath,  O_CREAT | O_RDWR | O_APPEND);
    int fd = aos_open(filepath,  O_CREAT | O_RDWR | O_APPEND);

    char * req_url = url ;
    //printf("usr ls %s , filepath is %s \r\n",req_url,param->filepath);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    ret = httpclient_conn(&client, req_url);

    if (!ret) {
        ret = httpclient_send(&client, req_url, HTTP_GET, &client_data);

        do{
            ret = httpclient_recv(&client, &client_data);
            // printf("response_content_len=%d, retrieve_len=%d,content_block_len=%d\n", client_data.response_content_len,client_data.retrieve_len,client_data.content_block_len);
            // printf("ismore=%d \n", client_data.is_more);

            num = aos_write(fd, client_data.response_buf, client_data.content_block_len);
            if(num > 0){
                printf("aos_write num=%d\n", num);
            }
        }while(client_data.is_more);
    }
    // printf("************task_http_download_func0********** \r\n");

    ret = aos_sync(fd);
    // param->buffer = "http download success";
    // printf("************task_http_download_func1********** \r\n");

    httpclient_clse(&client);

    //amp_task_schedule_call(http_request_notify, param);
    // printf("************task_http_download_func2********** \r\n");
}



STATIC mp_obj_t obj_http_download(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);
    aos_task_t http_task;
    http_param_t *http_param = NULL;
    char *http_buffer = NULL;


    int ret = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char * url =  mp_obj_str_get_str(args[1]);
    char * filepath =  mp_obj_str_get_str(args[2]);

    http_param = (http_param_t *)aos_malloc(sizeof(http_param_t));
    if (!http_param)
    {
        goto done;
    }
    memset(http_param, 0, sizeof(http_param_t));
    http_param->buffer = http_buffer;
    http_param->url = url;
    http_param->filepath = filepath; 

    //aos_task_new_ext(&http_task, "amp http download task", task_http_download_func, http_param, 1024 * 8, AOS_DEFAULT_APP_PRI + 3);
    task_http_download_func(url,filepath);

done:
    if (http_buffer)
        aos_free(http_buffer);
    if (http_param && http_param->params)
        cJSON_free(http_param->params);
    if (http_param)
        aos_free(http_param);

    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_download, 3, obj_http_download);



STATIC mp_obj_t obj_http_request(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);
    aos_task_t http_task;
    http_param_t *http_param = NULL;
    char *http_buffer = NULL;


    int ret = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }
    char * url =  mp_obj_str_get_str(args[1]);
    char * filepath =  mp_obj_str_get_str(args[2]);

    http_param = (http_param_t *)aos_malloc(sizeof(http_param_t));
    if (!http_param)
    {
        goto done;
    }
    memset(http_param, 0, sizeof(http_param_t));
    http_param->buffer = http_buffer;
    http_param->url = url;
    http_param->filepath = filepath; 

    //aos_task_new_ext(&http_task, "amp http download task", task_http_download_func, http_param, 1024 * 8, AOS_DEFAULT_APP_PRI + 3);
    task_http_download_func(url,filepath);

done:
    if (http_buffer)
        aos_free(http_buffer);
    if (http_param && http_param->params)
        cJSON_free(http_param->params);
    if (http_param)
        aos_free(http_param);

    return mp_obj_new_str(http_client_obj->client_data.response_buf,strlen(http_client_obj->client_data.response_buf));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_http_request, 3, obj_http_request);



STATIC mp_obj_t obj_HTTPConnection(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);

    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    http_client_obj_t* http_client_obj = (http_client_obj_t *)self;
    if (http_client_obj == NULL)
    {
        LOG_E("http_client_obj is NULL\n");
        return mp_const_none;
    }




    return mp_obj_new_str(http_client_obj->client_data.response_buf,strlen(http_client_obj->client_data.response_buf));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_HTTPConnection, 2, obj_HTTPConnection);



STATIC const mp_rom_map_elem_t http_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_http)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_set_header), MP_ROM_PTR(&mp_obj_http_client_set_header)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_set_data), MP_ROM_PTR(&mp_obj_http_client_set_data)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_get), MP_ROM_PTR(&mp_obj_http_client_get)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_put), MP_ROM_PTR(&mp_obj_http_client_put)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_post), MP_ROM_PTR(&mp_obj_http_client_post)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_send), MP_ROM_PTR(&mp_obj_http_client_send)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_recv), MP_ROM_PTR(&mp_obj_http_client_recv)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_conn), MP_ROM_PTR(&mp_obj_http_client_conn)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_header), MP_ROM_PTR(&mp_obj_http_client_header)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_get_response_code), MP_ROM_PTR(&mp_obj_get_res_code)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_get_response_header), MP_ROM_PTR(&mp_obj_get_res_header_value)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_get_response), MP_ROM_PTR(&mp_obj_get_response)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_add_text), MP_ROM_PTR(&mp_obj_add_text)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_add_file), MP_ROM_PTR(&mp_obj_add_file)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_delete), MP_ROM_PTR(&mp_obj_delete)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_reset), MP_ROM_PTR(&mp_obj_reset)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_prepare), MP_ROM_PTR(&mp_obj_http_client_prepare)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_unprepare), MP_ROM_PTR(&mp_obj_http_client_unprapare)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_obj_http_client_close)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_download), MP_ROM_PTR(&mp_obj_http_download)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_request), MP_ROM_PTR(&mp_obj_http_request)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_HTTPConnection), MP_ROM_PTR(&mp_obj_HTTPConnection)},



};

STATIC MP_DEFINE_CONST_DICT(http_module_globals, http_module_globals_table);



const mp_obj_type_t http_client_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_client,
    .print = http_client_print,
    .make_new = http_client_new,
    .locals_dict = (mp_obj_dict_t *)&http_module_globals,
};
