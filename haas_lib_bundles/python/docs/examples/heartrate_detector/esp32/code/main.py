
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    心率上云
@Author     :    victor.wang
@version    :    1.0
'''

from aliyunIoT import Device      # iot组件是连接阿里云物联网平台的组件
import network                    # Wi-Fi功能所在库
import utime                      # 延时API所在组件
from driver import ADC            # ADC类，通过微处理器的ADC模块读取ADC通道输入电压
from driver import GPIO           # ESP32和使用GPIO控制LED
import heartbeat                  # 心率传感器驱动库
import ujson                      # json字串解析库

# 物联网平台连接标志位
iot_connected = False

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifiSsid = " 填写您的路由器名称"
wifiPassword = " 填写您的路由器密码"

# 最大和最小心率值设定
BPM_high=150                      # 最大运动心率
BPM_low=100                       # 最小运动心率

# 物联网平台连接标志位
iot_connected = False

wlan = None

# 物联网设备实例
device = None

# LED 心率传感器设备
redled=None
blueled=None
greenled=None
adcobj = None
buzzerobj = None

heartbeatdev = None

# 蜂鸣器控制变量
buzzeron =0

def buzzer_init():
    global buzzerobj
    buzzerobj = GPIO()
    buzzerobj.open('buzzer')        # board.json中buzzer节点定义的GPIO，对应esp32外接的上的蜂鸣器

    print("buzzer inited!")


def heartbeat_init():
    global adcobj, heartbeatdev,redled, blueled, greenled, BPM_high, BPM_low
    redled = GPIO()
    blueled = GPIO()
    greenled = GPIO()

    redled.open('led_r')          # board.json中led_r节点定义的GPIO，对应esp32外接的的红灯
    blueled.open('led_b')         # board.json中led_b节点定义的GPIO，对应esp32外接的上的蓝灯
    greenled.open('led_g')        # board.json中led_g节点定义的GPIO，对应esp32外接的上的绿灯

    adcobj = ADC()
    adcobj.open("heartbeat")     # 按照board.json中名为"heartbeat"的设备节点的配置参数（主设备adc端口号，从设备地址，采样率等）初始化adc类型设备对象

    heartbeatdev = heartbeat.HEARTBEAT(adcdev=adcobj, rled=redled, bled=blueled, gled=greenled, highBPM=BPM_high, lowBPM=BPM_low)   # 初始化心率传感器
    print("heartbeat inited!")

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
    if not wifi_connected:
        wlan.active(True)                  #激活界面
        wlan.scan()                        #扫描接入点
        wlan.disconnect()                  #断开Wi-Fi
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

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global buzzeron
    payload = ujson.loads(request['params'])

    # 获取dict状态字段
    if "Buzzer" in payload.keys():
        buzzeron = payload["Buzzer"]
        if(buzzeron):
            print("打开蜂鸣器")
    buzzerobj.write(buzzeron)      # 控制蜂蜜器

    # 要将更改后的状态同步上报到云平台
    prop = ujson.dumps({
        'Buzzer': buzzeron,
    })
    upload_data = {'params': prop}
    # 上报蜂鸣器属性到云端
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

def getBPM():
    global heartbeatdev
    value = heartbeatdev.getBPM()
    return value

def ajust(y):
    x=0
    if (y<70):
        x=65
    elif (y<80):
        x=75
    elif (y<90):
        x=85
    elif (y<100):
        x=95
    elif (y<110):
        x=105
    elif (y<120):
        x=115
    elif (y<130):
        x=125
    elif (y<140):
        x=135
    elif (y<150):
        x=145
    elif (y<160):
        x=155
    elif (y<170):
        x=165
    elif (y<180):
        x=175
    else:
        x=200
    return x

# 上传心率到物联网平台
def upload_heartbeat():
    global device
    while True:
        # 生成上报到物联网平台的属性值
        value = getBPM()
        print ("心率：", value)
        value = ajust(value)
        prop = ujson.dumps({
            'HeartRate': value
            })
        #print('uploading data: ', "\'HeartRate\':", value)
        # 上传心率值到物联网平台
        upload_data = {'params': prop}
        device.postProps(upload_data)
        utime.sleep(2)

if __name__ == '__main__':

    buzzer_init()

    # 请替换物联网平台申请到的产品和设备信息,可以参考README
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)

    print('sleep for 2s before start heartbeatsensor')
    utime.sleep(2)                  # 等待2s
    heartbeat_init()                # 初始化心率心率传感器， 采样周期默认2ms
    heartbeatdev.start()            # 启动心率传感器,  每隔2ms采集一次心跳

    upload_heartbeat()
