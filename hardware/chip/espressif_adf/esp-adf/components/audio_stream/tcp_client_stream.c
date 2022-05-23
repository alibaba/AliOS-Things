/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "lwip/sockets.h"
#include "esp_transport_tcp.h"
#include "esp_log.h"
#include "esp_err.h"
#include "audio_mem.h"
#include "tcp_client_stream.h"

static const char *TAG = "TCP_STREAM";
#define CONNECT_TIMEOUT_MS        100

typedef struct tcp_stream {
    esp_transport_handle_t        t;
    audio_stream_type_t           type;
    int                           sock;
    int                           port;
    char                          *host;
    bool                          is_open;
    int                           timeout_ms;
    tcp_stream_event_handle_cb    hook;
    void                          *ctx;
} tcp_stream_t;

static int _get_socket_error_code_reason(const char *str, int sockfd)
{
    uint32_t optlen = sizeof(int);
    int result;
    int err;

    err = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &result, &optlen);
    if (err == -1) {
        ESP_LOGE(TAG, "%s, getsockopt failed", str);
        return -1;
    }
    if (result != 0) {
        ESP_LOGW(TAG, "%s error, error code: %d, reason: %s", str, err, strerror(result));
    }
    return result;
}

static esp_err_t _dispatch_event(audio_element_handle_t el, tcp_stream_t *tcp, void *data, int len, tcp_stream_status_t state)
{
    if (el && tcp && tcp->hook) {
        tcp_stream_event_msg_t msg = { 0 };
        msg.data = data;
        msg.data_len = len;
        msg.sock_fd = tcp->t;
        msg.source = el;
        return tcp->hook(&msg, state, tcp->ctx);
    }
    return ESP_FAIL;
}

static esp_err_t _tcp_open(audio_element_handle_t self)
{
    AUDIO_NULL_CHECK(TAG, self, return ESP_FAIL);

    tcp_stream_t *tcp = (tcp_stream_t *)audio_element_getdata(self);
    if (tcp->is_open) {
        ESP_LOGE(TAG, "Already opened");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Host is %s, port is %d\n", tcp->host, tcp->port);
    esp_transport_handle_t t = esp_transport_tcp_init();
    AUDIO_NULL_CHECK(TAG, t, return ESP_FAIL);
    tcp->sock = esp_transport_connect(t, tcp->host, tcp->port, CONNECT_TIMEOUT_MS);
    if (tcp->sock < 0) {
        _get_socket_error_code_reason(__func__,  tcp->sock);
        esp_transport_destroy(t);
        return ESP_FAIL;
    }
    tcp->is_open = true;
    tcp->t = t;

    _dispatch_event(self, tcp, NULL, 0, TCP_STREAM_STATE_CONNECTED);

    return ESP_OK;
}

static esp_err_t _tcp_read(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    tcp_stream_t *tcp = (tcp_stream_t *)audio_element_getdata(self);
    int rlen = esp_transport_read(tcp->t, buffer, len, tcp->timeout_ms);
    if (rlen < 0) {
        _get_socket_error_code_reason(__func__, tcp->sock);
        return ESP_FAIL;
    } else if (rlen == 0) {
        ESP_LOGI(TAG, "Get end of the file");
    } else {
        audio_element_update_byte_pos(self, rlen);
    }
    ESP_LOGD(TAG, "read len=%d, rlen=%d", len, rlen);
    return rlen;
}

static esp_err_t _tcp_write(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    tcp_stream_t *tcp = (tcp_stream_t *)audio_element_getdata(self);
    int wlen = esp_transport_write(tcp->t, buffer, len, tcp->timeout_ms);
    if (wlen < 0) {
        _get_socket_error_code_reason(__func__, tcp->sock);
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "write len=%d, rlen=%d", len, wlen);
    return wlen;
}

static esp_err_t _tcp_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    int r_size = audio_element_input(self, in_buffer, in_len);
    int w_size = 0;
    if (r_size > 0) {
        w_size = audio_element_output(self, in_buffer, r_size);
        if (w_size > 0) {
            audio_element_update_byte_pos(self, r_size);
        }
    } else {
        w_size = r_size;
    }
    return w_size;
}

static esp_err_t _tcp_close(audio_element_handle_t self)
{
    AUDIO_NULL_CHECK(TAG, self, return ESP_FAIL);

    tcp_stream_t *tcp = (tcp_stream_t *)audio_element_getdata(self);
    AUDIO_NULL_CHECK(TAG, tcp, return ESP_FAIL);
    if (!tcp->is_open) {
        ESP_LOGE(TAG, "Already closed");
        return ESP_FAIL;
    }
    if (-1 == esp_transport_close(tcp->t)) {
        ESP_LOGE(TAG, "TCP stream close failed");
        return ESP_FAIL;
    }
    tcp->is_open = false;
    if (AEL_STATE_PAUSED != audio_element_get_state(self)) {
        audio_element_set_byte_pos(self, 0);
    }
    return ESP_OK;
}

static esp_err_t _tcp_destroy(audio_element_handle_t self)
{
    AUDIO_NULL_CHECK(TAG, self, return ESP_FAIL);

    tcp_stream_t *tcp = (tcp_stream_t *)audio_element_getdata(self);
    AUDIO_NULL_CHECK(TAG, tcp, return ESP_FAIL);
    if (tcp->t) {
        esp_transport_destroy(tcp->t);
        tcp->t = NULL;
    }
    audio_free(tcp);
    return ESP_OK;
}

audio_element_handle_t tcp_stream_init(tcp_stream_cfg_t *config)
{
    AUDIO_NULL_CHECK(TAG, config, return NULL);

    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    audio_element_handle_t el;
    cfg.open = _tcp_open;
    cfg.close = _tcp_close;
    cfg.process = _tcp_process;
    cfg.destroy = _tcp_destroy;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.stack_in_ext = config->ext_stack;
    cfg.tag = "tcp_client";
    if (cfg.buffer_len == 0) {
        cfg.buffer_len = TCP_STREAM_BUF_SIZE;
    }

    tcp_stream_t *tcp = audio_calloc(1, sizeof(tcp_stream_t));
    AUDIO_MEM_CHECK(TAG, tcp, return NULL);

    tcp->type = config->type;
    tcp->port = config->port;
    tcp->host = config->host;
    tcp->timeout_ms = config->timeout_ms;
    if (config->event_handler) {
        tcp->hook = config->event_handler;
        if (config->event_ctx) {
            tcp->ctx = config->event_ctx;
        }
    }

    if (config->type == AUDIO_STREAM_WRITER) {
        cfg.write = _tcp_write;
    } else {
        cfg.read = _tcp_read;
    }

    el = audio_element_init(&cfg);
    AUDIO_MEM_CHECK(TAG, el, goto _tcp_init_exit);
    audio_element_setdata(el, tcp);

    return el;
_tcp_init_exit:
    audio_free(tcp);
    return NULL;
}
