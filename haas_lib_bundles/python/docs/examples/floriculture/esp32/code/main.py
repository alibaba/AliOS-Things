# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    温湿度上云
@Author     :    ethan.lcz
@version    :    1.0
'''
from aliyunIoT import Device      # iot组件是连接阿里云物联网平台的组件
import network                    # Wi-Fi功能所在库
import utime                      # 延时API所在组件
from driver import I2C            # I2C总线驱动库
from driver import GPIO           # ESP32和使用GPIO控制LED
import sht3x                      # SHT3X-DIS温湿度传感器驱动库
import ujson                      # json字串解析库

# 空调和加湿器状态变量
airconditioner = 0
humidifier = 0
airconditioner_value = 0
humidifier_value = 0

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

i2cObj = None
humitureDev = None

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

# 通过温湿度传感器读取温湿度信息
def get_temp_humi():
    global humitureDev
    '''
    # 如果需要同时获取温湿度信息，可以呼叫getTempHumidity，实例代码如下:
    humniture = humitureDev.getTempHumidity()          # 获取温湿度传感器测量到的温湿度值
    temperature = humniture[0]                          # get_temp_humidity返回的字典中的第一个值为温度值
    humidity = humniture[1]                             # get_temp_humidity返回的字典中的第二个值为相对湿度值
    '''
    temperature = humitureDev.getTemperature()         # 获取温度测量结果
    # print("The temperature is: %.1f" % temperature)

    humidity = humitureDev.getHumidity()               # 获取相对湿度测量结果
    # print("The humidity is: %d" % humidity)

    return temperature, humidity                        # 返回读取到的温度值和相对湿度值

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global airconditioner, humidifier, airconditioner_value, humidifier_value

    # {"airconditioner":1} or {"humidifier":1} or {"airconditioner":1, "humidifier":1}
    payload = ujson.loads(request['params'])
    # print (payload)
    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "airconditioner" in payload.keys():
        airconditioner_value = payload["airconditioner"]
        if (airconditioner_value):
            print("打开空调")
        else:
            print("关闭空调")

    if "humidifier" in payload.keys():
        humidifier_value = payload["humidifier"]
        if (humidifier_value):
            print("打开加湿器")
        else:
            print("关闭加湿器")

    # print(airconditioner_value, humidifier_value)

    airconditioner.write(airconditioner_value) # 控制空调开关
    humidifier.write(humidifier_value)         # 控制加湿器开关

    # 要将更改后的状态同步上报到云平台
    prop = ujson.dumps({
        'airconditioner': airconditioner_value,
        'humidifier': humidifier_value,
    })

    upload_data = {'params': prop}
    # 上报空调和加湿器属性到云端
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

# 上传温度信息和湿度信息到物联网平台
def upload_temperature_and_Humidity():
    global device

    while True:
        data = get_temp_humi()                      # 读取温度信息和湿度信息
        # 生成上报到物联网平台的属性值字串
        prop = ujson.dumps({
            'CurrentTemperature': data[0],
            'CurrentHumidity': data[1]
            })
        print('uploading data: ', prop)

        upload_data = {'params': prop}
        # 上传温度和湿度信息到物联网平台
        device.postProps(upload_data)
        utime.sleep(2)

if __name__ == '__main__':
    # 硬件初始化
    i2cObj = I2C()

    i2cObj.open("sht3x")                 # 按照board.json中名为"sht3x"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
    print("sht3x inited!")

    humitureDev = sht3x.SHT3X(i2cObj)   # 初始化SHT3X-DIS传感器
    # 初始化 GPIO
    airconditioner = GPIO()
    humidifier = GPIO()

    humidifier.open('led_g')     # 加湿器使用board.json中led_g节点定义的GPIO，对应esp32外接的的绿灯
    airconditioner.open('led_b') # 空调使用board.json中led_b节点定义的GPIO，对应esp32外接的上的蓝灯

    # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
    get_wifi_status()

    connect_lk(productKey, deviceName, deviceSecret)
    upload_temperature_and_Humidity()

    i2cObj.close()
