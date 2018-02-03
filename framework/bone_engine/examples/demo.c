/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
//#include "alink_export.h"
//#include "json_parser.h"
#include "aos/aos.h"
#include "aos/network.h"
#include "kvmgr.h"
#ifndef STM32L475xx
#include <netmgr.h>
#endif
//#include <accs.h>
#include "aos/log.h"

#include "be_jse_api.h"

static bool auto_netmgr = false;

/*
timer、alink、process等对象演示
*/
#define JS_TXT_1  " \
print('process.platform=' + process.platform);\
print('process.version=' + process.version);\
function randomNum(minNum,maxNum){\
    var ret = Math.random()*(maxNum-minNum+1)+minNum;\
    return ret;\
}\
var cmd = {}; \
cmd.attrSet = [\"ErrorCode\", \"Hue\", \"Luminance\", \"Switch\", \"WorkMode\"];\
cmd.ErrorCode = {value:0};\
cmd.Hue = {value:10};\
cmd.Luminance = {value:50};\
cmd.Switch = {value:1};\
cmd.WorkMode = {value:2};\
function onStatusChange(status, uuid){\
    print('call js func onStatusChange');\
    print('status =' + status);\
    print('uuid=' + uuid); \
    if( status == 2 ) {\
        cmd.Luminance = {value: randomNum(40, 100) };\
        print('json str='+JSON.stringify(cmd));\
        alink.postdata('postDeviceData', JSON.stringify(cmd));\
        process.nextTick(function(){\
            print('nextTick alink.getuuid()=' + alink.getuuid());\
            print('process.uptime()=' + process.uptime());\
            print(process.memoryUsage());\
        });\
    }\
}\
alink.on(\"onStatusChange\", onStatusChange); \
var jsonstr = JSON.stringify(cmd);\
print('jsonstr=' + jsonstr); \
print('------------------------\r\n');\
print('BoneEngine @ lite version =' + process.version);\
print('alink.isrunning()=' + alink.isrunning());\
print('alink.getuuid()=' + alink.getuuid());\
print('process.uptime()=' + process.uptime());\
print(process.memoryUsage());\
print('alink.getloglevel()=' + alink.getloglevel());\
print('------------------------\r\n');\
var timeout = 2000;\
var t1 = setInterval(function() {\
    var ip = getip(); \
    print('js get ip=' + ip);\
    if(ip != null ){\
        alink.start();\
        clearInterval(t1);}\
}, timeout);\
print('custom对象演示');\
print('custom.add(1,2)='+custom.add(1,2));\
print('custom.version=' + custom.version);\
print('custom.versionRW='+custom.versionRW);\
custom.versionRW='2.1';\
print('custom.versionRW='+custom.versionRW);\
print('解析完成');\
"

#ifdef USE_MK3060

/*

    小夜灯演示程序
id 2, GPIO29  -> write 控制led, 连接LED灯的负极
id 12, GPIO4  -> read D0, 1表示晚上, 连接光敏电阻传感器模块(LM393)DO管脚
每200ms读取一次D0, 根据当前光敏电阻传感器的输出来控制LED灯显示
*/

#define JS_TXT_2  " \
var gpioId=2;\
var night=0;\
gpio.open(gpioId,3);\
gpio.open(12,0);\
night=gpio.read(12);\
gpio.write(gpioId,!night);\
var val = night;\
var t2 = setInterval(function() {\
    val=gpio.read(12);\
    if(val != night){\
        night=val;\
        print('night='+night);\
        gpio.write(gpioId,!night);\
    }\
}, 200);\
"

/*
    串口测试
*/

#define JS_TXT_3  " \
var config={};\
config.port=1;\
config.baudRate=921600;\
config.dataBits=8;\
var serial=uart.open(config);\
print('uart.open = ' + serial);\
uart.write(config.port, 'uart0 write test\r\n');\
var str; \
var t3 = setInterval(function() {\
    str=uart.read(config.port, 0);\
    if(str!=null && str.length>0){ \
        print('uart read=' + str);\
    }\
}, 1000);\
"

#endif

/*
    转换JS symbol为string类型值并打印输出
*/
void bevPrintStringVar(be_jse_symbol_t *v)
{
    char buf[256];
    symbol_to_str(v, buf, 255);

    //LOGW("BoneEngine >","%s", buf);

#define GLOBAL_LOG_TAG "\e[65m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"

    csp_printf(GLOBAL_LOG_TAG MAG "BoneEngine > %s \r\n" RESET, buf);
}

#ifdef USE_ADC
#include "hal/soc/soc.h"
adc_dev_t adc_config;
#endif


static be_jse_node_t  customObjRef = 0;

be_jse_symbol_t *BeJseNativeFunctions(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
{
    be_jse_symbol_t* arg0;
    be_jse_symbol_t* arg1;
    int val;

    NOT_USED(execInfo);
    char str[32];
    //LOGW("BoneEngine","var = %p name = %var \n", var, name);

    if( var == NULL ) {
        // 全局函数
        if (strcmp(name,"print")==0) {
            // JS function print(text)
            be_jse_symbol_t *text = str_to_symbol(be_jse_handle_single_arg_function(), true);
            bevPrintStringVar(text);
            symbol_unlock(text);
            //printf("\n");
            return new_symbol(BE_SYM_NULL);
        }
        if( strcmp(name, "now")==0) {

            // 必须加上,获取函数参数
            be_jse_handle_function(0, 0, 0, 0, 0);

            #ifndef STM32L475xx

            // framework/protocol/alink/os/platform/platform.h  platform_get_utc_time
            extern uint64_t platform_get_utc_time(uint64_t *p_utc);
            be_jse_symbol_t *newValue = new_int_symbol( (be_jse_int_t)platform_get_utc_time(NULL));
            #else
            be_jse_symbol_t *newValue = new_int_symbol(0);
            #endif
            return newValue;
        }
        if( strcmp(name, "exit")==0) {
            be_jse_handle_function(0, 0, 0, 0, 0);
            LOGW("BoneEngine", "aos_loop_exit ...");
            aos_loop_exit();
            return new_symbol(BE_SYM_NULL);
        }
        if( strcmp(name, "getip")==0) {
            be_jse_handle_function(0, 0, 0, 0, 0);
            LOGW("BoneEngine", "getip ...");
            LOGW("BoneEngine","netmgr_get_ip_state=%d \r\n", netmgr_get_ip_state());
            memset(str, 0, sizeof(str));
            wifi_get_ip(str);
            LOGW("BoneEngine","wifi_get_ip=%s \r\n", str);
            if( strcmp(str,"0.0.0.0")==0 )
                return new_symbol(BE_SYM_NULL);
            return new_str_symbol(str);
        }
        if(strcmp(name, "trace")==0) {
            be_jse_handle_function(0, 0, 0, 0, 0);
#ifdef TRACE_JSE_INFO
            trace_symbol_info(bone_engine_get_executor()->root, 0);
#endif
            return new_symbol(BE_SYM_NULL);
        }
    }

    // 自定义对象的Native函数扩展实现 custom.add(a,b)
    if( customObjRef > 0 && customObjRef == get_symbol_node_id(var) ) {
        if( strcmp(name, "add")==0) {
            be_jse_handle_function(0, &arg0, &arg1, 0, 0);

            val = get_symbol_value_int(arg0) + get_symbol_value_int(arg1);

            symbol_unlock(arg0);
            symbol_unlock(arg1);

            return new_int_symbol(val);
        }

        if (strcmp(name,"version")==0) {
            // 特殊用法, 该属性只读, 并未加入到JSE符号表中, 使用返回值替换custom.version
            // 使用lexer_token_cmp完成该属性的词法分析并获取一下个token
            lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
            return new_str_symbol("1.0");
        }
    }

    return BE_JSE_FUNC_UNHANDLED;
}


static void clear_kv_and_reboot()
{
#ifndef STM32L475xx
    aos_kv_del(NETMGR_WIFI_KEY);
    LOGW("BoneEngine","KV cleared, will reboot now.");
    aos_reboot();
#endif

}

#define AUTO_HOTSPOT_TIMEOUT_S (2*60) // 2 min
#define HOTSPOT_TIMEOUT_KV_NAME "hotspot_timeout"
static int get_hotspot_timeout()
{
    char timeout_s[10];
    int len = sizeof(timeout_s);
    int timeout;

    if (aos_kv_get(HOTSPOT_TIMEOUT_KV_NAME, (void *)timeout_s, &len) == 0) {
        LOGW("BoneEngine","hotspot_timeout KV value will be used: %s seconds", timeout_s);
        timeout = atoi(timeout_s);
    } else {
        timeout = AUTO_HOTSPOT_TIMEOUT_S;
        LOGW("BoneEngine","Default hotspot timeout (%s) will be used.", timeout);
    }

    return timeout * 1000; // s -> ms
}

static void auto_netmgr_timer_fn(void *arg1, void *arg2)
{
    LOGW("BoneEngine","Info: Auto netmgr timeout hit.");
    clear_kv_and_reboot();
}

#define AUTO_NETMGR_TIMEOUT (10*60*1000) // 10 mins
static aos_timer_t auto_netmgr_timer = {NULL};
static aos_timer_t hotspot_timer = {NULL};
static void start_auto_netmgr_timer()
{
    int ms = AUTO_NETMGR_TIMEOUT;

    LOGW("BoneEngine","%s", __func__);
    if (auto_netmgr == false) return;
    if (aos_timer_new(&auto_netmgr_timer, auto_netmgr_timer_fn, NULL, ms, 0) != 0) {
        LOGW("BoneEngine","Error: aos_timer_new failed in %s", __func__);
        return;
    }
    if (aos_timer_start(&hotspot_timer) != 0) {
        LOGW("BoneEngine","Error: aos_timer_start failed in %s", __func__);
        return;
    }
}

static void stop_timer(aos_timer_t *timer)
{
    LOGW("BoneEngine","%s", __func__);
    if (!timer) {
        LOGW("BoneEngine","Timer is not set");
        return;
    }
    if (aos_timer_stop(timer) != 0) {
        LOGW("BoneEngine","Error: aos_timer_stop failed in ");
        return;
    }
}

static void hotspot_timer_fn(void *arg1, void *arg2)
{
    LOGW("BoneEngine","Info: Hotspot timeout hit.");
    clear_kv_and_reboot();
}


static void awss_hotspot_connected_handler()
{
    int ms;

    LOGW("BoneEngine","%s", __func__);
    if (auto_netmgr == false) return;
    ms = get_hotspot_timeout();
    stop_timer(&auto_netmgr_timer);
    if (aos_timer_new(&hotspot_timer, hotspot_timer_fn, NULL, ms, 0) != 0) {
        LOGW("BoneEngine","Error: aos_timer_new failed");
        return;
    }
    if (aos_timer_start(&hotspot_timer) != 0) {
        LOGW("BoneEngine","Error: aos_timer_start failed");
        return;
    }
}

static void awss_hotspot_switch_ap_done_handler()
{
    LOGW("BoneEngine","%s", __func__);
    if (auto_netmgr == false) return;
    stop_timer(&hotspot_timer);
}

#define ACTIVE_DELAY (5*1000)
static void do_auto_active()
{
    //alink_activate(NULL);
    LOGW("BoneEngine","--------auto active done------");
    aos_msleep(ACTIVE_DELAY);
    //clear_kv_and_reboot();
}

static void auto_active_handler(input_event_t *event, void *priv_data)
{
    if (auto_netmgr == false) return;
    if (event->type != EV_YUNIO) return;
    if (event->code != CODE_YUNIO_ON_CONNECTED) return;
    aos_post_delayed_action(ACTIVE_DELAY, do_auto_active, NULL);
}

#define AUTO_NETMGR_KEY "auto_netmgr"
static bool get_auto_netmgr_config()
{
    char c[5];
    int len = sizeof(c);
    bool ret;

    if (aos_kv_get(AUTO_NETMGR_KEY, (void *)c, &len) != 0) {
        ret = false;
        LOGE("alink", "kv(%s) not set, auto_netmgr will be disabled",
             AUTO_NETMGR_KEY);
    } else {
        ret = true;
        LOGD("alink", "kv(%s) found, auto_netmgr will be enabled",
             AUTO_NETMGR_KEY);
    }

    return ret;
}

static void handle_quit_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    LOGW("main", "call %s ...", __FUNCTION__);
    aos_loop_exit();
}

static struct cli_command quitcmd = {
    .name = "quit",
    .help = "quit example",
    .function = handle_quit_cmd
};


static void handle_meminfo_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    LOGW("main", "call %s ...", __FUNCTION__);
    LOGW("BoneEngine", "memory usage= %d max=%d", be_jse_get_memory_usage(), be_jse_get_memory_max_usage());
}

static struct cli_command meminfocmd = {
    .name = "meminfo",
    .help = "show BoneEngine meminfo",
    .function = handle_meminfo_cmd
};


void nextTick_run_js(void *arg)
{
    LOGW("BoneEngine","aos_task_name=%s \r\n", aos_task_name());
    LOGW("BoneEngine", "js=%s", arg);
    bone_engine_start(arg);
}

static void handle_eval_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    LOGW("BoneEngine","aos_task_name=%s \r\n", aos_task_name());
    LOGW("BoneEngine", "pwbuf=%s  blen=%d", pwbuf, blen);
    LOGW("BoneEngine", "argc=%d", argc);
    if( argc > 1) {
        LOGW("BoneEngine", "js=%s", argv[1]);
        aos_schedule_call(nextTick_run_js, argv[1]);
    }
}

static struct cli_command evalcmd = {
    .name = "eval",
    .help = "BoneEngine eval javascript",
    .function = handle_eval_cmd
};



void main_idle(void *arg)
{
    NOT_USED(arg);

    // aos_task_name= app
    LOGD("BoneEngine","aos_task_name= %s\r\n", aos_task_name());

}

static void wifi_process(input_event_t *event, void *priv_data)
{

    LOGW("BoneEngine","wifi_process, event->type=%d ", event->type);
    LOGW("BoneEngine","wifi_process, event->code=%d ", event->code);

    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

}

void key_process(input_event_t *event, void *priv_data)
{
    if (event->type != EV_KEY) {
        return;
    }

    LOGW("BoneEngine","event->code = %d , event->value=%d ", event->code, event->value);

    if (event->code == CODE_BOOT) {
        if (event->value == VALUE_KEY_CLICK) {
            /*
            if (cloud_is_connected() == false) {

            }
            */
        } else if(event->value == VALUE_KEY_LTCLICK) {

        } else if(event->value == VALUE_KEY_LLTCLICK) {

        }
    }
}

int application_start(int argc, char *argv[])
{

    aos_set_log_level(AOS_LL_WARN);
    //aos_set_log_level(AOS_LL_DEBUG);

    LOGW("BoneEngine", "aos_get_product_model=%s", aos_get_product_model());
    LOGW("BoneEngine", "aos_get_os_version=%s", aos_get_os_version());
    LOGW("BoneEngine", "aos_get_kernel_version=%s", aos_get_kernel_version());
    LOGW("BoneEngine", "aos_get_app_version=%s", aos_get_app_version());
    LOGW("BoneEngine", "aos_get_device_name=%s", aos_get_device_name());

    // mk3060平台: aos_task_name=aos-init
    LOGW("BoneEngine","aos_task_name=%s \r\n", aos_task_name());

    //  网络事件
    aos_register_event_filter(EV_WIFI, wifi_process, NULL);
    // KEY事件
    aos_register_event_filter(EV_KEY, key_process, NULL);

#if 0
    // zhengxiang.zzx ???
    aos_register_event_filter(EV_YUNIO, auto_active_handler, NULL);
    auto_netmgr = get_auto_netmgr_config();
    awss_register_callback(AWSS_HOTSPOT_CONNECTED, &awss_hotspot_connected_handler);
    awss_register_callback(AWSS_HOTSPOT_SWITCH_AP_DONE, &awss_hotspot_switch_ap_done_handler);
    if (auto_netmgr)
        start_auto_netmgr_timer();
#endif
    netmgr_init();

#ifndef STM32L475xx

// only test, connect my router
#if 1
    auto_netmgr = false;
    netmgr_ap_config_t apconfig;
    memset(&apconfig, 0, sizeof(apconfig));
    strcpy(apconfig.ssid,"xxxxxxxx");
    strcpy(apconfig.pwd, "xxxxxxxx");
    netmgr_set_ap_config(&apconfig);
#endif

    LOGW("BoneEngine","netmgr_start（%d) \r\n", auto_netmgr);

    netmgr_start(auto_netmgr);

    LOGW("BoneEngine","netmgr_get_ip_state=%d \r\n", netmgr_get_ip_state());

#endif


    aos_cli_register_command(&quitcmd);
    aos_cli_register_command(&meminfocmd);
    aos_cli_register_command(&evalcmd);

    // BoneEngine初始化
    bone_engine_init();
    bone_engine_native_function_register(BeJseNativeFunctions);

    // 注册自定义对象,必须在bone_engine_register之后调用!
    customObjRef = bone_engine_native_object_register("custom");
    /*
        为custom增加一个可读写的属性versionRW,并加入到JSE符号表中
    */
    be_jse_symbol_t* value = new_str_symbol("2.0");
    be_jse_symbol_t* customObj = symbol_lock(customObjRef);
    symbol_unlock(add_symbol_node_name(customObj, value, "versionRW"));
    symbol_unlock(value);
    symbol_unlock(customObj);


#ifdef USE_ADC
    int ret;
    adc_config.adc = 0;
    adc_config.config.sampling_cycle = 3;
    adc_config.priv = NULL;
    ret = hal_adc_init(&adc_config);  //undefined reference to `hal_adc_init'
    LOGW("BoneEngine","hal_adc_init ret=%d ", ret);
    ret = MicoAdcInitialize(0, 3);  //undefined reference to `MicoAdcInitialize'
    LOGW("BoneEngine","MicoAdcInitialize ret=%d ", ret);

#endif


    bone_engine_start(JS_TXT_1);
#ifdef USE_MK3060
    // JS_TXT_2、JS_TXT_3 顺序执行
    bone_engine_start(JS_TXT_2);
    bone_engine_start(JS_TXT_3);
#endif

    LOGW("BoneEngine","jse memory usage=%d max=%d ", be_jse_get_memory_usage(), be_jse_get_memory_max_usage() );


    // test
    //aos_schedule_call(main_idle, NULL);
    //aos_loop_schedule_call(NULL, main_idle, NULL); //作用同上

    aos_loop_run();

    LOGE("BoneEngine","after aos_loop_run\n");

    bone_engine_exit();

    LOGE("BoneEngine","bone_engine_exit Done!\n");


    return 0;
}

