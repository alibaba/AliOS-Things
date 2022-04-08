#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import utime
import ujson
from aliyunIoT import Device
import modbus as mb
import yuanda_htb485
import zzio606
import network
from driver import GPIO

############# wifi连网代码 ############
def connect_wifi(ssid, pwd):
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pwd)

########## 4G.Cat1连网代码 ############
g_connect_status = False
def on_4g_cb(args):
    print("4g callback.")
    global g_connect_status
    pdp = args[0]
    netwk_sta = args[1]
    if netwk_sta == 1:
        g_connect_status = True
    else:
        g_connect_status = False

def connect_4g_network():
    global on_4g_cb
    net = network.NetWorkClient()
    net_led = GPIO()
    net_led.open('net_led')
    
    g_register_network = False
    if net._stagecode is not None and net._stagecode == 3 and net._subcode == 1:
        g_register_network = True
    else:
        g_register_network = False
        
    if g_register_network:
        net.on(1,on_4g_cb)
        net.connect({"apn":"CMNET"})
    else:
        print('network register failed')
        
    while True:
        if g_connect_status==False:
            net_led.write(0)
        if g_connect_status:
            print('network register successed')
            #连上网之后，点亮cloud_led
            net_led.write(1)
            break
        utime.sleep_ms(20)
    return 0

######## 物联网平台相关代码 ############
productKey = "gyd2HK4GEJQ"
deviceName = "azz6ldcavPDjR4Luvq4t"
deviceName = "5cbe23fe7f5995d639fa079187d0f645"
device = None
zzio606Obj = None
switchList = ["switch0", "switch1", "switch2", "switch3", "switch4", "switch5"]

# 物联网平台连接成功的回调函数
def on_connect(data):
    print("物联网平台连接成功")

# 处理物联网平台下行指令
def on_props(request):
    global alarm_on, device, zzio606Obj, switchList
    
    payload = ujson.loads(request['params'])    
    for i in range(0, len(switchList)):
        if switchList[i] in payload.keys():
            print(switchList[i])
            value = payload[switchList[i]]
            if (value):
                zzio606Obj.openChannel(i)
            else:
                zzio606Obj.closeChannel(i)
                
            prop = ujson.dumps({
                switchList[i]: value,
            })
            print(prop)
            upload_data = {'params': prop}
            device.postProps(upload_data)

# 连接物联网平台
def connect_lk(productKey, deviceName, deviceSecret):
    global device
    key_info = {
        'region': 'cn-shanghai',
        'productKey': productKey,
        'deviceName': deviceName,
        'deviceSecret': deviceSecret,
        'keepaliveSec': 60
    }
    device = Device()
    device.on(Device.ON_CONNECT, on_connect)
    device.on(Device.ON_PROPS, on_props)
    device.connect(key_info)

# 上传htb数据
def upload_htb_data():
    global device, zzio606Obj

    htb485Obj = yuanda_htb485.HTB485(mb, 1)
    zzio606Obj = zzio606.ZZIO606(mb, 3)

    while True:
        if htb485Obj is None:
            break      
        htb = htb485Obj.getHTB()
        # "humidity"/"temperature"/"brightness"必须和物联网平台的属性一致
        upload_data = {'params': ujson.dumps({
            'humidity': htb[0],
            'temperature': htb[1],
            "brightness":htb[3]
        })
        }
        # 上传温度和湿度信息到物联网平台
        device.postProps(upload_data)
        utime.sleep(2)
        
    mb.deinit()

if __name__ == '__main__':
    # connect_wifi('KIDS', '12345678')
    connect_4g_network()
    mb.init('modbus_485_4800')
    utime.sleep(2)
    connect_lk(productKey, deviceName, deviceSecret)
    utime.sleep(2)
    upload_htb_data()