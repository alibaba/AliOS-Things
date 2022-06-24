
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    声音上云
@Author     :    victor.wang
@version    :    1.0
'''
from ulinksdk.aliyunIoT import Device      # iot组件是连接阿里云物联网平台的组件
import network                    # Wi-Fi功能所在库
import utime                      # 延时API所在组件
from driver import ADC            # ADC类，通过微处理器的ADC模块读取ADC通道输入电压
from driver import GPIO           # ESP32和使用GPIO控制LED
import noise                      # 声音传感器驱动库
import ujson                      # json字串解析库

# LED 蜂鸣器控制变量
redledon = 0
blueledon = 0
greenledon =0
buzzeron =0
voice_value = 0

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

# LED 蜂鸣器 声音传感器设备
redled=None
blueled=None
greenled=None
buzzer=None
adcobj = None
voicedetectordev = None

def led_init():
    global redled, blueled, greenled
    redled = GPIO()
    blueled = GPIO()
    greenled = GPIO()

    redled.open('led_r')     # board.json中led_r节点定义的GPIO，对应esp32外接的的红灯
    blueled.open('led_b')    # board.json中led_b节点定义的GPIO，对应esp32外接的上的蓝灯
    greenled.open('led_g')   # board.json中led_g节点定义的GPIO，对应esp32外接的上的绿灯
    print("led inited!")

def buzzer_init():
    global buzzer
    buzzer=GPIO()
    buzzer.open('buzzer')
    print("buzzer inited!")

def voice_init():
    global adcobj, voicedetectordev
    adcobj = ADC()
    adcobj.open("voice")                     # 按照board.json中名为"voice"的设备节点的配置参数（主设备adc端口号，从设备地址，采样率等）初始化adc类型设备对象
    voicedetectordev = noise.Noise(adcobj)   # 初始化声音传感器
    print("voice inited!")

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
    if not wifi_connected:
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

# 通过声音传感器检测声音分贝大小
def get_voice():
    global voicedetectordev

    voice = voicedetectordev.getVoltage()         # 获取温度测量结果

    return voice                        # 返回读取到的声音大小

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global redledon, blueledon, greenledon, buzzeron, voice_value

    payload = ujson.loads(request['params'])

    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "light_up_green_led" in payload.keys():
        greenledon = payload["light_up_green_led"]
        if (greenledon):
            print("点亮绿灯")

    if "light_up_blue_led" in payload.keys():
        blueledon = payload["light_up_blue_led"]
        if (blueledon):
            print("点亮蓝灯")

    if "light_up_red_led" in payload.keys():
        redledon = payload["light_up_red_led"]
        if (redledon):
            print("点亮红灯")

    if "switch_on_buzzer" in payload.keys():
        buzzeron = payload["switch_on_buzzer"]
        if (buzzeron):
            print("打开蜂鸣器")


    redled.write(redledon)           # 控制红灯
    blueled.write(blueledon)         # 控制蓝灯
    greenled.write(greenledon)       # 控制绿灯
    buzzer.write(buzzeron)          # 控制蜂蜜器

    # 要将更改后的状态同步上报到云平台
    prop = ujson.dumps({
        'light_up_green_led': greenledon,
        'light_up_blue_led': blueledon,
        'light_up_red_led':redledon,
        'switch_on_buzzer':buzzeron,
    })

    upload_data = {'params': prop}
    # 上报led和蜂鸣器属性到云端
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

# 上传声音大小到物联网平台
def upload_voice():
    global device

    while True:
        data = get_voice()                      # 读取声音大小信息
        # 生成上报到物联网平台的属性值字串
        prop = ujson.dumps({
            'SoundDecibelValue': data
            })
        print('uploading data: ', prop)

        upload_data = {'params': prop}
        # 上传声音信息到物联网平台
        device.postProps(upload_data)
        utime.sleep(2)

if __name__ == '__main__':
    # 硬件初始化
    voice_init()
    led_init()
    buzzer_init()
    nic = network.LAN()
    nic.active(1)
    connect_lk(productKey, deviceName, deviceSecret)
    upload_voice()
