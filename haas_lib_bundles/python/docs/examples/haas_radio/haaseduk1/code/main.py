import utime
import sh1106
from driver import SPI
from driver import GPIO
from tea5767 import Radio
import framebuf

from driver import I2C
from haaseduk1 import HAASEDUK1
from aliyunIoT import Device  # iot组件是连接阿里云物联网平台的组件
import ujson                  # json字串解析库

board = HAASEDUK1()                  # 新建HAASEDUK1对象
hwID = board.getHWID()               # 获取开发板ID

import netmgr as nm           # netmgr是Wi-Fi网络连接的组件
import utime

muteFlag = 0
searchFlag = 0
oled = None
K1 = None
K2 = None
K3 = None
K4 = None
radio = None
keyEvents = {'K1':False, 'K2':False, 'K3':False, 'K4':False}

# 按键K1~K4的初始化
def keyInit():
    global K1, K2, K3, K4
    K1 = GPIO()
    K1.open("KEY_1")
    K1.on(k1Handler)

    K2 = GPIO()
    K2.open("KEY_2")
    K2.on(k2Handler)

    K3 = GPIO()
    K3.open("KEY_3")
    K3.on(k3Handler)

    K4 = GPIO()
    K4.open("KEY_4")
    K4.on(k4Handler)

def k1Handler(obj):
    global radio
    print('K1 pressed')
    keyEvents['K1'] = True
    radio.set_frequency(87.5)

def k2Handler(obj):
    global radio
    print('K2 pressed')
    keyEvents['K2'] = True
    radio.change_freqency(0.1)

def k3Handler(obj):
    global radio
    print('K3 pressed')
    keyEvents['K3'] = True
    radio.search(True, dir=1, adc=5)

def k4Handler(obj):
    global radio
    print('K4 pressed')
    keyEvents['K4'] = True
    radio.change_freqency(-0.1)

# 检查是否有按键按下事件处于pending状态
def keyEventsPending():
    if all(value == False for value in keyEvents.values()):
        return False
    return True

# 清除按键事件
def clearKeyEvents():
    keyEvents['K1'] = False
    keyEvents['K2'] = False
    keyEvents['K3'] = False
    keyEvents['K4'] = False
    pass

#初始化自带oled屏幕模块
def oled_init():
    spi1 = SPI()
    spi1.open("oled_spi")

    gpio_dc = GPIO()
    gpio_dc.open("oled_dc")

    gpio_res = GPIO()
    gpio_res.open("oled_res")

    global oled
    oled = sh1106.SH1106_SPI(width=132, height=64, spi=spi1, dc = gpio_dc, res = gpio_res)

    oled.fill(0)   #清屏背景黑色
    oled.text('HaaS Radio', 30, 5)
    oled.text('k1:reset k2: +0.1', 2, 22)
    oled.text('K3:scan  k4: -0.1', 2, 42)
    oled.show()

# # 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    nm.init()
    nm.disconnect()
    wifi_connected = nm.getStatus()
#   print("start to connect " + wifiSsid)
    nm.connect('ALIYUN_322219_test', '@AliYun@Iot@123')       # 连接到指定的路由器（路由器名称为wifi_ssid, 密码为：wifi_password）

    while True :
        if wifi_connected == 5:               # nm.getStatus()返回5代表连线成功
            break
        else:
            wifi_connected = nm.getStatus() # 获取Wi-Fi连接路由器的状态信息
            utime.sleep(0.5)
            # print("wifi_connected: " + wifi_connected)
    print("Wi-Fi connected")
    print('DeviceIP:' + nm.getInfo()['ip'])  # 打印Wi-Fi的IP地址信息

# 物联网平台连接标志位
iot_connected = False

# 物联网设备实例
device = None

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

wlan = None

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global device
    global radio, muteFlag, searchFlag
    print(request)
    payload = ujson.loads(request['params'])

    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "SearchFrequency" in payload.keys():
        print(payload)
        search = payload["SearchFrequency"]
        if (search):
            searchFlag = 1
            radio.search(True, dir=1, adc=5)
        else:
            searchFlag = 0

    if "Mute" in payload.keys():
        print(payload)
        mute = payload["Mute"]
        if (mute):
            radio.mute(1)
            muteFlag = 1
        else:
            radio.mute(0)
            muteFlag = 0

    if "Frequency" in payload.keys():
        print(payload)
        frequency = payload["SetFrequency"]
        radio.set_frequency(frequency)

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

def connect_lk(productKey, deviceName, deviceSecret):
    global device, iot_connected
    key_info = {
        'region': 'cn-shanghai',
        'productKey': productKey,
        'deviceName': deviceName,
        'deviceSecret': deviceSecret,
        'keepaliveSec': 60
    }
    # 将三元组信息设置到iot组件中
    device = Device()

    # 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
    device.on(Device.ON_CONNECT, on_connect)

    # 配置收到云端属性控制指令的回调函数，如果收到物联网平台发送的属性控制消息，则调用on_props函数
    device.on(Device.ON_PROPS, on_props)

    # 启动连接阿里云物联网平台过程
    device.connect(key_info)

    # 等待设备成功连接到物联网平台
    while(True):
        if iot_connected:
            print('物联网平台连接成功')
            break
        else:
            print('sleep for 1 s')
            utime.sleep(1)
    print('sleep for 2s')
    utime.sleep(2)

# 上传当前信息到物联网平台
def upload_state():
    global device
    global radio, muteFlag, searchFlag

    # 上传信息到物联网平台
    upload_data = {'params': ujson.dumps({
        'Frequency': radio.frequency,
        'ADCLevel': radio.signal_adc_level,
        'Mute': muteFlag,
        'SearchFrequency': searchFlag
        })
    }
    device.postProps(upload_data)

if __name__ == '__main__':
    #硬件初始化
    oled_init()
    keyInit()
    i2cDev = I2C()
    i2cDev.open("tea5767")
    radio = Radio(i2cDev)
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)

    while True:
        # print('Frequency: FM {}\nReady: {}\nStereo: {}\nADC level: {}'.format(
        # radio.frequency, radio.is_ready,  radio.is_stereo, radio.signal_adc_level
        # ))
        oled.fill(0)
        oled.text('FM: {}'.format(radio.frequency), 20, 5)
        oled.text('Ready: {}'.format(radio.is_ready), 20, 20)
        oled.text('Stereo: {}'.format(radio.frequency), 20, 35)
        oled.text('ADC: {}'.format(radio.signal_adc_level), 20, 50)
        oled.show()
        upload_state()
        utime.sleep(1)


