# -*- encoding: utf-8 -*-

from aliyunIoT import Device  # iot组件是连接阿里云物联网平台的组件
from driver import GPIO
from driver import TIMER      # 定时器类
from driver import PWM        # PWM类
from driver import ADC        # ADC类
from driver import I2C        # I2C类
from driver import UART       # UART类
from driver import SPI        # SPI类
from mpu6050 import MPU6050   # 加速度计陀螺仪类
import uln2003                # 引入ULN2003步进电机驱动库
import ujson                  # json字串解析库
import utime                  # 延时函数在utime库中
import servo                  # 舵机控制库
import sht3x                  # 温湿度传感器库
import gnss                   # 定位模块gnss传感器库
import network                # Wi-Fi功能所在库
import magnetronSensor        # 磁控管开关传感器库
import photoresistor          # 光敏电阻传感器库
import motorspeed             # 马达测速传感器库
import mfrc522                # 门禁卡驱动库
import _thread                # 线程库

# 主线程超时时间
CASE_TIMEOUT_S = 3600

# 步进电机控制量
en_uln2003 = False
# 舵机控制量
en_servo = False
# 步进电机转数计数
motor_cnt = 0
# 步进电机转动圈数
uln2003_cnt = 0
# 舵机摆动次数
servo_cnt = 0
# 磁控管检测计数
magnetron_cnt = 0
# 入侵检测
invasion = 0
# 加速度
ax = 0
ay = 0
az = 0
# 陀螺仪
gx = 0
gy = 0
gz = 0
# GPS
longitude = None
latitude = None
altitude = None

# 物联网平台连接标志位
iot_connected = False
wlan = None

# 三元组信息
productKey = "产品key"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# 物联网设备实例
device = None

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 传感器设备类
magnetronDev = None
gpioDev = None
pwmDev = None
i2cDev = None
i2cDev1 = None
adcDev = None
sht3xDev = None
mpu6050Dev = None
uartDev = None
gnssDev = None
servoDev = None
photoresistorDev = None
gpioDev1 = None
motorspeedDev = None
spiDev = None
sdaGpioDev = None
rstGpioDev = None
mfrc522Dev = None
uln2003Dev = None

# 设备实例
A = None
A_ = None
B = None
B_ = None

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

    ifconfig = wlan.ifconfig()                 # 获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    utime.sleep(0.5)

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    payload = ujson.loads(request['params'])
    # print("payload:%s"%payload)

    if ("en_alert" in payload.keys()):
        alert = payload["en_alert"]
        if (alert):
            print("alarm: invasion ...\n")

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

# 设备初始化
def timer_init():
    global timerObj

    timerObj = TIMER(0)
    timerObj.open(mode=timerObj.PERIODIC, period=1000, callback=timer_cb)
    timerObj.start()

def uln2003_init():
    global uln2003Dev
    global A, A_, B, B_
    A = GPIO()
    A.open("uln2003_a")
    A_ = GPIO()
    A_.open("uln2003_a_")
    B = GPIO()
    B.open("uln2003_b")
    B_ = GPIO()
    B_.open("uln2003_b_")
    uln2003Dev = uln2003.ULN2003(A, A_, B, B_)

def magnetron_init():
    global gpioDev, magnetronDev
    gpioDev = GPIO()
    gpioDev.open("magnetron")
    magnetronDev = magnetronSensor.MagnetronSensor(gpioDev)

def motorspeed_init():
    global gpioDev1, motorspeedDev
    gpioDev1 = GPIO()
    gpioDev1.open("motorspeed")
    motorspeedDev = motorspeed.MOTORSPEED(gpioDev1, motorspeed_handler)

def servo_init():
    global pwmDev, servoDev
    pwmDev = PWM()
    pwmDev.open("servo")
    servoDev = servo.SERVO(pwmDev)

def photoresistor_init():
    global adcDev, photoresistorDev
    adcDev = ADC()
    adcDev.open("photoresistor")
    photoresistorDev = photoresistor.PHOTORESISTOR(adcDev)

def sht3x_init():
    global i2cDev, sht3xDev
    i2cDev = I2C()
    i2cDev.open("sht3x")
    sht3xDev = sht3x.SHT3X(i2cDev)

def mpu6050_init():
    global i2cDev1, mpu6050Dev
    i2cDev1 = I2C()
    i2cDev1.open("mpu6050")
    mpu6050Dev = MPU6050(i2cDev1)

def gnss_init():
    global gnssDev, uartDev
    uartDev = UART()
    uartDev.open('gnss')
    gnssDev = gnss.Gnss(uartDev)

def mfrc522_init():
    global spiDev, sdaGpioDev, rstGpioDev, mfrc522Dev
    spiDev = SPI()
    spiDev.open("mfrc522")
    sdaGpioDev = GPIO()
    sdaGpioDev.open("mfrc522_sda")
    rstGpioDev = GPIO()
    rstGpioDev.open("mfrc522_rst")
    mfrc522Dev = mfrc522.MFRC522(spiDev, sdaGpioDev, rstGpioDev)

# 关闭设备
def timer_deinit():
    global timerObj
    timerObj.stop()
    timerObj.close()

def uln2003_deinit():
    global A, A_, B, B_
    A.close()
    A_.close()
    B.close()
    B_.close()

def magnetron_deinit():
    global gpioDev
    gpioDev.close()

def motorspeed_deinit():
    global gpioDev1
    gpioDev1.close()

def servo_deinit():
    global pwmDev
    pwmDev.close()

def photoresistor_deinit():
    global adcDev
    adcDev.close()

def sht3x_deinit():
    global i2cDev
    i2cDev.close()

def mpu6050_deinit():
    global i2cDev1
    i2cDev1.close()

def gnss_deinit():
    global uartDev
    uartDev.close()

def mfrc522_deinit():
    global spiDev, sdaGpioDev, rstGpioDev
    spiDev.close()
    sdaGpioDev.close()
    rstGpioDev.close()

# 步进电机控制
def uln2003_ctrl(cmd = 'stop', step = 0, speed = 4):
    global uln2003Dev
    step_tmp = step

    if (cmd is 'stop'): # 停止
        uln2003Dev.motorStop()
        return

    while (step_tmp > 0):
        if (cmd is 'cw'): # 顺时针转动
            uln2003Dev.motorCw(speed)
        elif (cmd is 'ccw'): # 逆时针转动
            uln2003Dev.motorCcw(speed)

        step_tmp -= 1

# 定时器回调函数
def timer_cb(args):
    global ax, ay, az, gx, gy, gz
    global longitude, latitude, altitude

    print("timer run ...\n")

    (ax, ay, az) = mpu6050_read("A")
    (gx, gy, gz) = mpu6050_read("G")

    location = gnss_read()
    if(location):
        # print(location.longitude, location.latitude, location.altitude)
        longitude = location.longitude[0]
        latitude = location.latitude[0]
        altitude = location.altitude

# 磁体检测，0-检测到磁体，1-未检测到磁体；
def get_magnetron_status():
    global magnetronDev
    status = magnetronDev.isMagnetic()
    return status

# 舵机控制
def servo_control(data):
    global servoDev
    servoDev.setOptionSero(data)

# 读取液位数据
def get_lightness():
    global photoresistorDev
    return photoresistorDev.getLightness()

# 温湿度读取
def sht3x_read(param):
    global sht3xDev

    if (param is "H"):
        data = sht3xDev.getHumidity()
    elif(param is "T"):
        data = sht3xDev.getTemperature()
    else:
        raise ValueError("sht3x_read param error.")

    return data

# 加速度/角速度读取
def mpu6050_read(param):
    global mpu6050Dev

    if (param is "A"):
        data = mpu6050Dev.getAcceleration()
    elif(param is "G"):
        data = mpu6050Dev.getGyroscope()
    else:
        raise ValueError("mpu6050_read param error.")

    return data

# 位置信息读取
def gnss_read():
    global gnssDev
    return gnssDev.getLocation()

# 马达转数计数回调
def motorspeed_handler(data):
    global motor_cnt
    motor_cnt += 1
    print('motor speed cnts:', motor_cnt)

# 门禁控制线程
def mfrc522_th():
    global mfrc522Dev
    while (True):
        rfid_card = mfrc522Dev.readCardID()
        utime.sleep(1)
        print("rfid_card:", rfid_card)

# 电机控制线程
def motor_th():
    global en_servo, en_uln2003
    global uln2003_cnt, servo_cnt

    servo_control(0) # 舵机复位
    utime.sleep(1)

    while (True):
        if (en_servo):
            print("run servo ...\n")
            servo_cnt += 1
            servo_control(-90) # 舵机旋转-90度
            utime.sleep(3)
            servo_control(90)  # 舵机旋转90度
            utime.sleep(1)
            en_servo = False

        if (en_uln2003):
            print("run motor ...\n")
            uln2003_cnt += 1
            # 驱动步进电机走512步，即电机转动一周
            uln2003_ctrl('cw', 512, 4)
            en_uln2003 = False

        print("servo_cnt=", servo_cnt)
        print("uln2003_cnt=", uln2003_cnt)
        utime.sleep_ms(500)

def control_th():
    global invasion, en_uln2003, en_servo
    global magnetron_cnt

    while (True):
        if (not get_magnetron_status()): # 获取磁控管传感器的状态,0-检测到入侵，1-未检测入侵
            invasion = 1
            magnetron_cnt += 1
        else:
            invasion = 0

        if (en_uln2003 is False):
            lightness = get_lightness()
            if (lightness < 1000):
                en_uln2003 = True

            print("lightness: ", lightness)

        if (en_servo is False):
            humidity = sht3x_read("H")
            temp = sht3x_read("T")
            if (humidity > 70.0 or humidity < 30.0 or temp > 30.0 or temp < 15.0):
                en_servo = True # 开启舵机打开温湿度均衡系统

            print("temperature:", temp, "humidity:", humidity)

        utime.sleep(1)


def worst_case_main():
    global wlan, invasion
    global productKey, deviceName, deviceSecret
    global ax, ay, az, gx, gy, gz
    global longitude, latitude, altitude

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)

    timer_init()
    motorspeed_init()
    uln2003_init()
    magnetron_init()
    servo_init()
    photoresistor_init()
    sht3x_init()
    mpu6050_init()
    gnss_init()
    mfrc522_init()

    try:
        # 启动门禁线程
        th0_id = _thread.start_new_thread(mfrc522_th, ())
        # 启动电机驱动线程
        th1_id = _thread.start_new_thread(motor_th, ())
        # 启动控制线程
        th2_id = _thread.start_new_thread(control_th, ())
    except:
        raise ValueError("start thread failed.")

    start = utime.time()
    while True:
        now = utime.time()
        if ((now - start) > CASE_TIMEOUT_S):
            break

        print("upload...\n")
        upload_data = {
            'params': ujson.dumps({
                'GeoLocation': {
                    'Longitude': longitude,
                    'Latitude': latitude,
                    'Altitude': altitude,
                    'CoordinateSystem': 1
                },
                "accel": {
                    'ax':ax,
                    'ay':ay,
                    'az':az
                },
                "gyro": {
                    'gx':gx,
                    'gy':gy,
                    'gz':gz
                },
                "invasion": invasion
            })
        }
        # 上报数据至物联网平台
        device.postProps(upload_data)
        utime.sleep(2)

    _thread.stop_thread(th0_id)
    _thread.stop_thread(th1_id)
    _thread.stop_thread(th2_id)

    mfrc522_deinit()
    gnss_deinit()
    mpu6050_deinit()
    sht3x_deinit()
    photoresistor_deinit()
    servo_deinit()
    magnetron_deinit()
    uln2003_deinit()
    motorspeed_init()
    timer_deinit()

if __name__ == '__main__':
    worst_case_main()
