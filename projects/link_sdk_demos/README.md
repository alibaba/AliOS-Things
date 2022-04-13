# 介绍
该工程用于演示Link SDK4.0的功能。

# 特性
- 支持接入bootstrap(获取最佳接入点)功能演示
- 支持cota 配置升级演示
- 支持fota固件升级演示
- 支持物模型功能演示
- 支持设备信息上报演示
- 支持动态注册功能演示（http/mqtt）
- 支持http连接演示
- 支持mqtt连接演示
- 支持ntp时间获取演示
- 支持设备影子功能
- 支持网关子设备功能演示

# 目录
```sh

link_sdk_demos/
├── aos.mk             #AliOS Thing makefile
├── Config.in          #AliOS Thing menuconfig配置文件，用于配置当前测试例
├── include            #project通用头文件
│   ├── u_api.h
│   └── u_config.h
└── src
    ├── app.c                        #link sdk demos入口文件
    ├── demos                        #demos集合
    │   ├── bootstrap_posix_demo.c   #bootstrap demo
    │   ├── cota_basic_demo.c        #配置更新demo
    │   ├── data_model_basic_demo.c  #物模型单品demo/AliOS Things OTA demo
    │   ├── devinfo_posix_demo.c     #设备信息demo
    │   ├── diag_basic_demo.c        #诊断demo
    │   ├── dynreg_basic_demo.c      #通过http获取密钥的动态注册demo
    │   ├── dynregmq_basic_demo.c    #通过mqtt获取密钥的动态注册demo
    │   ├── http_basic_demo.c        #http连云demo
    │   ├── logpost_basic_demo.c     #日志上报demo
    │   ├── mqtt_basic_demo.c        #mqtt基础demo
    │   ├── mqtt_broadcast_demo.c    #mqtt广播demo
    │   ├── mqtt_rrpc_demo.c         #mqtt rrpc同步调用demo
    │   ├── ntp_posix_demo.c         #ntp时间更新demo
    │   ├── shadow_basic_demo.c      #设备影子demo
    │   └── subdev_basic_demo.c      #子设备代理上云demo
    ├── entry_bin.c                  #针对.bin类型的程序入口文件，如haas100
    └── entry_elf.c                  #针对.elf类型的程序入口文件
```
# 依赖
- link_sdk
- mbedtls
- netmgr_rpc
- uspace
- uspace_lib
- cli
- socket_rpc_client
- ota

# 使用
    aos make link_sdk_demos@haas100-mk -c config
    aos make menuconfig 可选择demos集合中的任意一个，默认data_model_basic_demo.c
    aos make MBINS=app MBINS_APP=app1

# 参考
N/A