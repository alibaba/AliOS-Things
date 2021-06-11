@page uagent uagent

[更正文档](https://gitee.com/alios-things/uagent/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
uagent是AliOS Things的模块，支持其他客户模块(ulog，cli，ota，uND)将其服务注册到其中。 从云(面向维护平台)的服务调用可以通过uagent调用设备的服务，uagent还可以帮助客户模块发送信息，以帮助显示其工作状态。

## 版权信息
> Apache license v2.0

## 目录结构
```tree
uagent/
├── doc
│   └──README.md
├── example
│   └──uagent_example.c
├── include
│   ├── uagent_config.h
│   ├── uagent_type.h
│   └── uagent.h
├── internal
│   └── uagent_inner_api.h
├── src
│   ├── uagent_comm_mqtt.c
│   ├── uagent_core.c
│   ├── uagent_send.c
│   └── uagent_utility.c
└── package.yaml
```

## 依赖组件
* linksdk

# 常用配置

# API说明
@ref uagent_init
@ref uagent_mqtt_client_set
@ref uagent_ext_comm_start
@ref uagent_register
@ref uagent_unregister
@ref uagent_request_service
@ref uagent_send


# 使用示例
组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

> linkitsdk_dmeo solutions的package.yaml中添加依赖组件uagent组件
```yaml
depends:                                       # 该组件依赖其他的组件
  - linksdk: master
  - haas100: master
  - mbedtls: master
  - netmgr: master
  - uagent: master   #添加uagent组件
```
## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install uagent

```

上述命令执行成功后，组件源码则被下载到了./components/uagent路径中。

## 步骤4 添加示例

> uagent组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/uagent/tree/master/example)：
```yaml
source_file:
  - "src/uagent_comm_mqtt.c"
  - "src/uagent_core.c"
  - "src/uagent_send.c"
  - "src/uagent_utility.c"
  - "example/uagent_example.c"
```

>在data_model_basic_demo.c中修改三元组信息为阿里云物联网平台创建的设备信息
```c
    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "a1jMbw3BI9t";
    char *device_name       = "HAAS100SDFASDFAS";
    char *device_secret     = "61eda487b3ffcfa7d08eae012e40ba28";
```

>在data_model_basic_demo.c中添加uagent设置mqtthandle，在连接上阿里云物联网平台(MQTT)后添加
```c
    uagent_mqtt_client_set(mqtt_handle); //设置uagent用于连接mqtt的handle，必须在linksdk连接上云后设置
    uagent_ext_comm_start(product_key, device_name);  //设置uagent的PK与DN，与linksdk中的pk，dn一致
    /* 主循环进入休眠 */
    while (1) {
        aos_msleep(1000);
    }
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

**CLI命令行输入：**
>连接网络
```sh
netmgr -t wifi -c <wifi_ssid> <wifi_pwd>
```
> CLI关键日志：
```sh
[49.030][LK-0313] MQTT connect success in 202 ms
```
**CLI命令行输入：**
> uagent测试cli
```sh
uagent_example
```
# FAQ
