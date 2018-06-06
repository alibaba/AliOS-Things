/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#define CONFIG_LOGMACRO_DETAILS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <be_osal.h>
#include "be_jse_module.h"
#include "board-mgr/board_mgr.h"
#include "sensor_dht11.h"


static be_jse_symbol_t * hw_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name){
	if(0 == strcmp(name,"getDht11")){
		return dht11_get_temphum();
	}
	return (BE_JSE_FUNC_UNHANDLED);
}


void module_hw_register(void){
    be_jse_module_load("HW", hw_module_handle_cb);  
}



