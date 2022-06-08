# -*- encoding: utf-8 -*-
from driver import SPI
from driver import GPIO
from aliyunIoT import Device  # iot组件是连接阿里云物联网平台的组件
import ujson                  # json字串解析库
import utime                  # 延时函数在utime库中
import ele_scale              # 引入电子秤驱动库
import sh1106                 # 引入hasseduk1 oled屏去掉
import netmgr as nm


# 电子秤对象
scaleObj = None
# OLED对象
oledObj = None

# 设备实例
clkDev = None
dataDev = None
dcDev = None
resDev = None
spi1Dev = None

# 三元组信息
productKey = "产品key"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifi_ssid = "请填写您的路由器名称"
wifi_password = "请填写您的路由器密码"

# 物联网平台连接标志位
iot_connected = False

# 物联网设备实例
device = None

# 身高(cm)
g_height = 0.0

# 初始化设备
def scale_init():
    global scaleObj
    global clkDev, dataDev

    clkDev = GPIO()
    clkDev.open("hx711_clk")
    dataDev = GPIO()
    dataDev.open("hx711_data")

    scaleObj = ele_scale.EleScale(clkDev, dataDev, 430.0)

#初始化自带oled屏幕模块
def oled_init():
    global oledObj
    global spi1Dev, dcDev, resDev
    spi1Dev = SPI()
    spi1Dev.open("oled_spi")

    dcDev = GPIO()
    dcDev.open("oled_dc")

    resDev = GPIO()
    resDev.open("oled_res")

    oledObj = sh1106.SH1106_SPI(width=132, height=64, spi=spi1Dev, dc = dcDev, res = resDev)

    oledObj.fill(0)   #清屏背景黑色
    oledObj.show()

# 关闭设备
def scale_deinit():
    global clkDev, dataDev

    clkDev.close()
    dataDev.close()

def oled_deinit():
    global spi1Dev, dcDev, resDev

    dcDev.close()
    spi1Dev.close()
    resDev.close()

# 上传当前可供食用的宠物粮和累计投喂次数到物联网平台
def upload_msg(weight, bmi, strbmi, health_advice):
    global device

    data = ujson.dumps({
        'weight': weight,
        'bmi':bmi,
        'strbmi':strbmi,
        'health_advice':health_advice
    })
    # 生成上报到物联网平台的属性值字串
    # 如下所示的属性标识符必须和物联网平台的属性一致
    # "weight" - 代表体重
    # "bmi" - 代表体重指数
    # "strbmi" - 代表健康状态
    # "health_advice" - 代表健康建议
    uploadData = {'params': data}
    # 上传数据到物联网平台
    device.postProps(uploadData)

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    nm.init()
    nm.disconnect()

    # 连接到指定的路由器（路由器名称为wifi_ssid, 密码为：wifi_password）
    nm.connect(wifi_ssid, wifi_password)

    while True :
        wifi_connected = nm.getStatus() # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected == 5:         # nm.getStatus()返回5代表连线成功
            break
        else:
            utime.sleep(0.5)
            print("wifi_connected:", wifi_connected)

    print("Wi-Fi connected")
    print('DeviceIP:' + nm.getInfo()['ip'])  # 打印Wi-Fi的IP地址信息

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global g_height
    # {'height': xxx}
    payload = ujson.loads(request['params'])

    print("payload:%s"%payload)

    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "height" in payload.keys():
        height = payload["height"]
        if (height > 0):
            g_height = height
            print("height=%d\n"%g_height)

# 连接物联网平台
def connect_lk(productKey, deviceName, deviceSecret):
    global device, iot_connected
    key_info = {
        'region' : 'cn-shanghai' ,      #实例的区域
        'productKey': productKey ,      #物联网平台的PK
        'deviceName': deviceName ,      #物联网平台的DeviceName
        'deviceSecret': deviceSecret ,  #物联网平台的deviceSecret
        'keepaliveSec': 60
    }

    # 将三元组信息设置到iot组件中
    device = Device()
    # 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect
    device.on(Device.ON_CONNECT, on_connect)
    # 配置云端属性控制指令的回调函数，如果收到物联网平台发送的属性控制消息，则调用on_props
    device.on(Device.ON_PROPS, on_props)
    # 启动连接阿里云物联网平台过程
    device.connect(key_info)

    # 等待设备成功连接到物联网平台
    while True:
        if iot_connected:
            print("物联网平台连接成功")
            break
        else:
            print("sleep for 1 s")
            utime.sleep(1)

    print('sleep for 2s')
    utime.sleep(2)


if __name__ == '__main__':
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)

    oled_init()
    scale_init()
    print("capValue=%.2f\n"%scaleObj.capValue)

    while True:
        weight = scaleObj.getWeight()
        weight = weight / 1000

        # 体重指数=体重(kg) / (身高(m)^2)
        if (g_height > 0):
            bmi = weight / ((g_height / 100)**2)
        else:
            bmi = 0.0

        if (bmi > 0 and bmi <= 18.4):
            strbmi = "偏轻"
            health_advice = "少食多餐，保证供给充足的蛋白质和热量"
        elif (bmi > 18.4 and bmi <= 23.9):
            strbmi = "优秀"
            health_advice = "饭后吃一些水果，保持完美身材"
        elif (bmi > 23.9 and bmi <= 27.9):
            strbmi = "偏重"
            health_advice = "减少热量食物的摄入，建议多吃水果和蔬菜"
        elif (bmi > 27.9):
            strbmi = "肥胖"
            health_advice = "严格控制热量摄入，并保持一定的运动量"
        else:
            strbmi = ""
            health_advice = ""

        print("weight: %.2f kg, height: %.1f cm, bmi: %.2f\n" %(weight, g_height, bmi))

        oledObj.fill(0)
        oledObj.text('weight: {:.2f} kg'.format(weight), 10, 20)
        if (g_height > 0):
            oledObj.text('height: {:.1f} cm'.format(g_height), 10, 5)
            oledObj.text('bmi: {:.2f}'.format(bmi), 10, 35)

        oledObj.show()

        upload_msg(weight, bmi, strbmi, health_advice)
        utime.sleep(2)

    scale_deinit()
    oled_deinit()
