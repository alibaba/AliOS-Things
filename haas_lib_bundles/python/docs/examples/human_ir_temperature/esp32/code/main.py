#########
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson
##################

from driver import I2C,PWM,TIMER
import utime
from ssd1306 import SSD1306_I2C
import buzzer
import mlx90614
import time

INVALUDE_TEMP = 1037 # 失效测量值
LOWER_LEVEL = 30 # 最小有效测温阈值 【30 60】
HIGH_LEVEL = 60 # 最大有效测温阈值
ABNORMAL_TEMP = 35 # 异常温度设定阈值  【30 ... 60】

E_CAL_TEMP_INVALID = 1
E_CAL_TEMP_FINISH = 2
E_CAL_TEMP_HIGH = 3
E_CAL_TEMPING = 4
E_CAL_TEMP_START = 5

timer_interval_100ms = 100 #ms
cal_temp_timer = 100  #10s = (100*timer_interval_100ms)
high_timer = 50  #5s = (50*timer_interval_100ms)
TEMP_CAL_TIMER = (cal_temp_timer) #有效测量时间
TEMP_CAL_TIMER_HIGH = (high_timer) #异常温度，有效测量时间

timer_interval_1000ms = 1000
stay_long = 5  # 5s (5*timer_interval_1000ms)
DISPLAY_STAY_LONG = (stay_long) # 测量出有效温度后 屏幕持续显示时间

mlx90614Dev = 0
oled = 0
object_temp = 0.00
ambient_temp = 0.00

cal_hightemp_cnt = 0
cal_temp_cnt = 0
last_temp = 0
valid_temp = 0
blink = 0
display_time_cnt = 0
event_display = 0
start_cal_temp = 1
time_is_start = 0
displaytime_flag = 0

obj_buzzer = 0
pwmObj = 0

temptimerObj = 0
oledtimerObj = 0
displaytimerObj = 0

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
    pass

def report_event():
    upload_data = {'params': ujson.dumps({
        'object_temp': object_temp,
    })
    }
    # 上传测到的人体异常温度信息到物联网平台
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

# init function
def buzzer_init():
    global obj_buzzer
    pwmObj = PWM()
    pwmObj.open("buzzer")
    print("buzzer inited!")
    obj_buzzer = buzzer.BUZZER(pwmObj)
    # define init freq and duty
    data_r = {'freq':2000, 'duty': 99}
    obj_buzzer.setOptionDuty(data_r)

def buzzer_start():
    data_r = {'freq':2000, 'duty': 95}
    obj_buzzer.setOptionDuty(data_r)

def buzzer_stop():
    data_r = {'freq':2000, 'duty': 99}
    obj_buzzer.setOptionDuty(data_r)

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

# 初始化mlx90614传感器
def mlx9061_init():
    global mlx90614Dev

    i2cObj1 = I2C()
    i2cObj1.open("mlx90614")
    print("mlx90614 inited!")
    mlx90614Dev = mlx90614.MLX90614(i2cObj1)

def display():
    global oled,blink,display_time_cnt,start_cal_temp,time_is_start,displaytime_flag
    # i_am_temp = int(ambient_temp)
    i_obj_temp = int(object_temp)

    if i_obj_temp == INVALUDE_TEMP :
        return

    print("display ",event_display, display_time_cnt)
    oled.fill(0)   #清屏背景黑色

    if event_display == E_CAL_TEMP_HIGH:
        print('display high event')
        display_time_cnt += 1
        if display_time_cnt >= DISPLAY_STAY_LONG :
            displaytime_flag = 1
            oled_timer_stop()
            time_is_start = 0
            start_cal_temp = 1
            buzzer_stop()
        else :
            oled.text('hot'+'['+str('%d'%ABNORMAL_TEMP)+']', 20,20)
            oled.text(str('Cal:'+'%.2f'%valid_temp)+' C',20,40)
    elif event_display == E_CAL_TEMPING:
        if blink == 0:
            blink = 1
            oled.text('checking', 40, 20)
        else:
            blink = 0
            oled.text('', 60, 10)
        oled.text(str('hum:'+'%.2f'%object_temp)+' C',20,35)
        oled.text(str('amb:'+'%.2f'%ambient_temp)+' C',20,50)
    elif event_display == E_CAL_TEMP_FINISH:
        print('display finish enent')
        display_time_cnt += 1
        if display_time_cnt >= DISPLAY_STAY_LONG :
            displaytime_flag = 1
            oled_timer_stop()
            time_is_start = 0
            start_cal_temp = 1
        else :
            oled.text('Cal', 30, 20)
            oled.text(str('%.2f'%valid_temp)+' C',30,40)
    elif event_display == E_CAL_TEMP_INVALID:
        print('display ivalid enent')
        displaytime_flag = 1
        oled_timer_stop()
        time_is_start = 0
    oled.show()

def read_ir_temp():
    global ambient_temp,object_temp,mlx90614Dev

    ambient_temp = mlx90614Dev.read_ambient_temp()
    object_temp = mlx90614Dev.read_object_temp()
    # print("{0:>5.2f}C | {1:>5.2f}C".format(ambient_temp, object_temp), end='\r')
    # print("\r")

def is_abnormal(obj_temp):
    global cal_hightemp_cnt,cal_temp_cnt,last_temp,valid_temp

    i_obj_temp = int(obj_temp)
    if i_obj_temp == INVALUDE_TEMP :
        return

    if i_obj_temp >= HIGH_LEVEL or i_obj_temp <= LOWER_LEVEL:
        last_temp = i_obj_temp
        cal_hightemp_cnt = 0
        cal_temp_cnt = 0
        return E_CAL_TEMP_INVALID
    else:
        if last_temp >= HIGH_LEVEL or last_temp <= LOWER_LEVEL:
            last_temp = i_obj_temp
            cal_temp_cnt = 1
            cal_hightemp_cnt = 1
            return E_CAL_TEMP_START #开始测温

        if i_obj_temp >= ABNORMAL_TEMP:
            if last_temp < ABNORMAL_TEMP:
                cal_hightemp_cnt = 1 #重新开始测，第一次测到高温
            else:
                cal_hightemp_cnt += 1 #连续测到高温
        else:
            cal_hightemp_cnt = 0  #测到正常温度

    cal_temp_cnt += 1
    last_temp = i_obj_temp
    if cal_hightemp_cnt >= TEMP_CAL_TIMER_HIGH:
        cal_hightemp_cnt = 0   #连续测到10s高温，那么认定高温
        valid_temp = obj_temp
        return E_CAL_TEMP_HIGH

    if cal_temp_cnt >= TEMP_CAL_TIMER:
        cal_temp_cnt = 0
        valid_temp = obj_temp
        return E_CAL_TEMP_FINISH #连续15S，那么结束测温
    return E_CAL_TEMPING

def readtemp_callback(args):
    global event_display,display_time_cnt,start_cal_temp,time_is_start,displaytime_flag,last_temp

    if start_cal_temp == 1:
        read_ir_temp()
    else:
        return
    event_display = is_abnormal(object_temp)
    # print("{0:>5.2f}C | {1:>5.2f}C".format(ambient_temp, object_temp), end='\r')
    # print("\r")

    if event_display == E_CAL_TEMP_START:
        if time_is_start == 0:
            time_is_start = 1
            oled_timer_restart()
            displaytime_flag = 0
            print("start")
        display_time_cnt = 0
    elif event_display == E_CAL_TEMP_HIGH:
        start_cal_temp = 0
        last_temp = 0
        print("high")
        buzzer_start()
        # 当前温度异常，上报当前温度
        report_event()
        print("{0:>5.2f}C | {1:>5.2f}C".format(ambient_temp, object_temp), end='\r')
        print("\r")
    elif event_display == E_CAL_TEMP_FINISH:
        start_cal_temp = 0
        last_temp = 0
        print("finish")
        print("{0:>5.2f}C | {1:>5.2f}C".format(ambient_temp, object_temp), end='\r')
        print("\r")
    elif event_display == E_CAL_TEMP_INVALID:
        start_cal_temp = 1

def oleddisplay_callback(args):
    display()

def display_time(timeArray):
    global displaytime_flag

    # print('flag :',displaytime_flag)
    if displaytime_flag == 1:
        oled.fill(0)
        oled.text(str('%d-%02d-%02d'%(timeArray[0],timeArray[1],timeArray[2])),30,5)
        oled.text(str('%02d:%02d:%02d'%(timeArray[3],timeArray[4],timeArray[5])),30,22)
        oled.text(str('----------------------'),3,32)
        oled.text('ready', 40, 40)
        oled.show()

def displaytime_callback(args):
    timeArray = time.localtime()
    display_time(timeArray)

def oled_timer_stop():
    oledtimerObj.stop()
    print('time stop:')

def oled_timer_start():
    oledtimerObj.start()

def oled_timer_restart():
    print('------------------')
    ret1 = oledtimerObj.reload()
    print('reload time:',ret1)

def temp_timer_stop():
    temptimerObj.stop()

def temp_timer_start():
    temptimerObj.start()

def timer_init():
    global temptimerObj,oledtimerObj,displaytimerObj

    temptimerObj = TIMER(0)
    temptimerObj.open(mode=temptimerObj.PERIODIC, period=timer_interval_100ms, callback=readtemp_callback)
    temptimerObj.start()

    oledtimerObj = TIMER(1)
    oledtimerObj.open(mode=oledtimerObj.PERIODIC, period=1000, callback=oleddisplay_callback)

    displaytimerObj = TIMER(2)
    displaytimerObj.open(mode=displaytimerObj.PERIODIC, period=1000, callback=displaytime_callback)

if __name__ == '__main__' :
    mlx9061_init()
    oled_init()
    ##########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    ##########
    buzzer_init()
    timer_init()
    while True:
        utime.sleep(2)
