import utime as time
from driver import I2C
import gxht30

#创建并打开一个I2C实例
i2cDev=I2C()
i2cDev.open('gxht30')
#创建一个温湿度实例
tem_hum_dev=gxht30.GXHT30(i2cDev)
#获取温湿度数据
while True :
    cTemp,fTemp,humidity=tem_hum_dev.measure()
    print ("Temperature in Celsius : %.2f C" %cTemp)
    print ("Temperature in Fahrenheit : %.2f F" %fTemp)
    print ("Relative Humidity : %.2f %%RH" %humidity)
    print("------------------------------------------\n")
    time.sleep(1)
