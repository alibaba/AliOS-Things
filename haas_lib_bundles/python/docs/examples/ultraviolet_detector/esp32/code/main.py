# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    紫外线数据上云
@Author     :    victor.wang
@version    :    1.0
'''
from aliyunIoT import Device      # iot组件是连接阿里云物联网平台的组件
import network                    # Wi-Fi功能所在库
import utime                      # 延时API所在组件
from driver import GPIO           # ESP32和使用GPIO控制LED
from driver import ADC
import uv                      # SHT3X-DIS温湿度传感器驱动库
import ujson                      # json字串解析库

# 紫外线等级
redledon = 0
greenledon = 0
blueledon = 0
buzzeron = 0

# 物联网平台连接标志位
iot_connected = False

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 物联网平台连接标志位
iot_connected = False

wlan = None

# 物联网设备实例
device = None

# led灯，蜂鸣器和紫外线传感器对象
adcObj = None
uvDev = None
redleddev = None
greenleddev = None
blueleddev = None
buzzerdev = None

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
    if not wifi_connected:
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

# 通过紫外线传感器读取紫外线强度
def get_uvlevel():
    global uvDev

    uvlevl = uvDev.measureUVLevel()         # 获取紫外线等级值

    return uvlevl                           # 返回读取到的紫外线等级值

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global redledon, greenledon, blueledon, buzzeron, redleddev, blueleddev, greenleddev, buzzerdev

    payload = ujson.loads(request['params'])

    if "redledSwith" in payload.keys():
        redledon = payload["redledSwith"]
        if (redledon):
            print("点亮红色LED")

    if "blueledSwitch" in payload.keys():
        blueledon = payload["blueledSwitch"]
        if (blueledon):
            print("点亮蓝色LED")

    if "greenledSwitch" in payload.keys():
        greenledon = payload["greenledSwitch"]
        if (greenledon):
            print("点亮绿色LED")

    if "buzzer" in payload.keys():
        buzzeron = payload["buzzer"]
        if (buzzeron):
            print("打开蜂鸣器")

    redleddev.write(redledon)                  # 控制红灯开关
    blueleddev.write(blueledon)                # 控制蓝灯开关
    greenleddev.write(greenledon)              # 控制绿灯开关
    buzzerdev.write(buzzeron)                  # 控制蜂鸣器

    # 要将更改后的状态同步上报到云平台
    prop = ujson.dumps({
        'redledSwith': redledon,
        'blueledSwitch': blueledon,
        'greenledSwitch': greenledon,
        'buzzer': buzzeron,
    })

    upload_data = {'params': prop}
    # 上报LED和蜂鸣器属性到云端
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

# 上传紫外线等级信息到物联网平台
def upload_uvlevel():
    global device

    while True:
        data = get_uvlevel()                      # 读取紫外线等级
        # 生成上报到物联网平台的属性值字串
        prop = ujson.dumps({
            'uvlevel': data
            })
        print('uploading data: ', prop)

        upload_data = {'params': prop}
        # 上传紫外线等级值到物联网平台
        device.postProps(upload_data)
        utime.sleep(2)

if __name__ == '__main__':
    # 硬件初始化
    adcObj = ADC()

    adcObj.open("uv")                 # 按照board.json中名为"uv"的设备节点的配置参初始化adc类型设备对象
    print("uv inited!")

    uvDev = uv.UV(adcObj)             # 初始化紫外线传感器
    # 初始化 GPIO
    redleddev = GPIO()
    greenleddev = GPIO()
    blueleddev = GPIO()
    buzzerdev = GPIO()

    redleddev.open('led_r')             # 使用board.json中led_r节点定义的GPIO，对应esp32外接的的红灯
    blueleddev.open('led_b')            # 使用board.json中led_b节点定义的GPIO，对应esp32外接的上蓝灯
    greenleddev.open('led_g')           # 使用board.json中led_g节点定义的GPIO，对应esp32外接的上绿灯
    print("LED inited!")
    buzzerdev.open('buzzer')            # 使用board.json中buzzer节点定义的GPIO，对应esp32外接的上蜂鸣器
    print("buzzer inited!")

    # 请替换物联网平台申请到的产品和设备信息,可以参考README.md
    get_wifi_status()

    connect_lk(productKey, deviceName, deviceSecret)
    upload_uvlevel()
