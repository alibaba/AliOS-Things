# coding=utf-8

from driver import GPIO
from driver import UART
import network
import ujson
import utime as time
import modem
from  aliyunIoT import Device
import kv



 
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


#纬度
def latitude(d,h):
    if d=="":
        return 0
    hemi="" if h=="N" else "-"
    #度
    deg=int(d[0:2])
    #分
    min=str(float(d[2:])/60)[1:]
    return hemi +str(deg)+min


#经度
def  longitude(d,h):
    if d=="":
        return 0
    hemi="" if h=="E" else "-"
    #度
    deg=int(d[0:3])
    #分
    min=str(float(d[3:])/60)[1:]

    return hemi +str(deg)+min


def main():
    while True:
        #串口读
        size=gps_module.read(readBuf)
        data=readBuf
        #将字节数据转化成字符串数据
        data_str=data.decode()
        # 判断是否有数据 且数据中是否包含"$GNRMC"
        if size!=0 and "$GNRMC" in data_str  and "$GNVTG" in data_str:
            print(readBuf)
            print("------------0-----------------------")
            #删除"\r\n"后，字符串变为列表
            data_list=data_str.split('\r\n')
            print(data_list)     
            print("------------------1------------------")
            for i in range(len(data_list)):   
                if "$GNRMC" in data_list[i]:
                    print(data_list[i])
                    #删除","
                    result=data_list[i].split(',')
                    print(result)
                    #$GNRMC,075622.000,A,3116.56922,N,12044.12475,E,0.00,0.00,020422,,,A,V*01
                    #['$GNRMC', '075622.000', 'A', '3116.56922', 'N', '12044.12475', 'E', '0.00', '0.00', '020422', '', '', 'A', 'V*01']
                    # 在GNRMC中取数据
                    if len(result)==14:
                        lat=latitude(result[3],result[4])
                        long=longitude(result[5],result[6])
                        print("lat:",lat)
                        print("long:",long)
                        #数据上传
                        gbs_data["Longitude"]= float(long)
                        gbs_data["Latitude"]= float(lat)
                        gbs_data_str=ujson.dumps(gbs_data)
                        data={
                            'params':gbs_data_str
                            }      
                        device.postProps(data)
            print("---------------------2-------------------")
        time.sleep(1)    


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

    #gbs操作
    gps_module=UART()               #创建一个串口实例
    gps_module.open("serial1")      #打开
    gps_module.setBaudRate(9600)    #gps模块的波特率是9600
    readBuf=bytearray(512)          #创建一个字节数组，用于接受串口数据
    #主函数
    gbs_data = {}
    main()

