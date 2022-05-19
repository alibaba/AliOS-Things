# -*- coding: UTF-8 -*-

from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson                     # json字串解析库
import utime                     # 延时API所在组件
from driver import I2C          # I2C类，用于控制微处理器的输入输出功能
from driver import UART         # UART类，用于控制微处理器的输入输出功能
import bmp280              # bmp280气压温度传感器
import gnss               # gnss 位置传感器
from micropython import const

tempDev = 0
gnssDev = 0

# 物联网平台连接标志位
iot_connected = False
wlan = None

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名字"
deviceSecret = "设备密钥"

# 物联网设备实例
device = None

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 等待Wi-Fi成功连接到路由器


def get_wifi_status():
    global wlan
    wifi_connected = False

    wlan.active(True)  # 激活界面
    wlan.scan()  # 扫描接入点
    wlan.disconnect()  # 断开Wi-Fi
    #print("start to connect ", wifiSsid)
    # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）
    wlan.connect(wifiSsid, wifiPassword)

    while True:
        wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected:                     # Wi-Fi连接成功则退出while循环
            break
        else:
            utime.sleep(0.5)
            print("wifi_connected:", wifi_connected)

    ifconfig = wlan.ifconfig()  # 获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    utime.sleep(0.5)

# 物联网平台连接成功的回调函数


def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）


def on_props(request):
    pass


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

def bmp280_check_init():
    global tempDev

    i2cObj = I2C()
    i2cObj.open("bmp280")   # 按照board.json中名为"bmp280"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
    tempDev = bmp280.BMP280(i2cObj)  # 初始化BMP280传感器

def gnss_check_init():
    global gnssDev

    uartObj = UART()
    uartObj.open("gnss")
    gnssDev = gnss.GNSS(uartObj)
    print("gnss inited!")

def delivery_detecting():
    global tempDev, gnssDev
    temperature = 0
    longitude = 0
    latitude = 0
    altitude = 0
    while True:             # 无限循环
        temperature = tempDev.getTemperature()
        print('temperature = ', temperature)
        gnssDev.getLocation()
        location = gnssDev.getLocation()
        print(location.longitude, location.latitude, location.altitude)
        # 判断定位信息是否发生变化
        if(longitude != location.longitude[0] or latitude != location.latitude[0] or altitude != location.altitude):
            longitude = location.longitude[0]
            latitude = location.latitude[0]
            altitude = location.altitude
            print(longitude, latitude, altitude)
            # 如果有变化，则上报地理位置信息至物联网平台
            loc_data = {
                'params': ujson.dumps({
                    'GeoLocation': {
                        'Longitude': longitude,
                        'Latitude': latitude,
                        'Altitude': altitude,
                        'CoordinateSystem': 1
                    }
                })
            }
            device.postProps(loc_data)
        upload_data = {'params': ujson.dumps({
                'temperature': temperature,
                 })
        }
        # 上传状态到物联网平台
        device.postProps(upload_data)

        utime.sleep(1)      # 打印完之后休眠1秒


if __name__ == '__main__':

    wlan = network.WLAN(network.STA_IF)  # 创建WLAN对象
    get_wifi_status()

    connect_lk(productKey, deviceName, deviceSecret)
    bmp280_check_init()
    gnss_check_init()
    delivery_detecting()

