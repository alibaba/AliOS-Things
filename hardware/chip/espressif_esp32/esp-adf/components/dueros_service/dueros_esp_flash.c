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

#include "dueros_esp_flash.h"
#include "esp_partition.h"
#include "audio_mem.h"
#include "esp_log.h"

#define MAX_FLASH_ALERT_COUNT   30

// 4k Byte erase size (12 bits), for erase.
#define FLASH_SECTOR_ALIGN_BITS         12
// 4 Byte page size (2 bits), for write.
#define FLASH_PAGE_ALIGN_BITS           8
// 4 byte (2 bits), for read flash data.
#define FLASH_WORD_ALIGN_BITS           2

static const char *TAG              = "DUER_FLASH";

static const duer_flash_config_t s_flash_config = {
    FLASH_SECTOR_ALIGN_BITS,
    FLASH_PAGE_ALIGN_BITS,
    FLASH_WORD_ALIGN_BITS,
};

static const char *s_parti_label_list[] = {
    "alert",
};

static duer_flash_string_context_t s_ctx_list[] = {
    {{NULL, 0, NULL}, MAX_FLASH_ALERT_COUNT, NULL},
};

duer_flash_string_context_t *duer_flash_string_init(duer_flash_module module)
{
    int i = 0;
    const char *label = s_parti_label_list[module];
    duer_flash_string_context_t *rt = &s_ctx_list[module];

    duer_set_flash_config(&s_flash_config);

    if (module >= MAX_FLASH_MODULE) {
        ESP_LOGE(TAG, "Invalid flash module");
        return NULL;
    }

    if (!label) {
        ESP_LOGE(TAG, "Invalid partition label.");
        return NULL;
    }

    rt->ctx.handle = (void *)esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY,
                     label);
    if (!rt->ctx.handle) {
        ESP_LOGE(TAG, "cannot find partition label %s.", label);
        return NULL;
    }

    rt->ctx.len = ((esp_partition_t *)(rt->ctx.handle))->size;

    if (!rt->ele_list) {
        rt->ele_list = (uint32_t *)audio_malloc(sizeof(uint32_t) * rt->max_ele_count);
        for (i = 0; i < rt->max_ele_count; ++i) {
            rt->ele_list[i] = FLASH_INVALID_ADDR;
        }
    }
    return rt;
}

duer_status_t duer_esp_flash_init(const char *label,
                                  duer_flash_context_t *ctx,
                                  duer_flash_config_t *config)
{
    if (!label || !ctx || !config) {
        return DUER_ERR_INVALID_PARAMETER;
    }

    ctx->handle = (void *)esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY,
                  label);
    if (!ctx->handle) {
        ESP_LOGE(TAG, "cannot find partition label %s.", label);
        return DUER_ERR_FAILED;
    }

    ctx->len = ((esp_partition_t *)(ctx->handle))->size;
    memcpy(config, &s_flash_config, sizeof(*config));

    return DUER_OK;
}

int duer_flash_read(
    duer_flash_context_t *ctx,
    unsigned int offset,
    void *buf,
    unsigned int len)
{
    return esp_partition_read((esp_partition_t *)(ctx->handle), offset, buf, len);
}

int duer_flash_write(
    duer_flash_context_t *ctx,
    unsigned int offset,
    void *buf,
    unsigned int len)
{
    return esp_partition_write((esp_partition_t *)(ctx->handle), offset, buf, len);
}

int duer_flash_erase(
    duer_flash_context_t *ctx,
    unsigned int offset,
    unsigned int len)
{
    return esp_partition_erase_range((esp_partition_t *)(ctx->handle), offset, len);
}
