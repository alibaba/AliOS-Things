# -*- coding: UTF-8 -*-

from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson                     # json字串解析库
import utime                     # 延时API所在组件
from driver import GPIO         # GPIO类，用于控制微处理器的输入输出功能
from ir import IR
import modbus
from micropython import const

irDev = 0

# 继电器设备ID
MODBUS_SLAVE_ID = const(1)

# modbus 访问超时，单位是毫秒
MODBUS_TIMEOUT_MS = const(200)

# 红外传感器检测周期，单位是秒
CHECK_PERIOD_SEC = const(3) 

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
wifiSsid = "WiFi名称"
wifiPassword = "WiFi密码"

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False

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

def ir_init():
    global irDev

    gpioirDev = GPIO()
    gpioirDev.open("ir")
    irDev = IR(gpioirDev)

def modbus_init():
    modbus.init('modbus_9600')

    #设置/获取 设备地址仅仅针对特定的modbus终端，不代表所有设备都能访问。
    # 设置设备地址为 1
    data=bytearray([MODBUS_SLAVE_ID, 0x00])
    ret = modbus.writeMultipleHoldingRegisters(0, 0, 1, data, MODBUS_TIMEOUT_MS)
    if(ret[0] != 0):
        print('Failed to set slave ID')

    # 获取设备地址
    data=bytearray([0xAA, 0x55])
    ret = modbus.readHoldingRegisters(0, 0, 1, data, MODBUS_TIMEOUT_MS)
    if(data[0] != MODBUS_SLAVE_ID):
        print('Slave ID is not set to 1') 

def open_relay(index):
    modbus.writeCoil(MODBUS_SLAVE_ID, index, 0xFF00, MODBUS_TIMEOUT_MS)

def close_relay(index):
    modbus.writeCoil(MODBUS_SLAVE_ID, index, 0x0000, MODBUS_TIMEOUT_MS)

def person_detector() :
    global irDev

    lastIRExist = 0
    while True:             # 无限循环
        thisIRExist = irDev.irDetect()
        print('thisIRExist = ', thisIRExist)

        if(lastIRExist != thisIRExist):
            # 生成上报到物联网平台的属性值字串，此处的属性标识符"door_opened"必须和物联网平台的属性一致
            # "door_opened" - 表示入户门开关状态
            upload_data = {'params': ujson.dumps({
                'is_person_exist': thisIRExist,
                })
            }
            # 上传状态到物联网平台
            device.postProps(upload_data)

            if(thisIRExist == 1):
                open_relay(0)
            else:
                close_relay(0)    

        lastIRExist = thisIRExist
        utime.sleep(CHECK_PERIOD_SEC)      # 打印完之后休眠3秒


if __name__ == '__main__' :
    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    get_wifi_status()

    connect_lk(productKey, deviceName, deviceSecret)
    ir_init()

    #初始化modbus设备
    modbus_init()
    person_detector()

