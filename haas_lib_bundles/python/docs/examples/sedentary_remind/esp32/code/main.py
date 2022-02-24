#########
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson
##################

from driver import I2C,GPIO,TIMER
import utime
from ssd1306 import SSD1306_I2C
import ir
import time

temptimerObj = 0
time_dict = {}

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
    wlan.connect(wifiSsid, wifiPassword)    # 连接到指定路由器的名称和密码

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

# 上传休息提醒状态到物联网平台
def report_event(data):
    upload_data = {'params': ujson.dumps({
        'call_rst': data,
        })
    }
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

PERSON_MOVE = 1
PERSON_LEAVE = 2
PERSON_NOMOVE = 3

ir_event = 0

tick_time = 0
total_time = 0
first_move = 0
has_rest = 0
has_leve = 0

timer_interval_1000ms = 1000 #单位ms
check_max_time1 = (90) #单位s ,用于判断当前区域没有人的时间阈值 (60*timer_interval_1000ms)
check_max_time2 = (45*60) #单位s,用于判断人在当前区域的最大时间阈值 (60*60*timer_interval_1000ms)
CHECK_NOPERSON_MAX_TIME = check_max_time1
CHECK_HASPERSON_MAX_TIME = check_max_time2

gpioirDev =0
def irq_handler(data):
    global tick_time,total_time,gpioirDev,first_move,has_rest,has_leve,ir_event

    if first_move == 0:
        first_move = 1

    ret = gpioirDev.read()
    if ret == 1:
        print('person come')
        if tick_time >= CHECK_NOPERSON_MAX_TIME:
            print("person has left ",tick_time)
            total_time = 0
            tick_time = 0
            has_leve = 1
            ir_event = PERSON_LEAVE
            return
        else:
            total_time += tick_time
            print("check person move ",tick_time,total_time)
            tick_time = 0
            ir_event = PERSON_MOVE
    elif ret == 0:
        total_time += tick_time
        print('check person nomove ',tick_time,total_time)
        tick_time = 0
        ir_event = PERSON_NOMOVE

    if total_time >= CHECK_HASPERSON_MAX_TIME:
        has_rest = 1
        total_time = 0
        print('you need has rest,because you work too long')

# init function
def ir_check_init():
    global gpioirDev,ir_event

    gpioirDev = GPIO()
    gpioirDev.open("ir")
    gpioirDev.on(irq_handler)
    irDev = ir.IR(gpioirDev)

    ret = gpioirDev.read()
    if ret == 1:
        print('person come')
        ir_event = PERSON_MOVE
    elif ret == 0:
        print('no person')
        ir_event == PERSON_NOMOVE


#初始化oled模块
def oled_init():
    global oled

    i2cObj = I2C()
    i2cObj.open("ssd1306")
    print("ssd1306 inited!")

    oled = SSD1306_I2C(128, 64, i2cObj)
    oled.fill(0)   #清屏背景黑色
    oled.text('2022-01-03', 30, 5)
    oled.text('12:00:00', 30, 22)
    oled.text(str('----------------------'),3,32)
    oled.text('connecting', 30, 40)

    oled.show()

def display_time(timeArray):
    global total_time,tick_time

    val_time = int((total_time + tick_time)/60)
    oled.fill(0)
    oled.text(str('%d-%02d-%02d'%(timeArray[0],timeArray[1],timeArray[2])),30,5)
    oled.text(str('%02d:%02d:%02d'%(timeArray[3],timeArray[4],timeArray[5])),30,22)
    oled.text(str('----------------------'),3,32)
    if ir_event == PERSON_MOVE:
        oled.text('move', 40, 40)
        oled.text(str('%02d'%(val_time)),4,13)
    elif ir_event == PERSON_LEAVE:
        oled.text('person leave', 25, 40)
        oled.text(str('--'),2,13)
    elif ir_event == PERSON_NOMOVE:
        oled.text('no move', 40, 40)
        oled.text(str('%02d'%(val_time)),4,13)
    oled.show()

def timer_callback(args):
    global tick_time,first_move,time_dict

    timeArray = time.localtime()
    display_time(timeArray)

    if first_move == 1 and has_leve == 0:
        tick_time += 1

def temp_timer_stop():
    temptimerObj.stop()

def temp_timer_start():
    temptimerObj.start()

def timer_init():
    global temptimerObj

    temptimerObj = TIMER(0)
    temptimerObj.open(mode=temptimerObj.PERIODIC, period=timer_interval_1000ms, callback=timer_callback)
    temptimerObj.start()

if __name__ == '__main__' :
    oled_init()
    #########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    #########
    ir_check_init()
    timer_init()
    while True:
        if has_rest == 1:
            has_rest = 0
            report_event(1)
            print('report rest')
        if has_leve == 1:
            has_leve = 0
            report_event(0)
            print('report leve')
        utime.sleep(1)
