# -*- encoding: utf-8 -*-

from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import utime                     # 延时API所在组件
import ujson                     # json字串解析库
from driver import GPIO          # LED需使用GPIO进行控制
from driver import ADC           # ADC类，通过微处理器的ADC模块读取ADC通道输入电压
import fire                      # fire火焰传感器类

adcDev = 0                     # ADC通道对象
ledDev = 0               # 报警LED对象
alarm_on = 0                # 记录报警状态

# 物联网平台连接标志位
iot_connected = False
wlan = None

# 物联网设备实例
device = None

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

def alarm_control(on):
    global ledDev

    if on == 0:
        ledDev.write(0)  # GPIO写入0，执行灭灯操作
    else:
        ledDev.write(1)  # GPIO写入 1，执行亮灯报警动作

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

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global alarm_on, device

    # {"alarmState":1} or {"alarmState":0}
    payload = ujson.loads(request['params'])

    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "alarmState" in payload.keys():
        print(payload)
        alarm_on = payload["alarmState"]
        if (alarm_on):
            print("开始报警")
        else:
            print("结束报警")

    # 根据云端设置的报警灯状态改变本地LED状态
    alarm_control(alarm_on)

    # 要将更改后的状态同步上报到云平台
    prop = ujson.dumps({
        'alarmState': alarm_on,
    })
    upload_data = {'params': prop}
    # 上报本地报警灯状态到云端
    device.postProps(upload_data)


# 连接物联网平台
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

# 上传火焰传感器检测电压信息和报警信息到物联网平台
def upload_fire_detector_state():
    global device, alarm_on

    fireVoltage = 0

    # 无限循环
    while True:
        fireVoltage = fireDev.getVoltage()   # 获取电压值
        print("The fire status Voltage ",fireVoltage)

        # 生成上报到物联网平台的属性值字串，此处的属性标识符"fireVoltage"和"alarmState"必须和物联网平台的属性一致
        # "fireVoltage" - 代表燃气传感器测量到的电压值
        # "alarmState" - 代表报警灯的当前状态
        upload_data = {'params': ujson.dumps({
            'fireVoltage': fireVoltage,
            'alarmState': alarm_on
        })
        }
        # 上传火焰传感器检测电压信息和报警信息到物联网平台
        device.postProps(upload_data)

        # 每2秒钟上报一次
        utime.sleep(2)

if __name__ == '__main__':
    global fireDev

    alarm_on = 0
    # 硬件初始化
    # 初始化 ADC
    adcDev = ADC()
    adcDev.open("fire")
    fireDev = fire.Fire(adcDev)

    # 初始化LED所连接GPIO
    ledDev = GPIO()
    ledDev.open("led")
    alarm_control(alarm_on)         # 关闭报警灯

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
    get_wifi_status()

    connect_lk(productKey, deviceName, deviceSecret)
    upload_fire_detector_state()

    adcDev.close()
    ledDev.close()
