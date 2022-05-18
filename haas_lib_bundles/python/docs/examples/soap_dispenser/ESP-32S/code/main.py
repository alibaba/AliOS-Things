# -*- coding: UTF-8 -*-

from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson                     # json字串解析库
import utime                     # 延时API所在组件
from driver import ADC           # ADC类，用ADC外设的接入
from driver import PWM           # PWM类，用舵机控制
from driver import GPIO          # GPIO类，用于控制微处理器的输入输出功能
import water4level               # 液位传感器类
import servo                     # 舵机控制类
import ir                        # ir人体红外传感器类


irDev = None
adcDev = None
pwmDev = None

# 物联网平台连接标志位
iot_connected = False
wlan = None

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# 物联网设备实例
device = None

# Wi-Fi SSID和Password设置
wifiSsid = "请输入您的路由器名称"
wifiPassword = "请输入您的路由器密码"

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False

    wlan.active(True)                  #激活界面
    wlan.scan()                        #扫描接入点

    # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）
    wlan.connect(wifiSsid, wifiPassword)

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


# dev init
def ir_init():
    global irDev
    irDev = GPIO()
    irDev.open("ir")

def water4level_init():
    global adcDev
    adcDev = ADC()
    adcDev.open("waterlevel")

def servo_init():
    global pwmDev
    pwmDev = PWM()
    pwmDev.open("servo")

# dev deinit
def ir_deinit():
    global irDev
    irDev.close()

def water4level_deinit():
    global adcDev
    adcDev.close()

def servo_deinit():
    global pwmDev
    pwmDev.close()

# dev control
# 人手检测，0-检测到手，1-未检测到手；检测距离调整为10cm，调整方法见README
def get_ir_status():
    irObj = ir.IR(irDev)
    status = irObj.irDetect()
    return status

# 皂液余量测量
def get_waterlevel():
    waterlevelObj = water4level.WATER4LEVEL(adcDev)
    level = waterlevelObj.measureLevel()
    return level

# 舵机控制
def servo_control(data):
    servoObj = servo.SERVO(pwmDev)
    servoObj.setOptionSero(data)


if __name__ == '__main__' :

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
    # global productKey, deviceName, deviceSecret ,on_request, on_play
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)

    ir_init()
    servo_init()
    water4level_init()

    while True:
        servo_control(0) # 舵机回归0位，皂液释放器恢复原状

        if (not get_ir_status()): # 获取ir传感器的值,0-检测到手，1-未检测到手
            utime.sleep(1)
            if (not get_ir_status()): # 1s之后仍检测到人手，说明人手到来
                print("hand is here .....\r\n")
                servo_control(90) # 舵机旋转90度，驱动皂液释放器释放皂液
            else:
                continue

        utime.sleep(1.5)

        # "soap_status" - 皂液容量状态
        soap = get_waterlevel()
        if (soap >= 4):
            soap_status="plenty"
        elif ((soap >=1) and (soap < 4)):
            soap_status="insufficient"
        elif (soap < 1):
            soap_status="exhausted"

        # 生成上报到物联网平台的属性值字串
        # 此处的属性标识符"soap_status"必须和物联网平台的属性一致
        upload_data = {'params': ujson.dumps({'soap_status': soap_status})}
        # 上传数据到物联网平台
        device.postProps(upload_data)

    ir_deinit()
    servo_deinit()
    water4level_deinit()
