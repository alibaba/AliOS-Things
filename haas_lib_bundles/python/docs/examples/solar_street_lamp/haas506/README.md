# 太阳能路灯远程监控
&emsp;&emsp;
本案例开发需要经过4步。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-导览.png width=90%/>
</div>

## 简介

&emsp;&emsp;
太阳能路灯已经被广泛使，如何远程监控已安装路灯的电池状态？ 如何优化路灯工作时长？如何保证最大限度利用太阳能资源？ 本案例将基于HaaS506 DTU开发板以及阿里云物联网平台和阿里云IoT Studio组态软件搭建云端一体太阳能路灯运行状态监控案例。


&emsp;&emsp;
在本案例中,手机小程序可以远程查看路灯的电池状态、当前光照强度、远程开关路灯、查看当前供电类型、光伏电和市电占比等信息


<br>



## 准备

**1. 硬件器材**

<div align="center">

|配件名称|数量|功能|购买链接|
|-----|:---:|:-----|----|
|HaaS506开发|1||[点我购买](https://item.taobao.com/item.htm?spm=a230r.1.14.1.712a1a7bHThfbt&id=651798670356&ns=1&abbucket=18#detail)|
|3.3V继电器|1|通过gpio口控制线路通断|[点我购买](https://detail.tmall.com/item.htm?id=530650857798&spm=a1z09.2.0.0.741c2e8dtOYS6q&_u=oh5g3cj6d75)|
|光伏发电套装（18V发电板20W + 12V控制器）|1||[点我购买](https://item.taobao.com/item.htm?spm=a230r.1.14.193.51d852b1owN1A2&id=660886029445&ns=1&abbucket=18#detai)|
|12V蓄电池|1||[点我购买](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.741c2e8dtOYS6q&id=645279478844&_u=oh5g3cj7f2b)|
|12V直流供电路灯|1||[点我购买](https://detail.tmall.com/item.htm?id=668423563320&spm=a1z09.2.0.0.741c2e8dtOYS6q&_u=oh5g3cjc9a7&sku_properties=20397:3234583)|
|DCDC降压模块|1||[点我购买](https://detail.tmall.com/item.htm?id=19905413537&spm=a1z09.2.0.0.741c2e8dtOYS6q&_u=oh5g3cjc239)|
|1M电阻|若干||[点我购买](https://detail.tmall.com/item.htm?spm=a220m.1000858.1000725.1.d4f7629475swvF&id=13302997879&skuId=3756188445694&areaId=330100&user_id=738263294&cat_id=2&is_b=1&rn=76e39d7f60e7768027b8149ac84e8bbb)|
|12V DC电源|1||根据灯泡功率自备|
|光照传感器|1|采集当前环境光线强度|
|杜邦线/导电线|若干||

</div>
<br>

**2. 硬件连线图**
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-连线图.png width=100%/>
</div>
<br>


## 物联网平台开发

物联网平台开发需要依次完成以下5步
1. 开通物联网平台实例
2. 创建云端产品
3. 定义产品功能
4. 为产品创建云端设备
5. 基于IoT Studio开发移动应用

### 开通物联网平台实例
&emsp;&emsp;
登陆[物联网平台](https://iot.console.aliyun.com/lk/summary/new)。
第一次使用物联网平台时候，首先需要在物联网平台创建一个实例。本案例选择使用免费的公共实例进行开发。如果您需要保证更多设备同时上云，需要购买企业实例。物联网平台创建公共实例的方式如下图所示， 左上角选择“华东2-上海”，点击“公共实例”，即可开通。
<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=100%/>
</div>

<br>

### 创建云端产品
&emsp;&emsp;
点击上一小节创建的公共实例就可以进入实例管理页面，在页面的左侧菜单中选中“设备管理” -> “产品”菜单项开始创建物联网产品。具体创建过程如下图所示：


**1. 点击创建产品按钮**
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建产品1.png
 width=100%/>
</div>

&emsp;&emsp;

**2. 填写产品基础信息以后点击“确认”按钮**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建产品2.png
 width=60%/>
</div>

&emsp;&emsp;
创建产品成功以后，页面的左侧菜单中选中“设备管理” -> “产品”菜单，可以查看已经创建的产品

### 定义产品功能

&emsp;&emsp;
点击“设备管理” -> "产品"菜单进入产品列表页，双击刚才创建的“太阳能路灯控制”产品 并 点击 “功能定义” 菜单，开始定义产品功能。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-定义产品功能1.png width=90%>
</div>


**1. 功能定义页面点击“编辑草稿”**
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-定义产品功能2.png width=90%>
</div>



**2. 点击“快速导入”**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-定义产品功能3.png width=60%>
</div>


**3. 点击“上传物模型”**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-定义产品功能4.png width=60%>
</div>


**3. 物模型成功以后，效果如下图，点击“发布上线”**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-定义产品功能5.png width=100%>
</div>

<br>

### 为产品创建云端设备


**1. 在产品列表页面中，点击”太阳能路灯监控“对应的“管理设备”按钮，进入设备管理页面。**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建云端设备1.png
 width=100%/>
</div>


**2. 点击“添加设备”按钮**
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建云端设备2.png width=80%/>
</div>


**3. 弹框中不填写任何信息，直接点击“确认”完成设备添加**
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建云端设备3.png width=50%/>
</div>


**4. 创建完云端设备以后，点击“设备管理”-> “设备” 菜单可以看到刚才创建的设备， 点击设备对应的“查看”按钮进入设备详情页面。 在详情页点击“查看” 按钮获取设备三元组。 设备三元组信息需要填写到设备端代码中。**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建云端设备4.png width=90%/>
</div>
<br>

### 基于IoT Studio开发移动应用


**1. 新建一个空白项目**

&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，点击屏幕左侧“项目管理”菜单 -> 点击“新建项目”按钮 -> 点击“创建空白项目“，项目名称填写“传感器数据采集”
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建IoTStudio项目.png width=90%/>
</div>

<br>

**2. 关联物联网产品和物联网设备”**

&emsp;&emsp;
点击“产品”旁边的“关联”按钮，然后选中前一章节创建的物联网产品完成关联。

&emsp;&emsp;
点击“设备”旁边的“关联”按钮，然后选中前一章节创建的物联网设备完成关联。

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-关联设备和产品.png width=90%/>
</div>

**3. 新建“移动应用”**

&emsp;&emsp;
点击“移动应用 ”按钮 ->点击“新建“按钮， 开始创建项目，选择“空白项目”。 
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建移动应用.png width=90%/>
</div>
&emsp;&emsp;
创建完毕以后自动跳转到应用UI可视化搭建页面。


**4. 可视化搭建”**
需要从左侧组件列表中依次拖动道UI面板对应位置
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-可视化搭建.png width=90%/>
</div>

设定“实时曲线组件”的数据源
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-设定实时曲线.png width=90%/>
</div>

参考“实时曲线组件”数据源的设定方式完成其他组件的数据源设定

<br>


## 设备端开发

### 设备端开发流程
    1. 搭建开发环境
    2. 创建HaaS Studio工程
    3. 拷贝案例代码
    4. 填写三元组信息
    5. 部署运行


### 搭建开发环境
&emsp;&emsp;
参考[HaaS506开发环境](../../../startup/HaaS506_startup.md)说明文档搭建软件开发环境。

&emsp;&emsp;
参考本文章开始处的“硬件连接图”连接各硬件模块

<br>

### 创建HaaS Studio工程

&emsp;&emsp;
如下图所示，打开HaaS Studio之后在新建一个基于helloworld的python工程，设定好工程名称（“environment_monitor”）及工作区路径之后，硬件类型选择HaaS506，点击立即创建，创建一个Python轻应用的解决方案。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建项目.png width=60%/>
</div>


### 拷贝案例代码

&emsp;&emsp;
将本案例代码复制到“环境监测”工程根目录下并覆盖工程原来的同名文件。 代码详细逻辑可以参考代码中的注释。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-代码工程.png width=100%/>
</div>

<br>

### 填写三元组信息
&emsp;&emsp;
根据创建云端设备章节中获取到的设备三元组信息 修改main.py中 "productKey" "deviceName" "deviceName"三个变量。 然后点击部署运行按钮并查看运行结果。

<br>

### 部署运行
&emsp;&emsp;
点击IDE左下角的“部署运行”按钮，部署应用到haas506开发板。部署完毕以后，自动连接云平台

<br>


## 调试验证

&emsp;&emsp;
再次进入iot studio 移动应用界面，点击"预览"
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-预览.png width=90%/>
</div>

&emsp;&emsp;
实时显示环境数据，远程切换供电源，远程开关等
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-运行效果.png width=40%/>
</div>

<br>

### 代码

代码流程：
1. 链接网络，网络链接成功以后，haas506开发版 网络状态led灯会点亮
2. 链接物联网平台，并注册平台消息监听器， 连接成功以后会打印“物联网平台连接成功”
3. 周期上报太阳能路灯信息

board.json
```
{
  "name": "haas506",
  "version": "1.0.0",
  "io": {
      "modbus_485_4800": {
          "type": "MODBUS",
          "mode": 0,
          "port": 2,
          "baudrate": 4800,
          "priority": 0,
          "timeout": 200
      },
      "cloud_led": {
        "type": "GPIO",
        "port": 9,
        "dir": "output",
        "pull": "pulldown"
      },
      "net_led": {
        "type": "GPIO",
        "port": 7,
        "dir": "output",
        "pull": "pulldown"
      },
      "electric_relay1_0": {
        "type": "GPIO",
        "port": 16,
        "dir": "output",
        "pull": "pulldown"
      },
      "electric_relay1_1": {
        "type": "GPIO",
        "port": 17,
        "dir": "output",
        "pull": "pulldown"
      },
      "electric_relay1_2": {
        "type": "GPIO",
        "port": 18,
        "dir": "output",
        "pull": "pulldown"
      },
      "ADC1": {
        "type": "ADC",
        "port": 1,
        "sampling": 12000000
      },
      "ina219": {
        "type": "I2C",
        "port": 0,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 64
      }
  },
  "debugLevel": "ERROR",
  "repl": "disable"
}
```

main.py
```
#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import utime
import ujson
from aliyunIoT import Device
import modbus as mb
import yuanda_htb485
import network
from driver import GPIO
from driver import ADC

# 4G.Cat1连网代码
g_connect_status = False
def on_4g_cb(args):
    print("4g callback.")
    global g_connect_status
    netwk_sta = args[1]
    if netwk_sta == 1:
        g_connect_status = True
    else:
        g_connect_status = False

def connect_4g_network():
    global on_4g_cb
    net = network.NetWorkClient()
    net_led = GPIO()
    net_led.open('net_led')
    
    g_register_network = False
    if net._stagecode is not None and net._stagecode == 3 and net._subcode == 1:
        g_register_network = True
    else:
        g_register_network = False
        
    if g_register_network:
        net.on(1,on_4g_cb)
        # net.connect({"apn":"CMNET"})
        net.connect({'apn':'CMIOTNJSHA.JS'})
    else:
        print('network register failed')
        
    while True:
        if g_connect_status==False:
            net_led.write(0)
        if g_connect_status:
            print('network register successed')
            #连上网之后，点亮cloud_led
            net_led.write(1)
            break
        utime.sleep_ms(20)
    return 0

power_type_switch = GPIO()
power_switch = GPIO()
work_mode = 0

# 上传路灯信息
def upload_lamp_info(device):
    global power_type_switch, power_switch, work_mode
    mb.init('modbus_485_4800')
    htb485Obj = yuanda_htb485.HTB485(mb, 1)
    power_type_switch.open("electric_relay1_1")
    power_switch.open("electric_relay1_2")
    adc = ADC()
    adc.open("ADC1")

    dc_time = 0
    bat_time = 0
    while True:
        brightness = htb485Obj.getBrightness()
        battery_voltage = adc.readVoltage() * 5 / 1000.0
        lamp_status = power_switch.read()
        power_type = power_type_switch.read()
        if (brightness < 600 and work_mode == 0):                
            if (battery_voltage < 11.0):
                power_type = 0
                power_type_switch.write(0)
                dc_time += 1
            else:
                power_type = 1
                power_type_switch.write(1)
                bat_time += 1
                
            power_switch.write(1)
            lamp_status = 1
            
        if (brightness > 700 and work_mode == 0):
            power_switch.write(0)
            lamp_status = 0

        # "brightness"/"lamp_state"/"battery_voltage/power_type/dc_time/bat_time"必须和物联网平台的属性一致
        upload_data = {'params': ujson.dumps({
            'brightness': brightness,
            "lamp_state": lamp_status,
            "battery_voltage":battery_voltage,
            'power_type': power_type,
            "dc_time": dc_time,
            "bat_time": bat_time,
            "work_mode": work_mode
        })
        }
        print(upload_data)
        # 上传温度和湿度信息到物联网平台
        device.postProps(upload_data)
        utime.sleep(2)
        
    mb.deinit()
    
# 物联网平台连接成功的回调函数
def on_connect(data):
    print("物联网平台连接成功")

# 处理物联网平台下行指令
cmd_name_list = ["lamp_state", "power_type", "work_mode"]
def on_props(request):
    global power_type_switch, power_switch, work_mode
    payload = ujson.loads(request['params'])    
    for i in range(0, len(cmd_name_list)):
        if cmd_name_list[i] in payload.keys():
            value = payload[cmd_name_list[i]]
            if cmd_name_list[i] is 'work_mode':
                work_mode = value
            else:
                work_mode = 1
            
            if cmd_name_list[i] is 'lamp_state':
                power_switch.write(value)
            
            if cmd_name_list[i] is 'power_type':
                power_type_switch.write(value)

            prop = ujson.dumps({
                cmd_name_list[i]: value,
            })
            upload_data = {'params': prop}
            device.postProps(upload_data)

# 连接物联网平台
def connect_lk():
    key_info = {
        'region': 'cn-shanghai',
        'productKey': "xxx",
        'deviceName': "xxx",
        'deviceSecret': "xxx",
        'keepaliveSec': 60
    }
    device = Device()
    device.on(Device.ON_CONNECT, on_connect)
    device.on(Device.ON_PROPS, on_props)
    device.connect(key_info)
    return device

if __name__ == '__main__':
    connect_4g_network()
    utime.sleep(2)
    device = connect_lk()
    utime.sleep(2)
    upload_lamp_info(device)
```

yuanda_htb486.py

```
import ustruct

class HTB485(object):
    
    def __init__(self, mbObj, devAddr):
        self.mbObj = mbObj
        self.devAddr = devAddr
    
    def getHumidity(self):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        value = bytearray(4)
        ret = self.mbObj.readHoldingRegisters(self.devAddr, 0, 2, value, 200)
        if ret[0] < 0:
            raise ValueError("readHoldingRegisters failed. errno:", ret[0])
        
        humidity = ustruct.unpack('hh',value)
        return humidity[0]
    
    def getTemperature(self):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        value = bytearray(4)
        ret = self.mbObj.readHoldingRegisters(self.devAddr, 0, 2, value, 200)
        if ret[0] < 0:
            raise ValueError("readHoldingRegisters failed. errno:", ret[0])
        temperature = ustruct.unpack('hh',value)
        return temperature[1]
    
    def getBrightness(self):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        value = bytearray(4)
        ret = self.mbObj.readHoldingRegisters(self.devAddr, 2, 2, value, 200)
        if ret[0] < 0:
            raise ValueError("readHoldingRegisters failed. errno:", ret[0])
        brightness = ustruct.unpack('hh',value)
        return brightness[1]
    
    def getHTB(self):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        value = bytearray(10)
        ret = self.mbObj.readHoldingRegisters(self.devAddr, 0, 5, value, 200)
        if ret[0] < 0:
            raise ValueError("readHoldingRegisters failed. errno:", ret[0])
        htb = ustruct.unpack('hhhhh',value)
        return htb

```