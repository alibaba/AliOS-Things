/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "module_uart.h"

#include "hal/soc/soc.h"
#include <aos/aos.h>

// 最多支持两个串口

#define UART_COUNT 2
static uart_dev_t uart[UART_COUNT];
static bool uartStatus[UART_COUNT] = {-1,-1};   // 0: opened

#ifdef USE_UART


be_jse_symbol_t* uart_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
{
    int ret=-1;
    be_jse_symbol_t* arg0;
    be_jse_symbol_t* arg1;
    be_jse_symbol_t* child;

    int port=0;
    int value;
    uart_dev_t *pUart=NULL;
    char data[128];

    if (get_symbol_node_id(var) == execInfo->executor->uartClass) {

        if (strcmp(name,"open")==0) {

            be_jse_handle_function(0, &arg0, 0, 0, 0);

            if( symbol_is_object(arg0)) {
                child = lookup_named_child_symbol(get_symbol_node_id(arg0), "port", false);
                if( child ) {
                    port = get_symbol_value_int(child);
                    symbol_unlock(child);
                    if( port < 0 || port >=UART_COUNT ) {
                        LOGE("MODULE_UART","parameter error, port = %d", port);
                        return new_int_symbol((be_jse_int_t)ret);
                    }
                } else {

                    LOGE("MODULE_UART","invalid parameter, need port !");
                    return new_int_symbol(ret);
                }

                if(uartStatus[port]==0) {
                    LOGE("MODULE_UART","uart port=%d, alread open !", port);
                    return new_int_symbol(ret);
                }

                pUart = &uart[port];
                pUart->port                = port;
                // 默认参数
                pUart->config.baud_rate    = 115200;
                pUart->config.data_width   = DATA_WIDTH_8BIT;
                pUart->config.parity       = NO_PARITY;
                pUart->config.stop_bits    = STOP_BITS_1;
                pUart->config.flow_control = FLOW_CONTROL_DISABLED;

                child = lookup_named_child_symbol(get_symbol_node_id(arg0), "baudRate", false);
                if( child ) {
                    pUart->config.baud_rate = get_symbol_value_int(child);
                    symbol_unlock(child);
                }

                child = lookup_named_child_symbol(get_symbol_node_id(arg0), "dataBits", false);
                if( child ) {
                    value = get_symbol_value_int(child);
                    symbol_unlock(child);
                    switch(value) {
                        case 5:
                            pUart->config.data_width = DATA_WIDTH_5BIT;
                            break;
                        case 6:
                            pUart->config.data_width = DATA_WIDTH_6BIT;
                            break;
                        case 7:
                            pUart->config.data_width = DATA_WIDTH_7BIT;
                            break;
                        case 8:
                            pUart->config.data_width = DATA_WIDTH_8BIT;
                            break;
                        case 9:
                            pUart->config.data_width = DATA_WIDTH_9BIT;
                            break;
                        default:
                            break;
                    }
                }
            }

            symbol_unlock(arg0);

            LOGW("MODULE_UART","pUart->port=%d", pUart->port);
            LOGW("MODULE_UART","pUart->config.baud_rate=%d", pUart->config.baud_rate);
            LOGW("MODULE_UART","pUart->config.data_width=%d", pUart->config.data_width);

            uartStatus[port] = hal_uart_init(pUart);

            return new_int_symbol(uartStatus[port]);
        }

        if(strcmp(name,"close")==0) {

            be_jse_handle_function(0, &arg0, &arg1, 0, 0);

            port = get_symbol_int(arg0);
            symbol_to_str(arg1, data, 127);

            symbol_unlock(arg0);
            symbol_unlock(arg1);

            if( port < 0 || port >=UART_COUNT ) {
                LOGE("MODULE_UART","parameter error, port = %d", port);
                return new_int_symbol(ret);
            }

            if(uartStatus[port]!=0) {
                LOGE("MODULE_UART","uart port=%d, need open !", port);
                return new_int_symbol(ret);
            }

            pUart = &uart[port];
            ret = hal_uart_finalize(pUart);
            return new_int_symbol(ret);
        }

        if(strcmp(name,"write")==0) {

            be_jse_handle_function(0, &arg0, &arg1, 0, 0);

            port = get_symbol_int(arg0);
            symbol_to_str(arg1, data, 127);

            symbol_unlock(arg0);
            symbol_unlock(arg1);

            if( port < 0 || port >=UART_COUNT ) {
                LOGE("MODULE_UART","parameter error, port = %d", port);
                return new_int_symbol(ret);
            }

            if(uartStatus[port]!=0) {
                LOGE("MODULE_UART","uart port=%d, need open !", port);
                return new_int_symbol(ret);
            }

            pUart = &uart[port];

            value = strlen(data);

            LOGW("MODULE_UART", "uart port=%d, write=%s, len=%d", port, data, value);

            ret = hal_uart_send(pUart, data, value, AOS_WAIT_FOREVER);
            return new_int_symbol(ret);
        }

        if(strcmp(name,"read")==0) {

            be_jse_handle_function(0, &arg0, &arg1, 0, 0);

            port = get_symbol_int(arg0);
            value = get_symbol_int(arg1);    // timeout

            symbol_unlock(arg0);
            symbol_unlock(arg1);

            if( port < 0 || port >=UART_COUNT ) {
                LOGE("MODULE_UART","parameter error, port = %d", port);
                return new_int_symbol(ret);
            }

            if(uartStatus[port]!=0) {
                LOGE("MODULE_UART","uart port=%d, need open !", port);
                return new_int_symbol(ret);
            }

            //LOGW("MODULE_UART", "timeout=%d", value);
            if( value < 0 )
                value = 0;
            if( value > 10*1000)
                value = 10*1000;

            pUart = &uart[port];
            memset(data, 0, sizeof(data));
            ret = hal_uart_recv_II(pUart, data, 127, &value, value);

            //LOGW("MODULE_UART", "uart port=%d, read=%s, value=%d, ret=%d", port, data, value, ret);

            return new_str_symbol(data);
        }

    }
    return BE_JSE_FUNC_UNHANDLED;

}

#endif

