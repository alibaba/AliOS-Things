## AliOS Things 轻应用

AliOS Things Mini Program (AMP)

### 简介

**[AliOS Things](https://github.com/alibaba/AliOS-Things)轻应用**是一套运行在资源受限设备上的轻量级应用软件框架。

业务应用代码脚本化，使用JavaScript脚本迅速构建轻应用。
嵌入式系统底座和应用完全解耦，使得IoT设备端应用生态的广度大幅拓宽。

Runtime采用JavaScript引擎加载用户脚本代码并执行，配合高可用的服务和工具提供应用的在线分发、更新及运维
* Runtime采用JavaScript应用引擎加载用户脚本代码并执行，配合高可用的服务和工具提供应用的在线分发、更新及运维
* 无需编译、烧录即可开发在物联网设备上运行的轻量级应用，快速开发，高效运维
* 可通过云存储托管设备端应用和资源，也通过集成云服务API，使得云端计算能力获得在设备端的延伸

### 支持平台
轻应用框架支持各类os，已支持的典型平台有
* linux
* aos(AliOS Things)

### 移植说明
* 适配层
系统适配代码在`adapter`目录中，结构如下
```
├── Config.in
├── Makefile            Linux平台编译文件
├── aos.mk              aos平台编译文件
├── include
│   ├── amp_pcm.h       音频PCM读写API
│   ├── aos_fs.h        文件系统API
│   ├── aos/kv.h        key,valueAPI
│   ├── aos_network.h   网络API
│   ├── aos_pm.h        电源管理API
│   ├── amp_httpc.h     HTTP Client
│   ├── aos_system.h    系统API
│   └── peripheral      外设API
└── platform
    ├── FreeRTOS-l610   l610适配参考
    ├── ThreadX-ec100y  ec100y适配参考
    ├── aos             AliOS Things适配参考
    └── linux           linux适配参考
```

适配层代码包含系统、网络、电源管理、音频、外设API等，移植请参考platform下已适配平台

* 入口
    AliOS Things 轻应用框架的主服务入口在 `entry/amp_entry.c` 中，调用 `amp_main()`，
    即可启动轻应用服务，在你的项目工程中创建一个任务，将amp_main()注册进去即可。

* 服务实现
    轻应用服务包含两个主要线程，一个是建立网络通道，服务于热更新、在线运维等后台服务
    另一个是脚本引擎服务线程，用来解析脚本引擎

### 使用说明

#### 1. 获取服务账号并绑定设备
* 联系SDK提供者获取账号token并绑定你的设备

#### 2. 获取命令行工具amp
* Mac版
  https://aos-oss-cn-shanghai.oss-cn-shanghai.aliyuncs.com/amp/v1.0.0/amp-macos.zip
* Windows版
  https://aos-oss-cn-shanghai.oss-cn-shanghai.aliyuncs.com/amp/v1.0.0/amp-win.zip

#### 3. 编写应用代码
按如下目录结构，创建一个新应用
```
app                     应用目录
├── board.json          配置文件
└── index.js            应用入口
```

#### 4. 应用代码热更新
AliOS Things轻应用框架使用热更新方式把应用代码推送到设备上，使用命令行工具amp以完成此功能。
命令用法例举如下

##### 用户登录
```
$ amp login <token>
```

##### 列出绑定设备
```
$ amp device list
```

##### 推送脚本到指定设备
```
$ amp put ./app <device name>
```
