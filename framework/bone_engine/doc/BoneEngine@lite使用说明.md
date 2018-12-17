
# BoneEngine@lite for aos 介绍

AliOS Things是阿里巴巴旗下物联网操作系统。阿里巴巴发布物联网操作系统AliOS Things主要目的是让阿里巴巴的物联网技术普惠更多的开发者和用户，助力实现更加美好的物联网时代。

`BoneEngine@lite for aos` 是一个专门为AOS系统设计的JavaScript引擎，为模组使用做了专门优化。


## aos编译说明


* **编译命令**
bone_engine 是模块名称
linuxhost 是指board类型
参数VERBOSE=1
参数BUILD_TYPE 默认：release_log，可选有debug,release,release_log

```
代码放置路径： aos.git/framework/bone_engine
cd aos.git
ln -s ../framework/bone_engine/examples example/boneengine_demo

aos make boneengine_demo@linuxhost
aos make boneengine_demo@mk3060
aos make boneengine_demo@mk3060 VERBOSE=1
aos make boneengine_demo@mk3060 BUILD_TYPE=release
aos make boneengine_demo@mk3060 JTAG=jlink download
```


* 查看CFLAG

重要flag: SYSINFO_PRODUCT_MODEL    用于区别产品类型

```
cat out/boneengine_demo@mk3060/libraries/bone_engine.c_opts
  -c -MD -DSYSINFO_KERNEL_VERSION=\"AOS-R-1.3.4\"  -DMESH_GATEWAY_SERVICE -DGATEWAY_SDK            -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w -DSYSINFO_PRODUCT_MODEL=\"ALI_AOS_MK3060\" -DSYSINFO_DEVICE_NAME=\"MK3060\" -DSYSINFO_APP_VERSION=\"APP-1.0.0-20171117.0857\" -ggdb -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  -DAOS_SDK_VERSION_MAJOR=3 -DAOS_SDK_VERSION_MINOR=2 -DAOS_SDK_VERSION_REVISION=3 -Iout/boneengine_demo@mk3060/resources/ -DPLATFORM=\"mk3060\"    -I./security/alicrypto/inc -I./security/alicrypto/test -I./framework/gateway/msdp/. -I./framework/gateway/devmgr/. -I./kernel/protocols/mesh/include -I./framework/fota/socket/./ -I./framework/fota/platform/./ -I./kernel/modules/fs/kv/include -I./utility/hashtable/. -I./utility/base64/. -I./framework/connectivity/wsf/. -I./kernel/protocols/net/include -I./kernel/protocols/net/port/include -I././platform/mcu/beken/beken7231/beken378/driver/entry/. -I./utility/digest_algorithm/. -I./utility/cjson/include -I./security/mbedtls/include -I./kernel/rhino/core/include -I././platform/arch/arm/armv5/. -I./framework/gateway/./ -I./framework/ywss/. -I./framework/bone_engine/include -I./framework/bone_engine/boards/ALI_AOS_MK3060 -I./framework/netmgr/include -I./framework/netmgr/../protocol/alink/os/platform/ -I./framework/fota/. -I./tools/cli/include -I./framework/protocol/alink/. -I./framework/protocol/alink/accs -I./framework/protocol/alink/json -I./framework/protocol/alink/system -I./framework/protocol/alink/os -I./kernel/vfs/include -I./kernel/vcall/./mico/include -I././platform/mcu/beken/beken7231/beken378/func/mxchip/lwip-2.0.2/port -I././platform/mcu/beken/beken7231/beken378/common -I././platform/mcu/beken/beken7231/beken378/app/config -I././platform/mcu/beken/beken7231/beken378/func/include -I././platform/mcu/beken/beken7231/beken378/os/include -I././platform/mcu/beken/beken7231/beken378/driver/include -I././platform/mcu/beken/beken7231/beken378/driver/common -I././platform/mcu/beken/beken7231/beken378/ip/common -I././platform/mcu/beken/beken7231/beken378/os/FreeRTOSv9.0.0/FreeRTOS/Source/portable/Keil/ARM968es -I./board/mk3060/. -I./include -I./example/boneengine_demo -DBUILD_BIN -DCONFIG_ALICRYPTO -DCONFIG_AOS_MESH -DAOS_KV -DAOS_LOOP -DWITH_LWIP -DCONFIG_NET_LWIP -DAOS_HAL -DCONFIG_YWSS -DAOS_FRAMEWORK_COMMON -DAOS_NETMGR -DAOS_FOTA -DHAVE_NOT_ADVANCED_FORMATE -DCONFIG_AOS_CLI -DAOS_VFS -DVCALL_RHINO -DCONFIG_MX108 -DCONFIG_AOS_KV_MULTIPTN_MODE -DCONFIG_AOS_KV_PTN=6 -DCONFIG_AOS_KV_SECOND_PTN=7 -DCONFIG_AOS_KV_PTN_SIZE=4096 -DCONFIG_AOS_KV_BUFFER_SIZE=8192 -DSTDIO_UART=0
```



* **查看board类型**

```
ls board
armhflinux  b_l475e  esp32devkitc  linuxhost  mk108  mk3060
```


* **烧录说明**

[庆科 MK3060 烧录说明](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Environment-Setup)


## 目录介绍

```
├── boards
│   ├── ALI_AOS_LINUXHOST
│   │   └── be_board.h
│   └── ALI_AOS_MK3060
│       └── be_board.h
├── bone_engine.mk
├── doc
│   ├── alink对象说明.md
│   ├── BoneEngine@lite for aos 使用说明.md
│   ├── gpio对象说明.md
│   ├── javascript支持范围.xlsx
│   ├── javascript能力集介绍.md
│   ├── process对象说明.md
│   ├── timers说明.md
│   └── uart对象说明.md
├── include
│   ├── aos_alink_impl.h
│   ├── be_bi_module.h
│   ├── be_iot_module.h
│   ├── be_jse_api.h
│   ├── be_jse_executor.h
│   ├── be_jse_feature.h
│   ├── be_jse_global.h
│   ├── be_jse.h
│   ├── be_jse_lex.h
│   ├── be_jse_symbol.h
│   ├── be_jse_types.h
│   ├── be_jse_utils.h
│   ├── module_alink.h
│   ├── module_gpio.h
│   ├── module_process.h
│   ├── module_timers.h
│   └── module_uart.h
├── jse
│   ├── be_jse_executor.c
│   ├── be_jse_lex.c
│   ├── be_jse_symbol.c
│   └── be_jse_utils.c
├── modules
│   ├── aos_alink_impl.c
│   ├── be_bi_module.c
│   ├── be_iot_module.c
│   ├── module_alink.c
│   ├── module_gpio.c
│   ├── module_process.c
│   ├── module_timers.c
│   └── module_uart.c
├── README.md
├── examples
│   ├── boneengine_demo.mk
│   └── demo.c
└── src
    └── bone_engine.c

```

* boards目录
板型配置
对于不同的产品，JS的扩展可以定制
可以配置JS扩展类，可以配置外设，如GPIO端口映射等
配置文件：`board/$(CONFIG_SYSINFO_PRODUCT_MODEL)/be_board.h`

* doc目录
相关文档资料

* include目录
头文件

* jse
JS引擎源文件

* modules
扩展模块文件

* src
AOS系统中对于JSE的调用封装

* boneengine_demo
AOS系统上应用参考例程


## API介绍

1. **void bone_engine_init();**
BoneEngine初始化

2. **void bone_engine_native_function_register(BE_JSE_FUNCTION_EXECUTE_CB extension);**
注册自定义JS对象Native扩展方法及属性的实现函数

3. **be_jse_node_t bone_engine_native_object_register(const char* objectName);**
用户注册自定义的JS对象,返回该对象的引用值
<font color=#DC143C><b>必须在bone_engine_init之后调用</b></font>

4. **void bone_engine_start(const char\* js); **
运行JS程序

5. **void bone_engine_restart(const char\* js);**
重新运行JS程序

6. **void bone_engine_exit();**
退出BoneEngine

7. **be_jse_executor_ctx_t* bone_engine_get_executor();**
获取当前BoneEngine引擎的executor对象


## natvie扩展


&emsp;&emsp;**用户可以根据需要扩展JS对象的方法及全局函数**



* 使用API: **bone_engine_native_object_register**
    在使用BoneEngine时，用户可以调用`bone_engine_native_object_register`注册一个自定义的命名对象

    参考代码: demo.c 中注册了一个自定义的`custom`对象
```
customObj = bone_engine_register_object("custom");
```


* 使用API: **`bone_engine_native_function_register`**

    在使用BoneEngine时，用户可以调用`bone_engine_native_function_register`注册JS对象Native扩展方法及属性的实现函数，在这个注册的函数中，用户可以实现全局函数扩展和自定义对象的方法及属性的扩展

    参考代码： `be_bi_module.c`文件中`handle_function_call`这个函数中有对内部对象(`Integer`、`Math`、`JSON`、`String`、`Array`)方法及全局函数扩展的实现

    参考代码： demo.c 中`BeJseNativeFunctions`函数

```
全局函数：
    print(x);
    now();
    exit();
    getip();
    trace();

custom对象：
    custom.add(a,b)； Native函数扩展
    custom.version    该对象属性扩展

be_jse_symbol_t *BeJseNativeFunctions(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
{
    be_jse_symbol_t* arg0;
    be_jse_symbol_t* arg1;
    int val;
    NOT_USED(execInfo);
    char str[32];

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

            // 必须加上,函数参数完成语法分析
            be_jse_handle_function(0, 0, 0, 0, 0);

            // framework/protocol/alink/os/platform/platform.h  platform_get_utc_time
            extern uint64_t platform_get_utc_time(uint64_t *p_utc);
            be_jse_symbol_t *newValue = new_int_symbol( (be_jse_int_t)platform_get_utc_time(NULL));

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
    if( customObj > 0 && customObj == get_symbol_node_id(var) ) {
        if( strcmp(name, "add")==0) {
            be_jse_handle_function(0, &arg0, &arg1, 0, 0);

            val = get_symbol_value_int(arg0) + get_symbol_value_int(arg1);

            symbol_unlock(arg0);
            symbol_unlock(arg1);

            return new_int_symbol(val);
        }

        if (strcmp(name,"version")==0) {
            lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
            return new_str_symbol("0.0.1");
        }
    }

    return BE_JSE_FUNC_UNHANDLED;
}
```



## ROM/RAM优化


* **编译成release版本,  BUILD_TYPE = release**

* **根据实际需要配置合适的BE_JSE_VAR_CACHE_SIZE**
**`#define BE_JSE_VAR_CACHE_SIZE 500`**
**BE_JSE_VAR_CACHE_SIZE每增加100, RAM增加2100Byte**
编译成release_log或debug版本， boneengine_demo中有一个`meminfo`可以查看当前系统js symbol运行中最大使用值，可以根据该值调整BE_JSE_VAR_CACHE_SIZE

* **定制be_board.h,删除无用的功能模块**
如：TRACE_JSE_INFO、DUMP_SYMBL_USAGE





