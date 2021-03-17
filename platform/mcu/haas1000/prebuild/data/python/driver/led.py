# coding=utf-8
# This is a sample Python script.
import utime
import driver

print("start led test")
GPIO = driver.gpio()
leds=("led1", "led2", "led3", "led4", "led5")
for i in range(5):
    for led in leds:
        GPIO.open("/data/python/config/led.json", led)
        GPIO.write(1)
        utime.sleep_ms(200)
        GPIO.write(0)
        utime.sleep_ms(200)
        GPIO.write(1)
        utime.sleep_ms(200)
        GPIO.close()

print("end led test")
