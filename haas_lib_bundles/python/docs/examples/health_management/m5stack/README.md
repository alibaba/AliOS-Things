# 饮食健康管理平台
&emsp;&emsp;
下图是本案例的四步导学。
<div align="center">
<img src=./../../../images/health_management/导学流程.png width=80%/>
</div>

## 1、简介
### 1.1、背景
&emsp;&emsp;
随着社会发展，人们的劳动方式逐渐从体力劳动转变成脑力劳动，人每天的活动量相比以前大大减少。长此以往，可能会带来体重失控等健康问题，因此需要关注自己每天的摄入的热量，注意饮食健康。

&emsp;&emsp;
本案例采用M5Stack Core2开发板，基于云端AI能力，通过摄像头传感器对食物进行识别，搭建了一套低成本、易实现的饮食健康管理平台。基于阿里云物联网平台，可在Web应用上查看自己的饮食及身体健康数据。

### 1.2、准备

1. [M5Stack Core2开发板](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C84FAF561DF6A00000001&dataId=800C84FAF561DF6A) 一套
2. M5Stack Unit CAM摄像头 一个
3. 连接线 一根

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack Core2开发版 | 1 | [M5Stack Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5Stack Core2开发套件自带 |
| 摄像头 | 1 | [M5Stack Unit CAM摄像头](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.29.698e2d4844EBZF&id=643872470244) |
| 连接线 | 1条 | [M5Stack GROVE连接线](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.11.6b6d5f86B5IYMF&id=610410604759) 请选用10cm长即可 |

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/COM_M5Stack硬件连线图.png width=80%/>
</div>
<br>

## 2、物联网平台开发

&emsp;&emsp;
整个过程包含以下4个步骤：

1. 开通公共实例
2. 创建产品（设备模型）
3. 定义产品功能（物模型）
4. 创建设备及获取三元组

### 2.1、开通公共实例
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。开通后点击“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。

![饮食健康管理平台物联网平台.png](./../../../images/health_management/饮食健康管理平台物联网平台.png)

### 2.2、创建产品（设备模型）
&emsp;&emsp;
进入[公共实例控制台](https://iot.console.aliyun.com/lk/summary/new)，点击“创建产品”按钮，即可进入[新建产品页面](https://iot.console.aliyun.com/product)。

![饮食健康管理平台空产品页.png](./../../../images/health_management/饮食健康管理平台空产品页.png)

&emsp;&emsp;
进入[新建产品页面](https://iot.console.aliyun.com/product)，设定“产品名称”，这里我们命名为“**饮食健康管理平台**”，读者也可以根据自己的喜好来命名。在“所属品类”中，选择“自定义品类”。

&emsp;&emsp;
产品的节点类型选择“直连设备”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。开发者可根据自己的需求在“产品描述”页面添加针对此产品的描述。

&emsp;&emsp;
对于 ESP32 等搭载 Wi-Fi 的设备而言，联网方式选择“Wi-Fi”。

![饮食健康管理平台新建产品.png](./../../../images/health_management/饮食健康管理平台新建产品.png)

&emsp;&emsp;
点击“确认”按钮，即可完成产品创建。

![饮食健康管理平台完成创建产品.png](./../../../images/health_management/饮食健康管理平台完成创建产品.png)

&emsp;&emsp;
点击“前往定义物模型”

![饮食健康管理平台尚未添加任何功能.png](./../../../images/health_management/饮食健康管理平台尚未添加任何功能.png)

### 2.3、定义产品功能（物模型）
&emsp;&emsp;
开发者可以使用准备好的[物模型文件](./link_platform/model.zip)来进行快速导入。点击左上角“快速导入”，选择物模型文件并上传，就能够生成案例对应的物模型。

![饮食健康管理平台快速导入.png](./../../../images/health_management/饮食健康管理平台快速导入.png)

&emsp;&emsp;
生成后的效果如下：
![饮食健康管理平台导入完成.png](./../../../images/health_management/饮食健康管理平台导入完成.png)

&emsp;&emsp;
定义好物模型后，需要发布物模型上线，并发布产品，以使变更生效。

![饮食健康管理平台发布物模型.png](./../../../images/health_management/饮食健康管理平台发布物模型.png)

![饮食健康管理平台发布产品.png](./../../../images/health_management/饮食健康管理平台发布产品.png)

&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

### 2.4、创建设备及获取三元组
&emsp;&emsp;
点击左侧栏中“设备“，在筛选框中选择要添加设备的产品，点击“添加设备”。这里这里我们命名为“**health_management**”，开发者也可以根据自己的喜好来命名。

![饮食健康管理平台添加设备.png](./../../../images/health_management/饮食健康管理平台添加设备.png)

&emsp;&emsp;
开发者也可以选择“批量添加”，一次性添加多个设备，并生成随机的DeviceName。

![饮食健康管理平台批量添加.png](./../../../images/health_management/饮食健康管理平台批量添加.png)

&emsp;&emsp;
生成的设备如下。

![饮食健康管理平台设备列表.png](./../../../images/health_management/饮食健康管理平台设备列表.png)

点击前往“查看”按钮，就可以看到此设备的详细信息了。
![饮食健康管理平台设备详情.png](./../../../images/health_management/饮食健康管理平台设备详情.png)

&emsp;&emsp;
点击右上角的“查看”按钮，就能看到设备的三元组信息了。
三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

![饮食健康管理平台设备证书.png](./../../../images/health_management/饮食健康管理平台设备证书.png)

&emsp;&emsp;
再次前往物联网平台的设备信息页面，若设备运行正确，此时应该可以看到设备名右侧的状态由“未激活”变为“在线”。
选中“实时刷新”，可以看到数据实时从设备上报到物联网平台。设备上云成功。

![饮食健康管理平台物模型数据.png](./../../../images/health_management/饮食健康管理平台物模型数据.png)

## 3、物联网应用开发

IoT Studio 提供了应用快速开发的能力，可以很方便地与物联网平台进行联动。本节的开发工作也将围绕 IoT Studio展开。

### 3.1、新建“普通项目”

打开IoT Studio官网，在项目管理中新建一个空白项目，如下图所示，将此项目命名为“**饮食健康管理平台**”,开发者也可以根据自己的喜好来命名。

![饮食健康管理平台IS项目管理.png](./../../../images/health_management/饮食健康管理平台IS项目管理.png)

![饮食健康管理平台IS新建项目.png](./../../../images/health_management/饮食健康管理平台IS新建项目.png)

### 3.2、关联产品

为了使本项目能够获取到目标设备上报的卡路里信息等属性值，我们首先需要将该项目和我们在前一节创建的产品“饮食健康管理平台”绑定。
在项目控制台，点击左侧的“产品”，点击“关联物联网平台产品”。此时可以看见我们创建的“饮食健康管理平台”。点击选中，并勾选“关联产品同时关联其下所有设备”，以便该项目可以访问到所有设备的属性值。

![饮食健康管理平台IS关联产品.png](./../../../images/health_management/饮食健康管理平台IS关联产品.png)

### 3.3、创建“Web应用”

![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/饮食健康管理平台创建web应用.jpg)

### 3.4、“Web应用”开发
&emsp;&emsp;
从左侧组件栏中选择饮食健康管理平台应用需要的组件，将其拖入中间的窗口，按自己的喜好进行布局。本案例中，选择了折线图、数字框、下拉框、文本框、文字、按钮等组件。具体配置过程如下：
&emsp;&emsp;
#### 3.4.1、配置文本框组件
  - 配置识别结果文本框
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-识别结果1.jpg)
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-识别结果2.jpg)
  - 配置饮食建议文本框
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-饮食建议.jpg)

#### 3.4.2、配置数字框组件
  - 配置今日总热量数字框
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-热量.jpg)
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-热量2.jpg)
  - 配置身高数字框
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-身高.jpg)
  - 配置体重数字框
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-体重.jpg)
  - 配置年龄数字框
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-年龄.jpg)
  - 配置BMI数字框
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-bmi1.jpg)
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-bmi2.jpg)

#### 3.4.3、配置下拉组件
  - 配置性别下拉框
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-性别1.jpg)
  下拉框需要填入静态数据作为数据源供选择：
    ```json
    [
    {
        "label": "请选择",
        "value": 0
    },
    {
        "label": "男",
        "value": 1
    },
    {
        "label": "女",
        "value": 2
    }
    ]
    ```
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-性别2.jpg)

  - 配置活动强度下拉框
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-活动强度1.jpg)
  下拉框需要填入静态数据作为数据源供选择：
    ```json
    [
    {
        "label": "请选择",
        "value": 0
    },
    {
        "label": "常坐办公室，几乎不动",
        "value": 1
    },
    {
        "label": "每周轻松运动3-5日",
        "value": 2
    },
    {
        "label": "每周中等强度运动3-5日",
        "value": 3
    },
    {
        "label": "每周高强度运动6-7日",
        "value": 4
    },
    {
        "label": "重体力劳动",
        "value": 5
    }
    ]
    ```
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-活动强度2.jpg)

#### 3.4.4、配置按钮组件
  - 配置开始录入按钮
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-按钮1.jpg)
  - 配置结束录入按钮
  ![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-按钮2.jpg)

#### 3.4.5、业务逻辑开发
&emsp;&emsp;
由于折线图组件需要使用特定的数据格式，因此在配置折线图组件之前，需要先进行业务逻辑开发，对设备上报的属性值格式进行转换。
![饮食健康管理平台IS创建web应用创建业务逻辑.png](./../../../images/health_management/web-创建业务逻辑开发.jpg)
&emsp;&emsp;
在业务逻辑开发页面，我们选择设备触发、Nodejs脚本、应用推送三个节点，并连接三个节点。
![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-业务逻辑开发1.jpg)

#### 3.4.5.1、设备触发节点配置
&emsp;&emsp;
设备触发中，需要设置触发条件为**全部属性**。
![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-业务逻辑-设备触发.jpg)
#### 3.4.5.2、Nodejs节点配置
![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-业务逻辑-nodejs.jpg)
&emsp;&emsp;
在nodejs脚本节点，需要对上一节点**设备触发**节点的数据进行整理，输出折线图需要的格式。具体js脚本如下：（代码含义可参考注释）
```javascript
var object = {}; //最终输出的对象
var params = []; //将每天的时间和卡路里存入该对象

/**
* @param {Object} payload 上一节点的输出
* @param {Object} node 指定某个节点的输出
* @param {Object} query 服务流第一个节点的输出
* @param {Object} context  { appKey, appSecret }
* @param {Object} global 变量
*/
module.exports = async function(payload, node, query, context, global) {
console.log("payload: ", payload);
var obj = {};

obj['Calories'] = payload.props.Calories.value;//将设备上报的属性数据中的卡路里信息，存入Calories中
obj['time'] = payload.props.Date.value + "";//将设备上报的属性数据中的时间信息，存入time中

if (params.findIndex(v => v.time == obj.time) >= 0)
{
    params[params.findIndex(v => v.time == obj.time)].Calories = obj.Calories;//如果日期重复，则对卡路里信息进行更新
}
else
{
    params.push(obj);//如果是新的一条数据，则记录下来
}

object['data'] = params;
return object;
}
```
#### 3.4.5.3、应用推送节点配置
![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-业务逻辑开发-应用推送.jpg)
#### 3.4.5.4、业务逻辑调试
&emsp;&emsp;
保存业务逻辑后，点击调试按钮，进行调试。在弹出的窗口中，点击**前往**，开启模拟器。
![饮食健康管理平台IS创建web应用.png](./../../../images/health_management/业务逻辑开发-部署调试.jpg)
&emsp;&emsp;
选择产品及设备，点击启动模拟器。
![饮食健康管理平台IS创建web应用.png](./../../../images/health_management/模拟器-启动调试.jpg)
&emsp;&emsp;
在属性上报界面，输入各属性值，点击发送指令。
![饮食健康管理平台IS创建web应用.png](./../../../images/health_management/模拟器-推送属性数据.jpg)
&emsp;&emsp;
回到业务逻辑开发界面，右键单击选择查看节点日志来查看调试信息。
![饮食健康管理平台IS创建web应用.png](./../../../images/health_management/业务逻辑开发-查看调试信息.jpg)
&emsp;&emsp;
在Nodejs节点输出中看到上报的属性值已经转换成折线图需要的数据格式，说明Nodejs脚本已经生效。
![饮食健康管理平台IS创建web应用.png](./../../../images/health_management/业务逻辑开发-节点输出调试.jpg)
&emsp;&emsp;
在业务逻辑界面，点击发布业务逻辑。
![饮食健康管理平台IS创建web应用.png](./../../../images/health_management/业务逻辑开发-发布.jpg)

#### 3.4.6、配置折线图组件
&emsp;&emsp;
配置好业务逻辑后，我们可以进行折线图配置。
![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-折线图1.jpg)
![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-折线图2.jpg)
由于折线图组件需要提供数据源，选择刚刚创建的Web应用业务逻辑。
![饮食健康管理平台IS创建移动应用.png](./../../../images/health_management/web-折线图3.jpg)
&emsp;&emsp;
需要注意，此处将数据源配置为应用推送后，返回结果的预览界面仍然是空，这是由于折线图配置好后，尚未收到数据。在上一节中的**模拟器调试界面**，再次点击上报属性数据，就能看到返回结果预览界面出现数据。
![饮食健康管理平台IS创建web应用折线图.jpg](./../../../images/health_management/web-折线图4.jpg)
&emsp;&emsp;
将category和cnt作为横轴和纵轴拖动到指定位置后，点击右上角完成配置即可完成配置。
![饮食健康管理平台IS创建web应用折线图.jpg](./../../../images/health_management/web-折线图5.jpg)

### 3.5、HaaS Cloud平台配置
&emsp;&emsp;
由于需要用到HaaS聚合服务中菜品识别能力，获取图片中菜品的类别、卡路里等信息，因此需要在HaaS Cloud平台绑定物联网平台产品。

&emsp;&emsp;
进入[HaaS官网](https://haas.iot.aliyun.com/)
使用阿里云账号登陆后，点击右上角“控制台”。

![{name_img_static_HaaS官网}](./../../../images/health_management/haas官网.jpg)

&emsp;&emsp;
进入HaaS Cloud平台的控制台页面：
在左侧导航栏“设备管理”下，可以看到“产品”和“设备”两个选项，点击“产品”，点击“绑定物联网平台产品”，在右侧弹出的标签页中勾选我们刚刚创建的“**饮食健康管理平台**”，点击确定完成关联。

 ![{name_img_step22}](./../../../images/health_management/饮食健康管理平台HaaS控制台绑定产品.png)

在左侧导航栏“设备管理”下，点击“设备”，选择关联的产品，即可看到关联的设备列表。此时可以看到“**health_management**”出现在了列表中，证明关联成功。

 ![饮食健康管理平台HaaS控制台绑定产品.png](./../../../images/health_management/饮食健康管理平台HaaS控制台查看设备.png)

## 4、设备端开发
### 4.1、开发环境
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕。详情请参考[M5StackCore2快速开始](../../../startup/M5StackCore2_startup.md)的说明。

### 4.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“Hello world”案例点击“立即创建”即可。
<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

&emsp;&emsp;
将[饮食健康管理平台代码](./code/)文件下的所有脚本进行复制到“health_management”工程目录下，然后进行如下设定完成设备端代码的开发。
> Python脚本的详细说明请参考脚本内嵌的文字注释

1. **修改路由器名称及密码**

&emsp;&emsp;
修改health_management工程里main.py中SSID和PWD的值为开发者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接开发者自己设定的路由器。

2. **修改设备的三元组信息**

&emsp;&emsp;
按照创建产品、设备步骤获取三元组信息，填入main.py中：
```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```

&emsp;&emsp;
最后点击VSCode中HaaS Studio工程底部的部署运行或串口，选择对应的端口编号烧录程序。

## 5、运行结果
### 5.1、运行方式
* 推送脚本：推送python脚本到esp32后，程序会自动连接平台，并初始化云端AI引擎，当初始化完成之后，系统就绪，可开始工作。
* 输入信息：在数字框中输入身高、体重、年龄等信息，在下拉框中选择性别、每日活动强度信息。
* 开始录入：点击Web应用页面的**开始录入**按钮，设备端就会抓拍图像上传到云端AI，并接收云端识别结果。（注意：为避免持续上报数据造成重复，每点击一次开始录入，只会进行一次识别）
* 结束运行：点击点击Web应用页面的**结束录入**，设备端就会结束识别。
* 数据查看：可在折线图中查看每日摄入的卡路里信息统计数据。

### 5.2、本地查看
&emsp;&emsp;
推送此脚本到M5StackCore2之后，通过PC端串口调试工具可以看到设备端打印如下日志。日志的含义为:
* “link platform connected” 代表成功连接到云端平台。
* "upload--->"之后跟着的为设备端向云端发送的请求信息，其中"eventName"为"recognizeFood"，表示调用云端增值服务中的菜品识别功能。
* "download <----"之后跟着的是收到云端回复的识别结果信息：
    - result：菜品识别是否获取成功。（success：成功，failed：失败）
    - items中为返回的识别结果信息
      - calorie：菜品的卡路里
      - category：菜品名称（如清蒸鲈鱼）
      - score：置信度
    - commandName：recognizeFoodReply表明该消息是recognizeFood服务的返回信息。
```json
establish tcp connection with server(host='xxxxx.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=55
link platform connected

upload--->{'qos': 1, 'payload': '{"version": "1.0", "id": 1, "params": {"ext": "{\\"fileName\\": \\"test.jpg\\", \\"enlarge\\": 2, \\"filePosition\\": \\"lp\\", \\"fileId\\": \\"xxxxx\\"}", "eventType": "haas.faas", "eventName": "recognizeFood", "argInt": 1}}', 'topic': '/sys/xxx/health_management/thing/event/hli_event/post'}

download <----{'msg_id': 85854036, 'service_id': 'hli_async_service', 'params': '{"ext":"{\\"elements\\":[{\\"calorie\\":\\"224\\",\\"category\\":\\"\u6c49\u5821\\",\\"score\\":0.43390274}],\\"result\\":\\"success\\"}","commandName":"recognizeFoodReply","commandType":"haas.faas","argInt":1}', 'code': 0, 'params_len': 480}
detect: 汉堡 calorie:224 score: 0.4339027
```

### 5.3、物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”在线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。
<div align="center">
<img src=./../../../images/health_management/物联网平台数据.jpg width=100%/>
</div>

### 5.4、Web应用查看设备上报结果
&emsp;&emsp;
输入身高、体重、年龄、性别、运动习惯等数据后，点击**开始录入**，摄像头会开始抓拍识别食物，并将识别到的卡路里信息上报。Web端运行效果如下：
<div align="center">
<img src=./../../../images/health_management/web应用演示.gif width=100%/>
</div>
