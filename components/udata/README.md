@page udata udata

# 概述
uData 框架设计思想是基于传统 sensorhub 概念基础之上的，结合IoT的业务场景和 AliOS  Things 物联网操作系统的特点设计而成的一个面对 IoT 的感知设备处理框架。uData主要分kernel和framework两层，kernel层主要是负责传感器驱动和相关的静态校准，包括轴向校准等；framework层主要是负责应用服务管理、动态校准管理和对外模块接口等。
## 组件支持以下功能：
- 传感器框架
- 集成传感器驱动
- 静态校准
- 动态校准算法
- 数据到云

## 版权信息
> Apache license v2.0

## 目录结构
```sh
.
├── abs_data_model  # abs layer
├── cali_data       # calibration algorithm
├── include
├── service         # application services
└── service_mgr     # service manager
```

## 依赖组件
* ulog
* vfs
* sensor

# 常用配置
系统中相关配置已有默认值，一般无需修改配置

# API说明
@ref udata_init()
udata框架初始化

@ref udata_register_msg_handler()
注册接收传感器数据的消息句柄函数

@ref udata_subscribe()
订阅传感器服务

@ref udata_report_publish()
读取传感器数据

# 使用示例 -- uData 本地演示
基于 uData 获取传感器数据，并在本地进行打印，参考 example/udata_cloud_demo.c

以运行 helloworld_demo 为例，具体步骤如下：

## 1. 选择要使用的开发板
> solutions/helloworld_demo/package.yaml 基础信息 solution 中配置要使用的开发板，默认是 haas100，如果使用 haaseduk1，修改如下
```sh
solution:
  cpu_id: cpu0
  board_name: haaseduk1
```

## 2. 添加示例代码
> solutions/helloworld_demo/package.yaml 配置信息 def_config 中添加要运行的 example 示例代码和 Sensor 设备驱动的宏。例如温湿度传感器 si7006 数据并在本地进行打印：
```sh
def_config:
  AOS_UDATA_LOCAL_DEMO_ENABLE: 1
  AOS_SENSOR_HUMI_SI_SI7006: 1
  AOS_SENSOR_TEMP_SI_SI7006: 1
```

## 3. 添加 udata 组件
> solutions/helloworld_demo/package.yaml 依赖信息 depends 中添加 udata 组件
```sh
depends:
  - cli: dev_aos
  - osal_aos: dev_aos
  - haaseduk1: dev_aos
  - udata: dev_aos
```

## 4. 编译
```sh
cd solutions/helloworld_demo && aos make
```

## 5. 烧录固件
> 参考具体板子的快速开始文档。

## 6. 示例测试
> CLI命令行输入：
```sh
udata_local_test
```

## 7. 关键日志
> CLI日志：
```sh
Temperature value :
Humidity value :
```

# 使用示例 -- uData 连云演示
基于uData 获取传感器数据，并通过 linkkit 上传到云端，参考 example/udata_cloud_demo.c

以运行 linksdk_demo 为例，具体步骤如下：

## 1. 选择要使用的开发板
> solutions/linksdk_demo/package.yaml 基础信息 solution 中配置要使用的开发板，默认是 haas100，如果使用 haaseduk1，修改如下
```sh
solution:
  cpu_id: cpu0
  board_name: haaseduk1
```

## 2. 添加示例代码
> 2.1 solutions/linksdk_demo/package.yaml 配置信息 def_config 中添加要运行的 example 示例代码和 Sensor 设备驱动的宏。例如温湿度传感器 si7006 数据并通过 linkkit 上传到云端：
```
def_config:
  AOS_UDATA_CLOUD_DEMO_ENABLE: 1
  AOS_SENSOR_HUMI_SI_SI7006: 1
  AOS_SENSOR_TEMP_SI_SI7006: 1
```
> 2.2 solutions/linksdk_demo/main.c 函数 entry_func 添加 udata_cloud_test_init() 函数调用

```
extern void udata_cloud_test_init(void);

static void entry_func(void *data)
{
    udata_cloud_test_init();
    demo_main(0 , NULL);
}
```
> 2.3 solutions/linksdk_demo/data_model_basic_demo.c 的 demo_main() 函数
- 替换自己在阿里云物联网平台上创建设备的三元组信息
```
int demo_main(int argc, char *argv[])
{
    ...

    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "a1eykua9RBq";
    char *device_name       = "haas-edu-k1-calon";
    char *device_secret     = "589bb9ff1d55675096ba6ba2e0d52392";

    ...
}
```
- 添加指针 g_dm_handle 声明并赋值
```
void *g_dm_handle = NULL;

int demo_main(int argc, char *argv[])
{
    ...

    /* 创建DATA-MODEL实例 */
    dm_handle = aiot_dm_init();
    if (dm_handle == NULL) {
        printf("aiot_dm_init failed");
        return -1;
    }
    g_dm_handle = dm_handle;

    ...
}
```
- 注释掉其它属性上报和事件上报，
```
int demo_main(int argc, char *argv[])
{
    ...

    /* 主循环进入休眠 */
    while (1) {
        /* TODO: 以下代码演示了简单的属性上报和事件上报, 用户可取消注释观察演示效果 */
        // demo_send_property_post(dm_handle, "{\"LightSwitch\": 0}");
        // demo_send_event_post(dm_handle, "Error", "{\"ErrorCode\": 0}");

        aos_msleep(1000);
    }

    ...
}
```
## 3. 添加 udata 组件
> solutions/linksdk_demo/package.yaml 依赖信息 depends 中行末添加 udata 组件
```sh
depends:
  - linksdk: dev_aos
  - haaseduk1: dev_aos
  - mbedtls: dev_aos
  - netmgr: dev_aos
  - udata: dev_aos
```

## 4. 编译
```sh
cd solutions/linksdk_demo && aos make
```

## 5. 烧录固件
> 参考具体板子的快速开始文档。

## 6. 示例测试
### 设备联网
> CLI命令行输入：打开连网成功后会自动保存AP信息的功能
```sh
netmgr -t wifi -b 1
```
> CLI命令行输入：netmgr -t wifi -c {ssid} {password} 连接名为ssid的路由器AP，其中 {ssid} {password}替换为自己的路由器配网信息
```sh
netmgr -t wifi -c my_wifi 12345678
```
> 设备在联网成功后会自动读取 Sensor 数据并上报到云端，同时可以看到设备在阿里云物联网平台处于在线状态，在日志服务中能查看设备上传到云端的日志信息

## 7. 关键日志
> CLI日志：
```
Temperature value : 44.0 centidegree
[172.620][LK-0309] pub: /sys/a1eykua9RBq/haas-edu-k1-calon/thing/event/property/post

[LK-030A] > 7B 22 69 64 22 3A 22 31  22 2C 22 76 65 72 73 69 | {"id":"1","versi
[LK-030A] > 6F 6E 22 3A 22 31 2E 30  22 2C 22 70 61 72 61 6D | on":"1.0","param
[LK-030A] > 73 22 3A 7B 22 43 75 72  72 65 6E 74 54 65 6D 70 | s":{"CurrentTemp
[LK-030A] > 65 72 61 74 75 72 65 22  3A 34 34 2E 30 7D 7D    | erature":44.0}}

[ 172.652]<A>AOS sensor:  humidity: 16.823683

[172.673][LK-0309] pub: /sys/a1eykua9RBq/haas-edu-k1-calon/thing/event/property/post_reply

[LK-030A] < 7B 22 63 6F 64 65 22 3A  32 30 30 2C 22 64 61 74 | {"code":200,"dat
[LK-030A] < 61 22 3A 7B 7D 2C 22 69  64 22 3A 22 31 22 2C 22 | a":{},"id":"1","
[LK-030A] < 6D 65 73 73 61 67 65 22  3A 22 73 75 63 63 65 73 | message":"succes
[LK-030A] < 73 22 2C 22 6D 65 74 68  6F 64 22 3A 22 74 68 69 | s","method":"thi
[LK-030A] < 6E 67 2E 65 76 65 6E 74  2E 70 72 6F 70 65 72 74 | ng.event.propert
[LK-030A] < 79 2E 70 6F 73 74 22 2C  22 76 65 72 73 69 6F 6E | y.post","version
[LK-030A] < 22 3A 22 31 2E 30 22 7D                          | ":"1.0"}

[172.674][LK-0A08] DM recv generic reply
demo_dm_recv_handler, type = 0
msg_id = 1, code = 200, data = {}, message = success
[ 174.620]<A>AOS sensor:  temperature: 44.023758
```

# 注意事项
sensor设备驱动 components/sensor/drv/drv_####_####_####.c 中 i2c_dev_t 结构体成员 port 要与 sensor_i2c_init() 初始化中的 port 相同。
```
void i2c_init(void)
{
    i2c_dev_t i2c_dev;
    i2c_dev.port                 = 1;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq          = I2C_BUS_BIT_RATES_100K;
    i2c_dev.config.mode          = I2C_MODE_MASTER;

    sensor_i2c_init(&i2c_dev);
}

i2c_dev_t ####_ctx = {
    .port                 = 1,
    .config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT,
    .config.freq          = I2C_BUS_BIT_RATES_100K,
    .config.mode          = I2C_MODE_MASTER,
    .config.dev_addr      = SI7006_I2C_ADDRESS,
};
```

# FAQ
Q1： 怎么自己添加特定型号的传感器驱动程序？
> 1. 参考 components/sensor/drv/drv_####_####_####.c 已集成的传感器设备驱动编写驱动文件；

> 2. components/sensor/drv/drv_init.c 中添加驱动程序的宏开关, 如
```sh
#if AOS_SENSOR_TEMP_SI_SI7006
    drv_temp_si_si7006_init();
#endif
```
> 3. components/sensor/package.yaml 中添加参与编译的源代码文件
```
source_file:
  - drv/drv_temp_humi_si_si7006.c ? <AOS_SENSOR_TEMP_SI_SI7006>
```
更具体的操作可以参考下面链接：
- [AliOS Things uData Framework Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Framework-Porting-Guide)

- [AliOS Things uData Sensor Driver Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Sensor-Driver-Porting-Guide)

- [AliOS Things uData第三方算法移植说明文档](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData%E7%AC%AC%E4%B8%89%E6%96%B9%E7%AE%97%E6%B3%95%E7%A7%BB%E6%A4%8D%E8%AF%B4%E6%98%8E%E6%96%87%E6%A1%A3)

- [传感器数据一键式上云的使用说明](https://github.com/alibaba/AliOS-Things/wiki/%E4%BC%A0%E6%84%9F%E5%99%A8%E6%95%B0%E6%8D%AE%E4%B8%80%E9%94%AE%E5%BC%8F%E4%B8%8A%E4%BA%91%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E)

- [基于AliOS Things Developer Kit开发板的外接传感器开发教程](https://github.com/alibaba/AliOS-Things/wiki/%E5%9F%BA%E4%BA%8EAliOS-Things-Developer-Kit%E5%BC%80%E5%8F%91%E6%9D%BF%E7%9A%84%E5%A4%96%E6%8E%A5%E4%BC%A0%E6%84%9F%E5%99%A8%E5%BC%80%E5%8F%91%E6%95%99%E7%A8%8B)
