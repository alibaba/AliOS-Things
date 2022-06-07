# -*- encoding: utf-8 -*-

from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import netmgr as nm              # Wi-Fi功能所在库
import utime                     # 延时API所在组件
import ujson                     # json字串解析库
from driver import ADC           # ADC类，通过微处理器的ADC模块读取ADC通道输入电压
import hcho                      # 甲醛hcho传感器类
adcObj = 0                     # ADC通道对象
uartObj = 0                    # UART通道对象


# 物联网平台连接标志位
iot_connected = False

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 三元组信息
productKey = "产品密钥"  #需要填入物联网云平台申请到的productKey信息
deviceName = "设备名称"  #需要填入物联网云平台申请到的deviceName信息
deviceSecret = "设备密钥" #需要填入物联网云平台申请到的deviceSecret信息

# 物联网设备实例
device = None

hchoDev = 0

def hcho_init():
    global adcObj,hchoDev

    adcObj = ADC()
    adcObj.open("hcho")
    hchoDev = hcho.HCHO(adcObj)

def get_hcho_value():
    global hchoDev

    return hchoDev.getPPM()

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
   nm.init()
   nm.disconnect()
   wifi_connected = nm.getStatus()
   print("start to connect " , wifiSsid)
   nm.connect(wifiSsid, wifiPassword)       # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）

   while True :
      if wifi_connected == 5:               # nm.getStatus()返回5代表连线成功
            break
      else:
            wifi_connected = nm.getStatus() # 获取Wi-Fi连接路由器的状态信息
            utime.sleep(0.5)
            print("wifi_connected: " , wifi_connected)

   print("Wi-Fi connected")
   print('DeviceIP:' + nm.getInfo()['ip'])  # 打印Wi-Fi的IP地址信息

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    pass

# 连接物联网平台
def connect_lk(productKey, deviceName, deviceSecret):
    global device, iot_connected
    key_info = {
        'region': 'cn-shanghai',
        'productKey': productKey,
        'deviceName': deviceName,
        'deviceSecret': deviceSecret,
        'keepaliveSec': 60
    }
    # 将三元组信息设置到iot组件中
    device = Device()

    # 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
    device.on(Device.ON_CONNECT, on_connect)

    # 配置收到云端属性控制指令的回调函数，如果收到物联网平台发送的属性控制消息，则调用on_props函数
    device.on(Device.ON_PROPS, on_props)

    # 启动连接阿里云物联网平台过程
    ret = device.connect(key_info)

    # 等待设备成功连接到物联网平台
    while(True):
        if iot_connected:
            print('物联网平台连接成功')
            break
        else:
            print('sleep for 1 s')
            utime.sleep(1)

# 上传甲醛浓度信息到物联网平台
def upload_hcho_detector_state():
    global device
    # 无限循环
    while True:
        data = get_hcho_value()
        H_str = "Hcho : " + str(round(data,2))+'ppm'
        print('Hcho :' + str(round(data,2)) +'ppm')

        # "HCHO" - 代表甲醛传感器测量到的浓度值
        upload_data = {'params': ujson.dumps({
            'HCHO': round(data,2),
        })}
        # 上传甲醛浓度信息到物联网平台
        device.postProps(upload_data)

        # 每2秒钟上报一次
        utime.sleep(2)

if __name__ == '__main__':
    # 运行此demo之前务必保证模块已经上电5分钟以上
    hcho_init()

    # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
    # global productKey, deviceName, deviceSecret ,on_request, on_play
    get_wifi_status()

    connect_lk(productKey, deviceName, deviceSecret)
    upload_hcho_detector_state()
