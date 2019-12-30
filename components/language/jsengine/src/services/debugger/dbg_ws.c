/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "jse_common.h"
#include "app_mgr.h"
#include "board_info.h"
#include "dbg_ssdp.h"
#include "dbg_ws.h"

#include "cJSON.h"

#if defined(WITH_LWIP) || defined(LINUXOSX)

static void* websocketMutex = NULL;

#define BWS_READ_SIZE 1024
#define BWS_LOG_TAG "PAL-WS"
#define BWS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

#define BWS_REQ                     \
    "GET %s HTTP/1.1\r\n"           \
    "Connection: Upgrade\r\n"       \
    "Host: %s:%d\r\n"               \
    "Sec-WebSocket-Key: %s\r\n"     \
    "Sec-WebSocket-Version: 13\r\n" \
    "Upgrade: websocket\r\n"        \
    "Target: %s\r\n"                \
    "\r\n"

/* #define BWD_PRINT_FRAME */

static int pingInterval = 25 * 1000;

#ifndef BONE_VERSION
#define BONE_VERSION "1.0"
#endif

#ifndef VERSION_DESCRIPTION
#define VERSION_DESCRIPTION "FREERTOS Version"
#endif

static int websocket_getVersion()
{
    char* p_resp_cmd = NULL;
    int err_code     = 200;

    p_resp_cmd = jse_calloc(1, 128);
    if (p_resp_cmd == NULL) {
        /* snprintf(p_resp_cmd, 128, "getVersion Fail\r\n"); */
        err_code = 201;
        goto do_exit;
    }

    snprintf(p_resp_cmd, 128, "Version:%s\r\nVersionInfo:%s\r\n", BONE_VERSION,
             VERSION_DESCRIPTION);

do_exit:
    bone_websocket_send_frame("/device/getVersion_reply", err_code, p_resp_cmd);

    jse_free(p_resp_cmd);
    return 0;
}

static void websocket_eval_js(void* arg)
{
    jsengine_start(arg);
    jse_free(arg);
}

static int random_string(unsigned char* buf, int len)
{
    int i = 0;

    srand((int)time(NULL));

    for (i = 0; i < len; i++) {
        buf[i] = (unsigned char)(rand() % 255 + 1);
    }
    return 0;
}

static int headers_get(const char* headers, const char* key,
                       unsigned char* value, size_t value_len)
{
    char* p;
    if ((p = strstr(headers, key)) == NULL) return -1;
    p += strlen(key);

    memset(value, 0, sizeof(value_len));
    sscanf(p, "%s", value);
    return 0;
}

static int respond_shake_key(const char* accept_key, unsigned char* respond_key,
                             size_t respond_key_len)
{
    if (accept_key == NULL) return -1;

    char client_key[256] = {0};
    strncat(client_key, accept_key, sizeof(client_key) - 1);
    strncat(client_key, BWS_GUID, sizeof(client_key) - 1);

    unsigned char decrypt[20]; /* 160 bits */

    /* 计算 decrypt */
    jse_sha1_context sha1_ctx;
    jse_sha1_init(&sha1_ctx);
    jse_sha1_starts(&sha1_ctx);
    jse_sha1_update(&sha1_ctx, (unsigned char*)client_key,
                        strlen((char*)client_key));
    jse_sha1_finish(&sha1_ctx, decrypt);
    jse_sha1_free(&sha1_ctx);

    size_t olen = 0;
    return jse_base64_encode(respond_key, respond_key_len, &olen, decrypt,
                                 sizeof(decrypt));
}

static int match_shake_key(const char* my_key, const char* accept_key)
{
    unsigned char respond_key[BWS_SHAKE_KEY_LEN] = {0};
    respond_shake_key(my_key, respond_key, sizeof(respond_key));

    if (strcmp((char*)respond_key, accept_key) == 0) {
        return 0;
    }
    return -1;
}

static int request_shake_key(unsigned char* key, int key_len)
{
    unsigned char tempKey[16] = {0};
    random_string(tempKey, sizeof(tempKey));

    size_t olen = 0;
    return jse_base64_encode(key, key_len, &olen, tempKey, sizeof(tempKey));
}

static int frame_encode(unsigned char* payload, size_t payload_len,
                        bone_websocket_frame_t* frame)
{
    int i = 0, count = 0;

    if (payload == NULL || frame->frame_total_len < 2 ||
        frame->frame_data == NULL)
        return -1;

    int frame_total_len                  = 0;
    frame->payload_len                   = payload_len;
    frame->frame_data[frame_total_len++] = frame->type;

    if (frame->is_mask) {
        frame->frame_data[frame_total_len] = 0x80;
    }

    if (payload_len < 126) {
        frame->frame_data[frame_total_len++] |= (payload_len & 0x7F);
    } else if (payload_len < 65536) {
        frame->frame_data[frame_total_len++] |= 0x7E;
        for (i = 1; i >= 0; --i) {
            frame->frame_data[frame_total_len++] |=
                (char)((payload_len >> (8 * i)) & 0xFF);
        }
    } else {
        frame->frame_data[frame_total_len++] |= 0x7F;
        /* set 7 -> 3, frame should never be so large */
        for (i = 3; i >= 0; --i) {
            frame->frame_data[frame_total_len++] |=
                (char)((payload_len >> (8 * i)) & 0xFF);
        }
    }

    if (frame->is_mask) {
        unsigned char mask_key[4] = {0};
        random_string(mask_key, sizeof(mask_key));
        for (i = 0; i < sizeof(mask_key); ++i) {
            frame->frame_data[frame_total_len++] = mask_key[i];
        }

        unsigned char temp1, temp2;
        for (i = 0, count = 0; i < payload_len; i++) {
            temp1 = mask_key[count];
            temp2 = payload[i];
            frame->frame_data[frame_total_len++] |=
                (char)(((~temp1) & temp2) | (temp1 & (~temp2)));
            count++;
            if (count >= sizeof(mask_key)) count = 0;
        }
    } else {
        memcpy(frame->frame_data + frame_total_len, payload, payload_len);
        frame_total_len += payload_len;
    }

    frame->frame_total_len = frame_total_len;

#ifdef BWD_PRINT_FRAME
    char frame_4hex[32] = {0};
    if (frame->frame_total_len > 3) {
        sprintf(frame_4hex, "0x%02X %02X %02X %02X ...(%d bytes) ",
                frame->frame_data[0], frame->frame_data[1],
                frame->frame_data[2], frame->frame_data[3],
                (int)frame->frame_total_len);
    } else if (frame->frame_total_len == 3) {
        sprintf(frame_4hex, "0x%02X %02X %02X", frame->frame_data[0],
                frame->frame_data[1], frame->frame_data[2]);
    } else {
        sprintf(frame_4hex, "0x%02X %02X", frame->frame_data[0],
                frame->frame_data[1]);
    }
    jse_debug("build frame: %s\r\n", frame_4hex);
#endif
    return 0;
}

/* close socket when timeout, reply ping frame */
static int pingTimeout = 5000;

static void bone_websocket_timeout(void* arg)
{
    bone_websocket_client_t* client = (bone_websocket_client_t*)arg;
    if (client == NULL) {
        return;
    }

    jse_debug("websocket pingTimeout (%d) client=%p", pingTimeout, client);
    jse_debug("websocket close,  client=%p", client);

    bone_websocket_deinit(client);

    jse_debug("websocket reconnect ...");
    be_debuger_websocket_reconnect();
}

static void ping_timer_cb(void* arg)
{
    bone_websocket_client_t* client = (bone_websocket_client_t*)arg;
    if (client == NULL) {
        return;
    }

    jse_debug("websocket 发送 ping packet, client=%p", client);

    int ret;
    unsigned char payload[2]     = "2";
    unsigned char frame_buf[100] = {0};
    int frame_size =
        bone_websocket_build_frame(payload, strlen((char*)payload), true,
                                   BWS_TXT_FRAME, frame_buf, sizeof(frame_buf));

    ret = bone_websocket_send_raw(client, frame_buf, frame_size);

    jse_debug("bone_websocket_send_raw return %d, frame_size = %d", ret,
              frame_size);

    if (ret > 0) {
        /* jse_osal_post_delayed_action(pingInterval, ping_timer_cb, client);
           jse_osal_post_delayed_action(pingTimeout, bone_websocket_timeout,
                                       client); */

        jse_task_cancel_timer(client->ping_tsk_handle);
        client->ping_tsk_handle = jse_task_timer_action(
            pingInterval, ping_timer_cb, client, JSE_TIMER_ONCE);

        jse_task_cancel_timer(client->timeout_tsk_handle);
        client->timeout_tsk_handle = jse_task_timer_action(
            pingTimeout, bone_websocket_timeout, client, JSE_TIMER_ONCE);

    } else {
        /* deinit websocket if socket closed */
        bone_websocket_deinit(client);
    }
}

static int bone_websocket_send_cmd(bone_websocket_client_t* client, char* cmd,
                                   bool is_mask)
{
    if (client == NULL) {
        return -1;
    }

    int ret;
    unsigned char payload[32];
    unsigned char frame_buf[64] = {0};

    strncpy(payload, cmd, sizeof(payload) - 1);
    int frame_size =
        bone_websocket_build_frame(payload, strlen((char*)payload), is_mask,
                                   BWS_TXT_FRAME, frame_buf, sizeof(frame_buf));

    ret = bone_websocket_send_raw(client, frame_buf, frame_size);

    jse_debug("bone_websocket_send_raw return %d, frame_size = %d", ret,
              frame_size);

    /* for(int i; i<frame_size; i++){
           jse_debug("%0x ", frame_buf[i]);
       }
       jse_debug("\n"); */

    return ret;
}

/* process and reply be-cli/webide commands
 * 42["/device/disconnect","arg0","arg1","arg2","arg3","arg4"]
 * 42["/ide/console",1,"2",{"3":"4","5":"rockzhou"}]
 * 0{"sid":"RyTmnKrgBDtX37bsAAAA","upgrades":[],"pingInterval":25000,"pingTimeout":5000}
 */
static int bone_websocket_on_command(bone_websocket_client_t* client,
                                     unsigned char* msg, size_t msg_len)
{
    char* strCmd;
    char* args[32];
    int argc = 0;
    int i;
    int count;
    strCmd = msg;

    /* 4 message：need to send
       Packet type CONNECT 0, DISCONNECT 1, EVENT 2, ACK 3, ERROR 4,
       BINARY_EVENT 5, BINARY_ACK 6 */

    jse_debug("%d, msg_len = %d, strCmd = %s \n", __LINE__, msg_len, strCmd);

    if (msg_len >= 2) {
        if (msg[0] == '4') {
            switch (msg[1]) {
                case '0': /* CONNECT */
                    break;
                case '1': /* DISCONNECT */
                    client->state = BWS_STATE_CLOSING;
                    break;
                case '2': /* EVENT */
                    break;
                case '3': /* ACK */
                    break;
                case '4': /* ERROR */
                    break;
            }
        } else if (msg[0] == '3') {
            if (strcmp("3probe", msg) == 0) {
                /* Upgrade帧 */
                bone_websocket_send_cmd(client, "5", true);
            }
        } else if (msg[0] == '6') {
            /* noop frame */
        }
    }

    for (i = 0; i < msg_len; i++) {
        if (msg[i] >= '0' && msg[i] <= '9') {
            strCmd = msg + i + 1;
        } else {
            break;
        }
    }

    jse_debug("%d, strCmd = %s\n", __LINE__, strCmd);

    if (strCmd == NULL) return 0;

    cJSON* root = NULL;
    cJSON* item;

    memset(args, 0, sizeof(args));

    root = cJSON_Parse(strCmd);

    jse_debug("%d, root = %p\n", __LINE__, root);

    if (root == NULL) return 0;

    if (cJSON_IsArray(root)) {
        /* array */
        count = cJSON_GetArraySize(root);
        jse_debug("count = %d\n", count);
        for (i = 0; i < count; i++) {
            item = cJSON_GetArrayItem(root, i);
            jse_debug("item type is %d\n", item->type);

            switch (item->type) {
                case cJSON_String:
                    args[argc] = item->valuestring;
                    argc++;
                    break;
                case cJSON_Number:
                    break;
            }

            /* jse_debug("%s\n", item->valuestring); */
        }
    } else if (cJSON_IsObject(root)) {
        /* object */
        item = cJSON_GetObjectItem(root, "pingInterval");
        if (cJSON_IsNumber(item)) {
            int Interval = item->valueint;

            if (Interval != pingInterval) {
                /* clear ping timeout */
                pingInterval = Interval;
                jse_task_cancel_timer(client->ping_tsk_handle);
                client->ping_tsk_handle = jse_task_timer_action(
                    pingInterval, ping_timer_cb, client, JSE_TIMER_ONCE);

                /* jse_osal_cancel_delayed_action(pingInterval, ping_timer_cb,
                                                 client);
                   pingInterval = Interval;
                   jse_osal_post_delayed_action(pingInterval, ping_timer_cb,
                                               client); */
            }

            jse_debug("pingInterval = %d \n", pingInterval);
        }

        item = cJSON_GetObjectItem(root, "pingTimeout");
        if (cJSON_IsNumber(item)) {
            pingTimeout = item->valueint;
            jse_debug("pingTimeout = %d \n", pingTimeout);
        }
    }

    if (argc > 0) {
        for (i = 0; i < argc; i++) {
            jse_debug("args[%d] = %s ", i, args[i]);
        }

        if (strcmp("/device/disconnect", args[0]) == 0) {
            /* close websocket */
            jse_debug("parse %s ", args[0]);
            bone_websocket_send_frame("/device/disconnect_reply", 200,
                                      "success");
#ifdef LINUXOSX /* simulator auto exit(0) */
            jse_osal_shutdown();
#endif
            client->state = BWS_STATE_CLOSING;

        } else if (strcmp("/device/reboot", args[0]) == 0) {
            /* reboot */
            jse_debug("parse %s ", args[0]);
            bone_websocket_send_frame("/device/reboot_reply", 200, "success");
            client->state = BWS_STATE_CLOSING;
            /* delay 200ms */
            jse_osal_delay(200);
            jse_system_reboot();

        } else if (strcmp("/device/wifi", args[0]) == 0) {
/* set WIFI */
#if defined(BE_OS_AOS)
            websocket_call_cli("wifi", args);
#endif
            bone_websocket_send_frame("/device/wifi_reply", 200, "success");

        } else if (strcmp("/device/burnKey", args[0]) == 0) {
            /* set device certificate */
            board_setDeviceInfo(args[1], args[2], args[3]);
            bone_websocket_send_frame("/device/burnKey_reply", 200, "success");

        } else if (strcmp("/device/updateimg", args[0]) == 0) {
            /* update image or firmware via http */
#ifdef LINUXOSX
            char* url = strdup(args[2]);
            upgrade_image_param_t* p_info =
                jse_calloc(1, sizeof(upgrade_image_param_t));
            p_info->url       = url;
            p_info->file_size = atoi(args[4]);
            p_info->type      = atoi(args[1]);
            strncpy(p_info->md5, args[3], 32 - 1);

            jse_task_schedule_call(simulator_upgrade, p_info);
#else
            /* websocket_call_cli("updateimg", args); */
#endif
        } else if (strcmp("/device/updateapp", args[0]) == 0) {
            /* update app.bin via http */
            char* url = strdup(args[1]);

            jse_task_schedule_call((bone_engine_call_t)apppack_upgrade, url);

        } else if (strcmp("/device/getVersion", args[0]) == 0) {
            /* get device version */
            websocket_getVersion();
        } else if (strcmp("/device/bshell", args[0]) == 0) {
            if (args[1]) {
                char* str = jse_calloc(1, strlen(args[1]) + 1);
                strcpy(str, args[1]);
                jse_task_schedule_call(websocket_eval_js, str);
                bone_websocket_send_frame("/device/bshell_reply", 200,
                                          "Success");
            } else {
                bone_websocket_send_frame("/device/bshell_reply", 201,
                                          "Bad Request");
            }
        }
    }

    cJSON_Delete(root);

    return 0;
}

static void bone_websocket_process_frame(bone_websocket_client_t* client,
                                         bone_websocket_frame_t* frame_ptr)
{
    jse_debug("frame.type:0x%x", frame_ptr->type);

    if (frame_ptr->type == BWS_CLOSE) {
        jse_debug("closing");
        client->state = BWS_STATE_CLOSING;
    } else if (frame_ptr->type == BWS_PING) {
        jse_debug("ping frame");
    } else if (frame_ptr->type == BWS_PONG) {
        jse_debug("pong frame");
    } else if (frame_ptr->type == BWS_TXT_FRAME) {
        /* jse_debug("text frame"); */

        jse_debug("call bone_websocket_on_command msg=%s", frame_ptr->payload);

        bone_websocket_on_command(client, frame_ptr->payload,
                                  frame_ptr->payload_len);

        if (client->on_message)
            client->on_message(client, frame_ptr->payload,
                               frame_ptr->payload_len);
    }

    if (frame_ptr->payload) jse_free(frame_ptr->payload);
}

static void on_read(int fd, void* arg)
{
    int data_len = 0;
    unsigned char* ptr;

    bone_websocket_client_t* client = (bone_websocket_client_t*)arg;
    if (client == NULL) return;
    if (client->magic != BONE_WEBSOCKET_MAGIC) return;

    unsigned char* data = client->frame_buf + client->tmp_length;
    /* rest length of frame_buf */
    size_t recv_buf_length = client->frame_buf_length - client->tmp_length;

    do {
        data_len = recv(fd, (void*)data, recv_buf_length, 0);
    } while (data_len < 0 && (errno == EINTR));

    if (data_len > 0) {
        jse_task_cancel_timer(client->timeout_tsk_handle);
        client->timeout_tsk_handle = NULL;

        /* jse_osal_cancel_delayed_action(pingTimeout, bone_websocket_timeout,
                                         client); */

        data[data_len] = 0;
        jse_debug("\nparse websocket data, client = %p", client);
        jse_debug("------recv: %d bytes------", data_len);
        if (data_len >= 3)
            jse_debug("0x%x 0x%x 0x%x \n", data[0], data[1], data[2]);

        if (client && client->state == BWS_STATE_CONNECTING) {
            if (strstr(data, "HTTP/1.1 101 Switching Protocols")) {
                unsigned char accept_key[BWS_SHAKE_KEY_LEN] = {0};

                int succ =
                    headers_get(data, "Sec-WebSocket-Accept: ", accept_key,
                                sizeof(accept_key));

                jse_debug("accept_key = %s", accept_key);

                if (succ == 0 && match_shake_key(client->shake_key,
                                                 (char*)accept_key) == 0) {
                    client->state = BWS_STATE_OPEN;

                    bone_websocket_send_cmd(client, "2probe", true);

                    /* parse ping */
                    jse_debug("websocket parse ping action, client=%p", client);
                    client->ping_tsk_handle = jse_task_timer_action(
                        pingInterval, ping_timer_cb, client, JSE_TIMER_ONCE);

                    if (client->on_open) client->on_open(client);

                    client->tmp_length = 0;
                    client->frame_size = 0;
                } else {
                    jse_debug("failed!");
                }
            } else {
                /* store data */
                client->tmp_length = client->tmp_length + data_len;
            }
        } else if (client && client->state == BWS_STATE_OPEN) {
            /* may receive multi frame once tcp handshark */

            /* update frame buffer and length */
            data     = client->frame_buf;
            data_len = client->tmp_length + data_len;

            /* bone_read_buff_t* buf = (bone_read_buff_t*)client->data; */
            bone_websocket_frame_t frame;
            int succ = -1;
            int left;

            memset(&frame, 0, sizeof(frame));

            /* new frame */
            succ = bone_websocket_parse_frame(data, data_len, &frame);
            if (succ == 0) {
                bone_websocket_process_frame(client, &frame);
                /* rest */
                ptr  = data + frame.frame_total_len;
                left = data_len - frame.frame_total_len;

            } else {
                ptr  = data;
                left = data_len;
            }

            /* may include next frame */
            while (succ == 0 && client->state == BWS_STATE_OPEN) {
                succ = bone_websocket_parse_frame(ptr, left, &frame);
                if (succ == 0) {
                    bone_websocket_process_frame(client, &frame);
                    /* rest */
                    ptr  = ptr + frame.frame_total_len;
                    left = left - frame.frame_total_len;
                }
            }

            if (left > 0) {
                client->tmp_length = left;
                memmove(client->frame_buf, ptr, left);
            } else {
                client->tmp_length = 0;
            }

            /* frame size > frame_buf_length? */
            if (succ > 0) {
                if (succ > client->frame_buf_length) {
                    /* reassign memory */
                    jse_debug("frame buf is large \n");
                }
                jse_debug("missing data, %d %d \n", succ, left);
            }
        }
    }

    /* data_len == -1 may call bone_websocket_deinit() multiple times */
    jse_debug("%s %d data_len = %d state = %d \n", __FUNCTION__, __LINE__,
              data_len, client->state);
    if (data_len == -1 || (client->state == BWS_STATE_CLOSING)) {
        client->state = BWS_STATE_CLOSED;
        if (client->on_close) client->on_close(client);
        jse_debug("%s %d data_len = %d state = %d \n", __FUNCTION__, __LINE__,
                  data_len, client->state);
        bone_websocket_deinit(client);
    }
}

static bone_websocket_client_t* gWebsocketClient = NULL;

bone_websocket_client_t* bone_websocket_getInstance()
{
    return gWebsocketClient;
}
int bone_websocket_init(bone_websocket_client_t* client)
{
    memset(client, 0, sizeof(bone_websocket_client_t));
    client->magic            = BONE_WEBSOCKET_MAGIC;
    client->state            = BWS_STATE_CONNECTING;
    client->frame_buf_length = 1024;
    client->frame_buf        = jse_calloc(1, client->frame_buf_length);
    request_shake_key((unsigned char*)client->shake_key,
                      sizeof(client->shake_key));
    client->fd = -1;
    return 0;
}

int bone_websocket_deinit(bone_websocket_client_t* client)
{
    if (client == NULL) return -1;
    gWebsocketClient = NULL;
    /* invoid double free */
    if (client->magic != BONE_WEBSOCKET_MAGIC) return -2;

    jse_debug("call bone_websocket_deinit, 0x%x ", client->magic);

    client->tsk_done = 1;
    client->magic    = 0;

    void* tsk_handle = client->tsk_handle;

    /* must clear */
    jse_debug("stop websocket ping,  client = %p ", client);

    /* jse_osal_cancel_delayed_action(pingInterval, ping_timer_cb, client);
       jse_osal_cancel_delayed_action(pingTimeout, bone_websocket_timeout,
       client); */

    jse_task_cancel_timer(client->ping_tsk_handle);
    client->ping_tsk_handle = NULL;
    jse_task_cancel_timer(client->timeout_tsk_handle);
    client->timeout_tsk_handle = NULL;

    if (client->fd != -1) close(client->fd);
    jse_debug("关闭 websocket, client = %p", client);

    jse_free(client->frame_buf);
    jse_free(client);

    /* stop websocket task */
    jse_debug("tsk_handle=%p", tsk_handle);
    jse_osal_delete_task(tsk_handle);

    return 0;
}

static void websocket_read(void* arg)
{
    bone_websocket_client_t* client = (bone_websocket_client_t*)arg;
    fd_set readfds;
    struct timeval tv;
    int result;
    int sockfd;

    sockfd = client->fd;

    /* int flags;
       flags = fcntl(sockfd, F_GETFL, 0);
       fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); */
    tv.tv_usec = 500000;
    tv.tv_sec  = 0;

    while (!client->tsk_done) {
        FD_ZERO(&readfds);
        FD_CLR(sockfd, &readfds);
        FD_SET(sockfd, &readfds);
        result = select(sockfd + 1, &readfds, 0, 0, &tv);
        if (result > 0 && (!client->tsk_done)) {
            if (FD_ISSET(sockfd, &readfds)) on_read(sockfd, client);
        } else if (result < 0) {
            break;
        } else {
            /* jse_debug("select timeout"); */
        }
    }

    jse_debug("[%s][%d] ~~ Byte ~~ \n", __FUNCTION__, __LINE__);
}

int bone_websocket_connect(bone_websocket_client_t* client)
{
    int succ = 0;
    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family      = AF_INET;
    remote_addr.sin_port        = htons(client->port);
    remote_addr.sin_addr.s_addr = inet_addr(client->ip);

    jse_debug("%s client->ip = %s \n", __FUNCTION__, client->ip);

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        jse_debug("websocket socket error: fd=%d", fd);
        jse_free(client->frame_buf);
        jse_free(client);
        return -1;
    }

    /* set timeout 500ms */
    struct timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 500 * 1000;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                   sizeof(timeout)) < 0) {
        jse_debug("setsockopt failed\n");
        close(fd);
        jse_free(client->frame_buf);
        jse_free(client);
        return -2;
    }

    succ = connect(fd, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if (succ != 0) {
        jse_debug("websocket connect error: fd=%d ret = %d client->ip=%s", fd,
                  succ, client->ip);
        close(fd);
        jse_free(client);
        return -2;
    }

    client->fd = fd;

    char buffer[256] = {0};
    sprintf(buffer, BWS_REQ, client->path, client->ip, client->port,
            client->shake_key, BONE_VERSION);
    int send_len =
        bone_websocket_send_raw(client, (unsigned char*)buffer, strlen(buffer));
    if (send_len < 0) {
        close(fd);
        jse_free(client->frame_buf);
        jse_free(client);
        jse_debug("websocket send error: fd=%d, send_len=%d", fd, send_len);
        return -4;
    }

    /* websocket task */
    client->tsk_done = 0;
    jse_osal_create_task("WebSocketTsk", websocket_read, client, 1024 * 8,
                         WEBSOCKET_TSK_PRIORITY, &client->tsk_handle);

    if (websocketMutex == NULL) websocketMutex = jse_osal_new_mutex();

    jse_debug("bone_websocket_connect 连接成功 ");
    gWebsocketClient = client;

    return succ;
}

int bone_websocket_send_raw(bone_websocket_client_t* client,
                            unsigned char* data, size_t data_len)
{
    int ret;
    if (client->fd >= 0) {
        jse_osal_lock_mutex(websocketMutex, 1000);
        ret = (int)send(client->fd, data, data_len, 0);
        jse_osal_unlock_mutex(websocketMutex);
        return ret;
    } else {
        return -1;
    }
}

/* avoid call jse_debug()/jse_warn()/jse_info()/jse_error() */
int bone_websocket_send_frame(char* topic, int level, char* msg)
{
    /* double check */
    if (gWebsocketClient == NULL) return 0;
    if (gWebsocketClient->state != BWS_STATE_OPEN) return 0;
    if (topic == NULL || msg == NULL) return 0;

    /* pack into socket.io frames,  42['topic','msg1', 'msg2', 'msg3'] */
    cJSON* root = NULL;
    char* payloadBuf;
    int payloadBuf_len;
    int ret;

    root = cJSON_CreateArray();
    if (root == NULL) {
        return 0;
    }
    cJSON_AddItemToArray(root, cJSON_CreateString(topic));
    cJSON_AddItemToArray(root, cJSON_CreateNumber(level));
    cJSON_AddItemToArray(root, cJSON_CreateString(msg));

    payloadBuf_len = 32 + strlen(topic) + strlen(msg);
    payloadBuf     = jse_calloc(1, payloadBuf_len);
    if (payloadBuf == NULL) return 0;

    payloadBuf[0] = '4';
    payloadBuf[1] = '2';
    ret =
        cJSON_PrintPreallocated(root, payloadBuf + 2, payloadBuf_len - 1, true);

    cJSON_Delete(root);

    if (!ret) {
        jse_error("cJSON_PrintPreallocated error\n");
        jse_free(payloadBuf);
        return 0;
    }

    int frame_buf_size = strlen(payloadBuf) + 32;
    uint8_t* frame_buf;

    frame_buf = jse_calloc(1, frame_buf_size);
    if (frame_buf == NULL) {
        jse_error("bone_websocket_send_frame calloc failed \n");
        jse_free(payloadBuf);
        return 0;
    }

    int frame_size =
        bone_websocket_build_frame(payloadBuf, strlen(payloadBuf), false,
                                   BWS_TXT_FRAME, frame_buf, frame_buf_size);

    jse_debug("[%s][%d] topic = %s msg = %s \n", __FUNCTION__, __LINE__, topic,
              msg);

    ret = bone_websocket_send_raw(gWebsocketClient, frame_buf, frame_size);

    jse_free(payloadBuf);
    jse_free(frame_buf);

    return ret;
}

int bone_websocket_build_frame(unsigned char* payload, size_t payload_len,
                               bool is_mask, bone_websocket_frame_type_t type,
                               unsigned char* frame_buf, size_t frame_buf_size)
{
    bone_websocket_frame_t frame;
    memset(&frame, 0, sizeof(bone_websocket_frame_t));
    frame.frame_total_len = frame_buf_size;
    frame.is_mask         = is_mask;
    frame.type            = type;
    frame.frame_data      = frame_buf;
    frame_encode(payload, payload_len, &frame);
    return (int)frame.frame_total_len;
}

int bone_websocket_parse_frame(unsigned char* frame_data, size_t frame_data_len,
                               bone_websocket_frame_t* frame)
{
    int i = 0, count = 0;

    if (frame_data_len < 2) return -1;

#ifdef BWD_PRINT_FRAME
    char frame_4hex[32] = {0};
    if (frame_data_len > 3) {
        sprintf(frame_4hex, "0x%02X %02X %02X %02X ...(%d bytes) ",
                frame_data[0], frame_data[1], frame_data[2], frame_data[3],
                (int)frame_data_len);
    } else if (frame_data_len == 3) {
        sprintf(frame_4hex, "0x%02X %02X %02X", frame_data[0], frame_data[1],
                frame_data[2]);
    } else {
        sprintf(frame_4hex, "0x%02X %02X", frame_data[0], frame_data[1]);
    }
    jse_debug("parse frame: %s\r\n", frame_4hex);
#endif

    frame->type = frame_data[0];

    int mask                  = 0;
    int payload_start         = 2;
    unsigned char mask_key[4] = {0};
    size_t payload_len        = frame_data[1] & 0x7F;

    if ((frame_data[1] & 0x80) == 0x80 && frame_data_len > 4) {
        mask = 1;
    }

    if (frame_data_len >= 3) {
        if (payload_len == 126) {
            payload_len = 0;
            for (i = 0; i < 2; ++i) {
                int len = frame_data[2 + i];
                if (i == 0) {
                    payload_len += len;
                } else {
                    payload_len = (payload_len << 8) + len;
                }
            }

            payload_start += 2;
            if (mask) {
                for (i = 0; i < 4; ++i) mask_key[i] = frame_data[4 + i];
                payload_start += 4;
            }
        } else if (payload_len == 127) {
            payload_len = 0;
            for (i = 0; i < 8; ++i) {
                int len = frame_data[2 + i];
                if (i == 0) {
                    payload_len += len;
                } else {
                    payload_len = (payload_len << 8) + len;
                }
            }
            payload_start += 8;
            if (mask) {
                for (i = 0; i < 4; ++i) mask_key[i] = frame_data[10 + i];
                payload_start += 4;
            }
        } else {
            if (mask) {
                for (i = 0; i < 4; ++i) mask_key[i] = frame_data[2 + i];
                payload_start += 4;
            }
        }

        frame->payload_len     = frame_data_len - payload_start;
        frame->frame_total_len = payload_start + payload_len;

        if (frame_data_len < payload_len) {
            return (int)frame->frame_total_len;
        }

        frame->payload = (unsigned char*)jse_calloc(1, frame->payload_len + 1);
        if (mask) {
            unsigned char temp1, temp2;
            for (i = 0, count = 0; i < frame->payload_len; i++, count++) {
                temp1 = mask_key[count];
                temp2 = frame_data[i + payload_start];
                frame->payload[i] =
                    (char)(((~temp1) & temp2) | (temp1 & (~temp2)));
                if (count >= sizeof(mask_key)) count = 0;
            }
        } else {
            memcpy(frame->payload, frame_data + payload_start,
                   frame->payload_len);
        }
    }

    return 0;
}

#else

int bone_websocket_send_frame(char* topic, int level, char* msg)
{
    return 0;
}
#endif