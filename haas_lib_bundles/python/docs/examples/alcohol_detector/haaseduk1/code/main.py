# -*- encoding: utf-8 -*-

from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import netmgr as nm              # Wi-Fi 功能所在库
import utime                     # 延时API所在组件
import ujson                     # json字串解析库
from driver import GPIO          # ADC类
from driver import SPI           # ADC类
from driver import ADC           # ADC类
from driver import PWM           # PWM类
from mq3 import MQ3              # 酒精传感器类
from buzzer import BUZZER        # 蜂鸣器类
import sh1106                    # SH1106 OLED驱动库
import framebuf                  # framebuf基类，用于设置字体库

ALCOHOL_ALARM_VALUE = 800

adcObj= None
pwmObj = None
buzzerDev = None
mq3Dev = None
alarming = False

# 物联网平台连接标志位
iot_connected = False
wlan = None

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 三元组信息
productKey = "产品密钥"  #需要填入物联网云平台申请到的productKey信息
deviceName = "设备名称"  #需要填入物联网云平台申请到的deviceName信息
deviceSecret = "设备密钥" #需要填入物联网云平台申请到的deviceSecret信息

# 物联网设备实例
device = None

def alcohol_init():
    global adcObj, pwmObj, mq3Dev, buzzerDev, sn74hc595Dev
    global rclk, sclk, dio

    # 初始化酒精传感器 MQ3
    adcObj = ADC()
    adcObj.open("mq3")
    mq3Dev = MQ3(adcObj)

    # 初始化蜂鸣器
    pwmObj = PWM()
    pwmObj.open("buzzer")
    pwm_init_data = {'freq':2000, 'duty': 0}
    buzzerDev =  BUZZER(pwmObj)

def get_alcohol_value():
    global mq3Dev
    vol = mq3Dev.getVoltage()
    return int(vol)

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wifiSsid, wifiPassword
    nm.init()
    print("start to connect ", wifiSsid)
    nm.connect(wifiSsid, wifiPassword)
    while True:
        wifi_connected = nm.getStatus()    # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected == 5:            # Wi-Fi连接成功则退出while循环
            info = nm.getInfo()
            print("\n")
            print("wifi 连接成功:")
            print("    SSID:", info["ssid"])
            print("    IP:", info["ip"])
            print("    MAC:", info["mac"])
            print("    RSSI:", info["rssi"])
            break
        else:
            print("wifi 连接失败")
            utime.sleep(0.5)
    print('sleep for 1s')
    utime.sleep(1)

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    pass

# OLED初始化
def oledInit():
    global oled

    # 字库文件存放于项目目录 font, 注意若用到了中英文字库则都需要放置
    framebuf.set_font_path(framebuf.FONT_ASC12_8, '/data/font/ASC12_8')
    framebuf.set_font_path(framebuf.FONT_ASC16_8, '/data/font/ASC16_8')
    framebuf.set_font_path(framebuf.FONT_ASC24_12, '/data/font/ASC24_12')
    framebuf.set_font_path(framebuf.FONT_ASC32_16, '/data/font/ASC32_16')

    oled_spi = SPI()
    oled_spi.open("oled_spi")

    oled_res = GPIO()
    oled_res.open("oled_res")

    oled_dc = GPIO()
    oled_dc.open("oled_dc")

    #oled像素132*64
    oled = sh1106.SH1106_SPI(132, 64, oled_spi, oled_dc, oled_res)

# OLED显示
# text:显示的文本
# x:水平坐标 y:垂直坐标
# color:颜色
# clear: True-清屏显示 False-不清屏显示
# sz:字体大小
def oledShowText(text, x, y, color, clear, sz):
    global oled
    if clear:
        oled.fill(0) # 清屏
    oled.text(text, x, y, color, size = sz)
    oled.show()

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

def buzzer_alarm(enable):
    if enable:
        if not alarming:
            buzzerDev.pwmObj.open("buzzer")
        pwm_option_data = {'freq':2000, 'duty': 50}
        buzzerDev.start(pwm_option_data)
    else:
        #pwm_option_data = {'freq':2000, 'duty': 0}
        #print("stop alarm:", pwm_option_data)
        #buzzerDev.close(pwm_option_data)
        buzzerDev.pwmObj.close()

# 上传酒精浓度信息到物联网平台
def upload_alcohol_detector_state():
    global device, alarming

    while True:
        data = get_alcohol_value()
        print('alcohol adc voltage:%d mv.' % data)
        if data < 0:
            print("Error: invalid alcohol adc voltage:", data)
            continue

        temp_str = "Alcohol:"
        temp_data_str = "%d" % data
        oledShowText(temp_str, 3, 1, 1, True, 16)
        oledShowText(temp_data_str, 40, 20, 1, False, 32)

        if (data > ALCOHOL_ALARM_VALUE):
            print("Info: alcohol detected, start alarm...")
            buzzer_alarm(True)
            alarming = True

        # "alcohol_adc_voltage" - 在云平台上创建产品时对应的酒精浓度属性的标识符
        upload_data = {'params': ujson.dumps({
            'alcohol_adc_voltage': data,
        })}
        device.postProps(upload_data)

        utime.sleep(3)
        if alarming:
            print("Info: stop alarm...")
            buzzer_alarm(False)
            alarming = False
            utime.sleep(2)

def alcohol_exit():
    adcObj.close()
    pwmObj.close()
    oled.spi.close()
    oled.res.close()
    oled.dc.close()

if __name__ == '__main__':
    # 如果想让酒精传感器有比较高的灵敏度，建议运行此模块前，先把酒精传感器上电预热24小时以上。
    alcohol_init()

    oledInit()

    get_wifi_status()

    connect_lk(productKey, deviceName, deviceSecret)

    upload_alcohol_detector_state()

    alcohol_exit()
