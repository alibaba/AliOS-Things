
@page ota ota

[更正文档](https://gitee.com/alios-things/ota/edit/rel_3.3.0/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
OTA是over the air的缩写，是AliIOS Things 提供的完备的升级方案，对各种升级场景都有很好的支持。目前AliIOS Things除一般的整包升级外，现有的高阶能力有：压缩升级、差分升级及安全升级；支持的升级通道：http、https、BLE、3G/4G，NB等；复杂场景支持：网关及子设备升级，连接型模组升级非连接主设备的间接升级；完备的配套工具：差分工具、本地签名工具、ymodem辅助升级工具，多固件打包工具等；
主要功能列表如下：
1、支持乒乓升级：固件可在两个分区运行，支持固件版本回退，保证设备安全不变砖；
2、支持断点续传：弱网环境下，支持固件从断点处继续下载；
3、支持固件验签：固件可在云端或用本地签名工具进行数字签名（防止固件被篡改，对固件hash值进行非对称加密），设备端可完成对固件验签（用端侧的公钥对已签名的固件进行验签）；
4、支持https安全下载方式：除支持http下载外，支持https下载方式；
5、支持MD5/SHA256固件完整性检验：为保证固件完成性，固件下载完成后，都有完整性校验；
6、支持网关子设备升级：当HaaS 100 做网关时，HaaS 100 OTA 除支持网关本身的升级外，也支持其子设备的升级；

## 版权信息
> Apache license v2.0 

## 目录结构
```sh
.
├── 2ndboot                               # 二级boot升级（差分恢复和固件解压缩）
├── hal                                   # OTA HAL适配层（适配flash、os）
│   ├── ota_hal_ctrl.c                    # flash和ota agent的中间层
│   ├── ota_hal_digest.c                  # 安全相关md5、sha256、rsa适配层
│   ├── ota_hal_digest.h
│   ├── ota_hal_fs_ctrl.c                 # 文件系统操作相关适配层
│   ├── ota_hal_fs_plat.c                 # 子设备升级文件相关接口
│   ├── ota_hal_os.c                      # os适配代码
│   ├── ota_hal_os.h                    
│   ├── ota_hal_param.c                   # bootloader相关参数
│   ├── ota_hal_plat.c                    # flash相关适配
│   ├── ota_hal_trans.c                   # 传输底层适配层（比如mqtt、coap、http）
│   └── ota_hal_trans.h
├── include                               # 头文件
│   ├── ota_agent.h                       # 初始化和回调函数相关头文件
│   ├── ota_hal.h                         # hal层头文件
│   ├── ota_import.h                      # 内部使用头文件（错误码、类型定义、内部接口等）
│   ├── ota_log.h                         # ota日志头文件（对接各种日志接口）
│   └── ota_updater.h                     # 对外接口头文件
├── ota_agent                             # 所有下载和消息传输、加解密功能
│   ├── download
│   │   ├── ota_download_file2fs_http.c   # 通过http下载文件（放在flash中）
│   │   └── ota_download_http.c           # 通过http下载文件（放在flash中）
│   ├── mcu
│   │   ├── ota_download_uart.c           # 通过串口下载文件
│   │   ├── ota_mcu_ymodem.c              # 通过ymodem协议下载文件（设备是server端）
│   │   └── ota_slaver_upgrade.c          # 子设备下载文件
│   ├── ota_service.c                     # ota初始化函数和主逻辑下载文件
│   ├── transport
│   │   └── ota_transport_mqtt.c          # ota mqtt消息通道
│   └── verify  
│       ├── ota_verify_hash.c             # 文件完整性校验，如md5，sha256
│       └── ota_verify_rsa.c              # 非对称加密验签
├── package.yaml                          # ymal编译环境配置文件
├── README.md                             # 说明文档
└── tools
    ├── ota_image_package.py              # 打包压缩文件
    ├── upack_data_file.c                 # js轻应用打包解压文件
    ├── upack_data_file.h                 
    └── xz                                # xz压缩算法库
```

## 依赖组件
* cjson
* mbedtls
* linksdk
* http

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> 是否支持使用https方式: 默认0, 如需修改，在yaml中修改CONFIG_HTTP_SECURE配置
```sh
def_config:
  CONFIG_HTTP_SECURE: 1
```

# API说明
@ref ota_service_init
@ref ota_service_start
@ref ota_service_register_cb

ota_service_init 订阅mqtt topic和系统上报版本，在mqtt连接成功时候调用
ota_service_start 开始执行升级（如从flash中读取url进行升级）
ota_service_register_cb 注册用户升级回调函数

# 使用示例
示例代码参考solutions/ota_demo/otaappdemo.c，以运行ota_demo为例(更详细的使用流程请移步solutions/ota_demo/README.md)，示例主要完成如下：

示例代码分成3个主要部分：
1、注册ota升级的主函数
> ota_register_cb(&ctx, OTA_CB_ID_UPGRADE, (void*)ota_upgrade_cb);

2、初始化ota模型
> ret = ota_service_init(&ctx);

3、在ota升级主函数中调用ota_service_start执行升级
> ota_thread_create(&thread, (void *)ota_service_start, (void *)pctx, NULL, 1024 * 6);

depends:
  - ota: dev_aos
```

## 烧录固件
> 详细参考ota_demo中的文档 solutions/ota_demo/README.md的基础固件烧录章节

## OTA示例测试

### CLI命令行输入：
```sh
netmgr -t wifi -c wifi_ssid wifi_password  # 连接wifi
```

> CLI关键日志：
```sh
[  16.497]<D>WIFI_SERVICE NETMGR_WIFI_EVENT_CONNECTED           # wifi连接成功
[  18.804]<D>WIFI_SERVICE NETMGR_DHCP_SUCCESS                   # DHCP完成，获取到IP地址
[  19.306]<I>WIFI_SERVICE [sntp] OK: sec 1616741677 usec 989111 # SNTP完成，获取到最新网络时间
```

### 云端执行升级：
> 详细参考ota_demo中的文档 solutions/ota_demo/README.md

> CLI关键日志：
```
[ 119.366]<I>ota download start upg_flag:0x9669       # 开始ota升级
[ 130.827]<E>ota download complete:0                  # ota升级完成
[ 142.523]<E>ota Download complete, rebooting ret:0.  # ota升级完成并重启
```
