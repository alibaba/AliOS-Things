# 敲击手势检测
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/敲击手势检测系统-步骤简介.png width=70%/>
</div>
<br>

## 简介
&emsp;&emsp;
手势识别是物联网智能感知领域里的一项重要技术，通过检测手势动作，可以将之映射成不同的指令，用于智能家居场景的各种控制，实现智能交互。本案例通过识别敲击（Tap）的手势动作，并将手势动作上报至云端，管理者可实时查看手势动作的识别记录。

&emsp;&emsp;
本案例旨在实现对“敲击手势动作（Tap）”的识别，可以设置敲击动作的参数，例如一个手势对应几次敲击动作、每个敲击动作的检测阈值、每两个敲击动作之间的最长时间间隔等。手势识别成功后，将手势识别结果上报至云端，做到实时记录。

<br>

## 背景知识

&emsp;&emsp;
当前，越来越多的电子设备开始支持手势识别功能，手势识别旨在识别人类的物理运动或“手势”，以实现智能交互。例如，在电子设备前以特定的模式挥手可以让它执行特定的功能。通过手势识别，与物联网相结合，可以实现对设备的控制，创造智能生活场景。手势识别也打开了智能交互的新大门，用户可以定义自己的手势交互，用手指触控未来。

<br>

## 准备

1. [M5StackCore2 开发板一套（内置 MPU6886 传感器）](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C84FAF561DF6A00000001&dataId=800C84FAF561DF6A)

<br>

## 物联网平台开发

### 开通公共实例

&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东 2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/敲击手势检测系统_物联网平台开发_物联网平台.png width=100%/>
</div>

开通物联网平台功能之后，需要完成下面的 3 个步骤完成云端设备的设定：

1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

<br>

### 创建产品（设备模型）

&emsp;&emsp;
进入[公共实例控制台](https://iot.console.aliyun.com/lk/summary/new)，点击“创建产品”按钮，即可进入[新建产品页面](https://iot.console.aliyun.com/product)。

![敲击手势检测系统_物联网平台开发_空产品页.png](./../../../images/敲击手势检测系统_物联网平台开发_空产品页.png)

&emsp;&emsp;
进入[新建产品页面](https://iot.console.aliyun.com/product)，设定“产品名称”，这里我们命名为“**敲击手势检测系统**”，读者也可以根据自己的喜好来命名。在“所属品类”中，选择“自定义品类”。

&emsp;&emsp;
产品的节点类型选择“直连设备”，数据格式选择“ICA 标准数据格式”，检验类型和认证方式选择默认设定即可。开发者可根据自己的需求在“产品描述”页面添加针对此产品的描述。

&emsp;&emsp;
对于 M5StackCore2 等搭载 Wi-Fi 的设备而言，联网方式选择“Wi-Fi”。

![敲击手势检测系统_物联网平台开发_新建产品.png](./../../../images/敲击手势检测系统_物联网平台开发_新建产品.png)

&emsp;&emsp;
点击“确认”按钮，即可完成产品创建。

![敲击手势检测系统_物联网平台开发_完成创建产品.png](./../../../images/敲击手势检测系统_物联网平台开发_完成创建产品.png)

&emsp;&emsp;
点击“前往定义物模型”。
<br>

### 定义产品功能（物模型）

&emsp;&emsp;
点击功能定义，再点击编辑草稿。

![敲击手势检测系统_物联网平台开发_尚未添加任何功能.png](./../../../images/敲击手势检测系统_物联网平台开发_尚未添加任何功能.png)

&emsp;&emsp;
开发者可以使用准备好的[物模型文件](https://hli.aliyuncs.com/o/config/tap_detect_model/model.zip)来进行快速导入。点击左上角“快速导入”，选择物模型文件并上传，就能够生成案例对应的物模型。

![敲击手势检测系统_物联网平台开发_快速导入.png](./../../../images/敲击手势检测系统_物联网平台开发_快速导入.png)

&emsp;&emsp;
生成后的效果如下：
![敲击手势检测系统_物联网平台开发_导入完成.png](./../../../images/敲击手势检测系统_物联网平台开发_导入完成.png)

&emsp;&emsp;
定义好物模型后，需要发布物模型上线，并发布产品，以使变更正式生效。

![敲击手势检测系统_物联网平台开发_发布物模型.png](./../../../images/敲击手势检测系统_物联网平台开发_发布物模型.png)

![敲击手势检测系统_物联网平台开发_发布产品.png](./../../../images/敲击手势检测系统_物联网平台开发_发布产品.png)

&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

### 创建设备及获取三元组

&emsp;&emsp;
点击左侧栏中“设备“，在筛选框中选择要添加设备的产品，点击“添加设备”。这里这里我们命名为“**test_device**”，开发者也可以根据自己的喜好来命名。

![敲击手势检测系统_物联网平台开发_添加设备.png](./../../../images/敲击手势检测系统_物联网平台开发_添加设备.png)

&emsp;&emsp;
开发者也可以选择“批量添加”，一次性添加多个设备，并生成随机的 DeviceName。

![敲击手势检测系统_物联网平台开发_批量添加.png](./../../../images/敲击手势检测系统_物联网平台开发_批量添加.png)

&emsp;&emsp;
生成的设备如下。

![敲击手势检测系统_物联网平台开发_设备列表.png](./../../../images/敲击手势检测系统_物联网平台开发_设备列表.png)

点击前往“查看”按钮，就可以看到此设备的详细信息了。
![敲击手势检测系统_物联网平台开发_设备详情.png](./../../../images/敲击手势检测系统_物联网平台开发_设备详情.png)

&emsp;&emsp;
点击右上角的“查看”按钮，就能看到设备的三元组信息了。
三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

![敲击手势检测系统_物联网平台开发_设备证书.png](./../../../images/敲击手势检测系统_物联网平台开发_设备证书.png)
<br>

## 设备端开发

### 开发环境

&emsp;&emsp;
在进行下一步之前请确保 M5StackCore2 开发环境已经搭建完毕。详情请参考[M5StackCore2 开发环境](../../../startup/M5StackCore2_startup.md)的说明。
<br>

### 创建解决方案

&emsp;&emsp;
如下图所示，打开 VS Code 之后在新建一个基于 helloworld 的 python 工程，设定好工程名称（“tap_detection”）及工作区路径之后，硬件类型选择 m5stackcore2，点击立即创建，创建一个 Python 轻应用的解决方案。

<div align="center">
<img src=./../../../images/敲击手势检测系统_工程创建.png width=80%/>
</div>

&emsp;&emsp;
将[敲击手势检测系统代码](./code/)文件下的所有脚本进行复制到“tap_detection”工程根目录中，main.py 代码如下所示。

```python
if __name__ == '__main__':
    wlan = network.WLAN(network.STA_IF)  # 创建WLAN对象
    get_wifi_status()
    connect_lp(productKey, deviceName, deviceSecret)

    # 硬件初始化
    i2cObj = I2C()
    # 按照board.json中名为"mpu6886"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
    i2cObj.open("mpu6886")
    if not isinstance(i2cObj, I2C):
        raise ValueError("parameter is not an I2C object")
    print("mpu6886 inited!")
    mpu6886Dev = mpu6886.MPU6886(i2cObj)   # 初始化MPU6886传感器

    # 跌倒检测算法初始化
    tapDetection = tap_detection.tap_detection(mpu6886Dev)
    tapDetection.enable_tap_detection()   # 设置tap_detection参数
    tapDetection.calibrate()              # 校准传感器

    while True:
        tap_count = tapDetection.detect_tap()  # 检测是否跌倒
        if (tap_count == True):                   # in event of a tap detection
            print("tap_count DETECTED using MPU sensor")
            # 上传敲击手势到物联网平台
            if (iot_connected):
                upload_data = {'params': ujson.dumps({
                    'tap_count': tapDetection.tap_detect_count,
                })
                }
                device.postProps(upload_data)
        utime.sleep_us(10)
    i2cObj.close()                                      # 关闭I2C设备对象
    del mpu6886Dev
```

1. **修改路由器名称及密码**

&emsp;&emsp;
修改 tap_detection 工程里 main.py 中 SSID 和 PWD 的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后 connect_lp 函数就会连接读者自己设定的路由器。

2. **修改设备的三元组信息**

&emsp;&emsp;
将在阿里云物联网平台申请的设备三元组信息，填入 main.py 中：

```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```

3. **设置手势动作检测参数**

&emsp;&emsp;
在 tap_detection 工程的 main.py 里，调用 enable_tap_detection 函数可以设置手势动作检测参数，如果不传参数，则 enable_tap_detection 函数将使用默认参数。

```python
# 设置enable_tap_detection参数
tapDetection.enable_tap_detection()
"""
        The tap detection parameters.

        :param int tap_count: 手势对应的敲击次数参数, 1代表识别单击手势, 2代表识别双击手势, 默认值2.

        :param float accelerator_up_threshold: 手势检测阈值，越小越灵敏，默认值为30.

        :param int latency: 两次敲击动作之间的最大间隔, 默认值150.

        .. code-block:: python
            # 如果不传参数，将会使用默认参数值。
            accelerometer.enable_tap_detection(
                tap_detect_count = 2,
                accelerator_up_threshold = 30,
                latency = 150
            )

 """
```

4. **检测手势动作**

&emsp;&emsp;
在 tap_detection 工程的 main.py 里，调用 tapDetection.detect_tap()函数来进行手势识别，并上传识别结果到云端。

```python
# 手势识别主循环
while True:
        tap_count = tapDetection.detect_tap()  # 检测是否跌倒
        if (tap_count == True):                # in event of a tap detection
            print("tap_count DETECTED using MPU sensor")
            # 上传敲击手势到物联网平台
            if (iot_connected):
                upload_data = {'params': ujson.dumps({
                    'tap_count': tapDetection.tap_detect_count,
                })
                }
                device.postProps(upload_data)
        utime.sleep_us(10)
```

## 运行程序

1. **推送脚本**

&emsp;&emsp;
推送此工程到 M5StackCore2 设备上，如下图所示。

<div align="center">
<img src=./../../../images/敲击手势检测系统-推送步骤.png width=90%/>
</div>

2. **推送脚本时的日志**

&emsp;&emsp;
推送过程中的日志输出如下图所示。如果需要选择串口号，请选择“/dev/cu.wchusbserial53190134851 'USB Single Serial'”，直接输入串口号前面的数字即可。

<div align="center">
<img src=./../../../images/敲击手势检测系统-推送脚本log.png width=90%/>
</div>

3. **敲击 M5StackCore2**

&emsp;&emsp;
用手指敲击 M5StackCore2 的屏幕 n 次，n 与 tapDetection.enable_tap_detection()初始化函数的 tap_detect_count 参数对应。

<div align="center">
<img src=./../../../images/敲击手势检测系统-敲击示例.png width=30%/>
</div>

4. **程序运行时的日志**

&emsp;&emsp;
设备上通过串口打印的信息如下，当打印“Now you can start tap detection!”后，即可开始敲击手势动作的检测。打印“tap_count DETECTED using MPU sensor”表示识别到一次“敲击手势动作（Tap）”。

<div align="center">
<img src=./../../../images/敲击手势检测系统-串口Log.png width=90%/>
</div>

<br>

## 云端查看

进入阿里云官网，用阿里云账号[登录物联网平台](https://iot.console.aliyun.com/devices/)查看状态

<div align="center">
<img src=./../../../images/敲击手势检测系统-物联网平台-手势识别结果记录.png width=90%/>
</div>

<br>

到此为止，敲击手势检测系统案例就已经完成了。如果想学习更多实验，请参考[HaaS 案例实践详解](https://gitee.com/haasedu/haasedu/tree/release_2.0)。
