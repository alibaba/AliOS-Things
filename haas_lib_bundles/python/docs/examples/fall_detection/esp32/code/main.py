from motion import motion
from mpu6886 import mpu6886
import utime   # 延时函数在utime
from driver import I2C               # 驱动库
import network                   # Wi-Fi功能所在库
import ujson                     # json字串解析库
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件

i2cObj = None
mpu6886Dev = None

# 物联网平台连接标志位
iot_connected = False
wlan = None

# 三元组信息
productKey = "Your-productKey"
deviceName = "Your-deviceName"
deviceSecret = "Your-deviceSecret"

# 物联网设备实例
device = None

# Wi-Fi SSID和Password设置
wifiSsid = "Your-wifiSsid"
wifiPassword = "Your-wifiPassword"

# 等待Wi-Fi成功连接到路由器


def get_wifi_status():
    global wlan
    wifi_connected = False

    wlan.active(True)  # 激活界面
    wlan.scan()  # 扫描接入点
    wlan.disconnect()  # 断开Wi-Fi
    #print("start to connect ", wifiSsid)
    # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）
    wlan.connect(wifiSsid, wifiPassword)

    while True:
        wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected:                     # Wi-Fi连接成功则退出while循环
            break
        else:
            utime.sleep(0.5)
            print("wifi_connected:", wifi_connected)

    ifconfig = wlan.ifconfig()  # 获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    utime.sleep(0.5)

# 物联网平台连接成功的回调函数


def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）


def on_props(request):
    pass


def connect_lp(productKey, deviceName, deviceSecret):
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
    print('sleep for 2s')
    utime.sleep(2)


def get_data():
    acc = mpu6886Dev.acceleration
    gyro = mpu6886Dev.gyro
    # print(acc)
    # print(gyro)
    return acc, gyro                    # 返回读取到的加速度、角速度值


def fall_detected():
    upload_data = {'params': ujson.dumps({
        'isFall': 1,
    })
    }
    # 上传状态到物联网平台
    if (iot_connected):
        device.postProps(upload_data)


if __name__ == '__main__':
    # 网络初始化
    wlan = network.WLAN(network.STA_IF)  # 创建WLAN对象
    get_wifi_status()
    connect_lp(productKey, deviceName, deviceSecret)

    # 硬件初始化
    i2cObj = I2C()
    # 按照board.json中名为"mpu6886"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
    i2cObj.open("mpu6886")
    print("mpu6886 inited!")
    mpu6886Dev = mpu6886.MPU6886(i2cObj)   # 初始化MPU6886传感器

    # 获取跌倒检测的motion实例
    motionObj = motion.Motion("fall", get_data, fall_detected)

    # 使能action检测，并以Dictionary格式传入灵敏度参数
    sensitivity = {"ACCELERATION_LOW_THREADHOLD": 4, "ACCELERATION_UP_THREADHOLD": 30,
                   "ANGULAR_VELOCITY_LOW_THREADHOLD": 1, "ANGULAR_VELOCITY_UP_THREADHOLD": 10}
    motionObj.enable(sensitivity)

    # 关闭action检测，可再次使能，支持传入新的灵敏度
    # motionObj.disable()

    # i2cObj.close()                                      # 关闭I2C设备对象
    # del mpu6886Dev
