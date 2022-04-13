@page auto_demo auto_demo
# 1. 案例简介
阿里在云栖大会上发布第一款机器人“小蛮驴”，瞬间激起千层浪。
无人车，智能物流，机器人等一些概念又火热了一把。
借“小蛮驴”的东风以及火热的HaaS，我们推出了更加亲民的“小小蛮驴”，丰富HaaS的场景打造，吸引更多的开发者和企业来加入HaaS的生态建设。
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106133/1616395304747-a2cfc5fe-b74f-43d6-b66a-5d65f76f8784.png#align=left&display=inline&height=292&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1264&originWidth=2248&size=7069041&status=done&style=none&width=519)
# 2. 基础知识
## 2.1  HaaS100开发板
**HaaS100** 是一款物联网场景中的标准开发板，并配套嵌入到硬件中的软件驱动及功能模块，为用户提供物联网设备高效开发服务。
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/113837/1607656919362-55ec2c19-a0dd-4d1d-ac95-2052ffeedd7f.png#align=left&display=inline&height=295&margin=%5Bobject%20Object%5D&name=image.png&originHeight=443&originWidth=443&size=130743&status=done&style=none&width=295)
HaaS100核心板有着丰富的外设接口，如下所示：

![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/106133/1607655121564-f799e873-94f0-485d-8d1c-95ba5cafaf5f.png#align=left&display=inline&height=352&margin=%5Bobject%20Object%5D&name=image.png&originHeight=886&originWidth=586&size=232115&status=done&style=none&width=233)

详细的资料参考[HaaS 100 硬件规格](https://help.aliyun.com/document_detail/184186.html?spm=a2c4g.11186623.6.645.718015814zDYZt)。
## 2.2 驱动电机
驱动电机芯片是小车最重要的模块之一，2A的强大输出电流为小车提供满满的动力。
在驱动模块接口的定义中包括：
IN1和IN2是控制小车的左电机，IN3和IN4是控制小车的右电机，ENA和ENB引脚为输出使能管脚，高电平有效，IN1、IN2、IN3和IN4可以通过PWM脉冲调速。
**电机控制：**
IN1高电平，IN2低电平，左电机正传；
IN1低电平，IN2高电平，左电机反传；
IN3高电平，IN4低电平，右电机正传；
IN3低电平，IN4高电平，右电机反传。
## 2.3 超声波测距模块
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/106133/1604469941791-97eb9651-b1fe-4983-88ce-785f0e1e24cc.png#align=left&display=inline&height=195&margin=%5Bobject%20Object%5D&name=image.png&originHeight=224&originWidth=306&size=121415&status=done&style=none&width=266)
超声波测距一般有4个管脚：
GND：接公共地
VCC：供5V电源
Trigger：触发控制，信号输入引脚
Echo：回响信号输出引脚
超声波测距原理是当发送的超声波遇到物体被发射回来，被接收端接收，然后通过发送信号到接收到回响的信号时间间隔可以测算出距离。
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/106133/1604471428556-e1c9fadf-3e93-4715-9b15-80dd5f33dc32.png#align=left&display=inline&height=231&margin=%5Bobject%20Object%5D&name=image.png&originHeight=301&originWidth=529&size=74124&status=done&style=none&width=406)
测距过程如下：HaaS100给Trigger引脚至少10uS的脉冲触发信号，这时候超声波模块发射8个40KHz的方波，然后模块自动检测是否有信号返回。若检测到回响信号，会通过Echo引脚输出一个高电平脉冲，脉冲的宽度就是超声波从发射到返回的时间t。假设小车距离障碍物距离L，2L=vt，L=vt/2，其中v声音的传播速度340m/s。
## 2.4 红外避障模块
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/106133/1604472600417-ceadf30e-19b0-477c-b308-572b940d4199.png#align=left&display=inline&height=144&margin=%5Bobject%20Object%5D&name=image.png&originHeight=287&originWidth=357&size=152067&status=done&style=none&width=178.5)
红外避障原理就是当障碍物靠近的时候，红外接收到的发射光强度越来越大，所以增大了输出的模拟信号，模拟信号接入比较器，经过比较器处理，输出数字信号。读取数字信号电平，就可以判断前方是否有障碍物。
## 2.5 测速模块
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/106133/1604473859445-25af0aaf-e883-4024-b681-69506e128181.png#align=left&display=inline&height=190&margin=%5Bobject%20Object%5D&name=image.png&originHeight=190&originWidth=211&size=74467&status=done&style=none&width=211)
测速模块主要原理是由码盘结合光电传感器，传感器一端为发射光，另一端接收光，通过测量单位时间内脉冲个数得出小车的速度，本模块采用施密特触发器输出的信号非常稳定( 去抖)。

# 3. 物料清单
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/106133/1604460555294-d7a45285-1ce1-4e86-9703-6995da520091.png#align=left&display=inline&height=262&margin=%5Bobject%20Object%5D&name=image.png&originHeight=660&originWidth=1432&size=1060798&status=done&style=none&width=568)
**智能车底板电路**

- 红外接收头子，实现红外遥控小车
- 5V电源稳压芯片
- 电机驱动芯片，电流可达2A，为小车提供强大的动力
- 电源指示灯
- 保护二极管

**其他丰富接口**

- 电机接口
- 超声波模块接口
- 舵机模块接口
- 测速模块接口
- 红外避障模块接口
- 电池座接口
- HaaS连接接口
| **物料** | **规格** | **购买链接** |
| :---: | :---: | :---: |
| HaaS100开发板 | 
 | 阿里云[天猫链接](https://detail.tmall.com/item.htm?spm=a230r.1.14.16.13123b209835rz&id=627354125244&ns=1&abbucket=16) |
| 智能车套件 | 提供完整一套小车配件，结合HaaS100，可以实现云端钉控制 | 微雪[微雪链接](https://www.waveshare.net/shop/AlphaBot.htm) |

# 4. 案例实现
本案例实现了小车模型中的电机控制部分，其他模块部分开发者可以自己研究摸索。
## 4.1 硬件连接
| **驱动电机接口** | **HaaS100** | **I/O模式** |
| :---: | :---: | :---: |
| **IN1** | **GPIO47** | **O** |
| **IN2** | **GPIO40** | **O** |
| **ENA** | **GPIO24** | **O** |
| **IN3** | **GPIO26** | **O** |
| **IN4** | **GPIO46** | **O** |
| **ENB** | **GPIO25** | **O** |

## 4.2 软件设计
### 4.2.1 云端创建产品
参考[《一步步打造能手机远程管理的HaaS花卉养植系统》](https://blog.csdn.net/HaaSTech/article/details/110505659)等系列文章，一步步在物联网平台创建产品、对应的物模型以及设备。也可以通过载入[TSL文件](https://github.com/alibaba/AliOS-Things/blob/dev_3.1.0_haas/application/miniapp/pages/HaasCar/HaasCar.TSL.json)，一键生成物模型。
创建了HaaS电动车的产品名称，然后定义了前进、后退、暂停、左转、右转等物模型属性。
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/106133/1607526392432-d1239385-56fd-4039-8187-37cd8489eb56.png#align=left&display=inline&height=365&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1624&originWidth=2620&size=629143&status=done&style=none&width=589)
按照一步步操作，云端创建产品、物模型以及设备以后，我们可以获取对应的三元组信息。
### 4.2.2 设备端代码
1、 配置初始化
HaaS100设备端电动小车参考代码的路径在：/solution/auto_demo/data_model_basic_demo.c下。
从物联网云平台创建的产品时候，可以获取调试设备的三元组信息。如下所示：
```c
//三元组信息初始化
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
        aos_msleep(1000);
    }
}
```
2、对驱动电机芯片控制引脚配置
参考代码的路径在：/solution/auto_demo/auto_drv.c下
```c
int auto_gpio_init(void)
{
    int ret = 0;

    temp_gpio.port = IN1_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);
    temp_gpio.port = IN2_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);
    temp_gpio.port = IN3_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);
    temp_gpio.port = IN4_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);

    temp_gpio.port = ENA_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);
    temp_gpio.port = ENB_PIN;
    temp_gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
    hal_gpio_init(&temp_gpio);

    //gpio open
    fd = open("/dev/gpio", 0);
    printf("open gpio %s, fd:%d\r\n", fd >= 0 ? "success" : "fail", fd);

    GPIO_Set(ENA_PORT,1);
    GPIO_Set(ENB_PORT,1);
    stop_ctl();

    aos_cli_register_command(&haas_cmd);
    return ret;
}
```
3、接收云端控制命令
接受处理云端的控制命令，包括后退、左转、右转、停止、前进等。
参考代码的路径在：/solution/auto_demo/data_model_basic_demo.c下
```c
static void demo_dm_recv_handler(void *dm_handle, const aiot_dm_recv_t *recv, void *userdata)
{
    uint8_t i;
    printf("demo_dm_recv_handler, type = %d\r\n", recv->type);

    switch (recv->type) {
        /* 云端下发的属性设置 */
        case AIOT_DMRECV_PROPERTY_SET: {
            printf("msg_id = %ld, params = %.*s\r\n",
                   (unsigned long)recv->data.property_set.msg_id,
                   recv->data.property_set.params_len,
                   recv->data.property_set.params);
            
            char *auto_temp[5]={"back","left","right","stop","front"};
            cJSON *root = cJSON_Parse(recv->data.property_set.params);
            for(i = 0 ;i<5 ;i++){
                cJSON *value = cJSON_GetObjectItem(root,auto_temp[i]);
                if (value != NULL && cJSON_IsNumber(value)) {
                    printf("%s is find!\r\n",auto_temp[i]);  
                    break;      
                }
            }
            switch(i){
                case 0:
                    back_ctl(); //后退
                    printf("---back\r\n");
                break;

                case 1:
                    left_ctl(); //左转
                    printf("---left\r\n");
                break;

                case 2:
                    right_ctl(); //右转
                    printf("---right\r\n");
                break;

                case 3:
                    stop_ctl(); //停止
                    printf("---stop\r\n");
                break;

                case 4:
                    front_ctl(); //前进
                    printf("---front\r\n");
                break;

                default:
                    break;
            }
        }
        break;
            
            ....
                
        default:
            break;
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
/ 
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
│    └─ HaasCar                    （示例1 HaaS小小蛮驴）
│             ├─ HaasCar.axml    （页面布局文件）
│             ├─ HaasCar.js        （控制逻辑，用户在这里定义交互行为）
│             ├─ HaasCar.acss    （页面样式）
│             ├─ HaasCar.json      （页面配置，用于配置页面标题等）
│             └─ HaasCar.TSL.json （示例对应的TSL文件，用户可以在物联网平台上导入该文件生成物模型）
│    └─ HaasFlower              （示例2 HaaS养花，目录结构同 HaasCar）
├─ app.js                            （注册小程序，在这里进行全局参数配置，如 AccessKey）
├─ app.acss                         （小程序全局样式）
├─ app.json                         （小程序全局配置，可以在这里设置小程序打开的默认页面）
└─ others


3、 SDK使用
使用小程序开发工具打开miniapp这个工程。
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/113837/1607309104502-3aa9a406-93a3-4b6e-84e4-4d2fa8a13894.png#align=left&display=inline&height=618&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1236&originWidth=2144&size=1065527&status=done&style=none&width=1072)

- step1 在app.js填写 AccessKeyID 以及 AccessKeySecret ， 获取云端API调用权限
- step2 填写目标设备 DeviceName 以及 ProductKey，这里是待控制的设备

```javascript
// app.js
let accessKey = {
  accessKeyId: '<- accessKeyId ->', 	// 填入阿里云平台生成的 assessKeyId 以及 Secret
  accessKeySecret: '<- accessKeySecret ->',
}

// HaasCar.js
let device = {
  DeviceName: '<- DeviceName ->',   // 填入目标设备 DeviceName 以及 ProductKey
  ProductKey: '<- ProductKey ->'
}
```


代码中提到的几个关键参数：

- AccessKeyID AccessKeySecret

云账号AccessKey是用户访问阿里云API的密钥。（[https://usercenter.console.aliyun.com/#/manage/ak](https://usercenter.console.aliyun.com/#/manage/ak)）
安全起见，我们可以采取创建[RAM子账号](https://ram.console.aliyun.com/users)的形式，来对权限进行分离。
![截屏2020-10-29 下午1.01.39.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603947703740-e6e2b231-57af-4968-94ed-377b10f65ff3.png#align=left&display=inline&height=732&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2020-10-29%20%E4%B8%8B%E5%8D%881.01.39.png&originHeight=732&originWidth=3584&size=210321&status=done&style=none&width=3584)
创建完成后，需要手动分配权限，点击右侧，添加权限。我这里直接选择了 **AdministratorAccess。 **添加完成后如下。
![截屏2020-10-29 下午1.03.13.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603947799141-4f21e9a1-ca11-46bb-be60-95075cadb597.png#align=left&display=inline&height=1920&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2020-10-29%20%E4%B8%8B%E5%8D%881.03.13.png&originHeight=1920&originWidth=3584&size=578859&status=done&style=none&width=3584)

- DeviceName ProducKey

这两个参数在[创建设备](#jSkOe)时生成。
4、 编译验证
点击小程序IDE右上角“真机调试”按钮，等待二维码生成后，使用钉钉APP扫码，即可发起小程序。
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1605585495799-64ecf7b0-ae07-44db-8d2a-d2219316295b.png#align=left&display=inline&height=98&margin=%5Bobject%20Object%5D&name=image.png&originHeight=156&originWidth=948&size=18271&status=done&style=none&width=594)
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/113837/1607657559548-26507376-e4f9-4f1b-b425-6ead23fcde4b.png#align=left&display=inline&height=339&margin=%5Bobject%20Object%5D&name=image.png&originHeight=678&originWidth=338&size=403202&status=done&style=none&width=169)
## 4.3 编译，烧入，运行
编译命令
```
...进入到目录/solution/auto_demo
aos make
```
代码烧写命令
```
aos burn
```
串口输入配网指令
```
netmgr -t wifi -c {ssid} {password}
例如 
ssid:HaaS-test
password:88888888
netmgr -t wifi -c HaaS-test 88888888
```
运行打印信息
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106133/1616137413179-c03526f3-268b-430c-ae5e-671b6a6e0b24.png#align=left&display=inline&height=403&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1224&originWidth=1836&size=616976&status=done&style=none&width=604)
# 5. 总结
这么丰富详细的操作步骤，开发者朋友是不是也用了1小时就打造出自己的小小蛮驴电动小车了呢^_^


我相信有上面这么详细的一步步说明，大家都自己独立完成了电动小车的打造，并且实现了云端钉控制。大家以此为出发点，挖掘越多有意思的智能硬件，通过借助HaaS这样丰富的生态，让很多不智能的设备也很容易的智能化起来。
