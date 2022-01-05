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

#include "freertos/FreeRTOS.h"
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "jsmn.h"
#include "audio_error.h"

static const char* TAG = "JSON_UTILS";

static bool jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return true;
    }
    return false;
}

char *json_get_token_value(const char *json_string, const char *token_name)
{
    jsmn_parser parser;
    jsmn_init(&parser);
    jsmntok_t t[20];
    int i;

    int r = jsmn_parse(&parser, json_string, strlen(json_string), t, 20);

    if (r < 0) {
        ESP_LOGE(TAG, "Failed to parse JSON: %d", r);
        return NULL;
    }
    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        ESP_LOGE(TAG, "Object expected");
        return NULL;
    }
    for (i = 1; i < r; i++) {
        if (jsoneq(json_string, &t[i], token_name) && i < r) {
            int tok_len = t[i+1].end - t[i+1].start;
            char *tok = calloc(1, tok_len + 1);
            AUDIO_MEM_CHECK(TAG, tok, return NULL);
            memcpy(tok, json_string + t[i+1].start, tok_len);
            return tok;
        }
    }
    return NULL;
}
