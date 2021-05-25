#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    ConnectWifi.py
@Description:    file description   
@Date       :    2021/04/22 14:48:16
@Author     :    guoliang.wgl
@version    :    1.0
'''
import netmgr as nm
import utime as time
import sys

nm.init()
connected = nm.getStatus()

def on_wifi_connected(status):
    global connected
    print('*******wifi connected*********')
    connected = True

if  not connected:
    nm.register_call_back(1,on_wifi_connected)
    if(len(sys.argv) == 3):
        nm.connect(sys.argv[1],sys.argv[2])
    else:
        nm.connect("KIDS","12345678")

while True :
    if connected:
        break
    else:
        print('Wait for wifi connected')
        time.sleep(1)

if nm.getStatus():
    print('DeviceIP:' + nm.getInfo()['IP'])
else:
    print('DeviceIP:get failed')

print("ConnectWifi finished")
