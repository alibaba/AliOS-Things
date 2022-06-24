# ！/usr/bin/python3
# -- coding: utf-8 --
# @Time : 2022/6/13 11:00 AM
# @Author : grady

from modem import voiceCall
from audio import Audio
import utime as time
from driver import GPIO
from driver import UART
import aliyun
import _thread

###############################
# 更改产品信息
productKey = "xxxxxxxx"
productSecret = "xxxxxxxx"
# 更改电话号码
# 需要发拨打的目标电话号码，长度为11的字符串格式
tele_phone = 'xxxxxxxxxxx'
###############################

global flag
flag = 0
# 实例化key1
key1 = GPIO(10, 10)
key1.open("KEY1")
# gbs初始化
gps_module = UART()
gps_module.open("serial1")
gps_module.setBaudRate(9600)  # gps模块的波特率是9600
readBuf = bytearray(512)  # 创建一个字节数组，用于接受串口数据


# 按键中断回调函数
def key1_callback(args):
    global flag, tele_phone
    if flag == 0:
        flag = 1
        # 拨打电话
        print('=== call ===')
        # 将yourphone改为用户电话号码，文本格式，加''
        vc.callStart(tele_phone)
        time.sleep(1)
    elif flag == 1:
        flag = 0
        # 挂断电话
        print('----callend----')
        vc.callEnd()
    else:
        pass
    key1.disableIrq()
    key1.clearIrq()


def enable_key():
    # 开启中断
    key1.enableIrq(key1_callback)


# 接听回调
def voice_callback(args):
    if args[0] == 10:
        print('voicecall incoming call, PhoneNO.: ', args[6])
    elif args[0] == 11:
        print('voicecall connected, PhoneNO.: ', args[6])
    elif args[0] == 12:
        print('voicecall disconnect')
    elif args[0] == 13:
        print('voicecall is waiting, PhoneNO.: ', args[6])
    elif args[0] == 14:
        print('voicecall dialing, PhoneNO.: ', args[6])
    elif args[0] == 15:
        print('voicecall alerting, PhoneNO.: ', args[6])
    elif args[0] == 16:
        print('voicecall holding, PhoneNO.: ', args[6])


# 纬度
def latitude(d, h):
    if d == "":
        return 0
    hemi = "" if h == "N" else "-"
    # 度
    deg = int(d[0:2])
    # 分
    min = str(float(d[2:]) / 60)[1:]
    return hemi + str(deg) + min


# 经度
def longitude(d, h):
    if d == "":
        return 0
    hemi = "" if h == "E" else "-"
    # 度
    deg = int(d[0:3])
    # 分
    min = str(float(d[3:]) / 60)[1:]

    return hemi + str(deg) + min


location_data = {}


def gps():
    while True:
        if flag == 1:
            # 串口读
            size = gps_module.read(readBuf)
            data = readBuf
            # print(readBuf)
            # 将字节数据转化成字符串数据
            data_str = data.decode()
            # 判断是否有数据 且数据中是否包含"$GNRMC"
            if size != 0 and "$GNRMC" in data_str and "$GNVTG" in data_str:
                # 删除"\r\n"后，字符串变为列表
                data_list = data_str.split('\r\n')
                for i in range(len(data_list)):
                    if "$GNRMC" in data_list[i]:
                        # print(data_list[i])
                        # 删除","
                        result = data_list[i].split(',')
                        # print(result)
                        # $GNRMC,075622.000,A,3116.56922,N,12044.12475,E,0.00,0.00,020422,,,A,V*01
                        # ['$GNRMC', '075622.000', 'A', '3116.56922', 'N', '12044.12475', 'E', '0.00', '0.00', '020422', '', '', 'A', 'V*01']
                        # 在GNRMC中取数据
                        if len(result) == 14:
                            lat = latitude(result[3], result[4])
                            long = longitude(result[5], result[6])
                            print("lat:", lat)
                            print("long:", long)
                            location_data['lat'] = lat
                            location_data['long'] = long
                            aliyun.up_data(location_data)
        else:
            pass
        time.sleep(1)


if __name__ == '__main__':
    aliyun.connect(productKey, productSecret)  # 连接阿里云
    # 拨打电话实例化
    vc = voiceCall()
    ad = Audio()
    ad.set_pa()  # 电话功能需要预先开启功放
    ad.setVolume(10)  # 设置音量
    time.sleep(1)
    # 设置自动应答
    vc.setCallback(voice_callback)  # 设置监听回调函数
    vc.setAutoAnswer(5000)  # 设置自动应答时间 ms

    print('-----start-----')
    _thread.start_new_thread(gps, ())  # 开启gps线程
    # 打开按键使能
    enable_key()
