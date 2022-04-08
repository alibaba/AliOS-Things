# 太阳能路灯远程监控
&emsp;&emsp;
本案例开发需要经过4步。
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-导览.png width=90%/>
</div>

## 简介

&emsp;&emsp;
光伏发电已经被广泛使，但是光伏板在使用过程中因为镜面堆积灰尘会导致发掉效率较低，所以，需要定期清洁，那么远程决策者如何获知光伏板的工作状态，并决策光伏板是否需要进行清洁呢？ 本案例将基于HaaS506 DTU开发板以及阿里云物联网平台搭建云端一体光伏运行效率监控案例，通过本案例可以远程监控光伏发电系统的运行状态。


<br>



## 准备

**1. 硬件器材**

<div align="center">

|配件名称|数量|功能|购买链接|
|-----|:---:|:-----|----|
|HaaS506开发|1||[点我购买](https://item.taobao.com/item.htm?spm=a230r.1.14.1.712a1a7bHThfbt&id=651798670356&ns=1&abbucket=18#detail)|
|光伏发电套装（18V发电板20W + 12V控制器）|1||[点我购买](https://item.taobao.com/item.htm?spm=a230r.1.14.193.51d852b1owN1A2&id=660886029445&ns=1&abbucket=18#detai)|
|12V蓄电池|1||[点我购买](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.741c2e8dtOYS6q&id=645279478844&_u=oh5g3cj7f2b)|
|DCDC降压模块|1||[点我购买](https://detail.tmall.com/item.htm?id=19905413537&spm=a1z09.2.0.0.741c2e8dtOYS6q&_u=oh5g3cjc239)|
|1M电阻|若干||[点我购买](https://detail.tmall.com/item.htm?spm=a220m.1000858.1000725.1.d4f7629475swvF&id=13302997879&skuId=3756188445694&areaId=330100&user_id=738263294&cat_id=2&is_b=1&rn=76e39d7f60e7768027b8149ac84e8bbb)|
|光照传感器|1|采集当前环境光线强度|
|杜邦线/导电线|若干||

</div>
<br>

**2. 硬件连线图**
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-连线图.png
width=90%/>
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
<img src=./../../../images/光伏面板监测/光伏面板监测-创建产品1.png
 width=100%/>
</div>

&emsp;&emsp;

**2. 填写产品基础信息以后点击“确认”按钮**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建产品2.png
 width=60%/>
</div>

&emsp;&emsp;
创建产品成功以后，页面的左侧菜单中选中“设备管理” -> “产品”菜单，可以查看已经创建的产品

### 定义产品功能

&emsp;&emsp;
点击“设备管理” -> "产品"菜单进入产品列表页，双击刚才创建的“太阳能路灯控制”产品 并 点击 “功能定义” 菜单，开始定义产品功能。
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-定义产品功能1.png width=90%>
</div>


**1. 功能定义页面点击“编辑草稿”**
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-定义产品功能2.png width=90%>
</div>



**2. 点击“快速导入”**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-定义产品功能3.png width=60%>
</div>


**3. 点击“上传物模型”**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-定义产品功能4.png width=60%>
</div>


**3. 物模型成功以后，效果如下图，点击“发布上线”**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-定义产品功能5.png width=100%>
</div>

<br>

### 为产品创建云端设备


**1. 在产品列表页面中，点击”太阳能路灯监控“对应的“管理设备”按钮，进入设备管理页面。**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建云端设备1.png
 width=100%/>
</div>


**2. 点击“添加设备”按钮**
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建云端设备2.png width=80%/>
</div>


**3. 弹框中不填写任何信息，直接点击“确认”完成设备添加**
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建云端设备3.png width=50%/>
</div>


**4. 创建完云端设备以后，点击“设备管理”-> “设备” 菜单可以看到刚才创建的设备， 点击设备对应的“查看”按钮进入设备详情页面。 在详情页点击“查看” 按钮获取设备三元组。 设备三元组信息需要填写到设备端代码中。**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建云端设备4.png width=90%/>
</div>
<br>

### 基于IoT Studio开发移动应用


**1. 新建一个空白项目**

&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，点击屏幕左侧“项目管理”菜单 -> 点击“新建项目”按钮 -> 点击“创建空白项目“，项目名称填写“传感器数据采集”
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建IoTStudio项目.png width=90%/>
</div>

<br>

**2. 关联物联网产品和物联网设备”**

&emsp;&emsp;
点击“产品”旁边的“关联”按钮，然后选中前一章节创建的物联网产品完成关联。

&emsp;&emsp;
点击“设备”旁边的“关联”按钮，然后选中前一章节创建的物联网设备完成关联。

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-关联设备和产品.png width=90%/>
</div>

**3. 新建“移动应用”**

&emsp;&emsp;
点击“移动应用 ”按钮 ->点击“新建“按钮， 开始创建项目，选择“空白项目”。 
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建移动应用1.png width=90%/>
</div>

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建移动应用2.png width=90%/>
</div>
&emsp;&emsp;
创建完毕以后自动跳转到应用UI可视化搭建页面。



**4. 可视化搭建”**
需要从左侧组件列表中依次拖动道UI面板对应位置
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-可视化搭建.png width=90%/>
</div>

参考以下步骤设定每个卡片数据源
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-设定实时曲线.png width=90%/>
</div>

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
<img src=./../../../images/光伏面板监测/光伏面板监测-创建工程.png width=60%/>
</div>


### 拷贝案例代码

&emsp;&emsp;
将本案例代码复制到“环境监测”工程根目录下并覆盖工程原来的同名文件。 代码详细逻辑可以参考代码中的注释。
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-代码工程.png width=100%/>
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
<img src=./../../../images/光伏面板监测/光伏面板监测-预览.png width=90%/>
</div>

&emsp;&emsp;
显示光伏面板状态，触发报警
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-运行效果.png width=40%/>
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
import ina219
from driver import GPIO
from driver import ADC
from driver import I2C

# 4G.Cat1连网代码
g_connect_status = False
def on_4g_cb(args):
    print("4g callback.")
    global g_connect_status
    pdp = args[0]
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


# 上传路灯信息
def upload_battery_info(device):
    mb.init('modbus_485_4800')
    htb485Obj = yuanda_htb485.HTB485(mb, 1)
    adc = ADC()
    adc.open("ADC1")
    
    i2cObj = I2C()
    i2cObj.open("ina219")
    ina219Dev = ina219.INA219(i2cObj)                                                                                                        
    while True:
        brightness = htb485Obj.getBrightness()
        battery_voltage = adc.readVoltage() * 5 / 1000.0
        # current =  ina219Dev.current() / 1000.0
        current = 0.11
        # "brightness"/"battery_voltage"/"current"必须和物联网平台的属性一致
        upload_data = {'params': ujson.dumps({
            'brightness': brightness,
            "battery_voltage":battery_voltage,
            'current': current
        })
        }
        print(upload_data)
        # 上传温度和湿度信息到物联网平台
        device.postProps(upload_data)
        utime.sleep(5)
        
    mb.deinit()
    
# 物联网平台连接成功的回调函数
def on_connect(data):
    print("物联网平台连接成功")

# 处理物联网平台下行指令
def on_props(request):
    print("新消息：", request)

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
    upload_battery_info(device)

```

ina219.py

```
import utime
from math import trunc
from micropython import const
from driver import I2C

class INA219(object):
    RANGE_16V = const(0)  # Range 0-16 volts
    RANGE_32V = const(1)  # Range 0-32 volts

    GAIN_1_40MV = const(0)  # Maximum shunt voltage 40mV
    GAIN_2_80MV = const(1)  # Maximum shunt voltage 80mV
    GAIN_4_160MV = const(2)  # Maximum shunt voltage 160mV
    GAIN_8_320MV = const(3)  # Maximum shunt voltage 320mV
    GAIN_AUTO = const(-1)  # Determine gain automatically

    ADC_9BIT = const(0)  # 9-bit conversion time  84us.
    ADC_10BIT = const(1)  # 10-bit conversion time 148us.
    ADC_11BIT = const(2)  # 11-bit conversion time 2766us.
    ADC_12BIT = const(3)  # 12-bit conversion time 532us.
    ADC_2SAMP = const(9)  # 2 samples at 12-bit, conversion time 1.06ms.
    ADC_4SAMP = const(10)  # 4 samples at 12-bit, conversion time 2.13ms.
    ADC_8SAMP = const(11)  # 8 samples at 12-bit, conversion time 4.26ms.
    ADC_16SAMP = const(12)  # 16 samples at 12-bit,conversion time 8.51ms
    ADC_32SAMP = const(13)  # 32 samples at 12-bit, conversion time 17.02ms.
    ADC_64SAMP = const(14)  # 64 samples at 12-bit, conversion time 34.05ms.
    ADC_128SAMP = const(15)  # 128 samples at 12-bit, conversion time 68.10ms.
    __ADDRESS = 0x40

    __REG_CONFIG = 0x00
    __REG_SHUNTVOLTAGE = 0x01
    __REG_BUSVOLTAGE = 0x02
    __REG_POWER = 0x03
    __REG_CURRENT = 0x04
    __REG_CALIBRATION = 0x05

    __RST = 15
    __BRNG = 13
    __PG1 = 12
    __PG0 = 11
    __BADC4 = 10
    __BADC3 = 9
    __BADC2 = 8
    __BADC1 = 7
    __SADC4 = 6
    __SADC3 = 5
    __SADC2 = 4
    __SADC1 = 3
    __MODE3 = 2
    __MODE2 = 1
    __MODE1 = 0

    __OVF = 1
    __CNVR = 2

    __BUS_RANGE = [16, 32]
    __GAIN_VOLTS = [0.04, 0.08, 0.16, 0.32]

    __CONT_SH_BUS = 7

    __AMP_ERR_MSG = ('Expected current %.3fA is greater '
                        'than max possible current %.3fA')
    __RNG_ERR_MSG = ('Expected amps %.2fA, out of range, use a lower '
                        'value shunt resistor')
    __VOLT_ERR_MSG = ('Invalid voltage range, must be one of: '
                        'RANGE_16V, RANGE_32V')

    __SHUNT_MILLIVOLTS_LSB = 0.01  # 10uV
    __BUS_MILLIVOLTS_LSB = 4  # 4mV
    __CALIBRATION_FACTOR = 0.04096
    __MAX_CALIBRATION_VALUE = 0xFFFE
    __CURRENT_LSB_FACTOR = 32800

    def __init__(self, i2cDev):
        self._i2c = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")

        self._i2c = i2cDev
        self._shunt_ohms = 0.1
        self._max_expected_amps = None
        self._min_device_current_lsb = self._calculate_min_current_lsb()
        self._gain = None
        self._auto_gain_enabled = False
        self.configure()

    def configure(self, voltage_range=RANGE_32V, gain=GAIN_AUTO,
                  bus_adc=ADC_12BIT, shunt_adc=ADC_12BIT):
        self.__validate_voltage_range(voltage_range)
        self._voltage_range = voltage_range

        if self._max_expected_amps is not None:
            if gain == self.GAIN_AUTO:
                self._auto_gain_enabled = True
                self._gain = self._determine_gain(self._max_expected_amps)
            else:
                self._gain = gain
        else:
            if gain != self.GAIN_AUTO:
                self._gain = gain
            else:
                self._auto_gain_enabled = True
                self._gain = self.GAIN_1_40MV

        self._calibrate(
            self.__BUS_RANGE[voltage_range], self.__GAIN_VOLTS[self._gain],
            self._max_expected_amps)
        self._configure(voltage_range, self._gain, bus_adc, shunt_adc)

    def voltage(self):
        value = self._voltage_register()
        return float(value) * self.__BUS_MILLIVOLTS_LSB / 1000

    def supply_voltage(self):
        return self.voltage() + (float(self.shunt_voltage()) / 1000)

    def current(self):
        self._handle_current_overflow()
        return self._current_register() * self._current_lsb * 1000

    def power(self):
        self._handle_current_overflow()
        return self._power_register() * self._power_lsb * 1000

    def shunt_voltage(self):
        self._handle_current_overflow()
        return self._shunt_voltage_register() * self.__SHUNT_MILLIVOLTS_LSB

    def sleep(self):
        configuration = self._read_configuration()
        self._configuration_register(configuration & 0xFFF8)

    def wake(self):
        configuration = self._read_configuration()
        self._configuration_register(configuration | 0x0007)
        # 40us delay to recover from powerdown (p14 of spec)
        utime.sleep_us(40)

    def current_overflow(self):
        return self._has_current_overflow()

    def reset(self):
        """Reset the INA219 to its default configuration."""
        self._configuration_register(1 << self.__RST)

    def _handle_current_overflow(self):
        if self._auto_gain_enabled:
            while self._has_current_overflow():
                self._increase_gain()
        else:
            if self._has_current_overflow():
                raise DeviceRangeError(self.__GAIN_VOLTS[self._gain])

    def _determine_gain(self, max_expected_amps):
        shunt_v = max_expected_amps * self._shunt_ohms
        if shunt_v > self.__GAIN_VOLTS[3]:
            raise ValueError(self.__RNG_ERR_MSG % max_expected_amps)
        gain = min(v for v in self.__GAIN_VOLTS if v > shunt_v)
        return self.__GAIN_VOLTS.index(gain)

    def _increase_gain(self):
        gain = self._read_gain()
        if gain < len(self.__GAIN_VOLTS) - 1:
            gain = gain + 1
            self._calibrate(self.__BUS_RANGE[self._voltage_range],
                            self.__GAIN_VOLTS[gain])
            self._configure_gain(gain)
            # 1ms delay required for new configuration to take effect,
            # otherwise invalid current/power readings can occur.
            utime.sleep_ms(1)
        else:
            raise DeviceRangeError(self.__GAIN_VOLTS[gain], True)

    def _configure(self, voltage_range, gain, bus_adc, shunt_adc):
        configuration = (
            voltage_range << self.__BRNG | gain << self.__PG0 |
            bus_adc << self.__BADC1 | shunt_adc << self.__SADC1 |
            self.__CONT_SH_BUS)
        self._configuration_register(configuration)

    def _calibrate(self, bus_volts_max, shunt_volts_max,max_expected_amps=None):
        max_possible_amps = shunt_volts_max / self._shunt_ohms
        self._current_lsb = \
            self._determine_current_lsb(max_expected_amps, max_possible_amps)

        self._power_lsb = self._current_lsb * 20

        max_current = self._current_lsb * 32767

        max_shunt_voltage = max_current * self._shunt_ohms

        calibration = trunc(self.__CALIBRATION_FACTOR /
                            (self._current_lsb * self._shunt_ohms))
        self._calibration_register(calibration)

    def _determine_current_lsb(self, max_expected_amps, max_possible_amps):
        if max_expected_amps is not None:
            if max_expected_amps > round(max_possible_amps, 3):
                raise ValueError(self.__AMP_ERR_MSG %
                                 (max_expected_amps, max_possible_amps))
            if max_expected_amps < max_possible_amps:
                current_lsb = max_expected_amps / self.__CURRENT_LSB_FACTOR
            else:
                current_lsb = max_possible_amps / self.__CURRENT_LSB_FACTOR
        else:
            current_lsb = max_possible_amps / self.__CURRENT_LSB_FACTOR

        if current_lsb < self._min_device_current_lsb:
            current_lsb = self._min_device_current_lsb
        return current_lsb

    def _configuration_register(self, register_value):
        self.__write_register(self.__REG_CONFIG, register_value)

    def _read_configuration(self):
        return self.__read_register(self.__REG_CONFIG)

    def _calculate_min_current_lsb(self):
        return self.__CALIBRATION_FACTOR / \
            (self._shunt_ohms * self.__MAX_CALIBRATION_VALUE)

    def _read_gain(self):
        configuration = self._read_configuration()
        gain = (configuration & 0x1800) >> self.__PG0
        return gain

    def _configure_gain(self, gain):
        configuration = self._read_configuration()
        configuration = configuration & 0xE7FF
        self._configuration_register(configuration | (gain << self.__PG0))
        self._gain = gain

    def _calibration_register(self, register_value):
        self.__write_register(self.__REG_CALIBRATION, register_value)

    def _has_current_overflow(self):
        ovf = self._read_voltage_register() & self.__OVF
        return (ovf == 1)

    def _voltage_register(self):
        register_value = self._read_voltage_register()
        return register_value >> 3

    def _read_voltage_register(self):
        return self.__read_register(self.__REG_BUSVOLTAGE)

    def _current_register(self):
        return self.__read_register(self.__REG_CURRENT, True)

    def _shunt_voltage_register(self):
        return self.__read_register(self.__REG_SHUNTVOLTAGE, True)

    def _power_register(self):
        return self.__read_register(self.__REG_POWER)

    def __validate_voltage_range(self, voltage_range):
        if voltage_range > len(self.__BUS_RANGE) - 1:
            raise ValueError(self.__VOLT_ERR_MSG)

    def __write_register(self, register, register_value):
        register_bytes = self.__to_bytes(register_value)
        # self._i2c.writeto_mem(self._address, register, register_bytes)
        self._i2c.memWrite(register_bytes,register,8)

    def __to_bytes(self, register_value):
        return bytearray([(register_value >> 8) & 0xFF, register_value & 0xFF])

    def __read_register(self, register, negative_value_supported=False):
        # register_bytes = self._i2c.readfrom_mem(self._address, register, 2)
        buf=bytearray(2)
        self._i2c.memRead(buf,register,8)
        register_value = int.from_bytes(buf, 'big')
        if negative_value_supported:
            # Two's compliment
            if register_value > 32767:
                register_value -= 65536
        return register_value


class DeviceRangeError(Exception):
    __DEV_RNG_ERR = ('Current out of range (overflow), '
                     'for gain %.2fV')

    def __init__(self, gain_volts, device_max=False):
        """Construct the class."""
        msg = self.__DEV_RNG_ERR % gain_volts
        if device_max:
            msg = msg + ', device limit reached'
        super(DeviceRangeError, self).__init__(msg)
        self.gain_volts = gain_volts
        self.device_limit_reached = device_max

```
yuanda_htb485.py

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
