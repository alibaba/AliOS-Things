# -*- coding: UTF-8 -*-

from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import netmgr as nm              # Wi-Fi 功能所在库
import ujson                     # json字串解析库
import utime                     # 延时API所在组件
from driver import GPIO          # GPIO类，用于控制微处理器的输入输出功能
import ir                        # ir人体红外传感器类
gpioirDev = 0
gpioledDev_1 = 0
gpioledDev_2 = 0
gpioledDev_3 = 0
irDev = 0

# 物联网平台连接标志位
iot_connected = False
wlan = None

productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# 物联网设备实例
device = None

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"


# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wifiSsid, wifiPassword
    nm.init()
    print("start to connect ", wifiSsid)
    nm.connect(wifiSsid, wifiPassword)
    while True:
        wifi_connected = nm.getStatus()    # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected == 5:            # Wi-Fi连接成功则退出while循环
            info = nm.getInfo()
            print("\n")
            print("wifi 连接成功:")
            print("    SSID:", info["ssid"])
            print("    IP:", info["ip"])
            print("    MAC:", info["mac"])
            print("    RSSI:", info["rssi"])
            break
        else:
            print("wifi 连接失败")
            utime.sleep(0.5)
    print('sleep for 1s')
    utime.sleep(1)

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

def human_check_init():
    global gpioirDev,gpioledDev_1,gpioledDev_2,gpioledDev_3,irDev

    gpioirDev = GPIO()
    gpioirDev.open("ir")
    irDev = ir.IR(gpioirDev)

    gpioledDev_1 = GPIO()
    gpioledDev_1.open("led01")

    gpioledDev_2 = GPIO()
    gpioledDev_2.open("led02")

    gpioledDev_3 = GPIO()
    gpioledDev_3.open("led03")

def led_control(on):
    global gpioledDev_1
    if on == 0:
        gpioledDev_1.write(0)        # GPIO写入0，灭灯
        gpioledDev_2.write(0)        # GPIO写入0，灭灯
        gpioledDev_3.write(0)        # GPIO写入0，灭灯
    else:
        gpioledDev_1.write(1)        # GPIO写入1，亮灯
        gpioledDev_2.write(1)        # GPIO写入1，亮灯
        gpioledDev_3.write(1)        # GPIO写入1，亮灯

def human_detector() :
    global gpioirDev,gpioledDev_1,gpioledDev_2,gpioledDev_3,irDev

    last_have_human = 0                 # 记录初始值
    while True :
        have_human = irDev.irDetect()   # 获取传感器的值
        if (have_human == 1) :
            print("human is here .....\r\n")
        if (have_human == 0) :
            print("human is not here .....\r\n")
        if (last_have_human != have_human) :
            led_control(have_human)         # 控制LED亮灭
            # 生成上报到物联网平台的属性值字串，此处的属性标识符"LEDSwith"必须和物联网平台的属性一致
            # "LEDSwitch" - 表示起夜灯的状态
            upload_data = {'params': ujson.dumps({
                'LEDSwitch': have_human,
            })
            }
            # 上传LED状态到物联网平台
            device.postProps(upload_data)

        last_have_human = have_human    # 记录当前状态
        utime.sleep(1)                   # 休眠1秒

    gpioirDev.close()
    gpioledDev_1.close()
    gpioledDev_2.close()
    gpioledDev_3.close()

if __name__ == '__main__' :

    # 请替换物联网平台申请到的产品和设备信息,可以参考README
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    human_check_init()
    human_detector()
