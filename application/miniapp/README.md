# HaaS云钉端一体开发实战

<br />“阿里云物联网平台为设备提供安全可靠的连接通信能力，向下连接海量设备，支撑设备数据采集上云；向上提供云端API，服务端通过调用云端API将指令下发至设备端，实现远程控制。”<br />本SDK借由物联网平台的能力，实现小程序控制设备能力，其基本链路如下：<br />**设备** <- -> **物联网平台** <- 阿里云API ->  **前端小程序**<br />

<a name="6YyvY"></a>
# 物联网平台配置
[https://iot.console.aliyun.com/](https://iot.console.aliyun.com/product)
<a name="RpYcx"></a>
## 创建产品
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603854965019-16a27704-f12b-415b-9113-4a92ba2df226.png#align=left&display=inline&height=592&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1790&originWidth=1428&size=199755&status=done&style=shadow&width=472)
<a name="lz4e0"></a>
## 配置物模型
产品创建完成后，点击功能定义 -> 编辑草稿 -> 添加自定义功能<br />这里我们为设备添加 向前、后、左、右、停止 五个属性值<br />注意标识符，我们在调用接口来操作设备属性时，参数必须与这里的标识符严格一致<br />![截屏2020-10-28 上午11.34.55.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603856103665-654390a2-5104-48db-8734-2817dfb0dfde.png#align=left&display=inline&height=618&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2020-10-28%20%E4%B8%8A%E5%8D%8811.34.55.png&originHeight=1432&originWidth=960&size=91463&status=done&style=shadow&width=414)<br />也可以直接导入对应示例中的  *.TSL.json  文件<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1605583990926-0f475ebb-5494-4c0e-b716-acddca5481c3.png#align=left&display=inline&height=309&margin=%5Bobject%20Object%5D&name=image.png&originHeight=716&originWidth=962&size=53082&status=done&style=shadow&width=415)<br />添加完属性后，记得将物模型发布上线才能让修改生效<br />![截屏2020-10-28 上午11.40.44.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603856450837-7b2f3539-ec4e-4435-a2b6-bd94703427ff.png#align=left&display=inline&height=1448&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2020-10-28%20%E4%B8%8A%E5%8D%8811.40.44.png&originHeight=1448&originWidth=2726&size=288263&status=done&style=shadow&width=2726)
<a name="jSkOe"></a>
## 创建设备
产品配置完成后，相当于定义了一个“类”，而 “设备” 就是 “产品” 的实例，继承“产品”的所有属性。<br />![截屏2020-10-28 上午11.44.55.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603856701673-52e3e785-eabb-436c-8bc8-f3ccdc2d038b.png#align=left&display=inline&height=1350&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2020-10-28%20%E4%B8%8A%E5%8D%8811.44.55.png&originHeight=1350&originWidth=3368&size=298273&status=done&style=shadow&width=3368)<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603856750156-baff5a95-5d02-4d7e-be6b-4f1b76e38138.png#align=left&display=inline&height=329&margin=%5Bobject%20Object%5D&name=image.png&originHeight=934&originWidth=960&size=113572&status=done&style=shadow&width=338)<br />点击查看设备信息后，显示如下，注意这里的的设备名下面，有三个参数 DeviceName ProducKey DeviceSecret 这个三元组是平台为每个设备分配的唯一标识码，后面的[接口调用](#3gRRw)会用到这个参数。<br />![截屏2020-10-29 下午1.07.11.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603948036374-31eb9a36-6bf8-4c2b-8462-f5c3581766d5.png#align=left&display=inline&height=1436&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2020-10-29%20%E4%B8%8B%E5%8D%881.07.11.png&originHeight=1436&originWidth=3584&size=355825&status=done&style=shadow&width=3584)<br />
<br />之后就是给设备烧录SDK直接接入物联网平台， 可以先行参看“硬件开发”部分。
<a name="VtgTE"></a>
# 小程序开发
本章节介绍了如何使用SDK进行钉钉小程序的开发，开发者也可以直接前往[小程序编译验证](#p2nLM)，使用代码直接进行体验。
<a name="FOHTH"></a>
## 小程序目录结构
/ <br />├─ lib (存放依赖库的文件夹，用户无需关心)<br />│    ├─ @alicloud/pop-core     （[https://github.com/aliyun/openapi-core-nodejs-sdk](https://github.com/aliyun/openapi-core-nodejs-sdk)）<br />│    ├─ kitx<br />│    └─ iot-packet.js              （封装给用户的文件）<br />├─ pages                              （页面文件夹，用户在这里自定义页面，示例持续更新中）<br />│    └─ index                        （首页 选择进入不同示例） <br />│             ├─ index.axml         <br />│             ├─ index.js           <br />│             ├─ index.acss <br />│             └─ index.json <br />│    └─ HaasCar                    （示例1 HaaS小小蛮驴）<br />│             ├─ HaasCar.axml    （页面布局文件）<br />│             ├─ HaasCar.js        （控制逻辑，用户在这里定义交互行为）<br />│             ├─ HaasCar.acss    （页面样式）<br />│             ├─ HaasCar.json      （页面配置，用于配置页面标题等）<br />│             └─ HaasCar.TSL.json （示例对应的TSL文件，用户可以在物联网平台上导入该文件生成物模型）<br />│    └─ HaasFlower              （示例2 HaaS养花，目录结构同 HaasCar）<br />├─ app.js                            （注册小程序，在这里进行全局参数配置，如 AccessKey）<br />├─ app.acss                         （小程序全局样式）<br />├─ app.json                         （小程序全局配置，可以在这里设置小程序打开的默认页面）<br />└─ others
<a name="4T2pr"></a>
## SDK使用

- step1 引入 iot-packet.js
- step2 填写 AccessKeyID 以及 AccessKeySecret ， 获取云端API调用权限
- step3 填写目标设备 DeviceName 以及 ProductKey，这里是待控制的设备
- step4 创建全局 IoTApi实例，可以在具体页面中调用<br />
```javascript
import { IotApi } from '/lib/iot-packet.js'		// 引入小程序SDK

App({
  onLaunch() {
    let accessKey = {
      accessKeyId: '<- accessKeyId ->', 	// 填入阿里云平台生成的 assessKeyId 以及 Secret
      accessKeySecret: '<- accessKeySecret ->',
    }
    let device = {
      DeviceName: '<- DeviceName ->',   // 填入目标设备 DeviceName 以及 ProductKey
      ProductKey: '<- ProductKey ->'
    }
    this.iotApi = new IotApi({ accessKey, device })		// 创建全局 IoTApi 实例
  },
});
```


代码中提到的几个关键参数：

- AccessKeyID AccessKeySecret

云账号AccessKey是用户访问阿里云API的密钥。（[https://usercenter.console.aliyun.com/#/manage/ak](https://usercenter.console.aliyun.com/#/manage/ak)）<br />安全起见，我们可以采取创建[RAM子账号](https://ram.console.aliyun.com/users)的形式，来对权限进行分离。<br />![截屏2020-10-29 下午1.01.39.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603947703740-e6e2b231-57af-4968-94ed-377b10f65ff3.png#align=left&display=inline&height=732&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2020-10-29%20%E4%B8%8B%E5%8D%881.01.39.png&originHeight=732&originWidth=3584&size=210321&status=done&style=none&width=3584)<br />创建完成后，需要手动分配权限，点击右侧，添加权限。我这里直接选择了 **AdministratorAccess。 **添加完成后如下。<br />![截屏2020-10-29 下午1.03.13.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603947799141-4f21e9a1-ca11-46bb-be60-95075cadb597.png#align=left&display=inline&height=1920&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2020-10-29%20%E4%B8%8B%E5%8D%881.03.13.png&originHeight=1920&originWidth=3584&size=578859&status=done&style=none&width=3584)

- DeviceName ProducKey

这两个参数在[创建设备](#jSkOe)时生成。

通过SSH将这段代码上传到服务器，并记得在阿里云控制台安全组中开放端口（侧边栏 云服务器ECS->网络与安全->安全组->修改->手动添加）。我这里设置的是2333.<br />![截屏2020-10-29 下午12.51.28.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1603947093325-26945963-a9bb-4989-bbf1-8fe98402fe73.png#align=left&display=inline&height=1156&margin=%5Bobject%20Object%5D&name=%E6%88%AA%E5%B1%8F2020-10-29%20%E4%B8%8B%E5%8D%8812.51.28.png&originHeight=1156&originWidth=3576&size=231779&status=done&style=none&width=3576)

- step5 调用相关接口
```javascript
// 读取云端对应属性的值，并将其置反
let value = await app.iotApi.getDeviceProperty({ Identifier: 'front' });
await app.iotApi.setDeviceProperty({ Identifier: 'front', Value: value == '1' ? 0 : 1 })
```
<a name="TtItm"></a>
## 页面开发
这里实现了一个简单的遥控器小程序，5个按键对应物模型中的5个属性，每按一次，物模型属性中的bool就翻转一次，小车接收到信号变化后，就会向对应方向行驶一段距离。<br />[支付宝小程序IDE](https://opendocs.alipay.com/mini/ide/download)提供了非常方便的页面开发，在[钉钉开放平台创建完小程序](https://open-dev.dingtalk.com/)后，开发者可以使用拖拽完成简单的页面创建。<br />![屏幕录制2020-11-13 下午3.gif](https://intranetproxy.alipay.com/skylark/lark/0/2020/gif/256836/1605255274763-061d0b0f-2e86-45f6-ae5f-4deb8c7acdfa.gif#align=left&display=inline&height=2194&margin=%5Bobject%20Object%5D&name=%E5%B1%8F%E5%B9%95%E5%BD%95%E5%88%B62020-11-13%20%E4%B8%8B%E5%8D%883.gif&originHeight=2194&originWidth=3584&size=2900480&status=done&style=none&width=3584)<br />创建完页面布局后，为每个按键添加动作触发函数。
```html
<button data-direction="front" onTap="arrowKeyTap">向前</button>
onTap="arrowKeyTap" 表示当该按键被点击时，将触发 JS中的 arrowKeyTap 函数
data-direction="front" 表示当 onTap 触发时， 将 direction="front" 作为参数传入 arrowKeyTap
从而实现在 arrowKeyTap 中判断哪个按键被触发，进而改变设备的对应属性。
<button data-direction="right" onTap="arrowKeyTap">向右</button>
<button data-direction="back" onTap="arrowKeyTap">向后</button>
<button data-direction="left" onTap="arrowKeyTap">向左</button>
<button data-direction="stop" onTap="arrowKeyTap">停止</button>
```
熟悉CSS的开发者可以尝试加入一些自定义样式。如图是笔者对页面进行了一些修饰。<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1605585954830-c2e52ee6-ce2e-46dd-94bc-f0e8b966d23f.png#align=left&display=inline&height=442&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1336&originWidth=750&size=72309&status=done&style=shadow&width=248)
<a name="4fmyZ"></a>
## 逻辑开发
定义好axml中的数据传输形式后，我们需要在js中实现对应的逻辑。
```javascript
  async arrowKeyTap(event) {
    let dir = event.target.dataset.direction
    // 读取前端页面传递的属性信息
    var value = await app.iotApi.getDeviceProperty({ Identifier: dir });
    // 使用api获取对应属性（Identifier）的值 
    console.log(value)
    // 打印获取的属性值
    await app.iotApi.setDeviceProperty({ Identifier: dir, Value: value == '1' ? 0 : 1 })
    // 将对应属性值置反
  },
```
<a name="p2nLM"></a>
## 小程序编译验证
点击小程序IDE右上角“真机调试”按钮，等待二维码生成后，使用钉钉APP扫码，即可发起小程序。<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/256836/1605585495799-64ecf7b0-ae07-44db-8d2a-d2219316295b.png#align=left&display=inline&height=156&margin=%5Bobject%20Object%5D&name=image.png&originHeight=156&originWidth=948&size=18271&status=done&style=none&width=948)<br />![云钉端演示 (1).gif](https://intranetproxy.alipay.com/skylark/lark/0/2020/gif/256836/1605585843326-90c328d0-c0c9-4d80-a8ce-7bfbd9a90a4a.gif#align=left&display=inline&height=1080&margin=%5Bobject%20Object%5D&name=%E4%BA%91%E9%92%89%E7%AB%AF%E6%BC%94%E7%A4%BA%20%281%29.gif&originHeight=1080&originWidth=1920&size=2260193&status=done&style=none&width=1920)
<a name="1AEiT"></a>
# 硬件开发
<a name="3dMoj"></a>
##  配置初始化
云端创建产品的时候，可以获取调试设备的四元组信息。
```c
//四元组信息初始化
#define PRODUCT_KEY "<PRODUCT_KEY>"
#define DEVICE_NAME "<DEVICE_NAME>"
#define DEVICE_SECRET "<DEVICE_SECRET>"
```
对驱动电机芯片控制引脚配置
```c
int auto_gpio_init(void)
{
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

	GPIO_Set(ENA_PORT,1); //使能驱动电机左
	GPIO_Set(ENB_PORT,1);//使能驱动电机右
	stop_ctl();
}
```
<a name="JqllL"></a>
## 接收云端控制命令
接受处理云端的控制命令，包括后退、左转、右转、停止、前进等。
```c
char *auto_temp[5]={"back","left","right","stop","front"};
static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    char i =0,*auto_c;
    cJSON *root = cJSON_Parse(request)
    for(i=0;i<5;i++){
        auto_c = auto_temp[i];
        cJSON *value = cJSON_GetObjectItem(root,auto_c);
        if (value == NULL || !cJSON_IsNumber(value)) {
            EXAMPLE_TRACE("not find %d",i);        
        }
        else{
            goto hass_send;
        }
    }
    cJSON_Delete(root);
    if(i == 5){
        EXAMPLE_TRACE("not find %d",i);       
        return -1;
    }
hass_send:
    switch(i){
        case 0:
            back_ctl();
            EXAMPLE_TRACE("---back");
        break;

        case 1:
            left_ctl();
            EXAMPLE_TRACE("---left");
        break;

        case 2:
            right_ctl();
            EXAMPLE_TRACE("---right");
        break;

        case 3:
            stop_ctl();
            EXAMPLE_TRACE("---stop");
        break;

        case 4:
            front_ctl();
            EXAMPLE_TRACE("---front");
        break;

        default:
            break;
    }
    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)request, request_len);
    return 0;
}
```
<a name="DIiML"></a>
# 展示效果
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/106133/1604476856909-43328ac3-1f15-472c-87d9-b01ce0e36f34.png#align=left&display=inline&height=368&margin=%5Bobject%20Object%5D&name=image.png&originHeight=4032&originWidth=3024&size=17734909&status=done&style=none&width=276)<br />![normal video.gif](https://intranetproxy.alipay.com/skylark/lark/0/2020/gif/256836/1605508063192-1f09f083-fb60-4e9c-956b-89c3dae190a7.gif#align=left&display=inline&height=544&margin=%5Bobject%20Object%5D&name=normal%20video.gif&originHeight=544&originWidth=960&size=6961895&status=done&style=none&width=960)
