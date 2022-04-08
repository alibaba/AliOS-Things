# coding=utf-8
from driver import GPIO
import network
import ujson
import utime as time
import modem
from  aliyunIoT import Device
import kv
import read
import write
import _thread
import mfrc522
import ujson
from driver import WDT


wdt = WDT()
wdt.open('wdt')
wdt.feed()

global  g_connect_status,net,device,deviceSecret,deviceName,productKey,productSecret,device_dyn_resigter_succed,cloud_led
g_connect_status= False
net= None
device = None
deviceSecret = None
deviceName = None
#input your productKey and productSecret
productKey = "yourProductKey"
productSecret = "yourProductSecret"
device_dyn_resigter_succed = False
led1 = GPIO()


#当iot设备连接到物联网平台的时候触发'connect' 事件
def on_connect(data):
    print('***** connect lp succeed****')

#当iot云端下发属性设置时，触发'props'事件
def on_props(request):
    print('clound req data is {}'.format(request))

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
        print('network register failed')
    while True:
        if g_connect_status==False:
            pass
        if g_connect_status:
            print('network register successed')
            break
        time.sleep_ms(20)

#动态注册回调函数
def on_dynreg_cb(data):
    global deviceSecret,device_dyn_resigter_succed
    deviceSecret = data
    device_dyn_resigter_succed = True

 # 连接物联网平台
def dyn_register_device(productKey,productSecret,deviceName):
    global on_dynreg_cb,device,deviceSecret
    key = '_amp_customer_devicesecret'
    deviceSecretdict = kv.get(key)
    print("deviceSecretdict:",deviceSecretdict)
    if isinstance(deviceSecretdict,str):    
        deviceSecret = deviceSecretdict 
    else:
        deviceSecret = None

    if deviceSecret is None:
        key_info = {
            'productKey': productKey  ,
            'productSecret': productSecret ,
            'deviceName': deviceName
            }
        # 动态注册一个设备，获取设备的deviceSecret
        device.register(key_info,on_dynreg_cb)

def rfid_read():
    global device,led1,wdt
    led1.open('led1')
    rdr=mfrc522.MFRC522()
    while True:
        ret,cardid = read.do_read(rdr)
        if ret == True:
            print('card authen ok!')
            senddata = {'cardinfo':'findCard','cardid':"0x%02x%02x%02x%02x" % (cardid[0], cardid[1], cardid[2], cardid[3])}
            data = {'params': ujson.dumps(senddata)}
            device.postProps(data)

            level = led1.read()
            if level == 1:
                led1.write(0)
            else:
                led1.write(1)
        else:
            print('card authen failed!')

        time.sleep(1)
        wdt.feed()

def main():
    global g_connect_status,net,device,deviceSecret,deviceName,productKey,productSecret,device_dyn_resigter_succed
    #连接网络
    connect_network()
    #获取设备的IMEI 作为deviceName 进行动态注册
    deviceName = modem.getDevImei()
    #初始化物联网平台Device类，获取device实例
    device = Device()
    if deviceName is not None and len(deviceName) > 0 :
        #动态注册一个设备
        dyn_register_device(productKey,productSecret,deviceName)
    else:
        print("can't get IMEI")
    while deviceSecret is None:
        time.sleep(0.2)

    print('dynamic successed:' + deviceSecret)
    key_info = {
        'region' : 'cn-shanghai' ,
        'productKey': productKey ,
        'deviceName': deviceName ,
        'deviceSecret': deviceSecret ,
        'keepaliveSec': 60
        }
    #打印设备信息
    print(key_info)
    #device.ON_CONNECT 是事件，on_connect是事件处理函数/回调函数
    device.on(device.ON_CONNECT,on_connect)           
    device.on(device.ON_PROPS,on_props)  
    device.on(device.ON_SERVICE,on_service)
    device.on(device.ON_ERROR,on_error)
    device.connect(key_info)  

    write.do_write()
    _thread.start_new_thread(rfid_read,())


if __name__=="__main__":
    main()