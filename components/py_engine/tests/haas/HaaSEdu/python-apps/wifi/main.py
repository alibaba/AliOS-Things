#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    ConnectWifi.py
@Description:    file description   
@Date       :    2021/04/22 14:48:16
@Author     :    guoliang.wgl
@version    :    1.0
'''
import network
import utime as time
import sys


net = network.NetWorkClient()
connected = net.getStatus()

def on_wifi_connected(status):
    global connected
    print('*******wifi connected*********')
    connected = 5

if  connected != 5 :
    net.on(1,on_wifi_connected)
    if(len(sys.argv) == 3):
        net.connect({'ssid':sys.argv[1],'password':sys.argv[2]})
    else:
        net.connect("KIDS","12345678")

while True :
    if connected == 5:
        break
    else:
        print('Wait for wifi connected')
        time.sleep(1)

if net.getStatus() == 5 :
    print('DeviceIP:' + net.getInfo()['IP'])
else:
    print('DeviceIP:get failed')

print("ConnectWifi finished")
