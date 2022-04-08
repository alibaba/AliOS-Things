import utime   # 延时函数在utime库中
from driver import GPIO,I2C
import sht3x
from ssd1306 import SSD1306_I2C

hum_s = 0
oled = None
sht3xDev = None
humi_gpio = None

def sht3x_init():
    global sht3xDev

    i2cDev = I2C()
    i2cDev.open("sht3x")
    sht3xDev = sht3x.SHT3X(i2cDev)

def humi_ctrl_init():
    global humi_gpio

    humi_gpio = GPIO()
    humi_gpio.open("hum_ctrl")

def start_hum():
    humi_gpio.write(0)

def stop_hum():
    humi_gpio.write(1)

def oled_init():
    global oled

    i2cObj = I2C()
    i2cObj.open("ssd1306")
    print("ssd1306 inited!")

    oled = SSD1306_I2C(128, 64, i2cObj)
    oled.fill(0)   #清屏背景黑色
    oled.text('welcome haas', 30, 5)
    oled.text('auto humi', 30, 22)
    oled.text(str('----------------------'),3,32)
    oled.text('', 30, 40)

    oled.show()

def oled_data_show(status,humi,time_arr):
    global oled
    oled.fill(0)
    oled.text(str('%d-%02d-%02d'%(time_arr[0],time_arr[1],time_arr[2])),30,5)
    oled.text(str('%02d:%02d:%02d'%(time_arr[3],time_arr[4],time_arr[5])),30,22)
    oled.text(str('----------------------'),3,32)
    if status == 1:
        oled.text('open', 25, 40)
        oled.text(str('%02d'%(humi)+'%H'),75,40)
    elif status == 0:
        oled.text('close', 25, 40)
        oled.text(str('%02d'%(humi)+'%H'),75,40)
    oled.show()

if __name__ == '__main__':
    sht3x_init()
    humi_ctrl_init()
    oled_init()
    while True:
        humidity = sht3xDev.getHumidity()
        if humidity <= 60.0:
            if hum_s == 0:
                hum_s = 1
                print("start")
                start_hum()
        else :
            if hum_s == 1:
                hum_s = 0
                print("stop")
                stop_hum()
        timeArray = utime.localtime()
        oled_data_show(hum_s,int(humidity),timeArray)
        utime.sleep(1)
