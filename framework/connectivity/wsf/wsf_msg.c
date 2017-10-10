/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <aos/network.h>

#include "os.h"
#include "wsf_defines.h"
#include "wsf_msg.h"
#include "wsf_config.h"
#include "wsf_network.h"
#include "wsf_log.h"

void *msg_id_lock = NULL;
static uint32_t global_msg_id = 0;

#define MAGIC_CODE "WSF1"
#define MODULE_NAME "wsf_msg"
void wsf_msg_session_init(wsf_msg_session_t *session)
{
    session->psem = os_semaphore_init();
    session->request = NULL;
    session->response = NULL;
    session->cb = NULL;
    session->extra = NULL;
}


void wsf_msg_session_destroy(wsf_msg_session_t *session)
{
    os_semaphore_destroy(session->psem);
    session->request = NULL;
    session->response = NULL;
    session->cb = NULL;
    session->extra = NULL;
}

void wsf_msg_session_wait(wsf_msg_session_t *session)
{
    os_semaphore_wait(session->psem, OS_WAIT_INFINITE);
}

int wsf_msg_session_timewait(wsf_msg_session_t *session, int timeout)
{
    return os_semaphore_wait(session->psem, timeout * 1000);
}

void wsf_msg_session_signal(wsf_msg_session_t *session)
{
    os_semaphore_post(session->psem);
}

int wsf_msg_id_get()
{
    int ret;
    os_mutex_lock(msg_id_lock);
    ret = global_msg_id++;
    os_mutex_unlock(msg_id_lock);
    return ret;
}

wsf_msg_t *wsf_msg_response_create(int msg_id, invoke_result_code result,
                                   const char *data, int length)
{
    uint32_t temp;
    int msg_size = 0;
    char *session_id = wsf_session_id_get();
    char *device_id = wsf_device_id_get();

    //count the msg size
    msg_size += sizeof(wsf_msg_header_t);       //header size
    msg_size += 1;                              //result code
    msg_size += 1;                              //session id length
    if (session_id) {
        msg_size += strlen(session_id);          //session id
    }
    msg_size += 1;                              //device id length
    if (device_id) {
        msg_size += strlen(device_id);          //device id
    }
    msg_size += 4;                              //response data length
    msg_size += length;

    wsf_msg_t *rsp = (wsf_msg_t *)os_malloc(msg_size);

    if (!rsp) {
        LOGE(MODULE_NAME, "failed to create response, out of memory");
        if (session_id) {
            os_free(session_id);
        }
        if (device_id) {
            os_free(device_id);
        }
        return NULL;

    }

    //fill header
    strncpy(rsp->header.magic_code, MAGIC_CODE, sizeof(rsp->header.magic_code));

    temp = msg_size;
    memcpy(rsp->header.msg_length, &temp, sizeof(temp));

    set_msg_type_version(&rsp->header, MSG_RESPONSE, wsf_get_config()->version);

    temp = msg_id;
    memcpy(rsp->header.msg_id, &temp, sizeof(temp));

    uint8_t *pp = rsp->payload;
    *pp = (unsigned char)(result & 0xFF); //result code
    pp++;
    if (session_id) {
        size_t len = strlen(session_id);
        *pp = (unsigned char)(len & 0xFF); //session id length
        pp++;
        memcpy(pp, session_id, len);       //session id
        pp += len;
        os_free(session_id);
    } else {
        *pp = 0;
        pp++;
    }

    if (device_id) {
        size_t len = strlen(device_id);
        *pp = (unsigned char)(len & 0xFF); //device id length
        pp++;
        memcpy(pp, device_id, len);       //device id
        pp += len;
        os_free(device_id);
    } else {
        *pp = 0;
        pp++;
    }

    int net_len = htonl(length);
    memcpy(pp, &net_len, sizeof(int));     //response data length
    pp += sizeof(int);

    if (data) {
        memcpy(pp, data, length);        //response data
    }

    return rsp;
}

wsf_msg_t *wsf_msg_request_create(const char *servcie_name,
                                  wsf_list_t *param_list)
{

    uint32_t temp;
    int msg_size = 0;
    char *session_id = wsf_session_id_get();
    char *device_id = wsf_device_id_get();

    int param_count = 0;
    wsf_list_t *plist = param_list;

    //count the msg size
    msg_size += sizeof(wsf_msg_header_t);       //header size
    msg_size += 1;                              //service name length
    msg_size += strlen(servcie_name);           //service name
    msg_size += 1;                              //session id length
    if (session_id) {
        msg_size += strlen(session_id);          //session id
    }
    msg_size += 1;                              //device id length
    if (device_id) {
        msg_size += strlen(device_id);           //device id
    }

    msg_size += 1;                              //secret length, reserved

    msg_size += 1;                              //parameter count

    if (plist) {
        wsf_list_node_t *pnode = plist->head;
        while (pnode) {
            param_count++;
            msg_size += 4;                      //parameter length
            msg_size += pnode->length;          //parameter data
            pnode = pnode->next;
        }
    }
    wsf_msg_t *req = (wsf_msg_t *) os_malloc(msg_size);
    if (!req) {
        LOGE(MODULE_NAME, "failed to create request, out of memory");
        if (session_id) {
            os_free(session_id);
        }
        if (device_id) {
            os_free(device_id);
        }
        return NULL;
    }

    //fill header
    strncpy(req->header.magic_code, MAGIC_CODE, sizeof(req->header.magic_code));

    temp = msg_size;
    memcpy(req->header.msg_length, &temp, sizeof(temp));

    set_msg_type_version(&req->header, MSG_REQUEST, wsf_get_config()->version);

    temp = wsf_msg_id_get();
    memcpy(req->header.msg_id, &temp, sizeof(temp));

    uint8_t *pp = req->payload;
    size_t len = strlen(servcie_name);
    *pp = (unsigned char)(len & 0xFF); //service name length
    pp++;
    memcpy(pp, servcie_name, len);     //service name
    pp += len;
    if (session_id) {
        len = strlen(session_id);
        *pp = (unsigned char)(len & 0xFF); //session id length
        pp++;
        memcpy(pp, session_id, len);       //session id
        pp += len;
        os_free(session_id);
    } else {
        *pp = 0;
        pp++;
    }

    if (device_id) {
        len = strlen(device_id);
        *pp = (unsigned char)(len & 0xFF); //device id length
        pp++;
        memcpy(pp, device_id, len);       //device id
        pp += len;
        os_free(device_id);
    } else {
        *pp = 0;
        pp++;
    }

    /* secret key reserved field used as user_heartbeat_interval */
    *pp = 0;
    pp++;

    *pp = (unsigned char)(param_count & 0xFF); //parameters count
    pp++;
    if (plist) {
        wsf_list_node_t *pnode = plist->head;
        while (pnode) {
            int p_len = htonl(pnode->length);
            memcpy(pp, &p_len, sizeof(int));     //parameter length
            pp += sizeof(int);

            memcpy(pp, pnode->data, pnode->length);
            pp += pnode->length;                 //parameter data

            pnode = pnode->next;

        }
    }

    return req;
}

wsf_msg_t *wsf_msg_register_request_create()
{

    uint32_t temp;
    int msg_size = 0;
    char *device_id = wsf_device_id_get();
    char *session_id = wsf_session_id_get();

    //count the msg size
    msg_size += sizeof(wsf_msg_header_t);       //header size
    msg_size++;                                 //session id length
    if (session_id) {
        msg_size += strlen(session_id);          //session id
    }
    msg_size++;                                 //device id length
    if (device_id) {
        msg_size += strlen(device_id);           //device id
    }

    /* secret key replaced by user heartbeat interval */
    msg_size++;                                 //control flag
    if (wsf_get_config()->user_heartbeat_interval) {  //set heartbeat internal
        msg_size += 1;
    }

    wsf_msg_t *req = (wsf_msg_t *) os_malloc(msg_size);

    if (!req) {
        LOGE(MODULE_NAME, "failed to create register request, out of memory");
        if (session_id) {
            os_free(session_id);
        }
        if (device_id) {
            os_free(device_id);
        }
        return NULL;
    }

    //fill header
    strncpy(req->header.magic_code, MAGIC_CODE, sizeof(req->header.magic_code));

    temp = msg_size;
    memcpy(req->header.msg_length, &temp, sizeof(temp));

    set_msg_type_version(&req->header, MSG_REGISTER_REQ, wsf_get_config()->version);

    temp = wsf_msg_id_get();
    memcpy(req->header.msg_id, &temp, sizeof(temp));

    uint8_t *pp = req->payload;

    if (session_id) {
        size_t len = strlen(session_id);
        *pp = (unsigned char)(len & 0xFF); //session id length
        pp++;
        memcpy(pp, session_id, len);       //session id
        pp += len;
        os_free(session_id);
    } else {
        *pp = 0;
        pp++;
    }

    if (device_id) {
        size_t len = strlen(device_id);
        *pp = (unsigned char)(len & 0xFF); //device id length
        pp++;
        memcpy(pp, device_id, len);       //device id
        pp += len;
        os_free(device_id);
    } else {
        *pp = 0;
        pp++;
    }

    if (wsf_get_config()->user_heartbeat_interval) {
        *pp = (unsigned char)(1 & 0xFF);
        pp++;
        *pp = (unsigned char)(wsf_get_config()->user_heartbeat_interval & 0xFF);
        pp++;
    } else {
        *pp = 0;
        pp++;
    }

    return req;
}
void wsf_msg_heartbeat_request_init(wsf_msg_t *hb_req)
{
    uint32_t temp;

    strncpy(hb_req->header.magic_code, MAGIC_CODE, sizeof(hb_req->header.magic_code));

    temp = sizeof(wsf_msg_t);
    memcpy(hb_req->header.msg_length, &temp, sizeof(temp));

    set_msg_type_version(&hb_req->header, MSG_HEARTBEAT_REQ,
                         wsf_get_config()->version);

    temp = wsf_msg_id_get();
    memcpy(hb_req->header.msg_id, &temp, sizeof(temp));
}

int wsf_msg_accept(const char *buf, int length)
{
    //small packet, hold it
    if (length < sizeof(wsf_msg_header_t)) {
        //LOGI(MODULE_NAME,"smalll packet receved, less than msg header");
        return 1;
    }

    if (strncmp(buf, MAGIC_CODE, strlen(MAGIC_CODE)) != 0) {
        return 0;
    } else {
        return 1;
    }

}

int wsf_msg_complete(const char *buf, int length)
{
    if (length < sizeof(wsf_msg_header_t)) {
        return 0;
    }
    wsf_msg_header_t *msg_header = (wsf_msg_header_t *)buf;
    uint32_t  msg_len = os_get_unaligned_be32(msg_header->msg_length);

    if (length >= msg_len) {
        return 1;
    } else {
        return 0;
    }
}


wsf_msg_header_t *wsf_msg_header_decode(const char *buf, int length)
{

    uint32_t temp;

    if (length < sizeof(wsf_msg_header_t)) {
        return 0;
    }
    wsf_msg_header_t *msg_header = (wsf_msg_header_t *)buf;

    memcpy(&temp, msg_header->msg_length, sizeof(uint32_t));
    temp = htonl(temp);
    memcpy(msg_header->msg_length, &temp, sizeof(temp));
    memcpy(&temp, msg_header->msg_id, sizeof(uint32_t));
    temp = htonl(temp);
    memcpy(msg_header->msg_id, &temp, sizeof(temp));
    return msg_header;
}

wsf_msg_header_t *wsf_msg_header_encode(const char *buf, int length)
{

    uint32_t temp;

    if (length < sizeof(wsf_msg_header_t)) {
        return 0;
    }
    wsf_msg_header_t *msg_header = (wsf_msg_header_t *)buf;

    memcpy(&temp, msg_header->msg_length, sizeof(uint32_t));
    temp = htonl(temp);
    memcpy(msg_header->msg_length, &temp, sizeof(temp));

    memcpy(&temp, msg_header->msg_id, sizeof(uint32_t));
    temp = htonl(temp);
    memcpy(msg_header->msg_id, &temp, sizeof(temp));

    return msg_header;
}
