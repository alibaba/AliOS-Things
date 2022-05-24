# -*- encoding: utf-8 -*-

from aliyunIoT import Device    # aliyunIoT组件是连接阿里云物联网平台的组件
import utime                    # 延时API所在组件
import ujson                    # json字串解析库
import network             # network是Wi-Fi网络连接的组件
import dds5188

# 物联网平台连接标志位
iot_connected = False

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 物联网设备实例
device = None

dds5188_dev = None

wlan = None
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
    device.connect(key_info)

    # 等待设备成功连接到物联网平台
    while(True):
        if iot_connected:
            print('物联网平台连接成功')
            break
        else:
            print('sleep for 1 s')
            utime.sleep(1)

# 上传火焰传感器检测电压信息和报警信息到物联网平台
def upload_state():
    global device, dds5188_dev

    # 无限循环
    while True:
        tup = dds5188_dev.getKWh()
        if tup[0] != 0:
            utime.sleep(2)
            continue
        energy = tup[1]
        tup = dds5188_dev.getVoltage()
        if tup[0] != 0:
            utime.sleep(2)
            continue
        voltage = tup[1]
        tup = dds5188_dev.getCurrent()
        if tup[0] != 0:
            utime.sleep(2)
            continue
        current = tup[1]
        tup = dds5188_dev.getActivePower()
        if tup[0] != 0:
            utime.sleep(2)
            continue
        active_power = tup[1]
        prop = ujson.dumps({
            'energy': energy,
            'voltage': voltage,
            'current': current,
            'activePower': active_power
        })
        print("uploading data to the cloud, ", prop)
        upload_data = {'params': prop}
        # 上传火焰传感器测量结果和报警灯状态到物联网平台
        device.postProps(upload_data)

        # 每2秒钟上报一次
        utime.sleep(2)

if __name__ == '__main__':
    wlan = network.WLAN(network.STA_IF)  # 创建WLAN对象
    dds5188_dev = dds5188.DDS5188('mb1', 2)

    # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
    get_wifi_status()

    connect_lk(productKey, deviceName, deviceSecret)
    upload_state()
