# -*- coding: UTF-8 -*-
import iot
import utime
# 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
productKey = "a1uTFk4xjko"
productSecret = "xxxxxxx"
deviceName  =    "mpy_001"
deviceSecret  =   "xxxxxxxxxxxxxxx"
# 初始化linkkit sdk
key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'productSecret': productSecret
}
device = iot.Device(key_info)
# 物联网平台连接成功的回调函数
def on_connect():
    print('linkkit is connected ')
device.on('connect',on_connect)
# 设置props 事件接收函数（本案例是云端控制led事件）
def on_props(request):
    from driver import GPIO
    import ujson
    ON = 0
    OFF = 1
    #服务端返回的json 转换成dict
    payload = ujson.loads(request)
    #获取dict 中的led 状态字段
    stat = payload["LEDSwitch"]
    gpio = GPIO()
    # open 函数支持的参数列表如下：
    # led1;led2;led3;led4;led5
    gpio.open('led1')
    if stat == 1:
        gpio.write(ON)
    else:
        gpio.write(OFF)
    utime.sleep_ms(200)
    gpio.close()
device.on('props',on_props)
# 连接物联网平台
device.connect()
# 触发linkit sdk持续处理server端信息
while(True):
    device.do_yield(200)
# 断开连接
device.close()
