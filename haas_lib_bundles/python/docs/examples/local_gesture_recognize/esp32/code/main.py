# 本地手势识别

######################
#### 请修改此处信息 ####
PRODUCT_KEY = '这里填写产品PK'
DEVICE_NAME = '这里填入设备名称DN'
DEVICE_SECRET = '这里填入设备密钥DS'
WIFI_SSID = 'WiFi名称'
WIFI_PWD = 'WiFi密码'
######################

# 导入框架模块
from time import sleep
import ujson as json
import network
import kv
from aliyunIoT import Device

# 导入工程内驱动模块
from buzzer import Buzzer
from paj7620 import Gesture

# 关闭固件内一分钟上云 MQTT 通道以释放内存
kv.set('app_upgrade_channel', 'disable')
buzzer = Buzzer("Buzzer")


def beep_ok():
    buzzer.beep(500)


def beep_error():
    buzzer.beep(100)
    sleep(0.1)
    buzzer.beep(100)
    sleep(0.1)
    buzzer.beep(100)


def beep_gesture():
    buzzer.beep(20)

# 连接WiFi
print("Connecting to WiFi...")
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.scan()
wlan.connect(WIFI_SSID, WIFI_PWD)
while True:
    if wlan.isconnected():
        break
    print("Waiting for WiFi connection...")
    sleep(1)
print(wlan.ifconfig())
print("WiFi Connected")

isIotConnected = False


def on_connect(data):
    global isIotConnected
    print("ON_CONNECT: ", data)
    isIotConnected = True

# 连接物联网平台
print("Connecting to IoT LinkPlatform...")
device = Device()
device.on(Device.ON_CONNECT, on_connect)
device.connect({
    'region': "cn-shanghai",
    'productKey': PRODUCT_KEY,
    'deviceName': DEVICE_NAME,
    'deviceSecret': DEVICE_SECRET,
    'keepaliveSec': 60
})

while True:
    if isIotConnected:
        print('IoT Platform connected')
        break
    else:
        print('Waiting for IoT Platform connection...')
        sleep(1)


def post_gesture_event(name):
    device.publish({
        'topic': '/sys/' + PRODUCT_KEY + '/' + DEVICE_NAME + '/thing/event/gesture/post',
        'payload': json.dumps({
            "id": "6",
            "version": "1.0",
            "params": {
                "name": name
            }
        }),
        'qos': 1
    })


def post_gesture_property(value):
    device.postProps({
        'params': json.dumps({
            'lastGesture': value
        })
    })

# 定义手势处理函数
def handle_wave():
    print("WAVE")
    post_gesture_event("WAVE")
    post_gesture_property(9)
    beep_gesture()


def handle_up():
    print("UP")
    post_gesture_event("UP")
    post_gesture_property(1)
    beep_gesture()


def handle_down():
    print("DOWN")
    post_gesture_event("DOWN")
    post_gesture_property(2)
    beep_gesture()


def handle_left():
    print("LEFT")
    post_gesture_event("LEFT")
    post_gesture_property(3)
    beep_gesture()


def handle_right():
    print("RIGHT")
    post_gesture_event("RIGHT")
    post_gesture_property(4)
    beep_gesture()


def handle_forward():
    print("FORWARD")
    post_gesture_event("FORWARD")
    post_gesture_property(5)
    beep_gesture()


def handle_backward():
    print("BACKWARD")
    post_gesture_event("BACKWARD")
    post_gesture_property(6)
    beep_gesture()


def handle_clockwise():
    print("CLOCKWISE")
    post_gesture_event("CLOCKWISE")
    post_gesture_property(7)
    beep_gesture()


def handle_anticlockwise():
    print("ANTICLOCKWISE")
    post_gesture_event("ANTICLOCKWISE")
    post_gesture_property(8)
    beep_gesture()

# 初始化手势处理模块
print("Initializing gesture recognizer...")
try:
    gesture = Gesture("PAJ7620_I2C", "PAJ7620_IRQ", {
        Gesture.WAVE: handle_wave,
        Gesture.UP: handle_up,
        Gesture.DOWN: handle_down,
        Gesture.LEFT: handle_left,
        Gesture.RIGHT: handle_right,
        Gesture.FORWARD: handle_forward,
        Gesture.BACKWARD: handle_backward,
        Gesture.CLOCKWISE: handle_clockwise,
        Gesture.ANTICLOCKWISE: handle_anticlockwise
    })
    print("Done")
    beep_ok()
except IOError as e:
    print("Gesture initialize error:", e)
    beep_error()

# 主程序不退出
while True:
    sleep(0.01)
