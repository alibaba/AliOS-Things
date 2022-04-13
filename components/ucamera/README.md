@page ucamera ucamera

# 概述
ucamera组件是摄像头图像数据处理中间框架，支持不同类型摄像头数据接入，目前已接入wifi摄像头，在使用该组件时，需要使用netmgr网络组件连接网络。

# 版权信息
> Apache 2.0 License

# 目录结构
```sh
.
├── include
│   ├── ucamera_common.h                 # 对外头文件
│   ├── ucamera_device.h                 # 摄像头device相关头文件定义
│   ├── ucamera_service.h                # 摄像头service相关头文件定义
│   └── device
│        └── wifi
│             ├── wifi_camera.h           # wifi摄像头device注册函数头文件
│             └── dechunk.h
├── package.yaml                          # 编译配置文件
└── src
    ├── ucamera_device.c                  # 摄像头设备相关函数实现
    ├── ucamera_service.c                 # 摄像头服务相关函数实现
    ├── example
    │    └── ucamera_example.c            # 使用示例代码
    └── device
         └── wifi
              ├── wifi_camera.c           # wifi摄像头device注册函数实现
              └── dechunk
                    └── dechunk.c         # http数据解析代码实现
```

# 依赖组件

* http
* netmgr

# 常用配置

# API说明
@ref ucamera_aos_api

# 使用示例
示例代码参考example/ucamera_example.c，以运行ucloud_ai_demo为例，具体步骤如下：

## 添加示例代码
> ucamera组件的package.yaml中添加
```C
source_file:
  - "src/*.c"
  - "src/device/wifi/*.c"
  - src/example/ucamera_example.c # add ucamera_example.c
```

## app中添加ucamera组件
> ucloud_ai_demo组件的package.yaml中添加
```C
depends:
  - ucamera: dev_aos # ucloud_ai_demo中引入ucamera组件
```

## 编译
```sh
cd solutions/ucloud_ai_demo && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

## ucamera示例测试
> 开发板网络连接命令(XXXX为网络SSID，设置不加密)：
```sh
netmgr -t wifi -c XXXX
```
> CLI命令行输入：
```sh
ucamera -t wifi
```
>测试结果正常确认(说明从wifi摄像头获取到图像数据)：
```sh
ucamera get frame OK
```

# FAQ
