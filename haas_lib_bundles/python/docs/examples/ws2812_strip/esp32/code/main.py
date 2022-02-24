import random                   # 随机数生成库
import neopixel                 # ws2812驱动库
import machine                  # 设备驱动库
import ujson                    # json字串解析库
import utime                    # 延时API所在组件
import network                  # Wi-Fi功能所在库
from aliyunIoT import Device    # 连接阿里云物联网平台的组件

# 灯带点亮模式
lightMode = 0

# 创建一个由GPIO19控制的neopixel对象
np = neopixel.NeoPixel(machine.Pin(19, machine.Pin.OUT), 60)

# 光效实现
def fadeMode(np):
    for i in range(0, 4 * 256, 8):
        if (i // 256) % 2 == 0:
            val = i & 0xff
        else:
            val = 255 - (i & 0xff)
        for j in range(np.n):
            np[j] = (val, val, val)
        np.write()

def constantMode(np, color):
    for i in range(np.n):
        np[i] = color

def cycleMode(np, frontColor, backgroundColor, index):
    for i in range(np.n):
        if (i == index):
            np[i] = frontColor
        else:
            np[i] = backgroundColor

def rainbowMode(np, num, bright=32, offset=0):
    rb = ((255, 0, 0), (255, 127, 0), (255, 255, 0), (0, 255, 0),
          (0, 255, 255), (0, 0, 255), (136, 0, 255), (255, 0, 0))
    for i in range(num):
        t = 7*i/num
        t0 = int(t)
        r = round((rb[t0][0] + (t-t0)*(rb[t0+1][0]-rb[t0][0]))*bright) >> 8
        g = round((rb[t0][1] + (t-t0)*(rb[t0+1][1]-rb[t0][1]))*bright) >> 8
        b = round((rb[t0][2] + (t-t0)*(rb[t0+1][2]-rb[t0][2]))*bright) >> 8
        np[(i+offset) % num] = (r, g, b)

def randomMode(np):
    for i in range(np.n):
        r = random.randint(0, 255)
        g = random.randint(0, 255)
        b = random.randint(0, 255)
        np[i] = (r, g, b)

###########################################################

# 物联网平台连接标志位
iot_connected = False

# 物联网设备实例
device = None

# 三元组信息
productKey = "产品key"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    wlan = network.WLAN(network.STA_IF)
    wifi_connected = False

    wlan.active(True)  # 激活界面
    wlan.disconnect()  # 断开之前的连接
    # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）
    wlan.connect(wifiSsid, wifiPassword)

    while True:
        wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected:                     # Wi-Fi连接成功则退出while循环
            break
        else:
            utime.sleep(0.5)
            print("wifi_connected:", wifi_connected)

    ifconfig = wlan.ifconfig()  # 获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    utime.sleep(0.5)

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global lightMode, device

    # e.g. {"mode":0} or {"mode":0}
    payload = ujson.loads(request['params'])

    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "mode" in payload.keys():
        print(payload)
        lightMode = payload["mode"]

    # 要将更改后的状态同步上报到云平台
    prop = ujson.dumps({
        'mode': lightMode,
    })
    upload_data = {'params': prop}

    # 上报到云端
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


if __name__ == '__main__':

    wlan = network.WLAN(network.STA_IF)

    get_wifi_status()

    connect_lk(productKey, deviceName, deviceSecret)

    frame = 0

    while True:

        frame = frame + 1 if frame < np.n else 0

        if lightMode == 0:
            #
            constantMode(np, (0, 0, 0))
            np.write()
            utime.sleep(1)

        elif lightMode == 1:
            #
            constantMode(np, (255, 255, 255))
            np.write()
            utime.sleep(1)

        elif lightMode == 2:
            # 跑马灯
            cycleMode(np, (255, 255, 255), (0, 0, 0), frame % (np.n))
            np.write()
            utime.sleep_ms(25)

        elif lightMode == 3:
            # random
            randomMode(np)
            np.write()
            utime.sleep_ms(100)

        elif lightMode == 4:
            # rainbow
            rainbowMode(np, np.n, 32, frame % (np.n))
            np.write()

        elif lightMode == 5:
            fadeMode(np)

        else:
            print("Do something yourself here.")
            utime.sleep(1)
