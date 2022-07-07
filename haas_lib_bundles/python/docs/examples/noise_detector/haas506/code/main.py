# coding=utf-8
from driver import ADC
from driver import GPIO
from driver import PWM
import network
import ujson
import utime as time
import modem
from  aliyunIoT import Device
import kv
import math
import _thread

#当iot设备连接到物联网平台的时候触发'connect' 事件
def on_connect(data):
    global module_name,default_ver,productKey,deviceName,deviceSecret,on_trigger,on_download,on_verify,on_upgrade
    print('***** connect lp succeed****')
    data_handle = {}
    data_handle['device_handle'] = device.getDeviceHandle()


#当连接断开时，触发'disconnect'事件
def on_disconnect():
    print('linkkit is disconnected')

red = 0
green = 0
blue = 0
bu = 0
#当iot云端下发属性设置时，触发'props'事件
def on_props(request):
    global switch_buzzers,switch_red,switch_green,switch_blue,red,green,blue,bu
    params=request['params']
    params=eval(params)
    if "buzzer" in params:
        switch_buzzers = params["buzzer"]
        if switch_buzzers and switch_buzzers != bu:
            print("buzzer - on")
            bu = switch_buzzers
    if "red_led" in params:
        switch_red = params["red_led"]
        if switch_red and switch_red != red:
            print("red_led - on")
            red = switch_red
    if "green_led" in params:
        switch_green = params["green_led"]
        if switch_green and switch_green != green:
            print("green_led - on")
            green = switch_green
    if "blue_led" in params:
        switch_blue = params["blue_led"]
        if switch_blue and switch_blue != blue:
            print("blue_led - on")
            blue = switch_blue
    params.clear()

#当iot云端调用设备service时，触发'service'事件
def on_service(id,request):
    print('clound req id  is {} , req is {}'.format(id,request))
#当设备跟iot平台通信过程中遇到错误时，触发'error'事件
def on_error(err):
    print('err msg is {} '.format(err))

#网络连接的回调函数
def on_4g_cb(args):
     global g_connect_status
     pdp = args[0]
     netwk_sta = args[1]
     if netwk_sta == 1:
         g_connect_status = True
     else:
         g_connect_status = False

#网络连接
def connect_network():
     global net,on_4g_cb,g_connect_status
     #NetWorkClient该类是一个单例类，实现网络管理相关的功能，包括初始化，联网，状态信息等.
     net = network.NetWorkClient()
     g_register_network = False
     if net._stagecode is not None and net._stagecode == 3 and net._subcode == 1:
         g_register_network = True
     else:
         g_register_network = False
     if g_register_network:
    #注册网络连接的回调函数on(self,id,func);  1代表连接，func 回调函数  ；return 0 成功
         net.on(1,on_4g_cb)    
         net.connect(None)
     else:
         print('网络注册失败')
     while True:
         if g_connect_status:
             print('网络连接成功')
             break
         time.sleep_ms(20)

#动态注册回调函数
def on_dynreg_cb(data):
     global deviceSecret,device_dyn_resigter_succed
     deviceSecret = data
     device_dyn_resigter_succed = True


 # 连接物联网平台
def dyn_register_device(productKey,productSecret,deviceName):
    global on_dynreg_cb,device,deviceSecret,device_dyn_resigter_succed
    key = '_amp_customer_devicesecret'
    deviceSecretdict = kv.get(key)
    print("deviceSecretdict:",deviceSecretdict)
    if isinstance(deviceSecretdict,str):    
        deviceSecret = deviceSecretdict 

    if deviceSecretdict is None or deviceSecret is None:
        key_info = {
            'productKey': productKey  ,
            'productSecret': productSecret ,
            'deviceName': deviceName
            }
        # 动态注册一个设备，获取设备的deviceSecret
        #下面的if防止多次注册，当前若是注册过一次了，重启设备再次注册就会卡住，
        if not device_dyn_resigter_succed:
            device.register(key_info,on_dynreg_cb)   


volume_data = {}
def upload_volume(n):
    global volume_data
    volume_data["sound"]= n
    volume_data_str=ujson.dumps(volume_data)
    data={
        'params':volume_data_str
        }      
    device.postProps(data)

warn_data = {}
def upload_warn():
    global warn_data
    warn_data["buzzer"]= 0
    warn_data["red_led"]= 0
    warn_data["green_led"]= 0
    warn_data["blue_led"]= 0
    warn_data_str=ujson.dumps(warn_data)
    data={
        'params':warn_data_str
        }      
    device.postProps(data)
    data.clear()

#蜂鸣器
buzzers = PWM()
buzzers.open("buzzer")
switch_buzzers = 0
buzzer = 0
def start_buzzers():
    global switch_buzzers,buzzer
    if switch_buzzers == 1 and buzzer == 0:
        param = {'freq':3000,'duty':50}
        buzzers.setOption(param)
        buzzer = 1
    if switch_buzzers == 0 and buzzer == 1:
        param = {'freq':3000,'duty':100}
        buzzers.setOption(param)
        buzzer = 0


#红灯
led_red = GPIO()
led_red.open('red')
switch_red = 0
def red_led():
    global switch_red
    led_red.write(switch_red)

#绿灯
led_green = GPIO()
led_green.open('green')
switch_green = 0
def green_led():
    global switch_green
    led_green.write(switch_green)

#蓝灯
led_blue = GPIO()
led_blue.open('blue')
switch_blue = 0
def blue_led():
    global switch_blue
    led_blue.write(switch_blue)


def warning():
    global switch_blue,switch_green,switch_red,switch_buzzers
    while True:
        time.sleep_ms(200)
        start_buzzers()
        red_led()
        green_led()
        blue_led()


if __name__ == '__main__':
    ICCID=None
    g_connect_status = False
    net = None
    device = None
    deviceSecret = None
    deviceName = None
    #替换下列产品信息
    ###################################
    productKey = "your-productKey"
    productSecret = "your-productSecret"
    ###################################
    device_dyn_resigter_succed = False

    # 连接网络
    connect_network()
     # 获取设备的IMEI 作为deviceName 进行动态注册
    deviceName = modem.getDevImei()
    #获取设备的ICCID
    ICCID=modem.sim.getIccid()
    #初始化物联网平台Device类，获取device实例
    device = Device()
    if deviceName is not None and len(deviceName) > 0 :
     #动态注册一个设备
        dyn_register_device(productKey,productSecret,deviceName)
    else:
        print("获取设备IMEI失败，无法进行动态注册")
    while deviceSecret is None:
        time.sleep(0.2)
    print('动态注册成功:' + deviceSecret)

    key_info = {
        'region' : 'cn-shanghai' ,
        'productKey': productKey ,
        'deviceName': deviceName ,
        'deviceSecret': deviceSecret ,
        'keepaliveSec': 60,
        }
    #打印设备信息
    print(key_info)

    #device.ON_CONNECT 是事件，on_connect是事件处理函数/回调函数
    device.on(device.ON_CONNECT,on_connect)
    device.on(device.ON_DISCONNECT,on_disconnect)
    device.on(device.ON_PROPS,on_props)
    device.on(device.ON_SERVICE,on_service)
    device.on(device.ON_ERROR,on_error)
    device.connect(key_info)

    #主程序
    time.sleep(2)
    # 检测光照
    adc=ADC()
    adc.open("ADC1")
    upload_warn()
    # 创建警报线程
    _thread.start_new_thread(warning, ())
    while True:
        volume=adc.readVoltage()
        DB = 20*(math.log((volume/0.775), 10))
        upload_volume(DB)
        print(DB)
        time.sleep_ms(500)