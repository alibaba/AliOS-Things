# -*- encoding: utf-8 -*-
from driver import GPIO
from driver import TIMER      # 定时器类
from aliyunIoT import Device  # iot组件是连接阿里云物联网平台的组件
import ujson                  # json字串解析库
import utime                  # 延时函数在utime库中
import hx711                  # 引入hx711传感器驱动库
import uln2003                # 引入ULN2003步进电机驱动库
import netmgr as nm


# 传感器对象
timerObj = None
hx711Obj = None
uln2003Obj = None

# 设备实例
clkDev = None
dataDev = None
A = None
A_ = None
B = None
B_ = None

# 可供食用的宠物粮
available_food = 0.0
# 投喂次数
feeding_times = 0

# 三元组信息
productKey = "产品key"
deviceName = "设备名称"
deviceSecret ="设备密钥"

# Wi-Fi SSID和Password设置
wifi_ssid = "请填写您的路由器名称"
wifi_password = "请填写您的路由器密码"

# 物联网平台连接标志位
iot_connected = False

# 物联网设备实例
device = None


# 设备初始化
def timer_init():
    global timerObj

    timerObj = TIMER(0)
    timerObj.open(mode=timerObj.PERIODIC, period=20000, callback=timer_cb)
    timerObj.stop()

def hx711_init():
    global hx711Obj
    global clkDev, dataDev

    clkDev = GPIO()
    clkDev.open("hx711_clk")
    dataDev = GPIO()
    dataDev.open("hx711_data")

    hx711Obj = hx711.HX711(clkDev, dataDev)

def uln2003_init():
    global uln2003Obj
    global A, A_, B, B_

    A = GPIO()
    A.open("uln2003_a")
    A_ = GPIO()
    A_.open("uln2003_a_")
    B = GPIO()
    B.open("uln2003_b")
    B_ = GPIO()
    B_.open("uln2003_b_")

    uln2003Obj = uln2003.ULN2003(A, A_, B, B_)

# 关闭设备
def timer_deinit():
    global timerObj

    timerObj.stop()
    timerObj.close()

def hx711_deinit():
    global clkDev, dataDev

    clkDev.close()
    dataDev.close()

def uln2003_deinit():
    global A, A_, B, B_
    A.close()
    A_.close()
    B.close()
    B_.close()

# 读取count次,除去最大最小值之后求平均作为一次测量结果
def hx711_read(count = 3):
    global hx711Obj

    # count必须>=3
    cnt = 3 if (count <= 3) else count

    idx = 0
    data = [0] * cnt
    while (idx < cnt):
        data[idx] = hx711Obj.getValue()
        idx += 1

    data.sort()
    return round(sum(data[1:-1]) / (len(data) - 2))

# 步进电机控制
def uln2003_ctrl(cmd = 'stop', step = 0, speed = 4):
    global uln2003Obj
    step_tmp = step

    if (cmd is 'stop'): # 停止
        uln2003Obj.motorStop()
        return

    while (step_tmp > 0):
        if (cmd is 'cw'): # 顺时针转动
            uln2003Obj.motorCw(speed)
        elif (cmd is 'ccw'): # 逆时针转动
            uln2003Obj.motorCcw(speed)

        step_tmp -= 1

# 定时器回调函数
def timer_cb(args):
    global feeding_times

    print("自动投喂一次...\n")
    # 驱动步进电机走512步，即电机转动一周
    uln2003_ctrl('cw', 512, 4)
    feeding_times += 1

# 上传当前可供食用的宠物粮和累计投喂次数到物联网平台
def upload_msg(food, times):
    global device

    data = ujson.dumps({
        'available_food': food,
        'feeding_times': times
    })
    # 生成上报到物联网平台的属性值字串
    # 此处的属性标识符"available_food"和"feeding_times"必须和物联网平台的属性一致
    # "available_food" - 代表当前可供宠物食用的宠物粮
    # "feeding_times"  - 代表截止目前已投放的次数
    uploadData = {'params': data}
    # 上传数据到物联网平台
    device.postProps(uploadData)

# 上传当前自动投喂开关状态到物联网平台
def upload_auto_feeding(feeding):
    global device

    data = ujson.dumps({
        'auto_feeding': feeding
    })

    # "auto_feeding" - 自动投喂开关的状态: 1 -> open, 0 -> close
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
    global feeding_times

    # {'feeding_cmd': 1 or 0, 'clean_cmd': 1 or 0}
    payload = ujson.loads(request['params'])

    print("payload:%s"%payload)

    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "feeding_cmd" in payload.keys():
        feeding_cmd = payload["feeding_cmd"]
        if (feeding_cmd):
            print("手动投喂一次...\n")
            uln2003_ctrl('stop')
            uln2003_ctrl('cw', 512)
            feeding_times += 1

    if "clean_cmd" in payload.keys():
        clean_cmd = payload["clean_cmd"]
        if (clean_cmd):
            print("投喂计数清零\n")
            feeding_times = 0

    if "auto_feeding" in payload.keys():
        auto_feeding = payload["auto_feeding"]
        if (auto_feeding):
            timerObj.reload()
            timerObj.start()
            print("打开自动投喂\n")
        else:
            timerObj.stop()
            uln2003_ctrl('stop')
            print("关闭自动投喂\n")

        upload_auto_feeding(auto_feeding)

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

    upload_auto_feeding(0)

    timer_init()
    hx711_init()
    uln2003_init()

    # 计算hx711空载偏移量
    hx711_offset = hx711_read(10)

    while True:
        hx711_data = hx711_read(5)
        if (hx711_data <= hx711_offset):
            available_food = 0.0
        else:
            available_food = (hx711_data - hx711_offset) / 430.0

        print("Amount of pet food delivered: %.1f g\n" %available_food)
        upload_msg(available_food, feeding_times)
        utime.sleep(2)

    timer_deinit()
    uln2003_deinit()
    hx711_deinit()
