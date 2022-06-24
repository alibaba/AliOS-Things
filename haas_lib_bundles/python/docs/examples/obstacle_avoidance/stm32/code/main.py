# -*- coding: UTF-8 -*-

from ulinksdk.aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson                     # json字串解析库
import utime                     # 延时API所在组件
from driver import GPIO          # GPIO类，用于控制微处理器的输入输出功能
import hcsr04                    # hcsr04超声波传感器类
from micropython import const
import buzzer
#import kv

#kv.set('app_upgrade_channel', 'disable')

disDev = 0
echoDev = 0
trigDev = 0
buzzerdev = None

# 安全距离
ALARM_DISTANCE = const(0)

# 物联网平台连接标志位
iot_connected = False
wlan = None

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名字"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 物联网设备实例
device = None

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

def buzzer_init():
    global buzzerdev
    buzzerdev = GPIO()
    # 使用board.json中buzzer节点定义的GPIO，对应esp32外接的上蜂鸣器
    buzzerdev.open('buzzer')
    print("buzzer inited!")

def obstacle_avoidance_init():  # obstacle init function
    global disDev, echoDev, trigDev

    echoDev = GPIO()
    echoDev.open("echo")

    trigDev = GPIO()
    trigDev.open("trig")

    disDev = hcsr04.HCSR04(trigDev, echoDev)

def obstacle_detector():
    global disDev, echoDev, trigDev, buzzerdev

    while True:             # 无限循环
        distance = disDev.measureDistance()
        print('distance = ', distance)

        if(distance < ALARM_DISTANCE):
            buzzerdev.write(1)
            upload_data = {'params': ujson.dumps({
                'LEDSwitch': distance,
            })
            }
            # 上传状态到物联网平台
            device.postProps(upload_data)
        else:
            buzzerdev.write(0)

        utime.sleep(1)      # 打印完之后休眠1秒

    echoDev.close()
    trigDev.close()

if __name__ == '__main__':
    nic = network.LAN()
    nic.active(1)
    connect_lk(productKey, deviceName, deviceSecret)
    buzzer_init()
    print("buzzer init success!!")
    obstacle_avoidance_init()
    obstacle_detector()
