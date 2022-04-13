# coding=utf-8
# This is a sample Python script.
import utime
from driver import GPIO

print("start led test")
gpio = GPIO()
leds=("led1", "led2", "led3", "led4", "led5")
for i in range(5):
    for led in leds:
        gpio.open(led)
        gpio.write(1)
        utime.sleep_ms(200)
        gpio.write(0)
        utime.sleep_ms(200)
        gpio.write(1)
        utime.sleep_ms(200)
        gpio.close()

print("end led test")
