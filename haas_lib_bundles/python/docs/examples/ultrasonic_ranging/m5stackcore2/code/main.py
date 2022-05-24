# -*- encoding: utf-8 -*-

from aliyunIoT import Device    # aliyunIoT组件是连接阿里云物联网平台的组件
import display
import utime
import ujson                    # json字串解析库
import netmgr as nm             # netmgr是Wi-Fi网络连接的组件
import _thread
import urm37
from driver import UART

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

dist = 65535

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
        wifi_connected = nm.getStatus()     # 获取Wi-Fi连接路由器的状态信息
        utime.sleep(0.5)
   print("Wi-Fi connected")
   print('DeviceIP:' + nm.getInfo()['ip'])  # 打印Wi-Fi的IP地址信息

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

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

def networkThread():
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    old_dist = 65535
    while True:
        if dist != old_dist:
            old_dist = dist
            prop = ujson.dumps({
                'distance': dist
            })
            upload_data = {'params': prop}
            device.postProps(upload_data)
        utime.sleep(1)

def main():
    global dist
    _thread.stack_size(20 * 1024)
    _thread.start_new_thread(networkThread, ())
    disp = display.TFT()
    uart_dev = UART()
    if uart_dev.open('serial2') != 0:
        print('Error: UART init error.')
        return
    urm37_dev = urm37.URM37(uart_dev)
    utime.sleep(1)
    while True:
        dist = urm37_dev.getRange()
        disp.clear()
        disp.font(disp.FONT_DejaVu40)
        if dist == 65535:
            disp.text(100, 100, 'NA', disp.RED)
        else:
            text = '%03d' %dist
        disp.text(90, 100, text, disp.GREEN)
        utime.sleep_ms(200)

if __name__ == '__main__':
    main()
