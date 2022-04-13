@page ble_netconfig ble_netconfig


# 组件概述
AliOS Things 3.3提供ble_netconfig组件，用内置的BLE功能给WI-FI配网。


## 功能支持
ble_netconfig组件主要支持如下功能：

- 开启BLE配网功能
- 关闭BLE配网功能


## 版权说明
> Apache license v2.0


## 目录结构


```
|-- ble_netconfig.c          #BLE配网核心代码
|-- ble_netconfig.h          #BLE配网头文件
|-- package.yaml             #makefile
|-- README.md                #README文档
|-- BLE_NetConfig.zip        #BLE配网的示例APK,适用于android6.0及以上机型
```



## 依赖组件

- osal_aos
- ble_host
- netmgr



# 常用配置
无


# API介绍
## API列表
|  BLE_NetCfg_init | BLE配网初始化 |
| :--- | :--- |
|  BLE_NetCfg_start | BLE配网功能开启 |
| BLE_NetCfg_stop | BLE配网功能关闭 |



## API详情
### BLE_NetCfg_init
BLE配网功能初始化。
**函数原型**


```
BLE_NETCFG_STATE BLE_NetCfg_init(BLE_netCfg_callck callback)
```
**输入参数**

| args | description |
| :--- | :--- |
| callback | 回调函数 |

**返回参数**
`0：`成功， `其他值`：失败。


### BLE_NetCfg_start
打开BLE配网功能。

**函数原型**


```
BLE_NETCFG_STATE BLE_NetCfg_start(void)
```
**输入参数**

| args | description |
| :--- | :--- |
| 无 |  |

**返回参数**
`0：`成功， `其他值`：失败。


### BLE_NetCfg_stop
关闭BLE配网功能。
**函数原型**


```
BLE_NETCFG_STATE BLE_NetCfg_stop(void)
```
**输入参数**

| args | description |
| :--- | :--- |
| 无 |  |

**返回参数**
`0：`成功， `其他值`：失败。

# 使用示例
## 案例工具

- 串口工具
- 安卓BLE配网测试APK（BLE_NetConfig.apk）



## 案例修改
以helloworld的案例为例，修改solutions/helloworld_demo/package.yaml的depends，增加ble_netconfig，如下
```
depends:
  - ble_netconfig: master
```
修改solutions/helloworld_demo/helloworld_demo.c，如下：
**头文件修改**
```
#include <ble_netconfig.h>
```
**代码修改**
```
int application_start(int argc, char *argv[])
{
    int count = 0;
    int ret;

    printf("nano entry here!\r\n");

#if 1
    /* bt stack init */
    ret = BLE_NetCfg_init(NULL);
    if (ret) {
        printf("error: BLE Net Config initial!, ret = %x\r\n", ret);
        return -1;
    }
#endif

    while(1) {
        printf("hello world! count %d \r\n", count++);
        aos_msleep(1000);
    };
}
```


## 编译烧录
在solutions/helloworld_demo中执行aos make，编译成功后烧录进haas100开发板。


## 运行观测
打开串口工具，在串口工具中输入ble_netCfg，可看到ble_netCfg的cli命令集。
![image.png](https://img.alicdn.com/imgextra/i4/O1CN01R0MwTj1cIJOanG8Sq_!!6000000003577-2-tps-832-98.png#align=left&display=inline&height=49&margin=%5Bobject%20Object%5D&name=image.png&originHeight=98&originWidth=832&size=25099&status=done&style=none&width=416)
## BLE配网示例
在串口工具中输入ble_netCfg start，可以在串口日志中看到如下打印
```
[15:10:06.498]收←◆ble_netCfg start
[ 141.509]<E>BLE_NETCFG BLE_NetCfg_start: state = 0
[ 145.532]<E>BLE_NETCFG adv start!
```
之后，在测试的安卓手机上执行如下操作。

- 解压BLE_NetConfig.zip后，在android手机(版本6.0及以上)侧安装并打开BLE_NetConfig.apk。
- 手机靠近开发板5米内，手机端点击FINDDEVICE按钮，可以在下拉列表中找到“HaaS NetConfig”这台设备并选择这个设备。
- 在SSID和Password区域填写需要连接的路由器的SSID和密码。
- 点击SETTING按钮，等待显示“**设置SSID与密码成功”**的提示。

![image.png](https://img.alicdn.com/imgextra/i2/O1CN01ea2Bma1VtgP4ELRs6_!!6000000002711-2-tps-664-858.png#align=left&display=inline&height=429&margin=%5Bobject%20Object%5D&name=image.png&originHeight=858&originWidth=664&size=104536&status=done&style=none&width=332)


# 常见问题
```
Q：是否有IOS版的配网工具
A：目前没有，后续推出跨平台的小程序用于配网。
```
