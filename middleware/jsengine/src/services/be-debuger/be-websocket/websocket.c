/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <errno.h>

#ifdef BE_OS_AOS
#include <network/network.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef WITH_LWIP
#include <lwip/sockets.h>
#endif

#ifdef LINUXOSX
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

#ifdef USE_FREERTOS
#include <arpa/inet.h>
#include <lwip/netdb.h>
#include <sys/socket.h>
#endif

#include "websocket.h"

#include "mbedtls/base64.h"
#include "mbedtls/sha1.h"

#include "be_port_osal.h"
#include "cJSON.h"
#include "hal/system.h"

#include "app_mgr.h"
#include "be_jse_export.h"
#include "be_jse_task.h"
#include "be_log.h"
#include "be_ssdp.h"
#include "board_info.h"
#include "cli.h"

#if defined(WITH_LWIP) || defined(LINUXOSX) || defined(USE_FREERTOS)

static void* websocketMutex = NULL;

#define BWS_READ_SIZE 1024
#define BWS_LOG_TAG "PAL-WS"
#define BWS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define MODULE_TAG ("websocket")

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

static int websocket_getVersion() {
    char* p_resp_cmd = NULL;
    int err_code     = 200;

    p_resp_cmd = calloc(1, 128);
    if (p_resp_cmd == NULL) {
        snprintf(p_resp_cmd, 128, "getVersion Fail\r\n");
        err_code = 201;
        goto do_exit;
    }

    snprintf(p_resp_cmd, 128, "Version:%s\r\nVersionInfo:%s\r\n", BONE_VERSION,
             VERSION_DESCRIPTION);

do_exit:
    bone_websocket_send_frame("/device/getVersion_reply", err_code, p_resp_cmd);

    free(p_resp_cmd);
    return 0;
}

static void websocket_eval_js(void* arg) {
    bone_engine_start(arg);
    free(arg);
}

static int random_string(unsigned char* buf, int len) {
    srand((int)time(NULL));

    for (int i = 0; i < len; i++) {
        buf[i] = (unsigned char)(rand() % 255 + 1);
    }
    return 0;
}

static int headers_get(const char* headers, const char* key,
                       unsigned char* value, size_t value_len) {
    char* p;
    if ((p = strstr(headers, key)) == NULL) return -1;
    p += strlen(key);

    memset(value, 0, sizeof(value_len));
    sscanf(p, "%s", value);
    return 0;
}

static int respond_shake_key(const char* accept_key, unsigned char* respond_key,
                             size_t respond_key_len) {
    if (accept_key == NULL) return -1;

    char client_key[256] = {0};
    strcat(client_key, accept_key);
    strcat(client_key, BWS_GUID);

    unsigned char decrypt[20];  /* 160 bits */

    /* 计算 decrypt */
#ifdef BE_OS_AOS
    int ctx_size;
    void* ctx_sha1;
    ali_hash_get_ctx_size(SHA1, &ctx_size);
    ctx_sha1 = calloc(1, ctx_size);
    ali_hash_init(SHA1, ctx_sha1);
    ali_hash_update((const uint8_t*)client_key, strlen(client_key), ctx_sha1);
    ali_hash_final(decrypt, ctx_sha1);
    ali_hash_reset(ctx_sha1);
    free(ctx_sha1);
#else
    mbedtls_sha1_context sha1_ctx;
    mbedtls_sha1_init(&sha1_ctx);
    mbedtls_sha1_starts(&sha1_ctx);
    mbedtls_sha1_update(&sha1_ctx, (unsigned char*)client_key,
                        strlen((char*)client_key));
    mbedtls_sha1_finish(&sha1_ctx, decrypt);
    mbedtls_sha1_free(&sha1_ctx);
#endif

    size_t olen = 0;
    return mbedtls_base64_encode(respond_key, respond_key_len, &olen, decrypt,
                                 sizeof(decrypt));
}

static int match_shake_key(const char* my_key, const char* accept_key) {
    unsigned char respond_key[BWS_SHAKE_KEY_LEN] = {0};
    respond_shake_key(my_key, respond_key, sizeof(respond_key));

    if (strcmp((char*)respond_key, accept_key) == 0) {
        return 0;
    }
    return -1;
}

static int request_shake_key(unsigned char* key, int key_len) {
    unsigned char tempKey[16] = {0};
    random_string(tempKey, sizeof(tempKey));

    size_t olen = 0;
    return mbedtls_base64_encode(key, key_len, &olen, tempKey, sizeof(tempKey));
}

static int frame_encode(unsigned char* payload, size_t payload_len,
                        bone_websocket_frame_t* frame) {
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
        for (int i = 1; i >= 0; --i) {
            frame->frame_data[frame_total_len++] |=
                (char)((payload_len >> (8 * i)) & 0xFF);
        }
    } else {
        frame->frame_data[frame_total_len++] |= 0x7F;
        for (int i = 7; i >= 0; --i) {
            frame->frame_data[frame_total_len++] |=
                (char)((payload_len >> (8 * i)) & 0xFF);
        }
    }

    if (frame->is_mask) {
        unsigned char mask_key[4] = {0};
        random_string(mask_key, sizeof(mask_key));
        for (int i = 0; i < sizeof(mask_key); ++i) {
            frame->frame_data[frame_total_len++] = mask_key[i];
        }

        unsigned char temp1, temp2;
        for (int i = 0, count = 0; i < payload_len; i++) {
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
    printf("build frame: %s\r\n", frame_4hex);
#endif
    return 0;
}

/* 超时关闭socket, 接收ping回复 */

static int pingTimeout = 5000;

static void bone_websocket_timeout(void* arg) {
    bone_websocket_client_t* client = (bone_websocket_client_t*)arg;
    if (client == NULL) {
        return;
    }

    be_debug(MODULE_TAG, "websocket pingTimeout (%d) client=%p", pingTimeout,
             client);
    be_debug(MODULE_TAG, "websocket close,  client=%p", client);

    bone_websocket_deinit(client);

    be_debug(MODULE_TAG, "websocket reconnect ...");
    be_debuger_websocket_reconnect();
}

static void ping_timer_cb(void* arg) {
    bone_websocket_client_t* client = (bone_websocket_client_t*)arg;
    if (client == NULL) {
        return;
    }

    be_debug(MODULE_TAG, "websocket 发送 ping packet, client=%p", client);

    int ret;
    unsigned char payload[2]     = "2";
    unsigned char frame_buf[100] = {0};
    int frame_size =
        bone_websocket_build_frame(payload, strlen((char*)payload), true,
                                   BWS_TXT_FRAME, frame_buf, sizeof(frame_buf));

    ret = bone_websocket_send_raw(client, frame_buf, frame_size);

    be_debug(MODULE_TAG, "bone_websocket_send_raw return %d, frame_size = %d",
             ret, frame_size);

    if (ret > 0) {
        /* be_osal_post_delayed_action(pingInterval, ping_timer_cb, client);
           be_osal_post_delayed_action(pingTimeout, bone_websocket_timeout,
                                       client); */

        be_jse_task_cancel_timer(client->ping_tsk_handle);
        client->ping_tsk_handle = be_jse_task_timer_action(
            pingInterval, ping_timer_cb, client, JSE_TIMER_ONCE);

        be_jse_task_cancel_timer(client->timeout_tsk_handle);
        client->timeout_tsk_handle = be_jse_task_timer_action(
            pingTimeout, bone_websocket_timeout, client, JSE_TIMER_ONCE);

    } else {
        /* 对方socket关闭
           关闭websocket */
        bone_websocket_deinit(client);
    }
}

static int bone_websocket_send_cmd(bone_websocket_client_t* client, char* cmd,
                                   bool is_mask) {
    if (client == NULL) {
        return -1;
    }

    int ret;
    unsigned char payload[32];
    unsigned char frame_buf[64] = {0};

    strcpy(payload, cmd);
    int frame_size =
        bone_websocket_build_frame(payload, strlen((char*)payload), is_mask,
                                   BWS_TXT_FRAME, frame_buf, sizeof(frame_buf));

    ret = bone_websocket_send_raw(client, frame_buf, frame_size);

    be_debug(MODULE_TAG, "bone_websocket_send_raw return %d, frame_size = %d",
             ret, frame_size);

    /* for(int i; i<frame_size; i++){
           printf("%0x ", frame_buf[i]);
       }
       printf("\n"); */

    return ret;
}

/* 处理 be-cli/webide 发来的命令
   原始命令  42["/device/disconnect","arg0","arg1","arg2","arg3","arg4"]
   42["/ide/console",1,"2",{"3":"4","5":"rockzhou"}]
   0{"sid":"RyTmnKrgBDtX37bsAAAA","upgrades":[],"pingInterval":25000,"pingTimeout":5000} */
static int bone_websocket_on_command(bone_websocket_client_t* client,
                                     unsigned char* msg, size_t msg_len) {
    char* strCmd;
    char* args[32];
    int argc = 0;
    int i;
    int count;
    strCmd = msg;

    /* 4 message：实际发送的消息
       Packet类型 CONNECT 0, DISCONNECT 1, EVENT 2, ACK 3, ERROR 4, BINARY_EVENT
       5, BINARY_ACK 6 */

    printf("%d, msg_len = %d, strCmd = %s \n", __LINE__, msg_len, strCmd);

    if (msg_len >= 2) {
        if (msg[0] == '4') {
            switch (msg[1]) {
                case '0':  /* CONNECT */
                    break;
                case '1':  /* DISCONNECT */
                    client->state = BWS_STATE_CLOSING;
                    break;
                case '2':  /* EVENT */
                    break;
                case '3':  /* ACK */
                    break;
                case '4':  /* ERROR */
                    break;
            }
        } else if (msg[0] == '3') {
            if (strcmp("3probe", msg) == 0) {
                /* Upgrade帧 */
                bone_websocket_send_cmd(client, "5", true);
            }
        } else if (msg[0] == '6') {
            /* noop帧 */
        }
    }

    for (i = 0; i < msg_len; i++) {
        if (msg[i] >= '0' && msg[i] <= '9') {
            strCmd = msg + i + 1;
        } else {
            break;
        }
    }

    printf("%d, strCmd = %s\n", __LINE__, strCmd);

    if (strCmd == NULL) return 0;

    cJSON* root = NULL;
    cJSON* item;

    memset(args, 0, sizeof(args));

    root = cJSON_Parse(strCmd);

    printf("%d, root = %p\n", __LINE__, root);

    if (root == NULL) return 0;

    if (cJSON_IsArray(root)) {
        /* 是数组 */
        count = cJSON_GetArraySize(root);
        printf("count = %d\n", count);
        for (i = 0; i < count; i++) {
            item = cJSON_GetArrayItem(root, i);
            printf("item type is %d\n", item->type);

            switch (item->type) {
                case cJSON_String:
                    args[argc] = item->valuestring;
                    argc++;
                    break;
                case cJSON_Number:
                    break;
            }

            /* printf("%s\n", item->valuestring); */
        }
    } else if (cJSON_IsObject(root)) {
        /* 是对象 */
        item = cJSON_GetObjectItem(root, "pingInterval");
        if (cJSON_IsNumber(item)) {
            int Interval = item->valueint;

            if (Interval != pingInterval) {
                /* 更新ping周期 */
                pingInterval = Interval;
                be_jse_task_cancel_timer(client->ping_tsk_handle);
                client->ping_tsk_handle = be_jse_task_timer_action(
                    pingInterval, ping_timer_cb, client, JSE_TIMER_ONCE);

                /* be_osal_cancel_delayed_action(pingInterval, ping_timer_cb,
                                                 client);
                   pingInterval = Interval;
                   be_osal_post_delayed_action(pingInterval, ping_timer_cb,
                                               client); */
            }

            printf("pingInterval = %d \n", pingInterval);
        }

        item = cJSON_GetObjectItem(root, "pingTimeout");
        if (cJSON_IsNumber(item)) {
            pingTimeout = item->valueint;
            printf("pingTimeout = %d \n", pingTimeout);
        }
    }

    if (argc > 0) {
        for (i = 0; i < argc; i++) {
            be_debug(MODULE_TAG, "args[%d] = %s ", i, args[i]);
        }

        if (strcmp("/device/disconnect", args[0]) == 0) {
            /* 断开连接 websocket */
            be_debug(MODULE_TAG, "处理 %s ", args[0]);
            bone_websocket_send_frame("/device/disconnect_reply", 200,
                                      "success");
#ifdef LINUXOSX  /* simulator auto exit(0) */
            be_osal_shutdown();
#endif
            client->state = BWS_STATE_CLOSING;

        } else if (strcmp("/device/reboot", args[0]) == 0) {
            /* 重启 */
            be_debug(MODULE_TAG, "处理 %s ", args[0]);
            bone_websocket_send_frame("/device/reboot_reply", 200, "success");
            client->state = BWS_STATE_CLOSING;
            /* 延时 200ms */
            be_osal_delay(200);
            hal_system_reboot();

        } else if (strcmp("/device/wifi", args[0]) == 0) {
/* 设置 WIFI */
#if defined(BE_OS_AOS) || defined(USE_FREERTOS)
            websocket_call_cli("wifi", args);
#endif
            bone_websocket_send_frame("/device/wifi_reply", 200, "success");

        } else if (strcmp("/device/burnKey", args[0]) == 0) {
            /* 设置 设备上云三要素 */
            board_setDeviceInfo(args[1], args[2], args[3]);
            bone_websocket_send_frame("/device/burnKey_reply", 200, "success");

        } else if (strcmp("/device/updateimg", args[0]) == 0) {
            /* http更新固件或应用分区镜像 */
#ifdef LINUXOSX
            char* url = strdup(args[2]);
            upgrade_image_param_t* p_info =
                calloc(1, sizeof(upgrade_image_param_t));
            p_info->url       = url;
            p_info->file_size = atoi(args[4]);
            p_info->type      = atoi(args[1]);
            strncpy(p_info->md5, args[3], 32);

            be_jse_task_schedule_call(simulator_upgrade, p_info);
#else
            /* websocket_call_cli("updateimg", args); */
#endif
        } else if (strcmp("/device/updateapp", args[0]) == 0) {
            /* http更新应用, app.bin */
            char* url = strdup(args[1]);

            be_jse_task_schedule_call((bone_engine_call_t)apppack_upgrade, url);

        } else if (strcmp("/device/getVersion", args[0]) == 0) {
            /* 获取设备版本信息 */
            websocket_getVersion();
        } else if (strcmp("/device/bshell", args[0]) == 0) {
            if (args[1]) {
                char* str = calloc(1, strlen(args[1]) + 1);
                strcpy(str, args[1]);
                be_jse_task_schedule_call(websocket_eval_js, str);
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
                                         bone_websocket_frame_t* frame_ptr) {
    be_debug(MODULE_TAG, "frame.type:0x%x", frame_ptr->type);

    if (frame_ptr->type == BWS_CLOSE) {
        be_debug(MODULE_TAG, "closing");
        client->state = BWS_STATE_CLOSING;
    } else if (frame_ptr->type == BWS_PING) {
        be_debug(MODULE_TAG, "ping frame");
    } else if (frame_ptr->type == BWS_PONG) {
        be_debug(MODULE_TAG, "pong frame");
    } else if (frame_ptr->type == BWS_TXT_FRAME) {
        /* be_debug(MODULE_TAG, "text frame"); */

        be_debug(MODULE_TAG, "调用 bone_websocket_on_command 处理 msg=%s",
                 frame_ptr->payload);

        bone_websocket_on_command(client, frame_ptr->payload,
                                  frame_ptr->payload_len);

        if (client->on_message)
            client->on_message(client, frame_ptr->payload,
                               frame_ptr->payload_len);
    }

    if (frame_ptr->payload) free(frame_ptr->payload);
}

static void on_read(int fd, void* arg) {
    int data_len = 0;
    unsigned char* ptr;

    bone_websocket_client_t* client = (bone_websocket_client_t*)arg;
    if (client == NULL) return;
    if (client->magic != BONE_WEBSOCKET_MAGIC) return;

    unsigned char* data = client->frame_buf + client->tmp_length;
    /* 剩余frame_buf长度 */
    size_t recv_buf_length = client->frame_buf_length - client->tmp_length;

    do {
        data_len = recv(fd, (void*)data, recv_buf_length, 0);
    } while (data_len < 0 && (errno == EINTR));

    if (data_len > 0) {
        be_jse_task_cancel_timer(client->timeout_tsk_handle);
        client->timeout_tsk_handle = NULL;

        /* */ be_osal_cancel_delayed_action(pingTimeout, bone_websocket_timeout,
                                         client); */

        data[data_len] = 0;
        be_debug(MODULE_TAG, "\n分析websocket 数据, client = %p", client);
        be_debug(MODULE_TAG, "------recv: %d bytes------", data_len);
        if (data_len >= 3)
            printf("0x%x 0x%x 0x%x \n", data[0], data[1], data[2]);

        /* 不处理握手信息，仅处理正式的command信息 */

        if (client && client->state == BWS_STATE_CONNECTING) {
            if (strstr(data, "HTTP/1.1 101 Switching Protocols")) {
                unsigned char accept_key[BWS_SHAKE_KEY_LEN] = {0};

                int succ =
                    headers_get(data, "Sec-WebSocket-Accept: ", accept_key,
                                sizeof(accept_key));

                be_debug(MODULE_TAG, "accept_key = %s", accept_key);

                if (succ == 0 && match_shake_key(client->shake_key,
                                                 (char*)accept_key) == 0) {
                    client->state = BWS_STATE_OPEN;

                    bone_websocket_send_cmd(client, "2probe", true);

                    /* 处理定期ping */
                    be_debug(MODULE_TAG,
                             "websocket 处理 ping action, client=%p", client);
                    client->ping_tsk_handle = be_jse_task_timer_action(
                        pingInterval, ping_timer_cb, client, JSE_TIMER_ONCE);

                    if (client->on_open) client->on_open(client);

                    client->tmp_length = 0;
                    client->frame_size = 0;
                } else {
                    be_debug(MODULE_TAG, "出错 !!!");
                }
            } else {
                /* 数据不够，暂存起来 */
                client->tmp_length = client->tmp_length + data_len;
            }
        } else if (client && client->state == BWS_STATE_OPEN) {
            /* 可以会存在tcp一次接收多个frame的情况 !!! */

            /* 更新frame buffer 数据及长度 */
            data     = client->frame_buf;
            data_len = client->tmp_length + data_len;

            /* bone_read_buff_t* buf = (bone_read_buff_t*)client->data; */
            bone_websocket_frame_t frame;
            int succ = -1;
            int left;

            memset(&frame, 0, sizeof(frame));

            /* new frame 开始 */
            succ = bone_websocket_parse_frame(data, data_len, &frame);
            if (succ == 0) {
                bone_websocket_process_frame(client, &frame);
                /* 剩余 */
                ptr  = data + frame.frame_total_len;
                left = data_len - frame.frame_total_len;

            } else {
                ptr  = data;
                left = data_len;
            }

            /* 可能还包含下一个frame */
            while (succ == 0 && client->state == BWS_STATE_OPEN) {
                succ = bone_websocket_parse_frame(ptr, left, &frame);
                if (succ == 0) {
                    bone_websocket_process_frame(client, &frame);
                    /* 剩余 */
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

            /* 判断最大frame size 是否超出 frame_buf_length */
            if (succ > 0) {
                if (succ > client->frame_buf_length) {
                    /* 重新分配内存 */
                    printf("frame buf is large \n");
                }
                printf("缺少数据, %d %d \n", succ, left);
            }
        }
    }

    /* data_len == -1 会产生多次调用bone_websocket_deinit */
    printf("%s %d data_len = %d state = %d \n", __FUNCTION__, __LINE__,
           data_len, client->state);
    if (data_len == -1 || (client->state == BWS_STATE_CLOSING)) {
        client->state = BWS_STATE_CLOSED;
        if (client->on_close) client->on_close(client);
        printf("%s %d data_len = %d state = %d \n", __FUNCTION__, __LINE__,
               data_len, client->state);
        bone_websocket_deinit(client);
    }
}

static bone_websocket_client_t* gWebsocketClient = NULL;

bone_websocket_client_t* bone_websocket_getInstance() {
    return gWebsocketClient;
}
int bone_websocket_init(bone_websocket_client_t* client) {
    memset(client, 0, sizeof(bone_websocket_client_t));
    client->magic            = BONE_WEBSOCKET_MAGIC;
    client->state            = BWS_STATE_CONNECTING;
    client->frame_buf_length = 1024;
    client->frame_buf        = calloc(1, client->frame_buf_length);
    request_shake_key((unsigned char*)client->shake_key,
                      sizeof(client->shake_key));
    client->fd = -1;
    return 0;
}

int bone_websocket_deinit(bone_websocket_client_t* client) {
    if (client == NULL) return -1;
    gWebsocketClient = NULL;
    /* 避免 double free */
    if (client->magic != BONE_WEBSOCKET_MAGIC) return -2;

    be_debug(MODULE_TAG, "call bone_websocket_deinit, 0x%x ", client->magic);

    client->tsk_done = 1;
    client->magic    = 0;

    void* tsk_handle = client->tsk_handle;

    /* 必须清除 */
    be_debug(MODULE_TAG, "终止 websocket ping,  client = %p ", client);

    /* be_osal_cancel_delayed_action(pingInterval, ping_timer_cb, client);
       be_osal_cancel_delayed_action(pingTimeout, bone_websocket_timeout,
       client); */

    be_jse_task_cancel_timer(client->ping_tsk_handle);
    client->ping_tsk_handle = NULL;
    be_jse_task_cancel_timer(client->timeout_tsk_handle);
    client->timeout_tsk_handle = NULL;

    if (client->fd != -1) close(client->fd);
    be_debug(MODULE_TAG, "关闭 websocket, client = %p", client);

    free(client->frame_buf);
    free(client);

    /* 结束websocket task, 必须放在最后 */
    be_debug(MODULE_TAG, "tsk_handle=%p", tsk_handle);
    be_osal_delete_task(tsk_handle);

    return 0;
}

static void websocket_read(void* arg) {
    bone_websocket_client_t* client = (bone_websocket_client_t*)arg;
    fd_set readfds;
    struct timeval tv;
    int result;
    int sockfd;

    sockfd = client->fd;

    /* */ int flags;
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
            /* be_debug(TAG, "select timeout"); */
        }
    }

    printf("[%s][%d] ~~ Byte ~~ \n", __FUNCTION__, __LINE__);
}

int bone_websocket_connect(bone_websocket_client_t* client) {
    int succ = 0;
    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family      = AF_INET;
    remote_addr.sin_port        = htons(client->port);
    remote_addr.sin_addr.s_addr = inet_addr(client->ip);

    printf("%s client->ip = %s \n", __FUNCTION__, client->ip);

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        be_debug(MODULE_TAG, "websocket socket error: fd=%d", fd);
        free(client->frame_buf);
        free(client);
        return -1;
    }

    /* 设置超时 500ms */
    struct timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 500 * 1000;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                   sizeof(timeout)) < 0) {
        be_debug(MODULE_TAG, "setsockopt failed\n");
        close(fd);
        free(client->frame_buf);
        free(client);
        return -2;
    }

    succ = connect(fd, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if (succ != 0) {
        be_debug(MODULE_TAG,
                 "websocket connect error: fd=%d ret = %d client->ip=%s", fd,
                 succ, client->ip);
        close(fd);
        free(client);
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
        free(client->frame_buf);
        free(client);
        be_debug(MODULE_TAG, "websocket send error: fd=%d, send_len=%d", fd,
                 send_len);
        return -4;
    }

    /* websocket task */
    client->tsk_done = 0;
    be_osal_create_task("WebSocketTsk", websocket_read, client, 1024 * 8,
                        WEBSOCKET_TSK_PRIORITY, &client->tsk_handle);

    if (websocketMutex == NULL) websocketMutex = be_osal_new_mutex();

    be_debug(MODULE_TAG, "bone_websocket_connect 连接成功 ");
    gWebsocketClient = client;

    return succ;
}

int bone_websocket_send_raw(bone_websocket_client_t* client,
                            unsigned char* data, size_t data_len) {
    int ret;
    if (client->fd >= 0) {
        be_osal_lock_mutex(websocketMutex, 1000);
        ret = (int)send(client->fd, data, data_len, 0);
        be_osal_unlock_mutex(websocketMutex);
        return ret;
    } else {
        return -1;
    }
}

/*
 *
 * 在这个函数中不能调用be_debug/be_warn/be_info/be_error,
 * 避免出现有可能存在的循环调用
 *
 */
int bone_websocket_send_frame(char* topic, int level, char* msg) {
    /* double check */
    if (gWebsocketClient == NULL) return 0;
    if (gWebsocketClient->state != BWS_STATE_OPEN) return 0;
    if (topic == NULL || msg == NULL) return 0;

    /* 打包成 socket.io 的frame,  42['topic','msg1', 'msg2', 'msg3'] */
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
    payloadBuf     = calloc(1, payloadBuf_len);
    if (payloadBuf == NULL) return 0;

    payloadBuf[0] = '4';
    payloadBuf[1] = '2';
    ret =
        cJSON_PrintPreallocated(root, payloadBuf + 2, payloadBuf_len - 1, true);

    cJSON_Delete(root);

    if (!ret) {
        printf("cJSON_PrintPreallocated 出错\n");
        free(payloadBuf);
        return 0;
    }

    int frame_buf_size = strlen(payloadBuf) + 32;
    uint8_t* frame_buf;

    frame_buf = calloc(1, frame_buf_size);
    if (frame_buf == NULL) {
        printf("bone_websocket_send_frame calloc failed \n");
        free(payloadBuf);
        return 0;
    }

    int frame_size =
        bone_websocket_build_frame(payloadBuf, strlen(payloadBuf), false,
                                   BWS_TXT_FRAME, frame_buf, frame_buf_size);

    printf("[%s][%d] topic = %s msg = %s \n", __FUNCTION__, __LINE__, topic,
           msg);

    ret = bone_websocket_send_raw(gWebsocketClient, frame_buf, frame_size);

    free(payloadBuf);
    free(frame_buf);

    return ret;
}

int bone_websocket_build_frame(unsigned char* payload, size_t payload_len,
                               bool is_mask, bone_websocket_frame_type_t type,
                               unsigned char* frame_buf,
                               size_t frame_buf_size) {
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
                               bone_websocket_frame_t* frame) {
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
    printf("parse frame: %s\r\n", frame_4hex);
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
            for (int i = 0; i < 2; ++i) {
                int len = frame_data[2 + i];
                if (i == 0) {
                    payload_len += len;
                } else {
                    payload_len = (payload_len << 8) + len;
                }
            }

            payload_start += 2;
            if (mask) {
                for (int i = 0; i < 4; ++i) mask_key[i] = frame_data[4 + i];
                payload_start += 4;
            }
        } else if (payload_len == 127) {
            payload_len = 0;
            for (int i = 0; i < 8; ++i) {
                int len = frame_data[2 + i];
                if (i == 0) {
                    payload_len += len;
                } else {
                    payload_len = (payload_len << 8) + len;
                }
            }
            payload_start += 8;
            if (mask) {
                for (int i = 0; i < 4; ++i) mask_key[i] = frame_data[10 + i];
                payload_start += 4;
            }
        } else {
            if (mask) {
                for (int i = 0; i < 4; ++i) mask_key[i] = frame_data[2 + i];
                payload_start += 4;
            }
        }

        frame->payload_len     = frame_data_len - payload_start;
        frame->frame_total_len = payload_start + payload_len;

        if (frame_data_len < payload_len) {
            return (int)frame->frame_total_len;
        }

        frame->payload = (unsigned char*)calloc(1, frame->payload_len + 1);
        if (mask) {
            unsigned char temp1, temp2;
            for (int i = 0, count = 0; i < frame->payload_len; i++, count++) {
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

int bone_websocket_send_frame(char* topic, int level, char* msg) { return 0; }
#endif