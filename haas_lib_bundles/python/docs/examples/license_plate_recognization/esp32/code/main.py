#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    车牌识别案例
@Author     :    luokui.lk
@version    :    1.0
'''
import display      # 显示库
import uai          # AI识别库
import network
import ucamera      # 摄像头库
import utime        # 延时函数在utime库中
import oss          # 对象存储库
import sntp

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
    global disp
    # 初始化网络
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pwd)
    while True:
        print('wifi is connecting...')
        disp.text(20, 30, 'wifi is connecting...', disp.RED)
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
            sntp.setTime('UTC')
            print('NTP done')
            disp.textClear(20, 70, 'sntp start...')
            disp.text(20, 70, 'NTP done', disp.RED)
            break
        utime.sleep_ms(500)
    utime.sleep(2)

def main():
    # 创建lcd display对象
    global disp
    disp = display.TFT()
    disp.font(disp.FONT_Default)

    # 连接网络
    connect_wifi(SSID, PWD)

    # 初始化摄像头, UART Rx PIN=33, TX PIN=32，M5STACK-CORE2上对应PORTA
    ucamera.init('uart', 33, 32)

    # 创建AI对象
    ai = uai.AI(uai.AI_ENGINE_ALIYUN, ACCESS_KEY, ACCESS_SECRET)

    while True:
        # 抓取一帧图像
        frame = ucamera.capture()
        if (frame != None):
            # 显示图像
            disp.image(0, 0, frame, 0)
            # 设置显示字体
            disp.font(disp.FONT_DejaVu18)
            # 显示文字
            disp.text(10, 30, 'Recognizing face...', disp.WHITE)
            # 上传图片到OSS
            image = oss.uploadContent(ACCESS_KEY, ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET, frame, "oss/test.jpg")
            print('content: %s' %image)
            #image = 'http://viapi-test.oss-cn-shanghai.aliyuncs.com/viapi-3.0domepic/imagerecog/DetectFruits/DetectFruits1.jpg'
            if image != None:
                frame = ucamera.capture()
                disp.image(0, 0, frame, 0)
                disp.text(0, 30, 'Recognizing License Plate...', disp.WHITE)
                # 车牌识别
                resp = ai.recognizeLicensePlate(image)
                print(resp)
                if (resp != None):
                    i = 0
                    # 解析识别结果
                    while (i < len(resp)):
                        plateNumber = resp[i]['plateNumber']
                        plateType = resp[i]['plateType']
                        confidence = resp[i]['confidence']
                        plateTypeConfidence = resp[i]['plateTypeConfidence']
                        x = resp[i]['x']
                        y = resp[i]['y']
                        w = resp[i]['w']
                        h = resp[i]['h']
                        # 当置信度 > 0.6，表示识别成功
                        if confidence > 0.6:
                            # 清除屏幕内容
                            disp.clear()
                            # 设置文字字体
                            disp.font(disp.FONT_DejaVu40)
                            # 显示识别的车牌号
                            disp.text(25, 90, plateNumber, disp.RED)
                            disp.text(25, 130, 'Deteted!!!', disp.RED)
                            utime.sleep(2)
                        else:
                            print('Recognizing license plate...')
                        i += 1
                else:
                    print('Recognizing license plate...')
            # 释放当前帧内存
            del frame

if __name__ == '__main__':
    main()
