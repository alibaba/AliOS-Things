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

#include "audio_error.h"
#include "esp_action_def.h"

#include "partition_action.h"

esp_err_t partition_find_action(void *instance, action_arg_t *arg, action_result_t *result)
{
    partition_find_args_t *find_arg = (partition_find_args_t *)arg->data;
    result->data = (void *)esp_partition_find_first(find_arg->type, find_arg->subtype, find_arg->label);
    result->err = result->data != NULL ? ESP_OK : ESP_FAIL;

    return result->err;
}

esp_err_t partition_read_action(void *instance, action_arg_t *arg, action_result_t *result)
{
    partition_read_args_t *read_arg = (partition_read_args_t *)arg->data;
    result->err = esp_partition_read(read_arg->partition, read_arg->src_offset, read_arg->dst, read_arg->size);
    return result->err;
}

esp_err_t partition_write_action(void *instance, action_arg_t *arg, action_result_t *result)
{
    partition_write_args_t *write_arg = (partition_write_args_t *)arg->data;
    result->err = esp_partition_write(write_arg->partition, write_arg->dst_offset, write_arg->src, write_arg->size);
    return result->err;
}
