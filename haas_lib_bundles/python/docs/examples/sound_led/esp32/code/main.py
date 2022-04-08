#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    杭漂
@version    :    1.0
@Description:    声控灯案例
'''


from aliyunIoT import Device
import network      # 网络库
import utime        # 延时函数在utime库中
import sntp         # 网络时间同步库
import _thread      # 线程库
import utime        # 延时函数在utime库中
import ujson as json
from driver import ADC
from driver import PWM

gLpConnect = False
gDebugLogFlag = True
gNumber = 0
gOpenLed = False
gDecibelValue = 1520

# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

class RgbLed():
    def __init__(self, pwmRObj, pwmGObj, pwmBObj):
        if not isinstance(pwmRObj, PWM):
            raise ValueError("parameter pwmRObj is not an PWM object")
        if not isinstance(pwmGObj, PWM):
            raise ValueError("parameter pwmGObj is not an PWM object")
        if not isinstance(pwmBObj, PWM):
            raise ValueError("parameter pwmBObj is not an PWM object")
        self.pwmR = pwmRObj
        self.pwmG = pwmGObj
        self.pwmB = pwmBObj
        self.pwmR.setOption({'freq': 255, 'duty': 0})
        self.pwmG.setOption({'freq': 255, 'duty': 0})
        self.pwmB.setOption({'freq': 255, 'duty': 0})

    def setColor(self, color):
        if isinstance(color, tuple) and len(color) is 3:
            red = color[0]
            green = color[1]
            blue = color[2]
            self.pwmR.setOption({'freq': 255, 'duty': red})
            self.pwmG.setOption({'freq': 255, 'duty': green})
            self.pwmB.setOption({'freq': 255, 'duty': blue})
        elif isinstance(color, int) and color >= 0 and color <= 0xffffff:
            red = color >> 12
            green = (color >> 8) & 0x00ff
            blue = color & 0x0000ff
            self.pwmR.setOption({'freq': 255, 'duty': red})
            self.pwmG.setOption({'freq': 255, 'duty': green})
            self.pwmB.setOption({'freq': 255, 'duty': blue})
        else:
            raise ValueError("color type error! color should be like (255, 255, 255) or 0xffffff")

def connect_wifi(ssid, pwd):
    # 初始化网络
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pwd)
    while True:
        print('Wi-Fi is connecting...')
        # 网络连接成功后，更新显示字符
        if (wlan.isconnected() == True):
            print('Wi-Fi is connected')
            ip = wlan.ifconfig()[0]
            print('IP: %s' %ip)
            # NTP时间更新，如果更新不成功，将不能进行识别
            print('NTP start')
            sntp.setTime()
            print('NTP done')
            break
        utime.sleep_ms(500)
    utime.sleep(2)

def cb_lk_service(data):
    print('handling sub data')
    
def cb_lk_connect(data):
    global gLpConnect
    print('link platform connected')
    gLpConnect = True

# 获取灯的颜色
def set_color_reference():
    global gLedColor,gNumber
    if gNumber == 0:
        # 白色
        ledColor = 0xffffff
    elif gNumber == 1:
        # 红色
        ledColor = 0xff0000
    elif gNumber == 2:
        # 蓝色
        ledColor = 0x0000ff
    else:
        # 绿色
        ledColor = 0x00ff00
    gLedColor = ledColor
    gNumber += 1
    gNumber %= 4
    return gLedColor

# 获取当前声音转换值
def get_decibel():
    voltage = gAdcObj.readVoltage()
    return voltage

# 属性上报
def lp_post_props(color):
    global gDev
    # 生成上报到物联网平台的属性值字串，此处的属性标识符"ledStatus"必须和物联网平台的属性一致
    value = {'ledStatus': color}
    data = {'params': json.dumps(value)}
    # 上传开关灯信息到物联网平台
    ret = gDev.postProps(data)
    if ret == 0 :
        print('属性上报成功')
    else :
        print('属性上报失败')

# 开启Led灯
def led_open(color):
    global gLed
    print('开灯')
    gLed.setColor(color)
    lp_post_props(color)

# 关闭Led灯
def led_close():
    global gLed,gLedColor
    print('关灯')
    gLedColor = 0x000000
    gLed.setColor(gLedColor)
    lp_post_props(gLedColor)

# 声音采集线程
def sound_thread():
    global gOpenLed
    print('启动声音采集线程')
    decibelValue = 0
    while True:
        # 采集声音
        decibelValue = get_decibel()
        # 判断分贝
        if decibelValue > gDecibelValue and gOpenLed == False:
            print("声波转换后的值%s"% (decibelValue))
            gOpenLed = True
            # 开灯
            led_open(set_color_reference())
            # 亮灯6秒
            utime.sleep_ms(6000)
            # 关灯
            led_close()
            gOpenLed = False
        # 监听获取声音的频率，间隔不能太长，延迟大会导致不精确
        utime.sleep_ms(10)

def main():
    global gAdcObj,gLed,gLedColor,gDev

    # 连接网络
    connect_wifi(SSID, PWD)
    # 设备初始化
    gDev = Device()
    gDev.on(Device.ON_CONNECT, cb_lk_connect)
    gDev.on(Device.ON_SERVICE, cb_lk_service)
    gDev.connect(key_info)
    while True:
        if gLpConnect:
            break
    # 创建ADC对象
    gAdcObj = ADC()
    # 开启声音传感器
    ret = gAdcObj.open('hw181_adc')
    if ret != 0 :
        print("错误：声音传感器开启失败！")
        print('%s' % ret)
    # 开启RGB
    rgbR = PWM()
    rgbR.open("rgbR")
    rgbG = PWM()
    rgbG.open("rgbG")
    rgbB = PWM()
    rgbB.open("rgbB")
    gLed = RgbLed(rgbR, rgbG, rgbB)

    try:
        # 启动声音采集线程
        _thread.start_new_thread(sound_thread, ())
    except:
       print("Error: unable to start thread")
    while True:
        utime.sleep_ms(1000)

if __name__ == '__main__':
    main()
