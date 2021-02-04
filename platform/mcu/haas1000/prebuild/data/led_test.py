# coding=utf-8
# This is a sample Python script.
import utime
from haascv import driver

GPIO = driver.gpio()
leds=(34,35,36,40,41)
for i in range(10):
    for led in leds:
        GPIO.open(led, 5, 0)
        GPIO.write(0)
        utime.sleep_ms(200)
        GPIO.write(1)
        utime.sleep_ms(200)
        GPIO.close()
