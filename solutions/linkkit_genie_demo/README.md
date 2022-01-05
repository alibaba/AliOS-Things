@page linkkit_genie_demo 天猫精灵Wi-Fi IoT设备示例

[更正文档](https://gitee.com/alios-things/linkkit_genie_demo/edit/rel_3.3.0/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 1. 案例简介
国内智能音箱IoT生态中，天猫精灵开放了天猫精灵开放平台可以让开发者学习如何开发一款智能设备并接入天猫精灵的生态中。<br>

linkkit_genie_demo则是基于HaaS200开发板通过Wi-Fi直连功能接入天猫精灵IoT平台的案例。
其完成的主要功能包括：
- 零配（通过对天猫精灵说“天猫精灵，找队友”）方式配网
- 通过天猫精灵平台（飞燕平台）进行IoT设备控制
- 上传设备状态信息到天猫精灵平台（飞燕平台）

该示例的运行依赖下述基本功能完成对接：
- cli组件
- kv组件
- linkkit组件
- netmgr网络管理组件

使用天猫精灵零配功能对IoT设备配网的流程请参考下图，下图中第1到第5步交互完成之后 ，IoT设备(插座)就可以获取到Wi-Fi的SSID和Password信息，并在连接到本地路由器之后连接到天猫精灵云平台。之后便可以通过语音或天猫精灵app对IoT设备进行控制了。

<div align="center">
<img src=https://img.alicdn.com/imgextra/i4/O1CN014lIX2N1oddGin0onQ_!!6000000005248-2-tps-1918-1134.png width=70%/>
</div>

# 2. 基础目录结构

```tree
.
├── Makefile          # aos make编译时入口
├── README.md         # 案例使用说明文档
├── SConstruct        # Makefile => Scon => aostools
├── genie_demo.c      # 案例程序的主要逻辑，入口**application_start**
├── k_app_config.h    # 内核组件的配置开关，优先级低于**k_config.h**
├── maintask.c        # 系统主任务入口处理，入口**aos_maintask**
└── package.yaml      # 编译系统配置文件
```

# 3. 操作步骤

## 3.1 硬件介绍
本案例只需要HaaS200开发板一台，除了使用HaaS200的Wi-Fi功能进行配网和连接阿里云生活物联网平台外，还会使用天猫精灵控制HaaS200开发板上的红色LED灯。

### 3.1.1 HaaS200开发板
HaaS200是一款由阿里云智能IoT HaaS团队认证的高性能、同时具有蓝牙/Wi-Fi连接能力的物联网开发板。其内嵌HaaS201核心模组，包含一个KM4内核的高性能MCU和一个KM0内核的低功耗MCU；自带512KB SRAM，4MB PSRAM和外置4MB FLASH；集成有双频Wi-Fi 2.4G/5G 和蓝牙5.0。搭载全新的AliOS Things 3.3操作系统和HaaS轻应用开发框架，以及丰富的接口资源，可以满足更多应用开发需求。其外观如下图所示：

<div align="center">
<img src=https://img.alicdn.com/imgextra/i3/O1CN016W3VE61zqsUvE7YgO_!!6000000006766-2-tps-600-297.png width=70%/>
</div>

HaaS200有着丰富的外设接口，如下图所示：
<div align="center">
<img src=https://img.alicdn.com/imgextra/i2/O1CN01QinTOH1kkGXckQegR_!!6000000004721-2-tps-514-462.png width=70%/>
</div>

HaaS200开发板的红色LED和HaaS200的GPIO PA_28相连，所以在收到天猫精灵云平台发送的控制灯开关的指令的时候，我们只需要去控制GPIO PA_28就可以控制开发板红色LED的亮灭。

## 3.2 生活物联网平台创建产品
请先点击登陆[生活物联网平台](https://living.aliyun.com/)（未注册阿里云账户的用户，请先完成账户注册），按下面步骤一步步操作。

### 3.2.1 创建项目
注册登入后，如下图所示的步骤创建一个名为“智能灯”的“天猫精灵生态项目”。

<div align="center">
<img src=https://img.alicdn.com/imgextra/i2/O1CN01YF1qnx26toLN9mvRU_!!6000000007720-2-tps-1246-520.png width=70%/>
</div>

项目创建成功之后会自动进入到项目的产品列表页面，此时产品列表是空的，如下图所示：

<div align="center">
<img src=https://img.alicdn.com/imgextra/i2/O1CN01EMY6uq1GlhJ6RU4rS_!!6000000000663-2-tps-1194-636.png width=70%/>
</div>

### 3.2.2 创建产品
此时点击上图中的“创建新产品”创建一个品类为"电工照明/灯"名称为“测试灯”，连接方式为Wi-Fi的产品，如下图所示。

<div align="center">
<img src=https://img.alicdn.com/imgextra/i4/O1CN01MuByPs1iQuZa798ck_!!6000000004408-2-tps-548-656.png width=70%/>
</div>

产品创建成功后系统提示如下图。
<div align="center">
<img src=https://img.alicdn.com/imgextra/i4/O1CN012VCKAR1YI4m89zsJP_!!6000000003035-2-tps-777-262.png width=70%/>
</div>

点击“立即前往”，此时系统会进入下图所示的产品功能定义页面。因为前面我们选择的是“照明产品/灯”的品类，所以该产品会默认继承这个品类的默认物模型属性。如果云端需要控制HaaS200开灯的时候就会在MQTT消息的payload中带入{"powerstate":1}的内容；同样的如果U云端需要控制HaaS200进行关灯的时候，就会在MQTT消息的payload中带入{"powerstate":0}。
> 物模型是用来描述一个物体的数字化定义。关于物模型的详细信息请读者自行到搜索引擎搜索相关信息。

如果所选品类的物模型不能满足读者的需求，读者也可以点击”添加功能“按钮自定义符合自己需求的物模型属性。

<div align="center">
<img src=https://img.alicdn.com/imgextra/i2/O1CN01a5tstJ1xXyvLufe3l_!!6000000006454-2-tps-1422-818.png width=70%/>
</div>

然后点击”下一步“会进入到”人机交互“的设计页面，本案例中不需UI，因此可以忽略这个页面的设定。
<div align="center">
<img src=https://img.alicdn.com/imgextra/i4/O1CN01gFLFsE1mhdV1ttFei_!!6000000004986-2-tps-3002-1687.png width=70%/>
</div>


直接点击”下一步“进入到模组选择页面，本案例仅仅是功能演示随便选择其中一款即可。如果需要对此项目进行量产，请直接联系天猫精灵上午团队沟通上午合作流程。

<div align="center">
<img src=https://img.alicdn.com/imgextra/i2/O1CN01qi7t1M1nBrdqnDW2b_!!6000000005052-2-tps-1606-902.png width=70%/>
</div>

选择好模组之后会进入到”设备调试页面“，如下图所示。
<div align="center">
<img src=https://img.alicdn.com/imgextra/i2/O1CN011kvfcK1R8jNlenwoA_!!6000000002067-2-tps-1598-904.png width=70%/>
</div>

### 3.2.3 创建测试设备

点击上图中的”新建测试设备“，为测试设备起一个名为"light_demo"的名字。读者也可以留空，让系统随机生成一个名称。
<div align="center">
<img src=https://img.alicdn.com/imgextra/i2/O1CN01qi7t1M1nBrdqnDW2b_!!6000000005052-2-tps-1606-902.png width=70%/>
</div>

测试设备创建成功后，系统会自动弹出设备标识符页面，如下图所示。其中ProductKey、DeviceName、DeviceSecret和产品秘钥ProductSecret在云端代表唯一一台设备，也成为设备的唯一标识符（或三元组）。
<div align="center">
<img src=https://img.alicdn.com/imgextra/i1/O1CN01mc90AD1EL10BT67Rk_!!6000000000334-2-tps-654-414.png width=70%/>
</div>

此时测试设备列表中变会出现上面创建的”light_demo“设备。上图中只看到了ProductKey、DeviceName、DeviceSecret三个信息。点击下图右边”Product Secret“后的显示就可以看到ProductSecret的值。
<div align="center">
<img src=https://img.alicdn.com/imgextra/i1/O1CN01nIHkXt1XXolxMaz3J_!!6000000002934-2-tps-1649-941.png width=70%/>
</div>

此时云端的设备就创建成功了。创建成功后，在HaaS200实际设备未和云端建立连线之前，此设备处于”未激活“的状态。

## 3.3 设备端软件配置
### 3.3.1 设备标识符配置
代码位置:solution/linkkit_genie_demo/genie_demo.c
将上一步中云端生成的设备标识符（ProductKey、ProductSecret、DeviceName、DeviceSecret）逐一复制替换下面宏定义中的xx。

```
#define PRODUCT_KEY      "xx"
#define PRODUCT_SECRET   "xx"
#define DEVICE_NAME      "xx"
#define DEVICE_SECRET    "xx"
```

### 3.3.2 设备端功能讲解
***设备接收到云端消息***后解析消息并对LED进行控制的代码及说明如下。其完成的主要功能为：
1. 解析云端下发的控制指令(json格式控制字串)
2. 根据云端下发的指令控制LED的亮灭
```
/** recv property set command from cloud **/
static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    cJSON *root = NULL, *power_state = NULL;
    EXAMPLE_TRACE("Property Set Received, Request: %s", request);

    /* parse json string */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }

    /* check whether powerstate control command exist or not */
    power_state = cJSON_GetObjectItem(root, "powerstate");
    if (power_state == NULL) {
        goto out;
    }
    EXAMPLE_TRACE("powerstate = %d", power_state->valueint);
    if (power_state->valueint) {
        led_state = 1;
    } else
        led_state = 0;

    // turn on/off the LED according to the command from cloud
    EXAMPLE_TRACE("turn %s the LED", led_state ? "on" : "off");
    led_ctrl(led_state);

    // post device's property back to the cloud
    user_post_property();
out:
    cJSON_Delete(root);

    return 0;
}
```
通过其它物模型属性进行设备控制的逻辑和控制LED等的逻辑链路是相同的，也就是说在收到云端的控制消息的时候都是在user_property_set_event_handler中去解析json字串，并根据产品功能的需求对外设做出控制。

***设备向云端发送设备属性信息***如下图所示。其主要功能如下：
* 读取LED状态变量led_state，并将其封装成json格式的字串{"powerstate": led_state}并送给云端(IOT_Linkkit_Report(id, ITM_MSG_POST_PROPERTY, payload, len))。

```
// report device's property to the cloud
void user_post_property(void)
{
    int res = 0;

    char property_payload[30] = {0};
    HAL_Snprintf(property_payload, sizeof(property_payload), "{\"powerstate\": %d}", led_state);

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload, strlen(property_payload));

    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}
```
您如果想更新其它设备属性信息到云端也可以在将对应属性组合成符合json格式的淄川后，调用IOT_Linkkit_Report(id, ITM_MSG_POST_PROPERTY, payload, len)将设备属性送到云端。

## 4.3 开发环境搭建、编译、烧入、运行
### 4.3.1 AliOS Things开发环境搭建
开发环境的搭建请参考[《AliOS Things集成开发环境使用说明之搭建开发环境》](https://help.aliyun.com/document_detail/302378.html)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

### 4.3.3 代码编译、烧录
编译linkkit_genie_demo的过程请参考如下链接：

-- 编译固件可参考[《AliOS Things集成开发环境使用说明之编译固件》](https://help.aliyun.com/document_detail/302384.html)。

-- 烧录固件可参考[《AliOS Things集成开发环境使用说明之烧录固件》](https://help.aliyun.com/document_detail/302383.html)。

### 4.3.4 运行
HaaS200系统启动后，在HaaS200 KV存储中没有保存配网信息(Wi-Fi的SSID和Password)的情况下，系统会自动进入配网模式。)
此时参考下图的步骤让天猫精灵找队友，就可以找到一台智能灯，让天猫精灵对其配网，配网成功后，在测试产品列表中就可以看到设备状态变为”在线"。

<div align="center">
<img src=https://img.alicdn.com/imgextra/i4/O1CN014lIX2N1oddGin0onQ_!!6000000005248-2-tps-1918-1134.png width=70%/>
</div>

此时对天猫精灵说”天猫精灵，开灯“，精灵云平台就会下发内容为{"powerstate":1}的MQTT消息，同时会呼叫user_property_set_event_handler函数，在此函数中解析此json字串，并打开红色LED。

同样的如果对天猫精灵说”天猫精灵，关灯“，精灵云平台就会下发内容为{"powerstate":0}的MQTT消息，同时会呼叫user_property_set_event_handler函数，在此函数中解析此json字串，并关闭红色LED。

> 如果读者自定义了物模型的属性信息，用天猫精灵控制对应的属性状态，精灵云则会在下发的MQTT消息中带上自定义属性的标识符及其目标值。读者可以按照json字串对其进行解析并完成实际的操作请求。


> 除了通过语音控制开关灯之外，还可以通过天猫精灵手机App或天猫精灵带屏音箱的”我家“页面中进去找到这款智能灯，通过App或者是带屏音箱的UI去远程控制智能等。

如果想清除配网设备则可以在串口中敲入如下两条指令删除已配网的SSID/Password信息。

```
kv del wifi_ssid
kv del wifi_password
```

## 4.3 如何更换三元组
每个IoT产品的三元组是不一样的，如果想在同一块设备上面进行不同设备的测试则需要使用目标测试设备的三元组信息，有两种方式可以使用：
* 使用如下的命令行修改kv中的三元组信息, <xxxx>为云端生成设备的三元组信息。
```
kv set hal_devinfo_dn <DeviceName>
kv set hal_devinfo_ds <DeviceSecret>
kv set hal_devinfo_pk <ProductKey>
kv set hal_devinfo_ps <ProductSecret>
```

* 将云端生成的设备标识符（ProductKey、ProductSecret、DeviceName、DeviceSecret）逐一复制替换下面宏定义中的xx（代码位置:solution/linkkit_genie_demo/genie_demo.c）。

```
#define PRODUCT_KEY      "xx"
#define PRODUCT_SECRET   "xx"
#define DEVICE_NAME      "xx"
#define DEVICE_SECRET    "xx"
```

这两种方式的逻辑优先级可以参考名为set_iot_device_info的函数，在kv中找不到device secret信息找不到的情况下才会使用到genie_demo.c中预设的三元组信息。所以在使用第二种方式更改三元组信息的时候可以先在命令行中输入"kv del DeviceSecret"将设备密钥清除以确保预设的三元组信息可以保存到kv系统中。
```
// make sure device's signature is correct
static void set_iot_device_info(void)
{
    char _device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};

    HAL_GetDeviceSecret(_device_secret);
    // if device secret does not exist, store pk/ps/dn/ds into kv system
    if (!strlen(_device_secret)) {
        HAL_SetProductKey(PRODUCT_KEY);
        HAL_SetProductSecret(PRODUCT_SECRET);
        HAL_SetDeviceName(DEVICE_NAME);
        HAL_SetDeviceSecret(DEVICE_SECRET);
    }

    return;
}
```

# 6. 总结
linkkit_genie_demo主要完成了通过Wi-Fi直连的方式接入天猫精灵IoT生态的案例。您可以此solution为基础，开始添加您自己的IoT设备，并定制修改自身需要的APP。
