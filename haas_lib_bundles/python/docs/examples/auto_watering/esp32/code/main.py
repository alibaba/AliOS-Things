#########
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson
##################

import utime   # 延时函数在utime库中
from driver import GPIO
import relay,soil_moisture

relayDev = None
humiDev = None
relayStatus = None

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
    global relayStatus
    payload = ujson.loads(request['params'])
    # print (payload)
    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "onoff" in payload.keys():
        value = payload["onff"]
        if value == 1:
            start_watering()
            relayStatus = 1
            print("打开小水泵")
            report_event()
        elif value == 0:
            stop_watering()
            relayStatus = 0
            print("关闭小水泵")
            report_event()
        else:
            print("无效参数")

def report_event():
    upload_data = {'params': ujson.dumps({'onoff': relayStatus})}
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

def relay_init():
    global relayDev

    gpioDev = GPIO()
    gpioDev.open("relay")
    relayDev = relay.Relay(gpioDev,1)

def humi_init():
    global humiDev

    gpioObj = GPIO()
    gpioObj.open("humidify")
    humiDev = soil_moisture.SoilMoisture(gpioObj)

def start_watering():
    relayDev.trigger()

def stop_watering():
    relayDev.untrigger()

if __name__ == '__main__':
    curstatus = None
    laststatus = None
    relay_init()
    humi_init()
    ##########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    ##########
    while True:
        humidity = humiDev.moistureDetect()
        if humidity == 0:
            curstatus = 0
            if curstatus != laststatus:
                stop_watering()
                relayStatus = 0
                report_event()
                print("关闭小水泵")
                laststatus = curstatus
        elif humidity == 1:
            curstatus = 1
            if curstatus != laststatus:
                start_watering()
                relayStatus = 1
                report_event()
                print("打开小水泵")
                laststatus = curstatus
        utime.sleep(1)
