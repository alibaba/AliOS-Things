@page linksdk_gateway_demo linksdk_gateway_demo

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

# 2.1 基础目录结构
```sh

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

# 3.1 HaaS100 硬件

[HaaS100 硬件简介](https://help.aliyun.com/document_detail/184426.html)

<img src="https://img.alicdn.com/imgextra/i4/O1CN01XxD6Xo217CB3FZnEU_!!6000000006937-2-tps-746-497.png" style="zoom:80%;" />

# 4. 案例实现

## 4.1 硬件连接
该案例只需要连接电源线以及串口线，如下图所示：
![硬件连接.png](https://img.alicdn.com/imgextra/i3/O1CN01tPYjF31bqpdGkFbdD_!!6000000003517-0-tps-4032-3024.jpg)

注意：由于本案例需要使用Wi-Fi联网，需将标有2.4G/5.8G的天线连接到HaaS100 靠近USB口的天线端子上，如上图所示。
## 4.2 软件实现
### 4.2.1 云端创建产品
请先点击登陆[物联网平台](https://www.aliyun.com/product/iot/iot_instc_public_cn)（未注册阿里云账户的用户，请先完成账户注册），按下面步骤一步步去打造
1、创建项目
注册登入后，如下图所示，前往管理控制台。
![image.png](https://img.alicdn.com/imgextra/i4/O1CN01BN7DMd1IibotD78f6_!!6000000000927-2-tps-1308-490.png#align=left&display=inline&height=170&margin=%5Bobject%20Object%5D&name=image.png&originHeight=490&originWidth=1308&size=276942&status=done&style=none&width=453#align=left&display=inline&height=490&margin=%5Bobject%20Object%5D&originHeight=490&originWidth=1308&status=done&style=none&width=1308)
点击公共实例，进行产品创建
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01AjRz9z294Sk2dsMXe_!!6000000008014-2-tps-1328-792.png#align=left&display=inline&height=302&margin=%5Bobject%20Object%5D&name=image.png&originHeight=792&originWidth=1328&size=142752&status=done&style=none&width=506#align=left&display=inline&height=792&margin=%5Bobject%20Object%5D&originHeight=792&originWidth=1328&status=done&style=none&width=1328)
2、创建网关演示产品
点击创建产品，见下图所示，这里创建了一个名称“haas_gateway”产品。
![image.png](https://img.alicdn.com/imgextra/i1/O1CN01vpxC1F1FQnZfx4Znh_!!6000000000482-2-tps-1470-1788.png#align=left&display=inline&height=239&margin=%5Bobject%20Object%5D&name=image.png&originHeight=600&originWidth=1490&size=211557&status=done&style=none&width=594)
3、创建网关设备
在设备列表中增加对应产品的设备。
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01a8mOHP1zd8ZL5UYE0_!!6000000006736-2-tps-1392-624.png#align=left&display=inline&height=247&margin=%5Bobject%20Object%5D&name=image.png&originHeight=874&originWidth=1910&size=364503&status=done&style=none&width=540)
4、创建子设备演示产品
点击创建产品，见下图所示，这里创建了一个名称“haas_subdev”产品。
![image.png](https://img.alicdn.com/imgextra/i3/O1CN01ur5LgC1h3GSlStLvf_!!6000000004221-2-tps-1504-1764.png#align=left&display=inline&height=239&margin=%5Bobject%20Object%5D&name=image.png&originHeight=600&originWidth=1490&size=211557&status=done&style=none&width=594)
5、创建多个子设备
在设备列表中增加对应产品的设备。
![image.png](https://img.alicdn.com/imgextra/i4/O1CN019rocNe1hsYp8IBufJ_!!6000000004333-2-tps-1816-738.png#align=left&display=inline&height=247&margin=%5Bobject%20Object%5D&name=image.png&originHeight=874&originWidth=1910&size=364503&status=done&style=none&width=540)

创建后的设备三元组信息需要同步到设备端的开发代码段中，在4.2.2章节会介绍。待设备端开发结束，就可以在物联网平台中参看设备在线状态。
### 4.2.2 设备端代码
演示用例路径为solutions/linksdk_gateway_demo/data_model_basic_demo.c 此演示用例演示Link SDK物模型单品上云的功能。
1.修改网关三元组，使用4.2.1章节得到的三元组填入代码如下区域：
```sh
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
## 4.3 编译下载
### 编译
```sh
cd solutions/linksdk_gateway_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make linksdk_gateway_demo@haas100 -c config
```

### 烧录镜像命令
```sh
aos burn
```

烧写步骤可以参考 [HaaS100快速开始](https://help.aliyun.com/document_detail/184184.html?spm=a2c4g.11186623.6.642.717c6489icKeP3)

## 4.4 调试

> CLI命令行输入联网命令：
```sh
netmgr -t wifi -c ssid password
```
注意：ssid和password需要替换成自己的路由的ssid及密码

连上路由器后会自动连接到阿里云物联网平台。

### 4.4.1 关键日志
> CLI日志：
```sh
success to establish mbedtls connection, fd = 3(cost 0 bytes in total, max used 0 bytes)
[104.526][LK-0313] MQTT connect success in 1908 ms
AIOT_MQTTEVT_CONNECT
```
> 云端查看创建的设备是否显示在线。

# 5. 总结
本用例简单介绍了如何使用AliOS Things及HaaS开发板进行端云一体网关设备的开发。了解更多连云相关知识请参考Link SDK[编程手册](https://help.aliyun.com/document_detail/163772.html?spm=a2c4g.11186623.6.576.50e76ba7rkfLbp)。
