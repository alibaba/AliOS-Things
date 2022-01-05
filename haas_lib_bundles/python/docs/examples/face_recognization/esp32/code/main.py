#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    人脸识别案例
@Author     :    luokui.lk
@version    :    1.0
'''
import display      # 显示库
import uai          # AI识别库
import network
import ucamera      # 摄像头库
import utime        # 延时函数在utime库中
import oss          # 对象存储库
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

# 拍照函数
def takePhoto():
    disp.font(disp.FONT_DejaVu40)
    disp.clear()
    disp.text(30, 110, 'Pose Please!', disp.WHITE)
    utime.sleep(2)
    disp.clear()
    disp.text(40, 110, 'Ready GO!', disp.WHITE)
    utime.sleep(1)
    disp.clear()
    i = 0
    while (i < 15):
        frame = ucamera.capture()
        if frame != None:
            disp.image(0, 0, frame, 0)
            if (i < 5):
                disp.text(150, 110, '3', disp.RED)
            elif (i > 5 and i < 10):
                disp.text(150, 110, '2', disp.RED)
            elif (i > 10 and i < 15):
                disp.text(150, 110, '1', disp.RED)
        i += 1
    utime.sleep(2)
    disp.clear()
    disp.text(60, 110, 'KaCha...', disp.WHITE)
    # 上传拍摄的照片
    image = oss.uploadContent(ACCESS_KEY, ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET, frame, "oss/face.jpg")
    utime.sleep(2)
    # 清除屏幕内容
    disp.clear()
    disp.text(20, 110, 'Upload Done!', disp.WHITE)
    utime.sleep(2)

    return image

def main():
    # 创建lcd display对象
    global disp
    disp = display.TFT()
    disp.font(disp.FONT_Default)

    # 连接网络
    connect_wifi(SSID, PWD)

    # 初始化摄像头, UART Rx PIN=33, TX PIN=32，M5STACK-CORE2上对应PORTA
    ucamera.init('uart', 33, 32)

    # 拍照上传人脸比对图片到OSS
    compareFaceImage = takePhoto()
    if (compareFaceImage == None):
        print('take photo fail, please restart!')
        disp.text(10, 110, 'Take Photo Fail, restart!', disp.WHITE)
        return

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
            if image != None:
                # 抓取一帧图像
                frame = ucamera.capture()
                # 显示图像
                disp.image(0, 0, frame, 0)
                # 显示文字
                disp.text(10, 30, 'Recognizing face...', disp.WHITE)
                # 人脸识别
                resp = ai.compareFace(image, compareFaceImage)
                print(resp)
                if (resp != None):
                    # 解析识别结果
                    confidence = resp['confidence']
                    x = resp['x']
                    y = resp['y']
                    w = resp['w']
                    h = resp['h']
                    # 置信度 > 60，表示识别成功
                    if confidence > 60:
                        # 清除屏幕内容
                        disp.clear()
                        # 设置文字字体
                        disp.font(disp.FONT_DejaVu40)
                        # 显示识别结果
                        disp.text(40, 90, 'Recognize', disp.RED)
                        disp.text(30, 130, 'successfully!', disp.RED)
                        print('Recognize successfully!!!')
                        utime.sleep(2)
                # 释放当前帧内存
                del frame

if __name__ == '__main__':
    main()
