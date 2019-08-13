/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS
#include "module_adc.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "be_jse_module.h"
#include "be_port_hal.h"
#include "board-mgr/board_mgr.h"

static be_jse_symbol_t *adc_open(void)
{
    int32_t len   = -1;
    char *data    = NULL;
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t adc_handle;
    adc_handle.handle     = 0xFFFFFFFF;
    be_jse_symbol_t *arg0 = NULL;
    adc_dev_t *adc_device = NULL;
    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_string(arg0)) {
        goto out;
    }

    len  = symbol_str_len(arg0);
    data = jse_calloc(1, sizeof(char) * (len + 1));
    if (NULL == data) {
        goto out;
    }
    symbol_to_str(arg0, data, len);

    ret = board_attach_item(MODULE_ADC, data, &adc_handle);
    if (0 != ret) {
        jse_error("board_attach_item fail!\n");
        goto out;
    }

    jse_debug("adc handle:%u\n", adc_handle.handle);
    adc_device = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    result = hal_adc_init(adc_device);
out:

    if (NULL != data) {
        jse_free(data);
        data = NULL;
    }
    symbol_unlock(arg0);
    if (0 != result) {
        board_disattach_item(MODULE_ADC, &adc_handle);
        return new_int_symbol(-1);
    }
    return new_int_symbol(adc_handle.handle);
}

static be_jse_symbol_t *adc_close(void)
{
    int8_t ret = -1;
    item_handle_t adc_handle;
    be_jse_symbol_t *arg0 = NULL;
    adc_dev_t *adc_device = NULL;
    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }

    adc_handle.handle = get_symbol_value_int(arg0);

    adc_device = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_adc_finalize(adc_device);
    board_disattach_item(MODULE_ADC, &adc_handle);

out:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *adc_read(void)
{
    int32_t ret       = -1;
    int32_t adc_value = -1;
    item_handle_t adc_handle;
    be_jse_symbol_t *arg0 = NULL;
    adc_dev_t *adc_device = NULL;
    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    adc_handle.handle = get_symbol_value_int(arg0);
    adc_device        = board_get_node_by_handle(MODULE_ADC, &adc_handle);
    if (NULL == adc_device) {
        jse_error("board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = hal_adc_value_get(adc_device, (void *)&adc_value, 0);
out:
    symbol_unlock(arg0);
    return new_int_symbol(adc_value > 0 ? adc_value : -1);
}

static be_jse_symbol_t *adc_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                             be_jse_symbol_t *var,
                                             const char *name)
{
    if (0 == strcmp(name, "open")) {
        return adc_open();
    }
    if (0 == strcmp(name, "read")) {
        return adc_read();
    }
    if (0 == strcmp(name, "close")) {
        return adc_close();
    }
    return (BE_JSE_FUNC_UNHANDLED);
}

void module_adc_register(void)
{
    be_jse_module_load("ADC", adc_module_handle_cb);
}
