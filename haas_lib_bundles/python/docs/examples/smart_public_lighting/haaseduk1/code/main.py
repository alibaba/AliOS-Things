from aliyunIoT import Device     # aliyunIoT组件是连接阿里云物联网平台的组件
import utime
import ap3216c
from driver import I2C
from driver import GPIO
import netmgr as nm
import ujson

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

adcDev = 0
ledDev = 0
lightness = 0

# 物联网平台连接标志位
iot_connected = False

device = None
on_connected = False

# EDU K1上灯的初始化
def ledInit():
    global ledDev
    ledDev = GPIO()
    ledDev.open("led")

# 点亮灯
def ledOn():
    global ledDev
    ledDev.write(1)

# 熄灭灯
def ledOff():
    global ledDev
    ledDev.write(0)

# ap3216c光照传感器初始化
def ap3216c_init():
    global ap3216cDev
    i2cDev = I2C()
    i2cDev.open("ap3216c")

    ap3216cDev = ap3216c.AP3216C(i2cDev)
    print("ap3216c init finished")

# ap3216c光照传感器亮度获取
def ap3216c_light():
    global ap3216cDev,lightness
    lightness = ap3216cDev.getIlluminance()

# 关闭ap3216c光照传感器
def ap3216c_close():
    global ap3216cDev
    i2cDev.close()
    del ap3216cDev

# 上报光照强度数据
def show_lightness():
    global lightness,i_light,last_light,system_reset

    i_light = int(lightness)
    if system_reset == 1:
        print("system first come")
        system_reset = 0    # 系统第一次启动
        report_light_data(i_light)
    elif (i_light >= 100 and last_light < 100) or (i_light < 100 and last_light >= 100):
        report_light_data(i_light) # 每次采样值是跨阈值100就上报
        print("light has change")
    else:
        print('no need report')

    if i_light < 10:
        T_str = "亮度:" + str(round(i_light, 1))
    elif i_light >= 10 and i_light < 100:
        T_str = "亮度:" + str(round(i_light, 2))
    elif i_light >= 100 and i_light < 1000:
        T_str = "亮度:" + str(round(i_light, 3))
    elif i_light >= 1000 and i_light < 10000:
        T_str = "亮度:" + str(round(i_light, 4))
    print(T_str)
    last_light = i_light

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    nm.init()
    nm.disconnect()

    wifi_connected = nm.getStatus()
    nm.connect(wifiSsid,wifiPassword)
    while True :
        if wifi_connected:
            break
        else:
            wifi_connected = nm.getStatus()
        utime.sleep(0.5)
    print("Wi-Fi connected")
    print('DeviceIP:' + nm.getInfo()['ip'])


# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 接收到到物联网服务端平台数据信息的回调函数
def on_props(request):
    #服务端返回的json转换成dict
    payload = ujson.loads(request['params'])
    #获取dict中的灯物模型状态字段
    stat = payload["onoff"]
    if stat == 1:
        print('led on')
        ledOn()
    else:
        print('led off')
        ledOff()
    utime.sleep_ms(200)

# 连接物联网平台
def connect_lk(productKey, deviceName, deviceSecret):

    global device, iot_connected
    # 初始化linkkit sdk
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

def report_light_data(l_data):
    # 生成上报到物联网平台的属性值字串
    upload_data = {'params': ujson.dumps({
        'Brightness': l_data,
    })
    }
    print("uploading data to the cloud:", upload_data)
    # 上传光强度信息到物联网平台
    device.postProps(upload_data)

def main():
    global i_light,last_light,system_reset
    i_light = 0
    last_light = 0
    system_reset = 1

    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    ap3216c_init()
    ledInit()
    while True:
        ap3216c_light()
        show_lightness() #光照信息
        utime.sleep(1.5) #每隔1.5s去刷新光照信息

if __name__ == '__main__':
    main()
