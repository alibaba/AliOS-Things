/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "ota_transport.h"

void platform_ota_init( void *signal)
{

}
int8_t platform_ota_parse_requset(const char *request, int *buf_len, ota_request_params *request_parmas)
{
    return 0;
}

int8_t platform_ota_parse_response(const char *buf, int buf_len, ota_response_params *response_parmas)
{
    return 0;
}

int8_t platform_ota_parse_cancel_responce(const char *response, int buf_len, ota_response_params *response_parmas)
{
    return 0;
}

int8_t platform_ota_cancel_upgrade(aos_cloud_cb_t msgCallback)
{
    return 0;
}


int8_t platform_ota_subscribe_upgrade(aos_cloud_cb_t msgCallback)
{
    return 0;
}

int8_t platform_ota_publish_request(ota_request_params *request_parmas)
{
    return 0;
}

const char *platform_ota_get_id()
{
    return 0;
}

int8_t platform_ota_status_post(int status, int percent)
{
    return 0;
}

int8_t platform_ota_result_post(void)
{
    return 0;
}

int8_t platform_destroy_connect(void){
    return 0;
}