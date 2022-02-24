#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    车牌识别案例
@Author     :    luokui
@version    :    1.0
'''

import display      # 显示库
import uai          # AI识别库
import network      # 网络库
import ucamera      # 摄像头库
import utime        # 延时函数在utime库中
import oss          # 对象存储库
import _thread      # 线程库
import sntp         # 网络时间同步库

# 阿里云访问账号
ACCESS_KEY = 'Your-Access-Key'
ACCESS_SECRET = 'Your-Access-Secret'

# OSS对象存储配置
OSS_ENDPOINT = "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET = "Your-OSS-Bucket"

# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'

# 网络连接函数
def connect_wifi(ssid, pwd):
    # 引用全局变量
    global disp
    # 初始化网络
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pwd)
    while True:
        print('wifi is connecting...')
        # 显示网络连接中
        disp.text(20, 30, 'wifi is connecting...', disp.RED)
        # 网络连接成功后，更新显示字符
        if (wlan.isconnected() == True):
            print('wifi is connected')
            disp.textClear(20, 30, 'wifi is connecting...')
            disp.text(20, 30, 'wifi is connected', disp.RED)
            ip = wlan.ifconfig()[0]
            print('IP: %s' %ip)
            disp.text(20, 50, ip, disp.RED)

            # NTP时间更新，如果更新不成功，将不能进行识别
            print('NTP start')
            disp.text(20, 70, 'NTP start...', disp.RED)
            sntp.setTime()
            print('NTP done')
            disp.textClear(20, 70, 'sntp start...')
            disp.text(20, 70, 'NTP done', disp.RED)
            break
        utime.sleep_ms(500)
    utime.sleep(2)

def recognizePlateLicenceThread():
    # 引用全局变量
    global detected, plateNumber
    # 创建AI对象
    ai = uai.AI(uai.AI_ENGINE_ALIYUN, ACCESS_KEY, ACCESS_SECRET)

    while True:
        if frame != None:
            # 上传图片到OSS
            image = oss.uploadContent(ACCESS_KEY, ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET, frame, "oss/test.jpg")
            if image != None:
                # 进行车牌识别
                resp = ai.recognizeLicensePlate(image)
                # 解析识别结果
                if (resp != None):
                    i = 0
                    while (i < len(resp)):
                        print(resp)
                        plateNumber = resp[i]['plateNumber']
                        plateType = resp[i]['plateType']
                        confidence = resp[i]['confidence']
                        plateTypeConfidence = resp[i]['plateTypeConfidence']
                        x = resp[i]['x']
                        y = resp[i]['y']
                        w = resp[i]['w']
                        h = resp[i]['h']
                        # 当得分 > 0.6，表示识别成功
                        if confidence > 0.6:
                            detected = True
                            # 休眠识别线程，切换到显示线程
                            utime.sleep(2)
                        else:
                            detected = False
                        i += 1
                else:
                    detected = False

# 显示线程函数
def displayThread():
    # 引用全局变量
    global disp, frame, detected, plateNumber
    # 定义清屏局部变量
    clearFlag = False
    # 定义显示文本局部变量
    textShowFlag = False
    while True:
        # 采集摄像头画面
        frame = ucamera.capture()
        if frame != None:
            if detected == True:
                # 清除屏幕内容
                if clearFlag == False:
                    disp.clear()
                    clearFlag = True
                # 设置文字字体
                disp.font(disp.FONT_DejaVu40)
                # 显示识别的车牌号
                disp.text(25, 90, plateNumber, disp.RED)
                disp.text(25, 130, 'Recognized!!!', disp.RED)
                print('License Plate Recognized!!!')
                textShowFlag = False
            else:
                # 显示图像
                disp.image(0, 20, frame, 0)
                if textShowFlag == False:
                    # 设置显示字体
                    disp.font(disp.FONT_DejaVu18)
                    # 显示文字
                    disp.text(2, 0, 'Recognizing License Plate...', disp.WHITE)
                    textShowFlag = True
                clearFlag = False

def main():
    # 全局变量
    global disp, frame, detected, plateNumber
    frame = None
    detected = False
    plateNumber = None

    # 创建lcd display对象
    disp = display.TFT()

    # 连接网络
    connect_wifi(SSID, PWD)

    # 初始化摄像头
    ucamera.init('uart', 33, 32)
    ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

    try:
        # 启动显示线程
        _thread.start_new_thread(displayThread, ())
        # 设置车牌识别线程stack
        _thread.stack_size(15 * 1024)
        # 启动车牌识别线程
        _thread.start_new_thread(recognizePlateLicenceThread, ())
    except:
       print("Error: unable to start thread")

    # 主线程IDLE
    while True:
        utime.sleep_ms(1000)

if __name__ == '__main__':
    main()
