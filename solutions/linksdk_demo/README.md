@page linksdk_demo linksdk_demo

# 1. 案例简介
linksdk_demo是Link SDK物模型单品上云最简示例。
其完成的主要功能包括：
- 系统板级初始化
- 内核基础组件初始化
- 网络配置
- 网络配置成功（获取ip后）创建Link SDK主线程
- Link SDK参数配置
- Link SDK初始化
- 创建独立线程用于MQTT保活及QoS1消息重发
- 创建独立线程用于下行数据接收
- 在Link SDK主任务中定时上报属性和事件

该示例的运行依赖下述基本功能完成对接：
- AOS API
- LwIP
- MbedTLS
- Link SDK

# 2. 基础知识

# 2.1 基础目录结构
```sh
.
├── data_model_basic_demo.c  # Link SDK 物模型单品演示例程
├── main.c                   # 网络配置及SDK启动入口
├── maintask.c               # 系统主任务入口处理，入口**aos_maintask**
├── Makefile                 # aos make编译时入口
├── package.yaml             # 编译系统配置文件
├── README.md                # 本说明文档
└── SConstruct               # Makefile => Scon => aostools
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
2、创建灯演示产品
点击创建产品，见下图所示，这里创建了一个名称“HaaS_Light”产品。
![image.png](https://img.alicdn.com/imgextra/i4/O1CN01JHVICa1e8M1CVLuQ4_!!6000000003826-2-tps-1522-1768.png#align=left&display=inline&height=1768&margin=%5Bobject%20Object%5D&originHeight=1768&originWidth=1522&size=0&status=done&style=none&width=1522)
3、设备管理
在设备列表中增加对应产品的设备。
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01QipCrM1YxklnN7iIu_!!6000000003126-2-tps-2280-1276.png#align=left&display=inline&height=1276&margin=%5Bobject%20Object%5D&originHeight=1276&originWidth=2280&size=0&status=done&style=none&width=2280)
查看设备信息,并复制三元组用于设备端编程
![image.png](https://img.alicdn.com/imgextra/i3/O1CN01uCSOTD29nLYXjgsRm_!!6000000008112-2-tps-2512-1204.png#align=left&display=inline&height=1204&margin=%5Bobject%20Object%5D&originHeight=1204&originWidth=2512&size=0&status=done&style=none&width=2512)
创建后的设备三元组信息需要同步到设备端的开发代码段中，在4.2.2章节会介绍。待设备端开发结束，就可以在物联网平台中参看设备在线状态。
### 4.2.2 设备端代码
演示用例路径为solutions/linksdk_demo/data_model_basic_demo.c 此演示用例演示Link SDK物模型单品上云的功能。
1.修改三元组，使用4.2.1章节得到的三元组填入代码如下区域：
```sh
int demo_main(int argc, char *argv[])
{
...
    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "此处请填入product_key";
    char *device_name       = "此处请填入device_name";
    char *device_secret     = "此处请填入device_secret";
...
```
## 4.3 编译下载
### 编译
```sh
cd solutions/linksdk_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make linksdk_demo@haas100 -c config
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
### 4.4.2 云端状态
在云端设备管理中，可以看到刚创建的设备状态由未激活变成了在线，说明此设备已成功上云。
![](https://img.alicdn.com/imgextra/i4/O1CN01DLB3xo1sPfbw46myX_!!6000000005759-2-tps-1234-482.png#align=left&display=inline&height=482&margin=%5Bobject%20Object%5D&originHeight=482&originWidth=1234&size=0&status=done&style=none&width=1234)

# 5. 扩展开发
上面案例我们仅通过修改三元组就让设备连接到了物联网平台。如何对设备进行功能开发，将数据从云端推送到设备或从设备推送到云端呢？
## 5.1 云端下发属性
### 5.1.2 设备端处理
设置云端数据回调函数：
```c
int demo_main(int argc, char *argv[])
{
   ...
   /* 配置消息接收处理回调函数 */
    aiot_dm_setopt(dm_handle, AIOT_DMOPT_RECV_HANDLER, (void *)demo_dm_recv_handler);
   ...
}

```
在用户回调函数中捕获AIOT_DMRECV_PROPERTY_SET事件，即云端设置属性的事件：
```c
    /* 用户数据接收处理回调函数 */
static void demo_dm_recv_handler(void *dm_handle, const aiot_dm_recv_t *recv, void *userdata)
{
    printf("demo_dm_recv_handler, type = %d\r\n", recv->type);

    switch (recv->type) {
        /* 属性设置 */
        case AIOT_DMRECV_PROPERTY_SET: {
            printf("msg_id = %ld, params = %.*s\r\n",
                   (unsigned long)recv->data.property_set.msg_id,
                   recv->data.property_set.params_len,
                   recv->data.property_set.params);

            /* TODO: 以下代码演示如何对来自云平台的属性设置指令进行应答, 用户可取消注释查看演示效果 */
            {
                aiot_dm_msg_t msg;

                memset(&msg, 0, sizeof(aiot_dm_msg_t));
                msg.type = AIOT_DMMSG_PROPERTY_SET_REPLY;
                msg.data.property_set_reply.msg_id = recv->data.property_set.msg_id;
                msg.data.property_set_reply.code = 200;
                msg.data.property_set_reply.data = "{}";
                int32_t res = aiot_dm_send(dm_handle, &msg);
                if (res < 0) {
                    printf("aiot_dm_send failed\r\n");
                }
            }
        }
        break;
        ...
  }
```

### 5.1.3 云端下发
在设备界面点击在线调试功能
![](https://img.alicdn.com/imgextra/i4/O1CN01cy33YO1Hnoi7A8vWV_!!6000000000803-2-tps-2176-536.png#align=left&display=inline&height=536&margin=%5Bobject%20Object%5D&originHeight=536&originWidth=2176&size=0&status=done&style=none&width=2176)
跳转到在线调试页面，进行属性调试
![](https://img.alicdn.com/imgextra/i2/O1CN01CWWBIG1dzC5unT1R7_!!6000000003806-2-tps-1934-976.png#align=left&display=inline&height=976&margin=%5Bobject%20Object%5D&originHeight=976&originWidth=1934&size=0&status=done&style=none&width=1934)


### 5.1.3 查看设备端日志
如下所示， {"LightSwitch":1}的命令已经下发到设备端，开发者可以对此json字符串进行解析并用于设备控制。
```c
[1606.833][LK-0309] pub: /sys/a1duSiRzIXc/light01/thing/service/property/set

[LK-030A] < 7B 22 6D 65 74 68 6F 64  22 3A 22 74 68 69 6E 67 | {"method":"thing
[LK-030A] < 2E 73 65 72 76 69 63 65  2E 70 72 6F 70 65 72 74 | .service.propert
[LK-030A] < 79 2E 73 65 74 22 2C 22  69 64 22 3A 22 31 35 39 | y.set","id":"159
[LK-030A] < 30 36 35 31 30 33 31 22  2C 22 70 61 72 61 6D 73 | 0651031","params
[LK-030A] < 22 3A 7B 22 4C 69 67 68  74 53 77 69 74 63 68 22 | ":{"LightSwitch"
[LK-030A] < 3A 31 7D 2C 22 76 65 72  73 69 6F 6E 22 3A 22 31 | :1},"version":"1
[LK-030A] < 2E 30 2E 30 22 7D                                | .0.0"}

[1606.835][LK-0A08] DM recv property set
demo_dm_recv_handler, type = 1
msg_id = 1590651031, params = {"LightSwitch":1}


```


## 5.2 设备上报属性
### 5.2.1 设备端处理
为了演示方便，我们在主任务中定时上报属性。在具体业务中，属性上报可在其他线程中触发。
```c
    /* 主循环进入休眠 */
    while (1) {
        /* TODO: 以下代码演示了简单的属性上报和事件上报, 用户可取消注释观察演示效果 */
        demo_send_property_post(dm_handle, "{\"LightSwitch\": 0}");
        aos_msleep(10000);
    }
```
### 5.2.2 云端查看上报数据
在监控运维-日志服务中，可以查看设备上报的所有数据。
![](https://img.alicdn.com/imgextra/i3/O1CN013cTaUO1q5rFSx3SZC_!!6000000005445-2-tps-2648-1526.png#align=left&display=inline&height=1526&margin=%5Bobject%20Object%5D&originHeight=1526&originWidth=2648&size=0&status=done&style=none&width=2648)

另外。物模型中主要数据类型处了属性外，还有服务和事件。具体详情请参考[官网文档](https://help.aliyun.com/document_detail/73727.html?spm=5176.11065259.1996646101.searchclickresult.40dd49deCWMVaW)。

# 6. 总结
本用例简单介绍了如何使用AliOS Things及HaaS开发板进行端云一体开发。了解更多连云相关知识请参考Link SDK[编程手册](https://help.aliyun.com/document_detail/163772.html?spm=a2c4g.11186623.6.576.50e76ba7rkfLbp)。
