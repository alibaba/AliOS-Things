# coding=utf-8
# This is a sample Python script.
import utime
from driver import GPIO

def cb(obj):
    print("gpio callback test")
    print(obj)

print("start led test")
gpio = GPIO()
gpio.open("GPIO23")
gpio.enableIrq(cb)
utime.sleep_ms(200)
gpio.disableIrq(cb)
gpio.clearIrq(cb)
gpio.close()

print("end led test")
