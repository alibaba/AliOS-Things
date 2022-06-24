# ！/usr/bin/python3
# -- coding: utf-8 --
# @Time : 2022/6/13 11:00 AM
# @Author : grady

from driver import ADC
import utime as time
from modem import sms

###############################
# 需要发送短信的目标电话号码，长度为11的字符串格式
tele_phone = 'xxxxxxxxxxx'
###############################

if __name__ == '__main__':
    # 报警次数
    warning = 0
    # 短信实例化
    smss = sms()
    # 火焰传感器
    adc = ADC()
    adc.open("ADC1")

    while True:
        time.sleep_ms(1000)
        value = adc.readVoltage()
        if value <= 1000:
            warning += 1
            if warning > 4:
                print('已累计上报3次，停止短信发送')
            elif warning > 1:
                # 将yourphone改为用户电话号码，文本格式，加''
                smss.sendTextMsg(tele_phone, '检测到明火，请及时处理！！！', 'UCS2')
                time.sleep(60)
            else:
                print('数据异常！')
        else:
            warning = 0
            print('正常运行，当前测量数值:', value)

