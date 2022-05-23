#########
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson
##################

import utime   # 延时函数在utime库中
from math import pi
from driver import I2C
import pca9685

robotDev = None

#########
# 物联网平台连接标志位
iot_connected = False
wlan = None

# 三元组信息
productKey = "产品密钥" #需要填入物联网云平台申请到的productKey信息
deviceName = "设备名称" #需要填入物联网云平台申请到的deviceName信息
deviceSecret = "设备密钥" #需要填入物联网云平台申请到的deviceSecret信息

# 物联网设备实例
device = None

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False

    wlan.active(True)                  #激活界面
    wlan.scan()                        #扫描接入点
    #print("start to connect ", wifiSsid)
    wlan.connect(wifiSsid, wifiPassword)       # 连接到指定路由器名称和密码

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

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    payload = ujson.loads(request['params'])
    # print (payload)
    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "operate_machine" in payload.keys():
        value = payload["operate_machine"]
        if value == 1:
            left_ctrl()
            print("左移")
        elif value == 2:
            right_ctrl()
            print("右移")
        elif value == 3:
            front_ctrl()
            print("前移")
        elif value == 4:
            back_ctrl()
            print("后移")
        elif value == 5:
            catch_ctrl()
            print("夹住")
        elif value == 6:
            uncatch_ctrl()
            print("放开")
        else:
            print("No operate")

def report_event():
    upload_data = {'params': ujson.dumps({'onoff': 0,})}
    # 上传开关状态到物联网平台
    device.postProps(upload_data)

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

    # 如果收到物联网平台发送的属性控制消息
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
#########

def pca9685_init():
    global robotDev

    i2cObj = I2C()
    i2cObj.open("pca9685")
    robotDev=pca9685.PCA9685(i2cObj)
    print('pca9865 inited')

def left_ctrl():
    robotDev.setServo(0,-pi/2)

def right_ctrl():
    robotDev.setServo(0,pi/2)

def front_ctrl():
    robotDev.setServo(1,-pi/2)

def back_ctrl():
    robotDev.setServo(1,pi/2)

def catch_ctrl():
    robotDev.setServo(3,-pi/2)

def uncatch_ctrl():
    robotDev.setServo(3,0)

if __name__ == '__main__':
    pca9685_init()
    ##########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    ##########
    while True:
        utime.sleep(1)
