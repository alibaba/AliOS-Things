from humiturePanel import HumiturePanel
import utime
import sht3x
from driver import I2C

# 创建I2C对象，用于创建I2C类型的温湿度设备
i2cObj = I2C()
# 打开I2C对象
i2cObj.open("sht3x")

# 创建SHT3X温湿度传感器对象
humitureDev = sht3x.SHT3X(i2cObj)
print('create humiture device')

humiturePage = HumiturePanel()
print('create humiture panel')

while True:
    temperature = humitureDev.getTemperature()
    humidity = humitureDev.getHumidity()

    print('temperature:', temperature)
    print('humidity:', humidity)

    humiturePage.showHumiture(temperature, humidity)
    utime.sleep(1)

i2cObj.close()
delete humitureDev
