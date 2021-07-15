#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    haas_speech.py
@Description:    file description
@Date       :    2021/04/19 13:56:50
@Author     :    
@version    :    1.0
'''
import iot
import utime
import http
import ujson as json
from speech_utils import *
# from const import *

# 语音播放相关的音频资源文件定义
resDir = "/data/python-apps/speech/resource/"
tonepathConnected = "fs:/data/python-apps/speech/resource/connected.wav"
tonepathPowerOn = "fs:/data/python-apps/speech/resource/poweron.wav"

# 物联网平台相关的key和serect定义
productSecret = "XXXXXX"
productKey = "XXXXXX"
deviceName  =    "XXXXXX"
deviceSecret  =   "XXXXXX"

on_request = False
on_play = False

def do_connect_lk():
   """
   @description  : 通过物联网平台连接千里传音服务
   ---------
   @param        : 空
   -------
   @Returns      : 空
   -------
   """
   # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
   global productSecret, productKey, deviceName, deviceSecret ,on_request, on_play
   # 初始化linkkit sdk
   key_info = {
      'region' : 'cn-shanghai' ,
      'productKey': productKey ,
      'deviceName': deviceName ,
      'deviceSecret': deviceSecret ,
      'productSecret': productSecret
   }
   device = iot.Device(key_info)
   # 物联网平台连接成功的回调函数
   def on_connect():
      global player, tonepathConnected
      print('linkkit is connected ')
      play(tonepathConnected)
   device.on('connect',on_connect)
   # 设置service 事件接收函数（本案例是千里传音）
   def on_service(data):
      global on_request, on_play
      import json
      print('****** on service ********')
      print(data)
      print(data['id'])
      print(data['param'])
      serviceid = data['id']
      data = json.loads(data['param'])
      if serviceid == "SpeechPost":
            on_request = data
      elif serviceid == "SpeechBroadcast":
            on_play = data
      else:
            pass
   device.on('service',on_service)
   # 连接物联网平台
   device.connect()
   # 触发linkit sdk持续处理server端信息
   while(True):
      device.do_yield(1000)
      if  on_request:
            download_resource_file(on_request,resDir)
            on_request = False
      elif on_play:
            play_voice(on_play,resDir)
            on_play = False
   # 断开连接
   device.close()
   player.stop()
   player.release()



if __name__ == '__main__':
   init_audio()
   play(tonepathPowerOn)
   do_connect_lk()