/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "module_gpio.h"
#include "be_jse_api.h"

#ifdef USE_GPIO


#include "aos/kernel.h"
#include "aos/yloop.h"
#include "hal/soc/soc.h"

/*
移至board.h中定义
// 根据实现情况配置GPIO
#define GPIO_COUNT 5

// GPIO id号, 与硬件对应
static const int gpio_id[GPIO_COUNT]= {1,2,3,4,5};
*/

// GPIO HAL
static gpio_dev_t gpioList[GPIO_COUNT];

static int bInit = 0;

void gpio_module_init()
{
    if(bInit)
        return;

    bInit = 1;

    int i;
    memset(gpioList, 0, sizeof(gpioList));

    for(i=0; i<GPIO_COUNT; i++) {
        gpioList[i].port = gpio_id[i];
    }
}

be_jse_symbol_t* gpio_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
{
    int ret=-1;
    be_jse_symbol_t* arg0;
    be_jse_symbol_t* arg1;

    int id;
    int value;

    if (get_symbol_node_id(var) == execInfo->executor->gpioClass) {

        if (strcmp(name,"open")==0) {

            be_jse_handle_function(0, &arg0, &arg1, 0, 0);

            id = get_symbol_int(arg0);
            value = get_symbol_int(arg1);
            symbol_unlock(arg0);
            symbol_unlock(arg1);

            if( id >=0 && id <= GPIO_COUNT ) {

                gpioList[id].config = value;

                LOGD(__FILE__, "hal_gpio_init, gpio port= %d, config=%d ", gpioList[id].port, gpioList[id].config);
                ret = hal_gpio_init(&gpioList[id]);
            }

            return new_int_symbol(ret);
        }
        if(strcmp(name, "write")==0) {
            be_jse_handle_function(0, &arg0, &arg1, 0, 0);
            id = get_symbol_int(arg0);
            value = get_symbol_int(arg1);
            symbol_unlock(arg0);
            symbol_unlock(arg1);

            if( id >=0 && id <= GPIO_COUNT ) {

                LOGD(__FILE__, "hal_gpio_output, port=%d value=%d ", gpioList[id].port, value);

                if( value > 0)
                    ret = hal_gpio_output_high(&gpioList[id]);
                else
                    ret = hal_gpio_output_low(&gpioList[id]);
            }


            return new_int_symbol(ret);

        }
        if(strcmp(name, "read")==0) {
            be_jse_handle_function(0, &arg0, &arg1, 0, 0);
            id = get_symbol_int(arg0);
            value = get_symbol_int(arg1);
            symbol_unlock(arg0);
            symbol_unlock(arg1);

            if( id >=0 && id <= GPIO_COUNT ) {

                // hal_gpio_input_get 没返回值
                value = -2;
                ret = hal_gpio_input_get(&gpioList[id], &value);
                ret = value;
            }

            return new_int_symbol(ret);

        }
        if(strcmp(name, "close")==0) {
            be_jse_handle_function(0, &arg0, &arg1, 0, 0);
            id = get_symbol_int(arg0);
            value = get_symbol_int(arg1);
            symbol_unlock(arg0);
            symbol_unlock(arg1);

            if( id >=0 && id <= GPIO_COUNT ) {
                ret = hal_gpio_finalize(&gpioList[id]);
            }

            return new_int_symbol(ret);

        }
    }
    return BE_JSE_FUNC_UNHANDLED;

}

#endif

