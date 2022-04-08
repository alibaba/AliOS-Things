#########
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson
##################

from driver import SPI,GPIO,PWM
import mfrc522
import utime
import servo

rfid_card  = 0
mfrc522Dev = 0
buzzerobj = 0
servoObj = 0

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
    payload = ujson.loads(request['params'])
    # print (payload)
    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "open_door" in payload.keys():
        opendoor_value = payload["open_door"]
        if (opendoor_value):
            door_open()
            buzzer_cardmarch()
            print("打开门")
        else:
            print("不反应")

def report_event():
    global rfid_card

    upload_data = {'params': ujson.dumps({
        'rfid_card': str(rfid_card),
    })
    }
    # 上传端测的RFID卡号到物联网平台
    print(upload_data)
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

def servo_init():
    global servoObj

    pwmObj = PWM()
    pwmObj.open("servo")
    data_r = {'freq':50, 'duty': 0}
    pwmObj.setOption(data_r)
    print("buzzer inited!")
    servoObj = servo.SERVO(pwmObj)

def door_open():
    servoObj.setOptionSero(90)
    print("90")
    utime.sleep(1)
    servoObj.setOptionSero(0)
    print("0")

def init_mfrc522():
    global mfrc522Dev

    spiDev = SPI()
    spiDev.open("mfrc522")

    sda_gpio = GPIO()
    sda_gpio.open("mfrc522_sda")
    reset_gpio = GPIO()
    reset_gpio.open("mfrc522_rst")

    mfrc522Dev = mfrc522.MFRC522(spiDev, sda_gpio, reset_gpio)
    print("mfrc522 inited!")


def init_buzzer():
    global buzzerobj
    buzzerobj = GPIO()
    buzzerobj.open('buzzer')

def buzzer_readcard():
    buzzerobj.write(1)
    utime.sleep_ms(500)
    buzzerobj.write(0)
    utime.sleep_ms(500)

def buzzer_cardmarch():
    buzzerobj.write(1)
    utime.sleep(1)
    buzzerobj.write(0)
    utime.sleep_ms(100)
    buzzerobj.write(1)
    utime.sleep_ms(100)
    buzzerobj.write(0)

if __name__ == '__main__' :
    ##########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    ##########
    servo_init()
    init_buzzer()
    init_mfrc522()
    while True:
        print("\nHold a tag near the reader")
        rfid_card = mfrc522Dev.readCardID()
        buzzer_readcard()
        report_event()
