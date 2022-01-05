# -*- encoding: utf-8 -*-

from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import utime                     # 延时API所在组件
import ujson                     # json字串解析库
from driver import ADC           # ADC类，通过微处理器的ADC模块读取ADC通道输入电压
from driver import GPIO
import photoresistor             # 热敏电阻photoresistor传感器类

adcDev = 0
ledDev = 0
photoresistorDev = 0
lightness = 0

# wifi连接的的ssid和pwd定义
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 物联网平台相关的key和serect定义
productKey     = "产品key"
deviceName     = "设备名称"
deviceSecret   = "设备密钥"

# 物联网平台连接标志位
iot_connected = False
wlan = None

# 物联网设备实例
device = None

# EDU K1上灯的初始化
def ledInit():
    global ledDev
    ledDev = GPIO()
    ledDev.open("led")

# 点亮灯
def ledOn():
    global ledDev
    ledDev.write(1)

# 熄灭灯
def ledOff():
    global ledDev
    ledDev.write(0)

# photoresistor 光敏电阻传感器初始化
def photoresistor_init():
    global photoresistorDev

    adcDev = ADC()
    adcDev.open("photoresistor")

    photoresistorDev = photoresistor.PhotoResistor(adcDev)
    print("photoresistor init finished")

# photoresistor 光照传感器亮度获取
def photoresistor_light():
    global photoresistorDev,lightness
    lightness = photoresistorDev.getLightness()
    print("The lightness is: ",lightness)

# 显示光照强度以及上报
def show_lightness():
    global lightness,i_light,last_light,system_reset

    i_light = int(lightness)
    if system_reset == 1:
        print("system first come")
        system_reset = 0    # 系统第一次启动
        report_light_data(i_light)
    elif (i_light >= 1000 and last_light < 1000) or (i_light < 1000 and last_light >= 1000):
        report_light_data(i_light)
        print("light has change")
    else:
        print('no need report')

    if i_light < 10:
        T_str = "亮度:" + str(round(i_light, 1))
    elif i_light >= 10 and i_light < 100:
        T_str = "亮度:" + str(round(i_light, 2))
    elif i_light >= 100 and i_light < 1000:
        T_str = "亮度:" + str(round(i_light, 3))
    elif i_light >= 1000 and i_light < 10000:
        T_str = "亮度:" + str(round(i_light, 4))
    print(T_str)
    last_light = i_light

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

    payload = ujson.loads(request['params'])

    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "onoff" in payload.keys():
        alarm_on = payload["onoff"]
        if (alarm_on):
            print("开灯")
            ledOn()
        else:
            print("关灯")
            ledOff()

    # 要将更改后的状态同步上报到云平台
    upload_data = {'params': ujson.dumps({
        'onoff': alarm_on,
    })
    }
    # 上报本地灯的状态到云端
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

def report_light_data(l_data):
    # 生成上报到物联网平台的属性值字串
    upload_data = {'params': ujson.dumps({
        'Brightness': l_data,
    })
    }
    # 上传光强度信息到物联网平台
    device.postProps(upload_data)

def main():
    global i_light,last_light,system_reset,wlan
    i_light = 0
    last_light = 0
    system_reset = 1

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    photoresistor_init()
    ledInit()
    while True:
        photoresistor_light()
        show_lightness()
        utime.sleep(1.5)

if __name__ == '__main__':
    main()