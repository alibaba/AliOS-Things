/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#include "http_stream.h"

#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "audio_element.h"
#include "audio_mem.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "hls_playlist.h"

static const char *TAG = "HTTP_STREAM";
#define MAX_PLAYLIST_LINE_SIZE  (512)
#define HTTP_STREAM_BUFFER_SIZE (2048)
#define HTTP_MAX_CONNECT_TIMES  (5)

typedef struct http_stream {
    audio_stream_type_t type;
    bool is_open;
    esp_http_client_handle_t client;
    http_stream_event_handle_t hook;
    audio_stream_type_t stream_type;
    void *user_data;
    bool enable_playlist_parser;
    bool auto_connect_next_track; /* connect next track without open/close */
    bool is_variant_playlist;
    bool is_playlist_resolved;
    playlist_t *variant_playlist; /* contains more playlists */
    playlist_t *playlist;         /* media playlist */
    int _errno;
    int connect_times;
} http_stream_t;

static esp_err_t http_stream_auto_connect_next_track(audio_element_handle_t el);

static esp_codec_type_t get_audio_type(const char *content_type)
{
    if (strcasecmp(content_type, "mp3") == 0 || strcasecmp(content_type, "audio/mp3") == 0 ||
        strcasecmp(content_type, "audio/mpeg") == 0 || strcasecmp(content_type, "binary/octet-stream") == 0 ||
        strcasecmp(content_type, "application/octet-stream") == 0) {
        return ESP_CODEC_TYPE_MP3;
    }
    if (strcasecmp(content_type, "audio/aac") == 0 || strcasecmp(content_type, "audio/x-aac") == 0 ||
        strcasecmp(content_type, "audio/mp4") == 0 || strcasecmp(content_type, "audio/aacp") == 0 ||
        strcasecmp(content_type, "video/MP2T") == 0) {
        return ESP_CODEC_TYPE_AAC;
    }
    if (strcasecmp(content_type, "audio/wav") == 0) {
        return ESP_CODEC_TYPE_WAV;
    }
    if (strcasecmp(content_type, "audio/opus") == 0) {
        return ESP_CODEC_TYPE_OPUS;
    }
    if (strcasecmp(content_type, "application/vnd.apple.mpegurl") == 0 ||
        strcasecmp(content_type, "vnd.apple.mpegURL") == 0) {
        return ESP_AUDIO_TYPE_M3U8;
    }
    if (strncasecmp(content_type, "audio/x-scpls", strlen("audio/x-scpls")) == 0) {
        return ESP_AUDIO_TYPE_PLS;
    }
    return ESP_CODEC_TYPE_UNKNOW;
}

static esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    audio_element_handle_t el = (audio_element_handle_t)evt->user_data;
    if (evt->event_id != HTTP_EVENT_ON_HEADER) {
        return ESP_OK;
    }

    if (strcasecmp(evt->header_key, "Content-Type") == 0) {
        ESP_LOGD(TAG, "%s = %s", evt->header_key, evt->header_value);
        audio_element_set_codec_fmt(el, get_audio_type(evt->header_value));
    }

    return ESP_OK;
}

static int dispatch_hook(audio_element_handle_t self, http_stream_event_id_t type, void *buffer, int buffer_len)
{
    http_stream_t *http_stream = (http_stream_t *)audio_element_getdata(self);

    http_stream_event_msg_t msg;
    msg.event_id = type;
    msg.http_client = http_stream->client;
    msg.user_data = http_stream->user_data;
    msg.buffer = buffer;
    msg.buffer_len = buffer_len;
    msg.el = self;
    if (http_stream->hook) {
        return http_stream->hook(&msg);
    }
    return ESP_OK;
}

static bool _is_playlist(audio_element_info_t *info, const char *uri)
{
    if (info->codec_fmt == ESP_AUDIO_TYPE_M3U8 || info->codec_fmt == ESP_AUDIO_TYPE_PLS) {
        return true;
    }
    char *dot = strrchr(uri, '.');
    if (dot && (strcasecmp(dot, ".m3u") == 0 || strcasecmp(dot, ".m3u8") == 0)) {
        return true;
    }
    return false;
}

static bool _get_line_in_buffer(http_stream_t *http, char **out)
{
    *out = NULL;
    char c;
    if (http->playlist->remain > 0) {
        bool is_end_of_line = false;
        *out = http->playlist->data + http->playlist->index;
        int idx = http->playlist->index;

        while ((c = http->playlist->data[idx])) {
            if (c == '\r' || c == '\n') {
                http->playlist->data[idx] = 0;
                is_end_of_line = true;
            } else if (is_end_of_line) {
                http->playlist->remain -= idx - http->playlist->index;
                http->playlist->index = idx;
                return true;
            }
            idx++;
        }
        if (http->playlist->total_read >= http->playlist->content_length) {
            http->playlist->remain = 0;
            return true;  // This is the last remaining line
        }
    }
    return false;
}

static char *_client_read_line(http_stream_t *http)
{
    int need_read = MAX_PLAYLIST_LINE_SIZE;
    int rlen;
    char *line;

    if (_get_line_in_buffer(http, &line)) {
        return line;
    }

    if (http->playlist->total_read >= http->playlist->content_length) {
        return NULL;
    }

    need_read -= http->playlist->remain;
    if (need_read > 0) {
        if (http->playlist->remain > 0) {
            memmove(http->playlist->data, http->playlist->data + http->playlist->index, http->playlist->remain);
            http->playlist->index = 0;
        }
        rlen = esp_http_client_read(http->client, http->playlist->data + http->playlist->remain, need_read);
        if (rlen > 0) {
            http->playlist->remain += rlen;
            http->playlist->total_read += rlen;
            http->playlist->data[http->playlist->remain] = '\0';
            if (_get_line_in_buffer(http, &line)) {
                return line;
            }
        } else {
            ESP_LOGD(TAG, "Finish reading data, rlen:%d", rlen);
            line = NULL;
        }
    }
    return line;
}

static esp_err_t _resolve_playlist(audio_element_handle_t self, const char *uri)
{
    audio_element_info_t info;
    http_stream_t *http = (http_stream_t *)audio_element_getdata(self);
    audio_element_getinfo(self, &info);

    if (http->is_playlist_resolved && http->is_variant_playlist) {
        // We do not support more than 2 levels of redirection in m3u.
        return ESP_ERR_INVALID_STATE;
    }
    if (http->is_playlist_resolved) {
        /**
         * The one we resolved is variant playlist
         * We need to move this playlist to variant_playlist and parse this one.
         */
        /* If there already is a variant data, free it */
        hls_playlist_clear(http->variant_playlist);
        http->is_variant_playlist = true;
        http->is_playlist_resolved = false;
        playlist_t *tmp = http->variant_playlist;
        http->variant_playlist = http->playlist;
        http->playlist = tmp;
    }

    http->playlist->content_length = info.total_bytes;
    http->playlist->remain = 0;
    http->playlist->index = 0;
    http->playlist->total_read = 0;
    if (http->playlist->host_uri) {
        audio_free(http->playlist->host_uri);
    }
    http->playlist->host_uri = audio_strdup(uri);

    char *line = NULL;
    bool valid_playlist = false;
    bool is_playlist_uri = false;

    if (info.codec_fmt == ESP_AUDIO_TYPE_PLS) {
        /* pls playlist */
        while ((line = _client_read_line(http))) {
            ESP_LOGD(TAG, "Playlist line = %s", line);
            if (!strncmp(line, "File", sizeof("File") - 1)) {  // this line contains url
                int i = 4;
                while (line[i++] != '=')
                    ;  // Skip till '='
                hls_playlist_insert(http->playlist, line + i);
            } else {
                /* Ignore all other lines */
            }
        }
        return ESP_OK;
    }

    /* M3U8 playlist */
    http->playlist->is_incomplete = true;

#define ENDLIST_TAG "#EXT-X-ENDLIST"
#define VARIANT_TAG "#EXT-X-STREAM-INF"

    while ((line = _client_read_line(http))) {
        ESP_LOGD(TAG, "Playlist line = %s", line);
        if (!valid_playlist && strcmp(line, "#EXTM3U") == 0) {
            valid_playlist = true;
            continue;
        }
        if (strstr(line, "http") == (void *)line) {
            hls_playlist_insert(http->playlist, line);
            valid_playlist = true;
            continue;
        }
        if (!valid_playlist) {
            break;
        }
        if (!is_playlist_uri && strstr(line, "#EXTINF") == (void *)line) {
            is_playlist_uri = true;
            continue;
        } else if (!is_playlist_uri && strstr(line, VARIANT_TAG) == (void *)line) {
            /**
             * Non-standard attribute.
             * There are multiple variants of audios. We do not support this for now.
             */
            is_playlist_uri = true;
            continue;
        } else if (strstr(line, ENDLIST_TAG) == (void *)line) {
            /* Got the ENDLIST_TAG, mark our playlist as complete and break! */
            http->playlist->is_incomplete = false;
            break;
        } else if (strncmp(line, "#", 1) == 0) {
            /**
             * Some other playlist field we don't support.
             * Simply treat this as a comment and continue to find next line.
             */
            continue;
        }
        if (!is_playlist_uri) {
            continue;
        }
        is_playlist_uri = false;

        hls_playlist_insert(http->playlist, line);
    }

    if (http->playlist->is_incomplete) {
        ESP_LOGI(TAG, "Live stream URI. Need to be fetched again!");
    }

    return valid_playlist ? ESP_OK : ESP_FAIL;
}

static char *_playlist_get_next_track(audio_element_handle_t self)
{
    http_stream_t *http = (http_stream_t *)audio_element_getdata(self);
    if (http->enable_playlist_parser && http->is_playlist_resolved) {
        return hls_playlist_get_next_track(http->playlist);
    }
    return NULL;
}

static esp_err_t _http_open(audio_element_handle_t self)
{
    http_stream_t *http = (http_stream_t *)audio_element_getdata(self);
    esp_err_t err;
    char *uri = NULL;
    audio_element_info_t info;
    ESP_LOGD(TAG, "_http_open");

    if (http->is_open) {
        ESP_LOGE(TAG, "already opened");
        return ESP_FAIL;
    }

    http->_errno = 0;
_stream_open_begin:

    uri = _playlist_get_next_track(self);
    if (uri == NULL) {
        if (http->is_playlist_resolved && http->enable_playlist_parser) {
            if (dispatch_hook(self, HTTP_STREAM_FINISH_PLAYLIST, NULL, 0) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to process user callback");
                return ESP_FAIL;
            }
            goto _stream_open_begin;
        }
        uri = audio_element_get_uri(self);
    }

    if (uri == NULL) {
        ESP_LOGE(TAG, "Error open connection, uri = NULL");
        return ESP_FAIL;
    }
    audio_element_getinfo(self, &info);
    ESP_LOGD(TAG, "URI=%s", uri);
    // if not initialize http client, initial it
    if (http->client == NULL) {
        esp_http_client_config_t http_cfg = {
            .url = uri,
            .event_handler = _http_event_handle,
            .user_data = self,
            .timeout_ms = 30 * 1000,
            .buffer_size = HTTP_STREAM_BUFFER_SIZE,
        };
        http->client = esp_http_client_init(&http_cfg);
        AUDIO_MEM_CHECK(TAG, http->client, return ESP_ERR_NO_MEM);
    } else {
        esp_http_client_set_url(http->client, uri);
    }

    if (info.byte_pos) {
        char rang_header[32];
        snprintf(rang_header, 32, "bytes=%d-", (int)info.byte_pos);
        esp_http_client_set_header(http->client, "Range", rang_header);
    }

    if (dispatch_hook(self, HTTP_STREAM_PRE_REQUEST, NULL, 0) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to process user callback");
        return ESP_FAIL;
    }

    if (http->stream_type == AUDIO_STREAM_WRITER) {
        err = esp_http_client_open(http->client, -1);
        if (err == ESP_OK) {
            http->is_open = true;
        }
        return err;
    }

    char *buffer = NULL;
    int post_len = esp_http_client_get_post_field(http->client, &buffer);
_stream_redirect:
    if ((err = esp_http_client_open(http->client, post_len)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open http stream");
        return err;
    }

    int wrlen = dispatch_hook(self, HTTP_STREAM_ON_REQUEST, buffer, post_len);
    if (wrlen < 0) {
        ESP_LOGE(TAG, "Failed to process user callback");
        return ESP_FAIL;
    }

    if (post_len && buffer && wrlen == 0) {
        if (esp_http_client_write(http->client, buffer, post_len) <= 0) {
            ESP_LOGE(TAG, "Failed to write data to http stream");
            return ESP_FAIL;
        }
        ESP_LOGD(TAG, "len=%d, data=%s", post_len, buffer);
    }

    if (dispatch_hook(self, HTTP_STREAM_POST_REQUEST, NULL, 0) < 0) {
        esp_http_client_close(http->client);
        return ESP_FAIL;
    }
    /*
     * Due to the total byte of content has been changed after seek, set info.total_bytes at beginning only.
     */
    int64_t cur_pos = esp_http_client_fetch_headers(http->client);
    audio_element_getinfo(self, &info);
    if (info.byte_pos <= 0) {
        info.total_bytes = cur_pos;
    }

    ESP_LOGI(TAG, "total_bytes=%d", (int)info.total_bytes);
    int status_code = esp_http_client_get_status_code(http->client);
    if (status_code == 301 || status_code == 302) {
        esp_http_client_set_redirection(http->client);
        goto _stream_redirect;
    }
    if (status_code != 200 && (esp_http_client_get_status_code(http->client) != 206)) {
        ESP_LOGE(TAG, "Invalid HTTP stream, status code = %d", status_code);
        if (http->enable_playlist_parser) {
            hls_playlist_clear(http->playlist);
            http->is_playlist_resolved = false;
            hls_playlist_clear(http->variant_playlist);
            http->is_variant_playlist = false;
        }
        return ESP_FAIL;
    }

    /**
     * `audio_element_setinfo` is risky affair.
     * It overwrites URI pointer as well. Pay attention to that!
     */
    audio_element_set_total_bytes(self, info.total_bytes);

    if (_is_playlist(&info, uri) == true) {
        /**
         * `goto _stream_open_begin` blocks on http_open until it gets valid URL.
         * Ensure that the stop command is processed
         */
        if (audio_element_is_stopping(self) == true) {
            ESP_LOGW(TAG, "Http_open got stop cmd at opening");
            return ESP_OK;
        }
        if (_resolve_playlist(self, uri) == ESP_OK) {
            http->is_playlist_resolved = true;
            goto _stream_open_begin;
        }
    }

    http->is_open = true;
    audio_element_report_codec_fmt(self);
    return ESP_OK;
}

static esp_err_t _http_close(audio_element_handle_t self)
{
    http_stream_t *http = (http_stream_t *)audio_element_getdata(self);
    ESP_LOGD(TAG, "_http_close");
    while (http->is_open) {
        http->is_open = false;
        if (http->stream_type != AUDIO_STREAM_WRITER) {
            break;
        }
        if (dispatch_hook(self, HTTP_STREAM_POST_REQUEST, NULL, 0) < 0) {
            break;
        }
        esp_http_client_fetch_headers(http->client);

        if (dispatch_hook(self, HTTP_STREAM_FINISH_REQUEST, NULL, 0) < 0) {
            break;
        }
    }
    if (http->enable_playlist_parser) {
        hls_playlist_clear(http->playlist);
        hls_playlist_clear(http->variant_playlist);
        http->is_variant_playlist = false;
        http->is_playlist_resolved = false;
    }
    if (AEL_STATE_PAUSED != audio_element_get_state(self)) {
        audio_element_report_pos(self);
        audio_element_set_byte_pos(self, 0);
    }
    if (http->client) {
        esp_http_client_close(http->client);
        esp_http_client_cleanup(http->client);
        http->client = NULL;
    }
    return ESP_OK;
}

static esp_err_t _http_reconnect(audio_element_handle_t self)
{
    esp_err_t err = ESP_OK;
    audio_element_info_t info = { 0 };
    AUDIO_NULL_CHECK(TAG, self, return ESP_FAIL);
    err |= audio_element_getinfo(self, &info);
    err |= _http_close(self);
    err |= audio_element_set_byte_pos(self, info.byte_pos);
    err |= _http_open(self);
    return err;
}

static int _http_read(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    http_stream_t *http = (http_stream_t *)audio_element_getdata(self);
    audio_element_info_t info;
    audio_element_getinfo(self, &info);
    int wrlen = dispatch_hook(self, HTTP_STREAM_ON_RESPONSE, buffer, len);
    int rlen = wrlen;
    if (rlen == 0) {
        rlen = esp_http_client_read(http->client, buffer, len);
    }
    if (rlen <= 0 && http->auto_connect_next_track) {
        if (http_stream_auto_connect_next_track(self) == ESP_OK) {
            rlen = esp_http_client_read(http->client, buffer, len);
        }
    }
    if (rlen <= 0) {
        http->_errno = esp_http_client_get_errno(http->client);
        ESP_LOGW(TAG, "No more data,errno:%d, total_bytes:%llu, rlen = %d", http->_errno, info.byte_pos, rlen);
        if (http->_errno != 0) {  // Error occuered, reset connection
            ESP_LOGW(TAG, "Got %d errno(%s)", http->_errno, strerror(http->_errno));
            return http->_errno;
        }
        if (http->auto_connect_next_track) {
            if (dispatch_hook(self, HTTP_STREAM_FINISH_PLAYLIST, NULL, 0) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to process user callback");
                return ESP_FAIL;
            }
        } else {
            if (dispatch_hook(self, HTTP_STREAM_FINISH_TRACK, NULL, 0) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to process user callback");
                return ESP_FAIL;
            }
        }
        return ESP_OK;
    } else {
        audio_element_update_byte_pos(self, rlen);
    }
    ESP_LOGD(TAG, "req lengh=%d, read=%d, pos=%d/%d", len, rlen, (int)info.byte_pos, (int)info.total_bytes);
    return rlen;
}

static int _http_write(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    http_stream_t *http = (http_stream_t *)audio_element_getdata(self);
    int wrlen = dispatch_hook(self, HTTP_STREAM_ON_REQUEST, buffer, len);
    if (wrlen < 0) {
        ESP_LOGE(TAG, "Failed to process user callback");
        return ESP_FAIL;
    }
    if (wrlen > 0) {
        return wrlen;
    }

    if ((wrlen = esp_http_client_write(http->client, buffer, len)) <= 0) {
        http->_errno = esp_http_client_get_errno(http->client);
        ESP_LOGE(TAG, "Failed to write data to http stream, wrlen=%d, errno=%d(%s)", wrlen, http->_errno,
                  strerror(http->_errno));
    }
    return wrlen;
}

static int _http_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    int r_size = audio_element_input(self, in_buffer, in_len);
    if (audio_element_is_stopping(self) == true) {
        ESP_LOGW(TAG, "No output due to stopping");
        return AEL_IO_ABORT;
    }
    int w_size = 0;
    if (r_size > 0) {
        http_stream_t *http = (http_stream_t *)audio_element_getdata(self);
        if (http->_errno != 0) {
            esp_err_t ret = ESP_OK;
            if (http->connect_times > HTTP_MAX_CONNECT_TIMES) {
                ESP_LOGE(TAG, "reconnect times more than %d, disconnect http stream", HTTP_MAX_CONNECT_TIMES);
                return ESP_FAIL;
            };
            http->connect_times++;
            ret = _http_reconnect(self);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to reset connection");
                return ret;
            }
            ESP_LOGW(TAG, "reconnect to peer successful");
            return ESP_ERR_INVALID_STATE;
        }
        else {
            http->connect_times = 0;
            w_size = audio_element_output(self, in_buffer, r_size);
            audio_element_multi_output(self, in_buffer, r_size, 0);
        }
    } else {
        w_size = r_size;
    }
    return w_size;
}

static esp_err_t _http_destroy(audio_element_handle_t self)
{
    http_stream_t *http = (http_stream_t *)audio_element_getdata(self);
    if (http->playlist) {
        audio_free(http->playlist->data);
        audio_free(http->playlist);
    }
    if (http->variant_playlist) {
        audio_free(http->variant_playlist->data);
        audio_free(http->variant_playlist);
    }
    audio_free(http);
    return ESP_OK;
}

audio_element_handle_t http_stream_init(http_stream_cfg_t *config)
{
    audio_element_handle_t el;
    http_stream_t *http = audio_calloc(1, sizeof(http_stream_t));

    AUDIO_MEM_CHECK(TAG, http, return NULL);

    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.open = _http_open;
    cfg.close = _http_close;
    cfg.process = _http_process;
    cfg.destroy = _http_destroy;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.stack_in_ext = config->stack_in_ext;
    cfg.out_rb_size = config->out_rb_size;
    cfg.multi_out_rb_num = config->multi_out_num;
    cfg.tag = "http";

    http->type = config->type;
    http->enable_playlist_parser = config->enable_playlist_parser;
    http->auto_connect_next_track = config->auto_connect_next_track;
    http->hook = config->event_handle;
    http->stream_type = config->type;
    http->user_data = config->user_data;

    if (http->enable_playlist_parser) {
        http->playlist = audio_calloc(1, sizeof(playlist_t));
        AUDIO_MEM_CHECK(TAG, http->playlist, {
            audio_free(http);
            return NULL;
        });
        http->playlist->data = audio_calloc(1, MAX_PLAYLIST_LINE_SIZE + 1);
        AUDIO_MEM_CHECK(TAG, http->playlist->data, {
            audio_free(http->playlist);
            audio_free(http);
            return NULL;
        });
        STAILQ_INIT(&http->playlist->tracks);

        http->variant_playlist = audio_calloc(1, sizeof(playlist_t));
        AUDIO_MEM_CHECK(TAG, http->variant_playlist, {
            audio_free(http->playlist);
            audio_free(http);
            return NULL;
        });
        http->variant_playlist->data = audio_calloc(1, MAX_PLAYLIST_LINE_SIZE + 1);
        AUDIO_MEM_CHECK(TAG, http->variant_playlist->data, {
            audio_free(http->playlist);
            audio_free(http->variant_playlist);
            audio_free(http);
            return NULL;
        });
        STAILQ_INIT(&http->variant_playlist->tracks);
    }

    if (config->type == AUDIO_STREAM_READER) {
        cfg.read = _http_read;
    } else if (config->type == AUDIO_STREAM_WRITER) {
        cfg.write = _http_write;
    }

    el = audio_element_init(&cfg);
    AUDIO_MEM_CHECK(TAG, el, {
        audio_free(http->playlist);
        audio_free(http->variant_playlist);
        audio_free(http);
        return NULL;
    });
    audio_element_setdata(el, http);
    return el;
}

esp_err_t http_stream_next_track(audio_element_handle_t el)
{
    http_stream_t *http = (http_stream_t *)audio_element_getdata(el);
    if (!(http->enable_playlist_parser && http->is_playlist_resolved)) {
        /**
         * This is not a playlist!
         * Do not reset states for restart element.
         * Just return.
         */
        ESP_LOGD(TAG, "Direct URI. Stream will be stopped");
        return ESP_OK;
    }
    audio_element_reset_state(el);
    audio_element_set_byte_pos(el, 0);
    audio_element_set_total_bytes(el, 0);
    http->is_open = false;
    return ESP_OK;
}

esp_err_t http_stream_auto_connect_next_track(audio_element_handle_t el)
{
    audio_element_info_t info;
    audio_element_getinfo(el, &info);
    http_stream_t *http = (http_stream_t *)audio_element_getdata(el);
    char *track = _playlist_get_next_track(el);
    if (track) {
        esp_http_client_set_url(http->client, track);
        char *buffer = NULL;
        int post_len = esp_http_client_get_post_field(http->client, &buffer);
    redirection:
        if ((esp_http_client_open(http->client, post_len)) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open http stream");
            return ESP_FAIL;
        }
        if (dispatch_hook(el, HTTP_STREAM_POST_REQUEST, NULL, 0) < 0) {
            esp_http_client_close(http->client);
            return ESP_FAIL;
        }
        info.total_bytes = esp_http_client_fetch_headers(http->client);
        ESP_LOGD(TAG, "total_bytes=%d", (int)info.total_bytes);
        int status_code = esp_http_client_get_status_code(http->client);
        if (status_code == 301 || status_code == 302) {
            esp_http_client_set_redirection(http->client);
            goto redirection;
        }
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t http_stream_fetch_again(audio_element_handle_t el)
{
    http_stream_t *http = (http_stream_t *)audio_element_getdata(el);
    if (!http->playlist->is_incomplete) {
        ESP_LOGI(TAG, "Finished playing.");
        return ESP_ERR_NOT_SUPPORTED;
    } else {
        ESP_LOGI(TAG, "Fetching again...");
        audio_element_set_uri(el, http->playlist->host_uri);
        http->is_playlist_resolved = false;
    }
    return ESP_OK;
}

esp_err_t http_stream_restart(audio_element_handle_t el)
{
    http_stream_t *http = (http_stream_t *)audio_element_getdata(el);
    http->is_playlist_resolved = false;
    return ESP_OK;
}
