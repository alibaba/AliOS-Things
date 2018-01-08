/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include "abs_data_model.h"
#include  <stdio.h> 
#include <stdbool.h>
#include <fcntl.h>
#include <aos/yloop.h>

int cali_example_process_cb(void* pdata)
{   
    /* just a simple example here
       show how to register inot the abs model */

    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return 0;
}


int cali_example_example_init(void){
    int ret = 0;
    ret = abs_cali_data_register(TAG_DEV_ACC, cali_example_process_cb);
    if(unlikely(ret)){
        return -1;
    }
    
    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return 0;
}




