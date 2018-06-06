/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
 
#define CONFIG_LOGMACRO_DETAILS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <be_osal.h>
#include "uart.h"
#include "be_jse_module.h"
#include "utils/be_common.h"
#include "board-mgr/board_mgr.h"
#include "be_osal.h"

#define UART_BUFF_SIZE	(512)
#define MAX_UART_PORT	(6)


typedef struct uart_module{
	uart_dev_t * uart;
	uint32_t loop_time;
	uint32_t item_handle;
	be_jse_symbol_t * fun_symbol;
}uart_module_t;

static  uart_module_t * * g_uart_module = NULL;
static inline uart_module_t * * get_handle(void){
	return g_uart_module;
}

static void * uart_get_free(){

	uint32_t i = 0;
	uart_module_t * * module = get_handle();

	if(NULL==module){
		return (NULL);
	}
	for(i=0;i<MAX_UART_PORT;++i){
		if(NULL != module[i]){
			continue;
		}
		return module[i];
	}
	return (NULL);
}

static void * uart_get_module(uint32_t item_handle){

	uint32_t i = 0;
	uart_module_t * * module = get_handle();
	if(NULL==module){
		return (NULL);
	}
	for(i=0;i<MAX_UART_PORT;++i){
		if(NULL == module[i] ||  item_handle != module[i]->item_handle){
			continue;
		}
		return module[i];
	}
	
	return (NULL);
}


static void uart_handle(void * data){
	
	uart_module_t * module = (uart_module_t *)data;
	if(NULL == module){
		return;
	}
	uint8_t raw_buff[UART_BUFF_SIZE] = {0x00};
	uint32_t recvsize = 0;
	int8_t ret = -1;
	uint32_t i = 0;
	be_jse_symbol_t * fun_symbol = (be_jse_symbol_t*)module->fun_symbol;
	
	if(NULL == fun_symbol || !symbol_is_function(fun_symbol)){
		return;
	}
	uart_dev_t *dev = (uart_dev_t*)module->uart;
	
	ret = hal_uart_recv_II(dev, &raw_buff[0], 1,&recvsize,module->loop_time);
	if(recvsize <= 0){
		be_osal_post_delayed_action(module->loop_time,uart_handle,module);
		return;
	}
	ret = hal_uart_recv_II(dev, &raw_buff[1], UART_BUFF_SIZE-1,&recvsize,module->loop_time*2);
	if(recvsize <= 0){
		be_osal_schedule_call(uart_handle,module);
		return;
	}
#if 0  /*todo?*/
	be_jse_symbol_t * arg[2];
	be_jse_symbol_t * arr = new_symbol(BE_SYM_ARRAY);
	for (i=0; i<recvsize;++i) {
		be_jse_symbol_t *val = new_int_symbol(raw_buff[i]);
		be_jse_symbol_t *idx = new_named_symbol(new_int_symbol(i), val);
		symbol_unlock(val);
		add_symbol_node(arr, idx);
		symbol_unlock(idx);
	}
	arg[0] = arr;
	arg[1] = new_int_symbol(recvsize);
	be_jse_execute_func(bone_engine_get_executor(),fun_symbol, 2, arg);
#else
	be_jse_symbol_t * arg[1];
	for (i=0; i<recvsize+1;++i) {
		arg[0] = new_int_symbol(raw_buff[i]);
		be_jse_execute_func(bone_engine_get_executor(),fun_symbol, 1, arg);
	}
#endif
	be_osal_schedule_call(uart_handle,module);
}




static int8_t uart_add_recv(uart_dev_t * uart,uint32_t item_handle,be_jse_symbol_t * fun_symbol){

	int8_t ret = -1;
	uart_module_t * module = uart_get_free();
	
	module = calloc(1,sizeof(uart_module_t));
	if(NULL == module){
		be_error("uart","uart_start_recv fail!\n");
		return(-1);
	}
	module->fun_symbol = fun_symbol;
	module->item_handle = item_handle;
	module->uart = uart;
	ret = be_osal_post_delayed_action(100,uart_handle,module);
	if(ret < 0){
		free(module);
		module = NULL;
		return (-1);
	}
	
	return (0);

	

}


static int8_t uart_del_recv(uint32_t item_handle){

	int8_t ret = -1;
	uart_module_t * module = uart_get_module(item_handle);
	
	if(NULL == module){
		return(-1);
	}
	symbol_unlock(module->fun_symbol);
	free(module);
	module = NULL;
	
	return (0);
}


static be_jse_symbol_t * uart_open(void){
	int32_t len = -1;
	char * data = NULL;
	int8_t ret = -1;
	int8_t result = -1;
	item_handle_t uart_handle;
	uart_handle.handle = 0xFFFFFFFF;
	be_jse_symbol_t * arg0 = NULL;
	uart_dev_t * uart_device = NULL;
	
	be_jse_handle_function(0,&arg0,NULL,NULL,NULL);
	if(!arg0 || !symbol_is_string(arg0)){
		goto out;
	}
	len = symbol_str_len(arg0);
	data = calloc(1,sizeof(char)*(len+1));
	if(NULL==data){
		goto out;
	}
	symbol_to_str(arg0,data,len);
	ret = board_attach_item(MODULE_UART,data,&uart_handle);
	if(0 != ret){
		be_error("uart","board_attach_item fail!\n");
		goto out;
	}
	be_debug("uart","uart handle:%u\n",uart_handle.handle);
	uart_device = board_get_node_by_handle(MODULE_UART,&uart_handle);
	if(NULL == uart_device){
		be_error("uart","board_get_node_by_handle fail!\n");
		goto out;
	}
	ret = hal_uart_init(uart_device);
	if(0 != ret){
		be_error("uart","hal_uart_init fail!\n");
		goto out;
	}
	result = 0;
out:
	if(NULL != data){
		free(data);
		data = NULL;
	}
	symbol_unlock(arg0);
	if(0 != result){
		board_disattach_item(MODULE_UART,&uart_handle);
		return new_int_symbol(-1);
	}
	
	return new_int_symbol(uart_handle.handle);
}



static be_jse_symbol_t * uart_close(void){
	int8_t ret = -1;
	int8_t result = -1;
	item_handle_t uart_handle;
	be_jse_symbol_t * arg0 = NULL;
	uart_dev_t * uart_device = NULL;
	
	be_jse_handle_function(0,&arg0,NULL,NULL,NULL);
	if(!arg0 || !symbol_is_int(arg0)){
		goto out;
	}
	uart_handle.handle = get_symbol_value_int(arg0);
	uart_device = board_get_node_by_handle(MODULE_UART,&uart_handle);
	if(NULL == uart_device){
		be_error("uart","board_get_node_by_handle fail!\n");
		goto out;
	}
	ret = hal_uart_finalize(uart_device);
	if(0 != ret){
		be_error("uart","hal_uart_init fail!\n");
		goto out;
	}
	board_disattach_item(MODULE_UART,&uart_handle);
	uart_del_recv(uart_handle.handle);
	result = 0;
out:
	symbol_unlock(arg0);
	
	return new_int_symbol(result);
}


static be_jse_symbol_t * uart_write(void){
	int8_t ret = -1;
	int8_t result = -1;
	char * data = NULL;
	uint32_t len = 0;
	item_handle_t uart_handle;
	be_jse_symbol_t * arg0 = NULL;
	be_jse_symbol_t * arg1 = NULL;
	uart_dev_t * uart_device = NULL;
	
	be_jse_handle_function(0,&arg0,&arg1,NULL,NULL);
	if(!arg0 || !symbol_is_int(arg0)){
		goto out;
	}
	uart_handle.handle = get_symbol_value_int(arg0);
	uart_device = board_get_node_by_handle(MODULE_UART,&uart_handle);
	if(NULL == uart_device){
		be_error("uart","board_get_node_by_handle fail!\n");
		goto out;
	}
	if(!arg1 || !symbol_is_string(arg1)){
		goto out;
	}
	len = symbol_str_len(arg1);
	data = calloc(1,sizeof(char)*(len+1));
	if(NULL==data){
		goto out;
	}
	symbol_to_str(arg1,data,len);
	ret = hal_uart_send(uart_device,data,len,0);
	if(-1 == ret){
		be_error("uart","hal_uart_send fail!\n");
		goto out;
	}
	result = 0;
out:
	symbol_unlock(arg0);
	symbol_unlock(arg1);
	if(NULL != data){
		free(data);
		data = NULL;
	}
	
	return new_int_symbol(len);
}


static be_jse_symbol_t * uart_read(void){

	be_error("uart","todo......\n");
	return new_int_symbol(-1);
}


static be_jse_symbol_t * uart_watch(void){
	int32_t len = -1;
	char * data = NULL;
	int8_t ret = -1;
	int8_t result = -1;
	item_handle_t uart_handle;
	uart_handle.handle = 0xFFFFFFFF;
	be_jse_symbol_t * arg0 = NULL;
	be_jse_symbol_t * arg1 = NULL;
	uart_dev_t * uart_device = NULL;
	
	be_jse_handle_function(0,&arg0,&arg1,NULL,NULL);
	if(!arg0 || !symbol_is_int(arg0)){
		goto out;
	}
	uart_handle.handle = get_symbol_value_int(arg0);
	uart_device = board_get_node_by_handle(MODULE_UART,&uart_handle);
	if(NULL == uart_device){
		be_error("uart","board_get_node_by_handle fail!\n");
		goto out;
	}
	if(NULL != uart_get_module(uart_handle.handle)){
		be_error("uart","uart_get_module fail!\n");
		goto out;
	}	
	if(!arg1 || !symbol_is_function(arg1)){
		goto out;
	}
	ret = uart_add_recv(uart_device,uart_handle.handle,arg1);
	if(ret < 0){
		be_error("uart","uart_add_recv fail!\n");
		goto out;
	}
	result = 0;
out:

	symbol_unlock(arg0);
	if(0 != result){
		symbol_unlock(arg1);
	}
	
	return new_int_symbol(result);

}



static be_jse_symbol_t * uart_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name){
	if(0 == strcmp(name,"open")){
		return uart_open();
	}
	if(0 == strcmp(name,"read")){
		return uart_read();
	}
	if(0 == strcmp(name,"write")){
		return uart_write();
	}
	if(0 == strcmp(name,"on")){
		return uart_watch();
	}
	if(0 == strcmp(name,"close")){
		return uart_close();
	}
	return (BE_JSE_FUNC_UNHANDLED);
}


void module_uart_register(void){
	g_uart_module = calloc(1,MAX_UART_PORT * sizeof(uart_module_t*));
	if(NULL == g_uart_module){
		be_error("uart","calloc fail!\n");
		return;
	}
    be_jse_module_load("UART", uart_module_handle_cb);
}






