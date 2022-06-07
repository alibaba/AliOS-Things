from aliyunIoT import Device     # aliyunIoT组件是连接阿里云物联网平台的组件
import utime
import netmgr as nm
from driver import UART
import gnss
import ujson

uartDev = 0
gnssDev = 0

# wifi连接的的ssid和pwd定义
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 物联网平台相关的key和serect定义
productKey     = "产品key"
deviceName     = "设备名称"
deviceSecret   = "设备密钥"

# 物联网平台连接状态标识
iot_connected = False

# 物联网设备实例
device = None

# 物联网平台连接成功时触发 on_connect
def on_connect():
    global on_connected
    on_connected = True

def gnssInit():
    global gnssDev,uartDev
    # 初始化定位模组串口
    uartDev = UART()
    uartDev.open('gnss')
    # 创建定位信息解析器
    gnssDev = gnss.Gnss(uartDev)

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    nm.init()
    wifi_connected = nm.getStatus()
    print("start to connect " , wifiSsid)
    nm.connect(wifiSsid, wifiPassword)       # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）

    while True :
        if wifi_connected == 5:               # nm.getStatus()返回5代表连线成功
            break
        else:
            wifi_connected = nm.getStatus() # 获取Wi-Fi连接路由器的状态信息
            utime.sleep(0.5)
    # utime.sleep(5)
    print("Wi-Fi connected")
    print('DeviceIP:' + nm.getInfo()['ip'])  # 打印Wi-Fi的IP地址信息


# 物联网平台连接成功时触发 on_connect
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    pass

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
    device.connect(key_info)

    # 等待设备成功连接到物联网平台
    while(True):
        if iot_connected:
            print('物联网平台连接成功')
            break
        else:
            print('sleep for 1 s')
            utime.sleep(1)

# 主程序入口
if __name__ == '__main__':
    gnssInit()

    # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)

    # 定义经纬度及海拔
    longitude = 0
    latitude = 0
    altitude = 0
    # 连续从串口种读取信息
    while True:
        # 串口读取定位模块语句
        location = gnssDev.getLocation()
        if(location):
            print(location.longitude, location.latitude, location.altitude)

            # 判断定位信息是否发生变化
            if(longitude != location.longitude[0] or latitude != location.latitude[0] or altitude != location.altitude):
                longitude = location.longitude[0]
                latitude = location.latitude[0]
                altitude = location.altitude
                print(longitude, latitude, altitude)

                # 如果有变化，则上报地理位置信息至物联网平台
                loc_data = {
                    'params': ujson.dumps({
                        'GeoLocation': {
                            'Longitude': longitude,
                            'Latitude': latitude,
                            'Altitude': altitude,
                            'CoordinateSystem': 1
                        }
                    })
                }
                device.postProps(loc_data)
        utime.sleep(2)
    device.close()
    uartDev.close()

