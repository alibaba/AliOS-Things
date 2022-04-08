import hx710                    # 引入HX710传感器驱动库
import tds                      # 引入tds传感器驱动库
import ds18b20                  # 引入ds18b20传感器驱动库
import relay                    # 引入继电器驱动库
from driver import ADC
from driver import GPIO
import utime                    # 延时函数在utime库中

from aliyunIoT import Device  # iot组件是连接阿里云物联网平台的组件
import network                # Wi-Fi功能所在库
import ujson                  # json字串解析库

#传感器对象
hx710Obj = 0
tdsObj = 0
ds18b20Obj = 0

# 继电器对象
relay1Obj = 0
relay2Obj = 0
relay3Obj = 0

#门限值
hx710Threshold = 120000
tdsThreshold = 800
ds18b20Threshold = 30

#本地控制开关
relay1Manual = 0
relay2Manual = 0
relay3Manual = 0

# 继电器开关状态
addWaterFlag = 0
startFilterFlag = 0
makeHeatFlag = 0

#保存实时参数
deep = 0
tdsValue = 0
temp = 0

def hx710_init():
    global hx710Obj

    clkDev = GPIO()
    clkDev.open("hx710_clk")

    dataDev = GPIO()
    dataDev.open("hx710_data")

    hx710Obj = hx710.HX710(clkDev, dataDev)

def get_hx710_value():
    global hx710Obj
    return hx710Obj.getValue()

def tds_init():
    global tdsObj
    tdsChannel = ADC()
    tdsChannel.open('tds')
    tdsObj = tds.TDS(tdsChannel, T = 10)

def get_tds_value():
    global tdsObj
    return tdsObj.getTDS()

def ds18b20_init():
    global ds18b20Obj
    gpioDev = GPIO()
    gpioDev.open("ds18b20")
    ds18b20Obj = ds18b20.DS18B20(gpioDev, 12)

def get_ds18b20_value():
    global ds18b20Obj
    return ds18b20Obj.getTemperature()

def relays_init():
    global relay1Obj, relay2Obj, relay3Obj

    relay1Dev = GPIO()
    relay1Dev.open("relay_1")
    relay1Obj =relay.Relay(relay1Dev, 1)

    relay2Dev = GPIO()
    relay2Dev.open("relay_2")
    relay2Obj =relay.Relay(relay2Dev, 1)

    relay3Dev = GPIO()
    relay3Dev.open("relay_3")
    relay3Obj =relay.Relay(relay3Dev, 1)

# 物联网平台连接标志位
iot_connected = False

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# 物联网设备实例
device = None

# Wi-Fi SSID和Password设置
wifiSsid = "请输入您的路由器名称"
wifiPassword = "请输入您的路由器密码"

wlan = None

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

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global relay1Manual, relay2Manual, relay3Manual
    global makeHeatFlag, startFilterFlag, addWaterFlag
    global deep, tdsValue, temp
    print(request)
    payload = ujson.loads(request['params'])

    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "AddWater" in payload.keys():
        print(payload)
        data = payload["AddWater"]
        if (data):
            print("开始加水")
            if (deep < hx710Threshold):
                relay1Obj.open()
                addWaterFlag = 1
        else:
            print("结束加水")
            relay1Obj.close()
            addWaterFlag = 0

    if "StartFilter" in payload.keys():
        print(payload)
        data = payload["StartFilter"]
        if data:
            print("开始过滤")
            relay2Obj.open()
            startFilterFlag = 1
        else:
            print("结束过滤")
            relay2Obj.close()
            startFilterFlag = 0

    if "MakeHeat" in payload.keys():
        print(payload)
        data = payload["MakeHeat"]
        if (data) and (temp < ds18b20Threshold):
            print("开始加热")
            relay3Obj.open()
            makeHeatFlag = 1
        else:
            print("结束加热")
            relay3Obj.close()
            makeHeatFlag = 0


# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

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

# 上传信息到物联网平台
def upload_detector_state():
    global device
    global relay1Manual, relay2Manual, relay3Manual
    global hx710Threshold, tdsThreshold, ds18b20Threshold
    global makeHeatFlag, startFilterFlag, addWaterFlag
    global deep, tdsValue, temp
    # 无限循环
    while True:

        deep = get_hx710_value()
        tdsValue = get_tds_value()
        temp = get_ds18b20_value()
        print('deep %.2f, tdsValue %d, temp %.2f ' % (deep/120000, tdsValue, temp))

        if (relay1Manual == 1):
            if (deep < hx710Threshold):
                relay1Obj.open()
            else:
                relay1Obj.close()

        if (relay2Manual == 1):
            if (tdsValue > tdsThreshold):
                relay2Obj.open()
            else:
                relay2Obj.close()

        if (relay3Manual == 1):
            if (temp < ds18b20Threshold):
                relay3Obj.open()
            else:
                relay3Obj.close()

        # 上传信息到物联网平台
        upload_data = {'params': ujson.dumps({
            'WaterLevel': round(deep/120000,2),
            'TDS': tdsValue,
            'WaterTemperature': temp,
            'MakeHeat': makeHeatFlag,
            'StartFilter': startFilterFlag,
            'AddWater': addWaterFlag
            })
        }
        device.postProps(upload_data)

        utime.sleep(0.5)

if __name__ == '__main__':
    hx710_init()
    tds_init()
    ds18b20_init()
    relays_init()
    get_wifi_status()

    connect_lk(productKey, deviceName, deviceSecret)
    upload_detector_state()
