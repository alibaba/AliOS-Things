# ADC采样实验

## 简介
&emsp;&emsp;
本案例是ESP32搭载HaaS-JS的第一个云端一体案例。本案例无需准备额外的硬件，只需要一块搭载HaaS-JS轻应用的ESP32开发板（NODEMCU32S/ M5STACKCORE2等）即可轻松体验。
&emsp;&emsp;
通过本案例了解ESP32的硬件能力，以及感受硬件设备如何通过云端与手机进行远程联动。

## 准备

&emsp;&emsp;
硬件器材：
1. ESP32开发板一套 (NODEMCU32S/ M5STACKCORE2)
2. 连接线若干

&emsp;&emsp;
NODEMCU32S硬件连线如下表所示：

|app.json|板级pin脚|说明|
|-----|----|----|
| adc0 | SVP | adc0电压测试点位 |
| adc7 | P35 | adc7电压测试点位 |
| pwm2 | P2 | pwm2信号输出点位 |

&emsp;&emsp;
M5STACKCORE2硬件连线如下表所示：

|app.json|板级pin脚|说明|
|-----|----|----|
| adc0 | G36 | adc0电压测试点位 |
| adc7 | G35 | adc7电压测试点位 |
| pwm2 | G2 | pwm2信号输出点位 |

## 一分钟上云体验
- 打开“支付宝”扫描下图二维码
<div align="center">
<img src=../../../images/案例-ADC采样实验（JavaScript）.png width=40%/>
</div>

- 在案例详情中，点击“立即体验”
<div align="center">
<img src=../../../images/adc_sample_1.png width=40%/>
</div>

## 物联网平台开发
&emsp;&emsp;
整个过程包含以下4个步骤：
1. 开通公共实例
2. 创建产品（设备模型）
3. 定义产品功能（物模型）
4. 创建设备及获取三元组

<br>

### 开通公共实例
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。开通后点击“公共实例”，即可进入控制台进行产品创建。
<div align="center">
<img src=../../../images/adc_sample_2.png width=40%/>
</div>

### 创建产品（设备模型）
&emsp;&emsp;
进入[公共实例控制台](https://iot.console.aliyun.com/lk/summary/new)，点击“创建产品”按钮，即可进入新建产品页面。
<div align="center">
<img src=../../../images/adc_sample_3.png width=40%/>
</div>

&emsp;&emsp;
进入新建产品页面，设定“产品名称”，这里我们命名为“nodemcu32s”，读者也可以根据自己的喜好来命名。在“所属品类”中，选择“自定义品类”。

&emsp;&emsp;
产品的节点类型选择“直连设备”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。开发者可根据自己的需求在“产品描述”页面添加针对此产品的描述。

&emsp;&emsp;
对于 ESP32 等搭载 Wi-Fi 的设备而言，联网方式选择“Wi-Fi”。
<div align="center">
<img src=../../../images/adc_sample_4.png width=40%/>
</div>

&emsp;&emsp;
点击“确认”按钮，即可完成产品创建。
<div align="center">
<img src=../../../images/adc_sample_5.png width=40%/>
</div>

&emsp;&emsp;
点击“前往定义物模型”
<div align="center">
<img src=../../../images/adc_sample_6.png width=40%/>
</div>

### 定义产品功能（物模型）
&emsp;&emsp;
开发者可以使用准备好的物模型文件来进行快速导入。点击左上角“快速导入”，选择物模型文件并上传，就能够生成案例对应的物模型。
<div align="center">
<img src=../../../images/adc_sample_7.png width=40%/>
</div>

&emsp;&emsp;
生成后的效果如下：
<div align="center">
<img src=../../../images/adc_sample_8.png width=40%/>
</div>

&emsp;&emsp;
定义好物模型后，需要发布物模型上线，并发布产品，以使变更生效。
<div align="center">
<img src=../../../images/adc_sample_9.png width=40%/>
</div>
<div align="center">
<img src=../../../images/adc_sample_10.png width=40%/>
</div>

&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

### 创建设备及获取三元组
&emsp;&emsp;
点击左侧栏中“设备“，在筛选框中选择要添加设备的产品，点击“添加设备”。这里这里我们命名为“test_device”，开发者也可以根据自己的喜好来命名。
<div align="center">
<img src=../../../images/adc_sample_11.png width=40%/>
</div>

&emsp;&emsp;
开发者也可以选择“批量添加”，一次性添加多个设备，并生成随机的DeviceName。
<div align="center">
<img src=../../../images/adc_sample_12.png width=40%/>
</div>

&emsp;&emsp;
生成的设备如下。
<div align="center">
<img src=../../../images/adc_sample_13.png width=40%/>
</div>

&emsp;&emsp;
点击前往“查看”按钮，就可以看到此设备的详细信息了。
<div align="center">
<img src=../../../images/adc_sample_14.png width=40%/>
</div>

&emsp;&emsp;
点击右上角的“查看”按钮，就能看到设备的三元组信息了。 三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。
<div align="center">
<img src=../../../images/adc_sample_15.png width=40%/>
</div>

## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保以下工作已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32-quick-start.md)的说明。
<br>
&emsp;&emsp;
(1) 烧录支持HaaS-JS开发的"ESP32 HaaS固件"。
<br>
&emsp;&emsp;
(2) 已安装ESP32 USB串口驱动。
<br>
&emsp;&emsp;
(2) 已下载HaaS-JS轻应用推送工具amp。
<br>

### 下载示例代码

&emsp;&emsp;
下载《[ESP32 adc_sample示例](https://gitee.com/alios-things/amp/tree/master/example-js/haaseduk1/fs)》，示例代码目录如下所示：
```bash
adc_sample
├── app.json         # 板级配置
└── app.js           # 应用代码
```
<br>

### 修改wifi ssid和password

&emsp;&emsp;
修改adc_sample工程里app.js中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```javascript
var wifiOptions = {
    ssid: "请填写您的路由器名称",
    password: "请填写您的路由器密码",
    bssid: '',
    timeout_ms: 18000
  };
```
<br>

### 修改设备端三元组

&emsp;&emsp;
修改adc_sample工程里app.js中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

```javascript
var iotOptions = {
    productKey: "请填写您在阿里云物联网平台申请的产品PK",
    deviceName: "请填写您在阿里云物联网平台申请的产品DN",
    deviceSecret: "请填写您在阿里云物联网平台申请的产品DS"});
    region: 'cn-shanghai',
    keepaliveSec: 60
  }
```
<br>

### 推送脚本至设备
&emsp;&emsp;
 注意: ESP32在Windows环境下执行amp-win工具下载JS脚本之前，需要先把ESP32的GPIO_0拉高（和3.3v脚短接），这样才能正常推送。Mac环境下则无此问题，不需该步骤。

&emsp;&emsp;
ESP32板子GPIO_0 硬件连线如下表所示：
|板子|板级pin脚|说明|
|-----|----|----|
| NODEMCU-32 | P0 | GPIO_0对应管脚 |
| M5StackCore2 | G0 | GPIO_0对应管脚 |
 
&emsp;&emsp;
执行以下amp命令。
> ./amp-clitool/dist/amp-macos/amp serialput ./adc_sample /dev/cu.wchusbserial537A003351 -m quickjs -e 0 -b 115200

&emsp;&emsp;
注意：amp工具路径和ESP32的串口ID请以实际为准。
<br>

## 运行结果
### 本地查看
&emsp;&emsp;
JavaScript脚本推送到ESP32之后 ，会自动运行，运行过程中日志如下。其中：

```log
wifi connected
create IotDeviceClient
iot platform connected
>> adc0 voltage is 511, adc7 voltage is 322
postProps is called
...
```
&emsp;&emsp;
点击“部署运行”，等待程序运行。

<br>

## 物联网平台端设备信息查看
&emsp;&emsp;
再次前往物联网平台的设备信息页面，若设备运行正确，此时应该可以看到设备名右侧的状态由“未激活”变为“在线”。
选中“实时刷新”，可以看到数据实时从设备上报到物联网平台。设备上云成功。
<div align="center">
<img src=../../../images/adc_sample_16.png width=40%/>
</div>
