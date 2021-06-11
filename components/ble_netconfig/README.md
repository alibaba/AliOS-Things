@page ble_netconfig ble_netconfig

[更正文档](https://gitee.com/alios-things/ble_netconfig/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
AliOS Things 3.3提供ble_netconfig组件，用BLE功能传递配置信息，包括Wi-Fi配网需要的SSID和密码信息，连接阿里云需要的三元组信息。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01eJ3mvA27dbuS4ofU3_!!6000000007820-2-tps-768-277.png" style="max-width:800px;" />
</div>

## 功能支持
ble_netconfig组件主要支持如下功能：

- 开启和关闭BLE配置通道功能
- 在BLE配置通道上传递消息给手机支付宝小程序功能


## 版权说明
> Apache license v2.0


## 目录结构


```tree
|-- ble_netconfig.c          #BLE网络配置核心代码
|-- ble_netconfig.h          #BLE网络配置头文件
|-- example                  #BLE网络配置实例
|    |-- netcfg_example.c
|-- package.yaml             #makefile
|-- README.md                #README文档
```

## 依赖组件

- osal_aos
- ble_host
- netmgr

# 常用配置
无


# API说明
## API列表
|  BLE_NetCfg_init | 初始化BLE配置通道 |
| :--- | :--- |
|  BLE_NetCfg_start | 开启BLE配置通道功能 |
| BLE_NetCfg_stop | 关闭BLE配置通道功能 |
| BLE_NetCfg_notificate | HaaS开发板通过蓝牙发送消息给HaaS小程序 |
| BLE_NetCfg_wifi_get | 获取Wi-Fi热点信息，SSID和密码 |
| BLE_NetCfg_wifi_set | 设置Wi-Fi热点信息，SSID和密码，配网逻辑会连接Wi-Fi热点 |
| BLE_NetCfg_devinfo_get | 获取设备三元组信息 |
| BLE_NetCfg_devinfo_set | 设置设备三元组信息 |



## API详情
### BLE_NetCfg_init
初始化BLE配置通道功能。
**函数原型**


```c
BLE_NETCFG_STATE BLE_NetCfg_init(BLE_netCfg_callck callback)
```
**输入参数**

| args | description |
| :--- | :--- |
| callback | 回调函数 |

**返回参数**
`0：`成功， `其他值`：失败。


### BLE_NetCfg_start
开启BLE配置通道功能。该功能在收到Wi-Fi热点SSID和密码后，会自动连接网络，并在获得IP后抛出事件。

**函数原型**


```c
BLE_NETCFG_STATE BLE_NetCfg_start(void)
```
**输入参数**

| args | description |
| :--- | :--- |
| 无 |  |

**返回参数**
`0：`成功， `其他值`：失败。


### BLE_NetCfg_stop
关闭BLE配置通道功能。
**函数原型**


```c
BLE_NETCFG_STATE BLE_NetCfg_stop(void)
```
**输入参数**

| args | description |
| :--- | :--- |
| 无 |  |

**返回参数**
`0：`成功， `其他值`：失败。

### BLE_NetCfg_notificate

HaaS开发板通过蓝牙发送消息给HaaS小程序

**函数原型**

```c
BLE_NETCFG_STATE BLE_NetCfg_notificate(const uint8_t *data, uint16_t size)
```

**输入参数**

| args | description  |
| :--- | ------------ |
| data | 传输消息内容 |
| size | 传输消息长度 |

**返回参数**

`0：`成功， `其他值`：失败。

### BLE_NetCfg_wifi_get

获取Wi-Fi热点信息，SSID和密码

**函数原型**

```c
BLE_NETCFG_STATE BLE_NetCfg_wifi_get(char **ssid, char **passwd)
```

**输入参数**

| args   | description                             |
| :----- | --------------------------------------- |
| ssid   | 用于存放返回Wi-Fi热点SSID信息的存储空间 |
| passwd | 用于存放返回Wi-Fi热点密码信息的存储空间 |

**返回参数**

`0：`成功， `其他值`：失败。

### BLE_NetCfg_wifi_set

设置Wi-Fi热点信息，SSID和密码，配网逻辑会连接Wi-Fi热点

**函数原型**

```c
BLE_NETCFG_STATE BLE_NetCfg_wifi_set(char *ssid, char *passwd)
```

**输入参数**

| args   | description           |
| ------ | --------------------- |
| ssid   | 传入Wi-Fi热点SSID信息 |
| passwd | 传入Wi-Fi热点密码信息 |

### BLE_NetCfg_devinfo_get

获取设备三元组信息

**函数原型**

```c
BLE_NETCFG_STATE BLE_NetCfg_devinfo_get(char **pk, char **dn, char **ds)
```

**输入参数**

| args | description                             |
| ---- | --------------------------------------- |
| pk   | 用于存放返回设备三元组信息product key   |
| dn   | 用于存放返回设备三元组信息device name   |
| ds   | 用于存放返回设备三元组信息device secret |

**返回参数**

`0：`成功， `其他值`：失败。

### BLE_NetCfg_devinfo_set

设置设备三元组信息

**函数原型**

```c
BLE_NETCFG_STATE BLE_NetCfg_devinfo_set(char *pk, char *dn, char *ds)
```

**输入参数**

| args | description                     |
| ---- | ------------------------------- |
| pk   | 传入设备三元组信息product key   |
| dn   | 传入设备三元组信息device name   |
| ds   | 传入设备三元组信息device secret |

**返回参数**

`0：`成功， `其他值`：失败。

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
```yaml
depends:
  - ble_netconfig: master
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install ble_netconfig

```

上述命令执行成功后，组件源码则被下载到了./components/ble_netconfig路径中。

## 步骤4 添加示例

ble_netconfig组件的package.yaml中添加示例代码

```yaml
source_file:
  - ble_netconfig.c
  - example/netcfg_example.c
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
netcfg_example

**案例工具**

- 串口工具
- 支付宝HaaS小程序

打开支付宝，搜索"HaaS小程序"进入小程序操作界面

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01dhcdE21uTS7SSsFCb_!!6000000006038-2-tps-664-704.png" style="max-width:800px;" />
</div>

**多个待配网设备**

当存在多个待配网设备时，在小程序界面点击“选择设备”，会弹出设备选择列表。在列表中选择待配网设备。

在CLI命令行输入以下命令，可以查看设备名称和蓝牙地址

```sh
(ash)# ble_netCfg name
netconfig device name HaaS-0000004333c5
BT address 6c-33-33-22-11-11
```

netconfig device name和小程序待配网设备列表是对应的。

这里要注意蓝牙地址BT address一定要不一样。如果蓝牙地址一样，则小程序待配网列表只会出现一个设备。此时说明该开发板蓝牙地址没有烧录，需要手动烧录蓝牙地址。

```sh
aos_mac WIFI xx:xx:xx:xx:xx:xx
aos_mac BT xx:xx:xx:xx:xx:xx
```

设置成功后，输入ble_netCfg name确认蓝牙地址设置成功。重启设备，小程序待配网设备列表会出现多台设备。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01HCkaIe1hM2hp6d48i_!!6000000004262-0-tps-1170-2532.jpg" style="max-width:800px;" />
</div>
