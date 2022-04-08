#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import utime
import ujson
from aliyunIoT import Device
import modbus as mb
import yuanda_htb485
import network
from driver import GPIO
from driver import ADC

# 4G.Cat1连网代码
g_connect_status = False
def on_4g_cb(args):
    print("4g callback.")
    global g_connect_status
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

power_type_switch = GPIO()
power_switch = GPIO()
work_mode = 0

# 上传路灯信息
def upload_lamp_info(device):
    global power_type_switch, power_switch, work_mode
    mb.init('modbus_485_4800')
    htb485Obj = yuanda_htb485.HTB485(mb, 1)
    power_type_switch.open("electric_relay1_1")
    power_switch.open("electric_relay1_2")
    adc = ADC()
    adc.open("ADC1")

    dc_time = 0
    bat_time = 0
    while True:
        brightness = htb485Obj.getBrightness()
        battery_voltage = adc.readVoltage() * 5 / 1000.0
        lamp_status = power_switch.read()
        power_type = power_type_switch.read()
        if (brightness < 600 and work_mode == 0):                
            if (battery_voltage < 11.0):
                power_type = 0
                power_type_switch.write(0)
                dc_time += 1
            else:
                power_type = 1
                power_type_switch.write(1)
                bat_time += 1
                
            power_switch.write(1)
            lamp_status = 1
            
        if (brightness > 700 and work_mode == 0):
            power_switch.write(0)
            lamp_status = 0

        # "brightness"/"lamp_state"/"battery_voltage/power_type/dc_time/bat_time"必须和物联网平台的属性一致
        upload_data = {'params': ujson.dumps({
            'brightness': brightness,
            "lamp_state": lamp_status,
            "battery_voltage":battery_voltage,
            'power_type': power_type,
            "dc_time": dc_time,
            "bat_time": bat_time,
            "work_mode": work_mode
        })
        }
        print(upload_data)
        # 上传温度和湿度信息到物联网平台
        device.postProps(upload_data)
        utime.sleep(2)
        
    mb.deinit()
    
# 物联网平台连接成功的回调函数
def on_connect(data):
    print("物联网平台连接成功")

# 处理物联网平台下行指令
cmd_name_list = ["lamp_state", "power_type", "work_mode"]
def on_props(request):
    global power_type_switch, power_switch, work_mode
    payload = ujson.loads(request['params'])    
    for i in range(0, len(cmd_name_list)):
        if cmd_name_list[i] in payload.keys():
            value = payload[cmd_name_list[i]]
            if cmd_name_list[i] is 'work_mode':
                work_mode = value
            else:
                work_mode = 1
            
            if cmd_name_list[i] is 'lamp_state':
                power_switch.write(value)
            
            if cmd_name_list[i] is 'power_type':
                power_type_switch.write(value)

            prop = ujson.dumps({
                cmd_name_list[i]: value,
            })
            upload_data = {'params': prop}
            device.postProps(upload_data)

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
    upload_lamp_info(device)