#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    人脸识别案例
@Author     :    luokui
@version    :    1.0
'''

from aliyunIoT import Device
import display      # 显示库
import uai          # AI识别库
import network      # 网络库
import ucamera      # 摄像头库
import utime        # 延时函数在utime库中
import _thread      # 线程库
import sntp         # 网络时间同步库
import ujson as json

# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'

# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"

g_lk_connect = False
g_lk_service = False
g_confidence = 0

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

def connect_wifi(ssid, pwd):
    # 引用全局变量
    global disp
    # 初始化网络
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pwd)
    while True:
        print('Wi-Fi is connecting...')
        # 显示网络连接中
        disp.text(20, 30, 'Wi-Fi is connecting...', disp.RED)
        # 网络连接成功后，更新显示字符
        if (wlan.isconnected() == True):
            print('Wi-Fi is connected')
            disp.textClear(20, 30, 'Wi-Fi is connecting...')
            disp.text(20, 30, 'Wi-Fi is connected', disp.RED)
            ip = wlan.ifconfig()[0]
            print('IP: %s' %ip)
            disp.text(20, 50, ip, disp.RED)

            # NTP时间更新，如果更新不成功，将不能进行识别
            print('NTP start')
            disp.text(20, 70, 'NTP start...', disp.RED)
            sntp.setTime()
            print('NTP done')
            disp.textClear(20, 70, 'NTP start...')
            disp.text(20, 70, 'NTP done', disp.RED)
            break
        utime.sleep_ms(500)
    utime.sleep(2)

def cb_lk_service(data):
    global g_lk_service, g_confidence, detected
    # dev.publish(compare_reply)
    # print(data)
    #resp = json.loads(data)
    if data != None:
        params = data['params']
        params_dict = json.loads(params)
        # print(params_dict)
        ext = params_dict['ext']
        ext_dict = json.loads(ext)
        result = ext_dict['result']
        if result == 'success':
            g_confidence = ext_dict['confidence']
            if g_confidence > 60:
                print('boss is comming!')
                detected = True
            else:
                # print('boss is not comming!')
                detected = False
        else:
            # print('boss is not comming!')
            detected = False

    g_lk_service = True
    # print('cb_lk_service done')

def cb_lk_connect(data):
    global g_lk_connect
    print('link platform connected')
    g_lk_connect = True

def compareFace(frame, fileName, productKey, devName, devSecret):
    start = utime.ticks_ms()
    global dev
    # 上传图片到LP
    fileid = dev.uploadContent(fileName, frame, None)
    # end = utime.ticks_ms()
    # print('uploadContent time : %d' %(end - start))
    if fileid != None:
        # start = utime.ticks_ms()
        ext = { 'filePosition':'lp', 'fileName': fileName, 'fileId': fileid }
        ext_str = json.dumps(ext)
        all_params = {'id': 1, 'version': '1.0', 'params': { 'eventType': 'haas.faas', 'eventName': 'compareFace', 'argInt': 1, 'ext': ext_str }}
        all_params_str = json.dumps(all_params)
        #print(all_params_str)
        upload_file = {
            'topic': '/sys/' + productKey + '/' + deviceName + '/thing/event/hli_event/post',
            'qos': 1,
            'payload': all_params_str
        }
        time_diff = utime.ticks_diff(utime.ticks_ms(), start)
        print('build str time : %d' % time_diff)

        start1 = utime.ticks_ms()
        # 上传完成通知HaaS聚合平台
        # print(upload_file)
        dev.publish(upload_file)
        # end = utime.ticks_ms()
        # print('publish time : %d' %(end - start))
        # start = utime.ticks_ms()
        while g_lk_service == False:
            continue
        time_diff = utime.ticks_diff(utime.ticks_ms(), start1)
        print('pub time : %d' % time_diff)
    else:
        print('filedid is none, upload content fail')
    time_diff = utime.ticks_diff(utime.ticks_ms(), start)
    print('get response time : %d' % time_diff)

# 人脸比较线程函数
def comapreFaceThread():
    global frame
    while True:
        if frame != None:
            compareFace(frame, 'face.jpg', productKey, deviceName, deviceSecret)
            utime.sleep_ms(1000)
        else:
            utime.sleep_ms(1000)

# 显示线程函数
def displayThread():
    # 引用全局变量
    global disp, frame, detected
    # 定义清屏局部变量
    clearFlag = False
    # 定义显示文本局部变量
    textShowFlag = False
    while True:
        # 采集摄像头画面
        # print('start to capture')
        frame = ucamera.capture()
        # print('end to capture')
        if frame != None:
            if detected == True:
                # 清除屏幕内容
                if clearFlag == False:
                    disp.clear()
                    clearFlag = True
                # 设置文字字体
                disp.font(disp.FONT_DejaVu40)
                # 显示识别结果
                disp.text(40, 90, 'Recognize', disp.RED)
                disp.text(30, 130, 'successfully!', disp.RED)
                print('Recognize successfully!!!')
                textShowFlag = False
            else:
                # 显示图像
                # print('start to display')
                disp.image(0, 20, frame, 0)
                utime.sleep_ms(100)
                if textShowFlag == False:
                    # 设置显示字体
                    disp.font(disp.FONT_DejaVu18)
                    # 显示文字
                    disp.text(2, 0, 'Recognizing face...', disp.WHITE)
                    textShowFlag = True
                clearFlag = False

def main():
    # 全局变量
    global disp, dev, frame, detected
    # 创建lcd display对象
    disp = display.TFT()
    frame = None
    detected = False

    # 连接网络
    connect_wifi(SSID, PWD)

    # 设备初始化
    dev = Device()

    dev.on(Device.ON_CONNECT, cb_lk_connect)
    dev.on(Device.ON_SERVICE, cb_lk_service)
    # dev.subscribe(compare_init)
    # dev.unsubscribe(compare_init)
    dev.connect(key_info)

    while True:
        if g_lk_connect:
            break

    # 初始化摄像头
    ucamera.init('uart', 33, 32)
    ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

    try:
        # 启动显示线程
        _thread.start_new_thread(displayThread, ())
        # 人脸基准照片上传成功后，启动人脸比对线程
        # 设置人脸比对线程stack
        _thread.stack_size(20 * 1024)
        # 启动人脸比对线程
        _thread.start_new_thread(comapreFaceThread, ())
    except:
       print("Error: unable to start thread")

    while True:
        utime.sleep_ms(1000)

if __name__ == '__main__':
    main()
