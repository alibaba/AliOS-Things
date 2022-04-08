#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import utime
import ujson
from aliyunIoT import Device
import modbus as mb
import yuanda_htb485
import network
import ina219
from driver import GPIO
from driver import ADC
from driver import I2C

# 4G.Cat1连网代码
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
        # net.connect({"apn":"CMNET"})
        net.connect({'apn':'CMIOTNJSHA.JS'})
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


# 上传路灯信息
def upload_battery_info(device):
    mb.init('modbus_485_4800')
    htb485Obj = yuanda_htb485.HTB485(mb, 1)
    adc = ADC()
    adc.open("ADC1")
    
    i2cObj = I2C()
    i2cObj.open("ina219")
    ina219Dev = ina219.INA219(i2cObj)                                                                                                                                                                                                                                                                                                                                                                                                                                                                

    while True:
        brightness = htb485Obj.getBrightness()
        battery_voltage = adc.readVoltage() * 5 / 1000.0
        # current =  ina219Dev.current() / 1000.0
        current = 0.11
        # "brightness"/"battery_voltage"/"current"必须和物联网平台的属性一致
        upload_data = {'params': ujson.dumps({
            'brightness': brightness,
            "battery_voltage":battery_voltage,
            'current': current
        })
        }
        print(upload_data)
        # 上传温度和湿度信息到物联网平台
        device.postProps(upload_data)
        utime.sleep(5)
        
    mb.deinit()
    
# 物联网平台连接成功的回调函数
def on_connect(data):
    print("物联网平台连接成功")

# 处理物联网平台下行指令
def on_props(request):
    print("新消息：", request)

# 连接物联网平台
def connect_lk():
    key_info = {
        'region': 'cn-shanghai',
        'productKey': "xxx",
        'deviceName': "xxx",
        'deviceSecret': "xxx",
        'keepaliveSec': 60
    }
    device = Device()
    device.on(Device.ON_CONNECT, on_connect)
    device.on(Device.ON_PROPS, on_props)
    device.connect(key_info)
    return device

if __name__ == '__main__':
    connect_4g_network()
    utime.sleep(2)
    device = connect_lk()
    utime.sleep(2)
    upload_battery_info(device)
