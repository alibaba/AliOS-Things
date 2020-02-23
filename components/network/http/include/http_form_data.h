/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef HTTP_FORM_DATA
#define HTTP_FORM_DATA

#define HTTP_DATA_SIZE   1500
#define FORM_DATA_MAXLEN 32
#define CLIENT_FORM_DATA_NUM  1

typedef struct formdata_node_t formdata_node_t;
struct formdata_node_t
{
    formdata_node_t *next;
    int   is_file;
    char  file_path[FORM_DATA_MAXLEN];
    char  *data;
    int   data_len;
};

typedef struct {
    int                is_used;
    formdata_node_t    *form_data;
    httpclient_data_t  *client_data;
} formdata_info_t;

void httpclient_clear_form_data(httpclient_data_t * client_data);
int httpclient_formdata_len(httpclient_data_t *client_data);
int httpclient_send_formdata(httpclient_t *client, httpclient_data_t *client_data);

#endif
