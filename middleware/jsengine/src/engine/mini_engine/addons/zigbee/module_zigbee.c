/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "module_zigbee.h"
#include <be_osal.h>
#include <jse/be_jse_module.h>
#include <mbedtls/sha1.h>
#include <zigbee/bone_zigbee.h>

#define JS_ZIGBEE_DELAYED_INTERNAL 50

static bone_zigbee_t zigbee;
static be_jse_symbol_t* on_message_func = NULL;
static be_jse_symbol_t* on_connect_func = NULL;
static be_jse_symbol_t* on_error_func   = NULL;

typedef struct {
    uint16_t addr;
    unsigned char msg[256];
    size_t msg_size;
    uint16_t type;
} schedule_msg_t;

typedef enum {
    ON_CONNECT = 0X81,
    ON_MESSAGE = 0X82,
} bone_zigbee_package_type_ext_t;

static void call_action(void* arg)
{
    schedule_msg_t* schedule_msg = (schedule_msg_t*)arg;
    if (schedule_msg != NULL) {
        if (schedule_msg->type == ON_CONNECT) {
            char productKey[64]   = {0};
            char deviceName[64]   = {0};
            char deviceSecret[64] = {0};
            sscanf((char*)schedule_msg->msg, "%[^|]|%[^|]|%s", productKey,
                   deviceName, deviceSecret);

            if (strlen(productKey) == 0 || strlen(deviceName) == 0 ||
                strlen(deviceSecret) == 0) {
                jse_error("productKey or deviceName or deviceSecret not exist");
                return;
            }

            unsigned char content[256] = {0};
            sprintf((char*)content, "clientId%sdeviceName%sproductKey%s",
                    deviceName, deviceName, productKey);

            mbedtls_sha1_context sha1_ctx;
            unsigned char k_ipad[64] = {0};
            unsigned char k_opad[64] = {0};
            unsigned char out[20];

            memcpy(k_ipad, deviceSecret, strlen(deviceSecret));
            memcpy(k_opad, deviceSecret, strlen(deviceSecret));

            for (int i = 0; i < sizeof(k_ipad); i++) {
                k_ipad[i] ^= 0x36;
                k_opad[i] ^= 0x5c;
            }

            mbedtls_sha1_init(&sha1_ctx);
            mbedtls_sha1_starts(&sha1_ctx);
            mbedtls_sha1_update(&sha1_ctx, k_ipad, sizeof(k_ipad));
            mbedtls_sha1_update(&sha1_ctx, content, strlen((char*)content));
            mbedtls_sha1_finish(&sha1_ctx, out);

            mbedtls_sha1_init(&sha1_ctx);
            mbedtls_sha1_starts(&sha1_ctx);
            mbedtls_sha1_update(&sha1_ctx, k_opad, sizeof(k_opad));
            mbedtls_sha1_update(&sha1_ctx, out, sizeof(out));
            mbedtls_sha1_finish(&sha1_ctx, out);
            mbedtls_sha1_free(&sha1_ctx);

            char sign[41] = {0};
            for (int i = 0; i < sizeof(out); ++i) {
                unsigned char byte[2] = {0};
                byte[0]               = out[i] / 16;
                byte[1]               = out[i] % 16;

                for (int j = 0; j < 2; ++j) {
                    if (byte[j] >= 0 && byte[j] <= 9)
                        sign[2 * i + j] = '0' + byte[j];
                    else
                        sign[2 * i + j] = 'a' + byte[j] - 10;
                }
            }

            char json_str[256] = {0};
            sprintf(json_str,
                    "{\"productKey\": \"%s\", \"deviceName\": \"%s\", "
                    "\"deviceSecret\": \"%s\", \"sign\": \"%s\"}",
                    productKey, deviceName, deviceSecret, sign);

            be_jse_symbol_t* argv[2];
            char addr_str[12] = {0};
            sprintf(addr_str, "%d", schedule_msg->addr);
            argv[0] = new_str_symbol(addr_str);
            argv[1] = new_json_symbol(json_str, strlen(json_str));
            /* symbol_relock(argv[0]);
               symbol_relock(argv[1]); */
            be_jse_execute_func(jsengine_get_executor(), on_connect_func, 2,
                                argv);
            /* symbol_unlock(argv[0]);
               symbol_unlock(argv[1]); */
        } else if (schedule_msg->type == ON_MESSAGE) {
            be_jse_symbol_t* argv[2];
            char addr_str[12] = {0};
            sprintf(addr_str, "%d", schedule_msg->addr);
            argv[0] = new_str_symbol(addr_str);
            argv[1] = new_json_symbol((char*)schedule_msg->msg,
                                      schedule_msg->msg_size);
            /* symbol_relock(argv[0]);
               symbol_relock(argv[1]); */
            be_jse_execute_func(jsengine_get_executor(), on_message_func, 2,
                                argv);
            /* symbol_unlock(argv[0]);
               symbol_unlock(argv[1]); */
        } else if (schedule_msg->type == BZB_NOTIFY) {
            bone_zigbee_notify(&zigbee);
        } else if (schedule_msg->type == BZB_SEARCH) {
            bone_zigbee_search(&zigbee);
        }
        jse_free(schedule_msg);
        schedule_msg = NULL;
    }
}

static void on_message(bone_zigbee_t* zigbee, uint16_t addr, unsigned char* msg,
                       size_t msg_size)
{
    jse_debug("on_message addr: %d, msg: %.*s (%d bytes)", addr,
             msg_size, msg, msg_size);

    if (on_message_func) {
        schedule_msg_t* schedule_msg =
            (schedule_msg_t*)jse_calloc(1, sizeof(schedule_msg_t));
        schedule_msg->type = ON_MESSAGE;
        schedule_msg->addr = addr;
        memcpy(schedule_msg->msg, msg, msg_size);
        schedule_msg->msg_size = msg_size;
        be_osal_schedule_call(call_action, schedule_msg);
    }
}

static void on_connect(bone_zigbee_t* zigbee, uint16_t addr, unsigned char* msg,
                       size_t msg_size)
{
    jse_debug("on_connect addr: %d, msg: %.*s (%d bytes)", addr,
             msg_size, msg, msg_size);

    if (on_connect_func) {
        schedule_msg_t* schedule_msg =
            (schedule_msg_t*)jse_calloc(1, sizeof(schedule_msg_t));
        schedule_msg->type = ON_CONNECT;
        schedule_msg->addr = addr;
        memcpy(schedule_msg->msg, msg, msg_size);
        schedule_msg->msg_size = msg_size;
        be_osal_schedule_call(call_action, schedule_msg);
    }
}

static be_jse_symbol_t* start()
{
    be_jse_int_t ret                      = -1;
    be_jse_symbol_t* arg                  = be_jse_handle_single_arg_function();
    be_jse_symbol_t* port_symbol          = NULL;
    be_jse_symbol_t* service_symbol       = NULL;
    be_jse_symbol_t* uart_port_symbol     = NULL;
    be_jse_symbol_t* uart_baudrate_symbol = NULL;
    be_jse_symbol_t* value_port_symbol    = NULL;
    be_jse_symbol_t* value_service_symbol = NULL;
    be_jse_symbol_t* value_uart_port_symbol     = NULL;
    be_jse_symbol_t* value_uart_baudrate_symbol = NULL;

    if (arg == NULL) goto end;

    port_symbol =
        lookup_named_child_symbol(get_symbol_node_id(arg), "port", false);
    service_symbol =
        lookup_named_child_symbol(get_symbol_node_id(arg), "service", false);
    uart_port_symbol =
        lookup_named_child_symbol(get_symbol_node_id(arg), "uartPort", false);
    uart_baudrate_symbol = lookup_named_child_symbol(get_symbol_node_id(arg),
                                                     "uartBaudrate", false);

    if (port_symbol == NULL || service_symbol == NULL ||
        uart_port_symbol == NULL || uart_baudrate_symbol == NULL) {
        ret = -2;
        goto end;
    }

    value_port_symbol          = get_symbol_value(port_symbol);
    value_service_symbol       = get_symbol_value(service_symbol);
    value_uart_port_symbol     = get_symbol_value(uart_port_symbol);
    value_uart_baudrate_symbol = get_symbol_value(uart_baudrate_symbol);

    if (value_port_symbol == NULL || value_service_symbol == NULL ||
        value_uart_port_symbol == NULL || value_uart_baudrate_symbol == NULL) {
        ret = -3;
        goto end;
    }

    zigbee.uart_dev.port = (uint8_t)get_symbol_int(value_uart_port_symbol);
    zigbee.uart_dev.config.baud_rate =
        (uint32_t)get_symbol_int(value_uart_baudrate_symbol);
    zigbee.uart_dev.config.data_width   = DATA_WIDTH_8BIT;
    zigbee.uart_dev.config.parity       = NO_PARITY;
    zigbee.uart_dev.config.stop_bits    = STOP_BITS_1;
    zigbee.uart_dev.config.flow_control = FLOW_CONTROL_DISABLED;
    zigbee.on_message                   = on_message;
    zigbee.on_connect                   = on_connect;
    zigbee.port = (uint8_t)get_symbol_int(value_port_symbol);
    symbol_to_str(value_service_symbol, zigbee.nt,
                  symbol_str_len(value_service_symbol));
    ret = bone_zigbee_start(&zigbee);
end:
    symbol_unlock(arg);
    symbol_unlock(port_symbol);
    symbol_unlock(service_symbol);
    symbol_unlock(uart_port_symbol);
    symbol_unlock(uart_baudrate_symbol);
    symbol_unlock(value_port_symbol);
    symbol_unlock(value_service_symbol);
    symbol_unlock(value_uart_port_symbol);
    symbol_unlock(value_uart_baudrate_symbol);
    return new_int_symbol(ret);
}

static be_jse_symbol_t* on()
{
    be_jse_int_t ret      = -1;
    be_jse_symbol_t* arg0 = NULL;
    be_jse_symbol_t* arg1 = NULL;
    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (arg0 == NULL || arg1 == NULL || !symbol_is_string(arg0) ||
        !symbol_is_function(arg1)) {
        ret = -1;
        symbol_unlock(arg1);
        goto end;
    }

    char name[32] = {0};
    symbol_to_str(arg0, name, symbol_str_len(arg0));

    /* need to free last on_xxx_func if call on func multiple times */
    if (strcmp(name, "message") == 0) {
        symbol_unlock(on_message_func);
        on_message_func = arg1;
    } else if (strcmp(name, "connect") == 0) {
        symbol_unlock(on_connect_func);
        on_connect_func = arg1;
    } else if (strcmp(name, "error") == 0) {
        symbol_unlock(on_error_func);
        on_error_func = arg1;
    }
end:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t* notify()
{
    be_jse_is_none_arg_function();
    schedule_msg_t* schedule_msg =
        (schedule_msg_t*)jse_calloc(1, sizeof(schedule_msg_t));
    schedule_msg->type = BZB_NOTIFY;
    be_osal_schedule_call(call_action, schedule_msg);
    return new_int_symbol(0);
}

static be_jse_symbol_t* search()
{
    be_jse_is_none_arg_function();
    schedule_msg_t* schedule_msg =
        (schedule_msg_t*)jse_calloc(1, sizeof(schedule_msg_t));
    schedule_msg->type = BZB_SEARCH;
    be_osal_schedule_call(call_action, schedule_msg);
    return new_int_symbol(0);
}

static be_jse_symbol_t* send()
{
    be_jse_int_t ret      = -1;
    be_jse_symbol_t* arg0 = NULL;
    be_jse_symbol_t* arg1 = NULL;
    be_jse_symbol_t* arg2 = NULL;
    be_jse_handle_function(0, &arg0, &arg1, &arg2, NULL);

    if (arg0 == NULL || arg1 == NULL || arg2 == NULL ||
        symbol_str_len(arg1) == 0 || !symbol_is_int(arg2)) {
        ret = -2;
        goto end;
    }

    size_t msg_size = symbol_str_len(arg1);
    if (msg_size > BONE_ZIGBEE_BUFFER_SIZE) {
        ret = -3;
        goto end;
    }

    char add_str[12] = {0};
    symbol_to_str(arg0, add_str, symbol_str_len(arg0));

    char msg[BONE_ZIGBEE_BUFFER_SIZE] = {0};
    symbol_to_str(arg1, msg, msg_size);
    ret = bone_zigbee_send(&zigbee, atoi(add_str), (unsigned char*)msg,
                           msg_size, (int)get_symbol_int(arg2));
end:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    return new_int_symbol(ret);
}

static be_jse_symbol_t* module_handle_cb(be_jse_vm_ctx_t* execInfo,
                                         be_jse_symbol_t* var, const char* name)
{
    if (strcmp(name, "VERSION") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_str_symbol("0.0.1");
    }

    if (strcmp(name, "BZB_NOTIFY") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol(BZB_NOTIFY);
    }

    if (strcmp(name, "BZB_SEARCH") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol(BZB_SEARCH);
    }

    if (strcmp(name, "BZB_SEARCH_REPLY") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol(BZB_SEARCH_REPLY);
    }

    if (strcmp(name, "BZB_CONNECT") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol(BZB_CONNECT);
    }

    if (strcmp(name, "BZB_CONNECT_REPLY") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol(BZB_CONNECT_REPLY);
    }

    if (strcmp(name, "BZB_MESSAGE") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol(BZB_MESSAGE);
    }

    if (strcmp(name, "start") == 0) return start();

    if (strcmp(name, "on") == 0) return on();

    if (strcmp(name, "notify") == 0) return notify();

    if (strcmp(name, "search") == 0) return search();

    if (strcmp(name, "send") == 0) return send();

    return BE_JSE_FUNC_UNHANDLED;
}

void module_zigbee_load(void)
{
    memset(&zigbee, 0, sizeof(zigbee));
    be_jse_module_load("ZIGBEE", module_handle_cb);
}
