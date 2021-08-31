import sh1106
import utime
from driver import SPI
from driver import GPIO
from mpu6050 import MPU6050

mpu6050Dev = MPU6050()
mpu6050Dev.open("mpu6050")
mpu6050Dev.init()
print("mpu6050 init finished")

spi0 = SPI()
spi0.open("oled_spi")

gpio_dc = GPIO()
gpio_dc.open("oled_dc")

gpio_res = GPIO()
gpio_res.open("oled_res")

display = sh1106.SH1106_SPI(width=132, height=64, spi=spi0, dc = gpio_dc, res = gpio_res)
# display.init_display()
ac = []
while(True):
    ac = mpu6050Dev.get_Accelerometer()
    #print("mpu6050 acc is: " , ac[0], ac[1], ac[2])
    display.fill(0)
    #display.fill_circle(50, 30, 20, 0xAF)
    #display.draw_circle(90, 30, 20, 2, 0xAF)
    display.draw_circle(66, 32, 10, 1, 1)
    display.fill_circle(int(66 - ac[0] / 250), int(32 + ac[1] / 500), 8, 1)
    display.show()
# display.test()
mpu6050Dev.close()
print("test mpu6050 success!")
