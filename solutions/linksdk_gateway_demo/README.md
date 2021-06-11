@page linksdk_gateway_demo WI-FI设备实现网关协助子设备连接阿里云示例

[更正文档](https://gitee.com/alios-things/linksdk_gateway_demo/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 1. 案例简介
linksdk_gateway_demo是Link SDK网关代理子设备上云最简示例。

其完成的主要功能包括：
- 系统板级初始化
- 内核基础组件初始化
- 网络配置
- 网络配置成功（获取ip）后创建Link SDK主线程
- Link SDK参数配置
- Link SDK初始化
- 创建独立线程用于MQTT保活及QoS1消息重发
- 创建独立线程用于下行数据接收
- 初始化子设备模块
- 批量添加子设备topo关系
- 子设备批量上线
- 子设备下线

该示例的运行依赖下述基本功能完成对接：
- AOS API
- LwIP
- MbedTLS
- Link SDK

# 2. 基础知识

## 2.1 基础目录结构
```tree

.
├── main.c                   # 网络配置及SDK启动入口
├── maintask.c               # 系统主任务入口处理，入口**aos_maintask**
├── Makefile                 # aos make编译时入口
├── package.yaml             # 编译系统配置文件
├── README.md                # 本说明文档
├── SConstruct               # Makefile => Scon => aostools
└── subdev_basic_demo.c      # Link SDK 网关(代理子设备上云)演示示例
```

# 3. 物料清单

## 3.1 HaaS100 硬件

[HaaS100 硬件简介](https://help.aliyun.com/document_detail/184426.html)

<img src="https://img.alicdn.com/imgextra/i4/O1CN01XxD6Xo217CB3FZnEU_!!6000000006937-2-tps-746-497.png" style="max-width:800px;" />

# 4. 案例实现

## 4.1 硬件连接
该案例只需要连接电源线以及串口线，如下图所示：

<img src="https://img.alicdn.com/imgextra/i3/O1CN01tPYjF31bqpdGkFbdD_!!6000000003517-0-tps-4032-3024.jpg" style="max-width:800px;" />

注意：由于本案例需要使用Wi-Fi联网，需将标有2.4G/5.8G的天线连接到HaaS100 靠近USB口的天线端子上，如上图所示。
## 4.2 软件实现
### 4.2.1 云端创建产品
请先点击登陆[物联网平台](https://www.aliyun.com/product/iot/iot_instc_public_cn)（未注册阿里云账户的用户，请先完成账户注册），按下面步骤一步步去打造。

1、创建项目

注册登入后，如下图所示，前往管理控制台。

<img src="https://img.alicdn.com/imgextra/i4/O1CN01BN7DMd1IibotD78f6_!!6000000000927-2-tps-1308-490.png" style="max-width:800px;" />

点击公共实例，进行产品创建。

<img src="https://img.alicdn.com/imgextra/i2/O1CN01AjRz9z294Sk2dsMXe_!!6000000008014-2-tps-1328-792.png" style="max-width:800px;" />

2、创建网关演示产品

点击创建产品，见下图所示，这里创建了一个名称“haas_gateway”产品。

<img src="https://img.alicdn.com/imgextra/i1/O1CN01vpxC1F1FQnZfx4Znh_!!6000000000482-2-tps-1470-1788.png" style="max-width:800px;" />

3、创建网关设备

在设备列表中增加对应产品的设备。

<img src="https://img.alicdn.com/imgextra/i2/O1CN01a8mOHP1zd8ZL5UYE0_!!6000000006736-2-tps-1392-624.png" style="max-width:800px;" />

4、创建子设备演示产品

点击创建产品，见下图所示，这里创建了一个名称“haas_subdev”产品。

<img src="https://img.alicdn.com/imgextra/i3/O1CN01ur5LgC1h3GSlStLvf_!!6000000004221-2-tps-1504-1764.png" style="max-width:800px;" />

5、创建多个子设备

在设备列表中增加对应产品的设备。

<img src="https://img.alicdn.com/imgextra/i4/O1CN019rocNe1hsYp8IBufJ_!!6000000004333-2-tps-1816-738.png" style="max-width:800px;" />

创建后的设备三元组信息需要同步到设备端的开发代码段中，在4.2.2章节会介绍。待设备端开发结束，就可以在物联网平台中参看设备在线状态。

### 4.2.2 AliOS Things开发环境搭建
开发环境的搭建请参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

### 4.2.3 linksdk_gateway_demo代码下载
物模型上云的代码下载请参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)，其中，
> 选择解决方案: “linksdk_gateway_demo”
> 选择开发板: HaaS100

### 4.2.4 设备端代码修改

演示用例路径为solutions/{your_project}/data_model_basic_demo.c 此演示用例演示Link SDK物模型单品上云的功能。
注意：{your_project} 为studio中创建的工程名（基于linksdk_gateway_demo模板）。

1.修改网关三元组，使用4.2.1章节得到的三元组填入代码如下区域：
```c
int32_t demo_mqtt_start(void **handle)
{
...
    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "替换为上章节创建的product_key";
    char *device_name       = "替换为上章节创建的device_name";
    char *device_secret     = "替换为上章节创建的device_secret";
...
```
1.修改子三元组，使用4.2.1章节得到的三元组填入代码如下区域：
```sh
aiot_subdev_dev_t g_subdev[] = {
    {
        "替换为上章节创建的子设备1的product_key",
        "替换为上章节创建的子设备1的product_secret",
        "替换为上章节创建的子设备1的device_name",
        "替换为上章节创建的子设备1的device_secret"
    },
    {
        "替换为上章节创建的子设备2的product_key",
        "替换为上章节创建的子设备2的product_secret",
        "替换为上章节创建的子设备2的device_name",
        "替换为上章节创建的子设备2的device_secret"
    },
...
};
```

### 4.2.5 代码编译、烧录
编译linksdk_gateway_demo的过程如下：

-- 编译固件可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

-- 烧录固件可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)。

## 4.3 调试

CLI命令行输入联网命令：
```sh
netmgr -t wifi -c ssid password
```
注意：ssid和password需要替换成自己的路由的ssid及密码。

连上路由器后会自动连接到阿里云物联网平台。

### 4.3.1 关键日志
CLI日志：
```sh
success to establish mbedtls connection, fd = 3(cost 0 bytes in total, max used 0 bytes)
[104.526][LK-0313] MQTT connect success in 1908 ms
AIOT_MQTTEVT_CONNECT
```
云端查看创建的主设备及子设备是否显示在线。

# 5. 总结
本用例简单介绍了如何使用AliOS Things及HaaS开发板进行端云一体网关设备的开发。了解更多连云相关知识请参考Link SDK[编程手册](https://help.aliyun.com/document_detail/163772.html?spm=a2c4g.11186623.6.576.50e76ba7rkfLbp)。
