/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "be_jse_module.h"
#include "be_port_hal.h"
#include "board-mgr/board_mgr.h"

static be_jse_symbol_t *dac_open(void) {
    int32_t len   = -1;
    char *data    = NULL;
    int8_t ret    = -1;
    int8_t result = -1;
    item_handle_t dac_handle;
    dac_handle.handle     = 0xFFFFFFFF;
    be_jse_symbol_t *arg0 = NULL;
    dac_dev_t *dac_device = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_string(arg0)) {
        goto out;
    }
    len  = symbol_str_len(arg0);
    data = calloc(1, sizeof(char) * (len + 1));
    if (NULL == data) {
        goto out;
    }
    symbol_to_str(arg0, data, len);
    ret = board_attach_item(MODULE_DAC, data, &dac_handle);
    if (0 != ret) {
        be_error("dac", "board_attach_item fail!\n");
        goto out;
    }
    be_debug("dac", "dac handle:%u\n", dac_handle.handle);
    dac_device = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        be_error("dac", "board_get_node_by_handle fail!\n");
        goto out;
    }
    result = hal_dac_init(dac_device);
    if (0 != result) {
        goto out;
    }
    result = hal_dac_start(dac_device, dac_device->port);
    if (0 != result) {
        be_error("dac", "hal_dac_init fail!\n");
    }
out:

    if (NULL != data) {
        free(data);
        data = NULL;
    }
    symbol_unlock(arg0);
    if (0 != result) {
        board_disattach_item(MODULE_DAC, &dac_handle);
        return new_int_symbol(-1);
    }

    return new_int_symbol(dac_handle.handle);
}

static be_jse_symbol_t *dac_set_vol(void) {
    int8_t ret       = -1;
    int8_t result    = -1;
    uint32_t voltage = 0;
    item_handle_t dac_handle;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    dac_dev_t *dac_device = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    dac_handle.handle = get_symbol_value_int(arg0);
    dac_device        = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        be_error("dac", "board_get_node_by_handle fail!\n");
        goto out;
    }
    if (!arg1 || !symbol_is_int(arg1)) {
        goto out;
    }
    voltage = get_symbol_value_int(arg1);
    result  = hal_dac_set_value(dac_device, dac_device->port, voltage);

out:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(result);
}

static be_jse_symbol_t *dac_get_vol(void) {
    int32_t ret = -1;
    item_handle_t dac_handle;
    be_jse_symbol_t *arg0 = NULL;
    dac_dev_t *dac_device = NULL;
    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    dac_handle.handle = get_symbol_value_int(arg0);
    dac_device        = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        be_error("dac", "board_get_node_by_handle fail!\n");
        goto out;
    }
    ret = (int)hal_dac_get_value(dac_device, dac_device->port);
out:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *dac_close(void) {
    int8_t ret = -1;
    item_handle_t dac_handle;
    be_jse_symbol_t *arg0 = NULL;
    dac_dev_t *dac_device = NULL;
    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0)) {
        goto out;
    }
    dac_handle.handle = get_symbol_value_int(arg0);
    dac_device        = board_get_node_by_handle(MODULE_DAC, &dac_handle);
    if (NULL == dac_device) {
        be_error("dac", "board_get_node_by_handle fail!\n");
        goto out;
    }
    hal_dac_stop(dac_device, dac_device->port);
    ret = hal_dac_finalize(dac_device);
    board_disattach_item(MODULE_DAC, &dac_handle);
out:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *dac_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                             be_jse_symbol_t *var,
                                             const char *name) {
    if (0 == strcmp(name, "open")) {
        return dac_open();
    }
    if (0 == strcmp(name, "setVol")) {
        return dac_set_vol();
    }
    if (0 == strcmp(name, "getVol")) {
        return dac_get_vol();
    }
    if (0 == strcmp(name, "close")) {
        return dac_close();
    }
    return (BE_JSE_FUNC_UNHANDLED);
}

void module_dac_register(void) {
    be_jse_module_load("DAC", dac_module_handle_cb);
}
