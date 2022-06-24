# 智能水果秤

## 1、简介
&emsp;&emsp;
智能水果秤是一种具备AI识别与自动计算能力的电子秤。它可根据摄像头拍摄到的水果照片自动识别出水果种类并在数据库中查询该水果的单价，然后结合电子秤称出的水果重量，最后计算出售价。

### 1.1、准备
硬件列表
1. m5stack开发板一个
2. uart camera一个
3. HX711电子秤传感器一个
4. 杜邦连接线若干

m5stack开发板与uart camera及电子秤传感器之间的硬件连线
<table>
    <tr>
        <td align="center">硬件</td>
        <td align="center">uart camera</td>
        <td align="center">HX711电子秤传感器</td>
        <td align="center">m5stack开发板</td>
    </tr>
    <tr>
        <td align="center" rowspan="7">端口标识</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">VCC</td>
        <td align="center">3.3V</td>
    </tr>
    <tr>
        <td align="center">VCC</td>
        <td align="center">-</td>
        <td align="center">5V</td>
    </tr>
    <tr>
        <td align="center">RX</td>
        <td align="center">-</td>
        <td align="center">TX</td>
    </tr>
    <tr>
        <td align="center">TX</td>
        <td align="center">-</td>
        <td align="center">RX</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">CLK</td>
        <td align="center">P19</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">DT</td>
        <td align="center">P27</td>
    </tr>
    <tr>
        <td align="center" rowspan="1">硬件说明</td>
        <td align="center" >-</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C359B2CFF37AB00000001&dataId=800C359B2CFF37AB" target="_blank">电子秤传感器详情</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C84FAF561DF6A00000001&dataId=800C84FAF561DF6A" target="_blank">详细端口定义</td>
    </tr>
</table>

&emsp;&emsp;
硬件连线图如下所示：
<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_电子秤接线图.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_连线图.jpg width=100%/>
</div>

## 2、物联网平台开发

&emsp;&emsp;
整个过程包含以下3个步骤：

1. 开通公共实例
2. 创建产品（设备模型）
3. 创建设备及获取三元组

### 2.1、开通公共实例
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。开通后点击“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_物联网平台.png width=100%/>
</div>

### 2.2、创建产品（设备模型）
&emsp;&emsp;
进入[公共实例控制台](https://iot.console.aliyun.com/lk/summary/new)，点击“创建产品”按钮，即可进入[新建产品页面](https://iot.console.aliyun.com/product)。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_空产品页.png width=100%/>
</div>

&emsp;&emsp;
进入[新建产品页面](https://iot.console.aliyun.com/product)，设定“产品名称”，这里我们命名为“**智能水果秤**”，读者也可以根据自己的喜好来命名。在“所属品类”中，选择“自定义品类”。

&emsp;&emsp;
产品的节点类型选择“直连设备”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。开发者可根据自己的需求在“产品描述”页面添加针对此产品的描述。

&emsp;&emsp;
对于搭载 Wi-Fi 的设备而言，联网方式选择“Wi-Fi”。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_新建产品.png width=100%/>
</div>

&emsp;&emsp;
点击“确认”按钮，即可完成产品创建。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_完成创建产品.png width=100%/>
</div>

&emsp;&emsp;
点击“前往定义物模型”

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_尚未添加任何功能.png width=100%/>
</div>

&emsp;&emsp;
这里不定义功能直接点击“发布”进行产品发布

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_发布产品.png width=100%/>
</div>

### 2.3、创建设备及获取三元组
&emsp;&emsp;
点击左侧栏中“设备“，在筛选框中选择要添加设备的产品，点击“添加设备”。这里这里我们命名为“**fruit_scale**”，开发者也可以根据自己的喜好来命名。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_添加设备.png width=100%/>
</div>

&emsp;&emsp;
开发者也可以选择“批量添加”，一次性添加多个设备，并生成随机的DeviceName。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_批量添加.png width=100%/>
</div>

&emsp;&emsp;
生成的设备如下。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_设备列表.png width=100%/>
</div>

点击前往“查看”按钮，就可以看到此设备的详细信息了。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_设备详情.png width=100%/>
</div>

&emsp;&emsp;
点击右上角的“查看”按钮，就能看到设备的三元组信息了。
三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_设备证书.png width=100%/>
</div>

## 3、HaaS云端积木平台

&emsp;&emsp;
进入[HaaS官网](https://haas.iot.aliyun.com/)
使用阿里云账号登陆后，点击右上角“控制台”。进入HaaS云端积木的控制台页面：

<div align="center">
<img src=./../../../images/fruit_scale/HaaS控制台.png width=100%/>
</div>

&emsp;&emsp;
在左侧导航栏“设备管理”下，可以看到“产品”和“设备”两个选项，点击“产品”，点击“绑定物联网平台产品”，在右侧弹出的标签页中勾选我们刚刚创建的“**智能水果秤**”，点击确定完成关联。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_HaaS控制台绑定产品.png width=100%/>
</div>

&emsp;&emsp;
在左侧导航栏“设备管理”下，点击“设备”，选择关联的产品，即可看到关联的设备列表。此时可以看到“**fruit_scale**”出现在了列表中，证明关联成功。

<div align="center">
<img src=./../../../images/fruit_scale/智能水果秤_HaaS控制台查看设备.png width=100%/>
</div>

## 4、设备端开发

### 4.1、开发环境

&emsp;&emsp;
在进行下一步之前请确保M5Stack开发环境已经搭建完毕。详情请参考[M5Stack开发环境](../../../startup/M5StackCore2_startup.md)的说明。

### 4.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“智能水果秤”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

1. **填写Wi-Fi名称及密码**

&emsp;&emsp;
在main.py中，填写可用的Wi-Fi名称及密码。

``` python
# wifi连接的的ssid和pwd定义
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

2. **修改设备端三元组**

&emsp;&emsp;
在main.py中，填写创建的设备三元组信息。关于设备三元组的获取，请参考[创建设备及获取三元组](./README.md "创建设备及获取三元组")中的步骤。

``` python
# 三元组信息
productKey     = "产品key"
deviceName     = "设备名称"
deviceSecret   = "设备密钥"
```