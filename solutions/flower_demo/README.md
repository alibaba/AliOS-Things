@page flower_demo 智能养花：用钉钉小程序通过HaaS100监测空气温湿度示例

[更正文档](https://gitee.com/alios-things/flower_demo/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 1. 案例简介
过去，很多人一听嵌入式～～智能硬件～～就有点头大，甚至会感到一丝丝的恐惧。
其实不然，HaaS积木真的就能让智能硬件简单～好玩～普惠。
接下来，就以HaaS打造的花卉养植盒子系统为例，指引开发者如何自己一步步简单快速搭建起属于自己的好玩智能硬件。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01uTFPGy1WVEslbcHQ6_!!6000000002793-1-tps-1440-1080.gif" style="max-width:800px;" />
</div>

# 2. 基础知识
本案例主要依赖的硬件环境有：HaaS100、温湿度传感器。
## 2.1  HaaS100开发板
**HaaS100**是一款物联网场景中的标准开发板，并配套嵌入到硬件中的软件驱动及功能模块，为用户提供物联网设备高效开发服务。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN011t59YF25YubwEfz98_!!6000000007539-2-tps-443-443.png" style="max-width:800px;" />
</div>
HaaS100核心板有着丰富的外设接口，如下所示：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN015hi7UZ1WC0FTTOZBs_!!6000000002751-2-tps-586-886.png" style="max-width:800px;" />
</div>

详细的资料参考[HaaS 100 硬件规格](https://help.aliyun.com/document_detail/184186.html?spm=a2c4g.11186623.6.645.718015814zDYZt)。
## 2.2  温湿度传感器

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01G4JbCl1qcplUe5ynk_!!6000000005517-2-tps-1604-872.png" style="max-width:800px;" />
</div>

DHT11温湿度传感器的工作原理
### 2.2.1 工作时序
主机发开始信号-》DHT11响应输出-》DHT11数据信号。（平时默认为高电平，起始信号为低电平）

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN017lZ9eZ1cIlnMoyjU3_!!6000000003578-2-tps-1398-352.png" style="max-width:800px;" />
</div>

### 2.2.2 起始信号和响应信号的规则
主机发开始信号：低电平大于18ms，高电平20-40us；
DHT11的响应信号：低电平80us，高电平80us；

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01etKzgU1UNKoYVqtxo_!!6000000002505-2-tps-1592-508.png" style="max-width:800px;" />
</div>

### 2.2.3 数字信号的规则

一次完整的数据传输为40bit,高位先出。
数据格式:8bit湿度整数数据+8bit湿度小数数据+8bit温度整数数据+8bit温度小数数据+8bit校验和
校验和数据=“8bit湿度整数数据+8bit湿度小数数据+8bi温度整数数据+8bit温度小数数据”所得结果的末8位。

DHT11传感器如果没有接收到主机发送开始信号,不会主动进行温湿度采集。
数字‘0’信号：低电平50us，高电平26-28us

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01BVYeO41aQtaXWn8Di_!!6000000003325-2-tps-1564-850.png" style="max-width:800px;" />
</div>

数字‘1’信号：低电平50us，高电平70us

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01jH1ELf1Z8Hspm5NNC_!!6000000003149-2-tps-1574-730.png" style="max-width:800px;" />
</div>

# 3. 物料清单

| **物料** | **规格** | **购买链接** |
| :---: | :---: | :---: |
| HaaS100开发板 | - | 阿里云[天猫链接](https://detail.tmall.com/item.htm?spm=a230r.1.14.16.13123b209835rz&id=627354125244&ns=1&abbucket=16) |
| DHT11温湿度传感器 | 用来采集器空气中的温度和湿度，为单总线数据交互模式 |  Risym [天猫链接](https://detail.tmall.com/item.htm?id=15598344236&ali_refid=a3_430582_1006:1104520036:N:TAiHo9GD8eTurMzH21/csQ==:fc360fe0aaa2a823247db3c6a3bd21fc&ali_trackid=1_fc360fe0aaa2a823247db3c6a3bd21fc&spm=a230r.1.14.1)、TELESKY [天猫链接](https://detail.tmall.com/item.htm?id=41248630584&ali_refid=a3_430582_1006:1109983619:N:77RKeLgkEXuYPsQPL/pGPA==:31c58e63c712d4a9ee33af685ecb1c61&ali_trackid=1_31c58e63c712d4a9ee33af685ecb1c61&spm=a230r.1.14.6) 、优信电子 [淘宝链接](https://item.taobao.com/item.htm?spm=a230r.1.14.15.77176c121VUnMs&id=522553143872&ns=1&abbucket=12#detail) |


<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01OkWDVc1Y6AQtrBbaQ_!!6000000003009-2-tps-1488-1234.png" style="max-width:800px;" />
</div>

# 4. 案例实现
## 4.1 硬件连接
HaaS100与温湿度传感器和土壤湿度传感器的接口说明。

| **HaaS100接口** | **传感器接口** | **说明** |
| :---: | :---: | :---: |
| GPIO01 | OUT | DHT11温湿度传感器 |
| GND | GND | 电源地 |
| VCC | 3V3 | 3V3电源 |

## 4.2 软件设计
### 4.2.1 云端创建产品
请先点击登陆[物联网平台](https://www.aliyun.com/product/iot/iot_instc_public_cn)（未注册阿里云账户的用户，请先完成账户注册），按下面步骤一步步去打造

1、创建项目
注册登入后，如下图所示，前往管理控制台。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01u0AGpn1uEnRRq2WAu_!!6000000006006-2-tps-1308-490.png" style="max-width:800px;" />
</div>

点击公共实例，进行产品创建

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01kDopR91FsHL3jRQOT_!!6000000000542-2-tps-1328-792.png" style="max-width:800px;" />
</div>

2、创建HaaS养花的产品
点击创建产品，见下图所示，这里创建了一个名称“HaaS养花”产品。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01PtUXOV1K3VVdbJc2v_!!6000000001108-2-tps-1490-600.png" style="max-width:800px;" />
</div>

在产品详情中增加产品的功能，比如：温度、湿度等。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01eb1RVm1XpDmMr8cqi_!!6000000002972-2-tps-2538-900.png" style="max-width:800px;" />
</div>

3、设备管理
在设备列表中增加对应产品的设备。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN015JH11p1cgaO8xzhJ9_!!6000000003630-2-tps-1910-874.png" style="max-width:800px;" />
</div>

查看设备信息

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN012ZcVF71EK6ADdOaxW_!!6000000000332-2-tps-2734-994.png" style="max-width:800px;" />
</div>

创建后的设备三元组信息需要同步到设备端的开发代码段中，在3.1章节会介绍。待设备端开发结束，就可以在物联网平台中参看设备在线状态。
### 4.2.2 设备端代码
传感器GPIO初始化接口，参考文档路径：solutions/flower_demo/flower_app.c
```c
int flower_gpio_init(void)
{
    gpio_dev_t temp_gpio;

    temp_gpio.port = HAL_IOMUX_PIN_P0_1;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);

    fd = open("/dev/gpio", 0);
    printf("open gpio %s, fd:%d\r\n", fd >= 0 ? "success" : "fail", fd);

    DHT11_GPIO_Set(1);
    DHT11_Reset();
    aos_cli_register_command(&temp_cmd);
    return 0;
}
```
与物联网平台连接需要使用三元组信息以及温湿度上报，参考文档路径：solutions/flower_demo/data_model_basic_demo.c
```c
int demo_main(int argc, char *argv[])
{
    int32_t     res = STATE_SUCCESS;
    void       *dm_handle = NULL;
    void       *mqtt_handle = NULL;
    char       *url = "iot-as-mqtt.cn-shanghai.aliyuncs.com"; /* 阿里云平台上海站点的域名后缀 */
    char        host[100] = {0}; /* 用这个数组拼接设备连接的云平台站点全地址, 规则是 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com */
    uint16_t    port = 443;      /* 无论设备是否使用TLS连接阿里云平台, 目的端口都是443 */
    aiot_sysdep_network_cred_t cred; /* 安全凭据结构体, 如果要用TLS, 这个结构体中配置CA证书等参数 */

    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "产品key";
    char *device_name       = "设备名";
    char *device_secret     = "设备密钥";

    ...

    /* 主循环进入休眠 */
    while (1) {
        report_2_cloud(dm_handle); //每隔3s检测是否有温湿度变化检测
        aos_msleep(3000);
    }
}
```
温湿度检测上报report_2_cloud()，参考文档路径：solutions/flower_demo/flower_app.c
```c
void report_2_cloud(void *dm_handle)
{
    uint8_t temp =0,humidity=0,d_flag = 0;
    char property_payload[30] = {0};

    if(mqtt_status == 0){
        printf("mqtt status :%d %p\r\n",mqtt_status,dm_handle);
        return;
    }

    d_flag = DHT11_Read_Data(&temp,&humidity);

    printf("temp ->%d  humidity->%d --%d\n",temp,humidity,d_flag);
    if((last_temp != temp)&&(!d_flag)){
        //温度变化上报接口
        snprintf(property_payload, sizeof(property_payload), "{\"Temperature\": %d}", temp);
        printf("report:%s\r\n",property_payload);
        demo_send_property_post(dm_handle, property_payload);
        last_temp = temp;
    }
    if((last_hum != humidity)&&(!d_flag)){
        //湿度变化上报接口
        snprintf(property_payload, sizeof(property_payload), "{\"Humidity\": %d}", humidity);
        printf("report:%s\r\n",property_payload);
        demo_send_property_post(dm_handle, property_payload);
        last_hum = humidity;
    } 
}
```
### 4.2.3 小程序开发

1、 云端钉一体小程序SDK获取

```
git clone -b dev_3.1.0_haas https://github.com/alibaba/AliOS-Things.git
```
SDK相关代码及操作readme在application/miniapp/目录下。

2、 SDK目录结构

```tree
.
├─ lib (存放依赖库的文件夹，用户无需关心)
│    ├─ @alicloud/pop-core     （[https://github.com/aliyun/openapi-core-nodejs-sdk](https://github.com/aliyun/openapi-core-nodejs-sdk)）
│    ├─ kitx
│    └─ iot-packet.js              （封装给用户的文件）
├─ pages                              （页面文件夹，用户在这里自定义页面，示例持续更新中）
│    └─ index                        （首页 选择进入不同示例） 
│             ├─ index.axml         
│             ├─ index.js           
│             ├─ index.acss 
│             └─ index.json 
│    └─ HaasFlower                    （示例1 HaaS养花）
│             ├─ HaasFlower.axml    （页面布局文件）
│             ├─ HaasFlower.js        （控制逻辑，用户在这里定义交互行为）
│             ├─ HaasFlower.acss    （页面样式）
│             ├─ HaasFlower.json      （页面配置，用于配置页面标题等）
│             └─ HaasFlower.TSL.json（示例对应的TSL文件，可以在物联网平台上导入该文件生成物模型）
│    └─ HaasCar              （示例2 HaaS小小蛮驴，目录结构同 HaasFlower）
├─ app.js                            （注册小程序，在这里进行全局参数配置，如 AccessKey）
├─ app.acss                         （小程序全局样式）
├─ app.json                         （小程序全局配置，可以在这里设置小程序打开的默认页面）
└─ others
```

3、 SDK使用

使用小程序开发工具打开miniapp这个工程。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN011gxxpV21LOSUrBJC9_!!6000000006968-2-tps-2144-1236.png" style="max-width:800px;" />
</div>

- step1 在app.js填写 AccessKeyID 以及 AccessKeySecret ， 获取云端API调用权限
- step2 填写目标设备 DeviceName 以及 ProductKey，这里是待控制的设备

```javascript
// app.js
let accessKey = {
  accessKeyId: '<- accessKeyId ->', 	// 填入阿里云平台生成的 assessKeyId 以及 Secret
  accessKeySecret: '<- accessKeySecret ->',
}

// HaasFlower.js
let device = {
  DeviceName: '<- DeviceName ->',   // 填入目标设备 DeviceName 以及 ProductKey
  ProductKey: '<- ProductKey ->'
}
```


代码中提到的几个关键参数：

- AccessKeyID AccessKeySecret

云账号AccessKey是用户访问阿里云API的密钥。（[https://usercenter.console.aliyun.com/#/manage/ak](https://usercenter.console.aliyun.com/#/manage/ak)）
安全起见，我们可以采取创建[RAM子账号](https://ram.console.aliyun.com/users)的形式，来对权限进行分离。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01WSv9th1Rm7OvqTbo9_!!6000000002153-2-tps-3584-732.png" style="max-width:800px;" />
</div>

创建完成后，需要手动分配权限，点击右侧，添加权限。我这里直接选择了 **AdministratorAccess。 **添加完成后如下。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01fFjDqn1OaihJrsA4o_!!6000000001722-2-tps-3584-1920.png" style="max-width:800px;" />
</div>

- DeviceName ProducKey

这两个参数在[创建设备](#jSkOe)时生成。
4、 编译验证
点击小程序IDE右上角“真机调试”按钮，等待二维码生成后，使用钉钉APP扫码，即可发起小程序。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01nYZ1R01FVpveL7bv9_!!6000000000493-2-tps-948-156.png" style="max-width:800px;" />
</div>

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01ixflz91HMKwrURJ2i_!!6000000000743-1-tps-746-1330.gif" style="max-width:800px;" />
</div>

## 4.3 开发环境搭建、编译、烧入、运行
### 4.3.1 AliOS Things开发环境搭建
开发环境的搭建请参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

### 4.3.2 智能养花系统代码下载
智能语音播放器的代码下载请参考 [《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)，其中，
> 选择解决方案: “养花案例”
> 选择开发板: HaaS100

### 4.3.3 代码编译、烧录
编译flower_demo的过程如下：

-- 编译固件可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

-- 烧录固件可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)。

### 4.3.4 运行
运行打印信息

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01hpPVq91TUpgUAFEqS_!!6000000002386-2-tps-958-900.png" style="max-width:800px;" />
</div>

# 5. 总结
至此，一个简单的智能手机远程管理的HaaS花卉养植系统就做好了。是不是很简单？


当然，这个HaaS花卉养植系统的打造仅仅是一个抛砖引玉，还有很大的扩展空间，比如 湿度过低触发自动洒水，温度过高自动开家里的空调等设备。还有非常大的想象和发挥空间。


同理，做一个 HaaS家庭鱼缸 远程监测包括喂食 等系统，也就很简单了。


另外，也希望大家能一起携手来丰富Haas的应用案例化，打造出更多更有意思的场景，比如：HaaS+人感打造智慧办公室、HaaS+Mic/Speaker打造和谐校园、HaaS+空净打造呼吸守护系统等等。


