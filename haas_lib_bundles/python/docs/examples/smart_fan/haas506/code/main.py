# coding=utf-8
from ahtx0 import AHT10
from driver import PWM
import network
import ujson
import utime as time
import modem
from  aliyunIoT import Device
import kv


# 警报开关以及时间段控制
gear1_temp = 22
gear2_temp = 27
gear3_temp = 32
FLAG_CUR_TEMP = "temp"
FLAG_GEAR1 = "gear1"
FLAG_GEAR2 = "gear2"
FLAG_GEAR3 = "gear3"
cur_gear = 0


#当iot设备连接到物联网平台的时候触发'connect' 事件
def on_connect(data):
    global module_name,default_ver,productKey,deviceName,deviceSecret,on_trigger,on_download,on_verify,on_upgrade
    print('***** connect lp succeed****')
    data_handle = {}
    data_handle['device_handle'] = device.getDeviceHandle()


#当连接断开时，触发'disconnect'事件
def on_disconnect():
    print('linkkit is disconnected')


#当iot云端下发属性设置时，触发'props'事件
def on_props(request):
    global FLAG_GEAR1, FLAG_GEAR2, FLAG_GEAR3,FLAG_CUR_TEMP,gear1_temp, gear2_temp, gear3_temp,temp
    params=request['params']
    data_all=eval(params)
    if FLAG_GEAR1 in data_all :
        gear1_temp = data_all[FLAG_GEAR1]
        value_gears()
    if FLAG_GEAR2 in data_all :
        gear2_temp = data_all[FLAG_GEAR2]
        value_gears()  
    if FLAG_GEAR3 in data_all :
        gear3_temp = data_all[FLAG_GEAR3]
        value_gears()            
    if FLAG_CUR_TEMP in data_all:
        temp = data_all[FLAG_CUR_TEMP]
        upload_temp()

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

def upload_date(key_value):
    data_str=ujson.dumps(key_value)
    data={
        'params':data_str
        }      
    device.postProps(data)
    value_data.clear()


value_data = {}
def upload_temp():
    global value_data,temp,FLAG_CUR_TEMP
    value_data[FLAG_CUR_TEMP]=temp
    upload_date(value_data)
    
def value_gears():
    global FLAG_GEAR1,FLAG_GEAR2,FLAG_GEAR3,gear1_temp, gear2_temp, gear3_temp,value_data
    value_data[FLAG_GEAR1]=gear1_temp
    upload_date(value_data)
    value_data[FLAG_GEAR2]=gear2_temp
    upload_date(value_data)
    value_data[FLAG_GEAR3]=gear3_temp
    upload_date(value_data)

#电机控制
def control(gear):
    if not isinstance(gear,int):
        raise ValueError("gear is not an int object")
    if not gear in range(4):
        raise ValueError("gear must be in range 0-3")
    if gear == 0:
        duty= 0
    if gear == 1:
        duty= 33
    if gear == 2:
        duty= 66
    if gear == 3:
        duty= 99
    param = {'freq':3000, 'duty': duty }
    pwm_pwm.setOption(param)


if __name__ == '__main__':
    ICCID=None
    g_connect_status = False
    net = None
    device = None
    deviceSecret = None
    deviceName = None
	#复制产品证书内容替换
    productKey = "your-productKey"
    productSecret = "your-productSecret"
    device_dyn_resigter_succed = False

    # 连接网络
    connect_network()
     # 获取设备的IMEI 作为deviceName 进行动态注册
    deviceName = modem.info.getDevImei()
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
    value_gears()
    print('start work')
    #初始化温度传感器
    a=AHT10()
    #set mode(℃ or F)
    a.set_mode(0)
    #初始化电机模块
    pwm_pwm = PWM()
    pwm_pwm.open("pwm_pwm")
    while True:
        time.sleep_ms(200)
        temp =  a.temperature()  #获取温度信息
        print('cur temp is {}'.format(temp))
        upload_temp()
        if temp <= gear1_temp and cur_gear != 0:
            cur_gear = 0
            control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))
        elif temp > gear1_temp and temp <= gear2_temp and cur_gear != 1:
            cur_gear = 1
            control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))
        elif temp > gear2_temp and temp <= gear3_temp and cur_gear != 2:
            cur_gear = 2
            control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))
        elif temp > gear3_temp and cur_gear != 3:
            cur_gear = 3
            control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))

