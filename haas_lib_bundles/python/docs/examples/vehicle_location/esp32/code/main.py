# -*- coding: UTF-8 -*-

from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import utime                     # 延时API所在组件
from driver import UART         # UART类，实现串口通信
import gnss                      # 定位模块gnss传感器类
import ujson

uartDev = 0
gnssDev = 0

# wifi连接的的ssid和pwd定义
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 物联网平台相关的key和serect定义
productKey     = "产品key"
deviceName     = "设备名称"
deviceSecret   = "设备密钥"

iot_connected = False
wlan = None

# 物联网设备实例
device = None

def gnssInit():
    global gnssDev,uartDev
    # 初始化定位模组串口
    uartDev = UART()
    uartDev.open('gnss')
    # 创建定位信息解析器
    gnssDev = gnss.Gnss(uartDev)

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False

    wlan.active(True)                  #激活界面
    wlan.scan()                        #扫描接入点
    #print("start to connect ", wifiSsid)
    wlan.connect(wifiSsid, wifiPassword)       # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）

    while True:
        wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected:                     # Wi-Fi连接成功则退出while循环
            break
        else:
            utime.sleep(0.5)
            print("wifi_connected:", wifi_connected)

    ifconfig = wlan.ifconfig()                    #获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    utime.sleep(0.5)

# 物联网平台连接成功时触发 on_connect
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

# 主程序入口
if __name__ == '__main__':
    gnssInit()

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)

    # 定义经纬度及海拔
    longitude = 0
    latitude = 0
    altitude = 0
    # 连续从串口种读取信息
    while True:
        # 串口读取定位模块语句
        location = gnssDev.getLocation()
        if(location):
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
        utime.sleep(2)
    device.close()
    uartDev.close()
