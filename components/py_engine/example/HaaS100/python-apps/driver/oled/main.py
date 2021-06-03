import sh1106
import utime
from driver import SPI
from driver import GPIO

spi0 = SPI()
spi0.open("SPI1")

gpio_dc = GPIO()
gpio_dc.open("oled_dc")

gpio_res = GPIO()
gpio_res.open("oled_res")

display = sh1106.SH1106_SPI(width=132, height=64, spi=spi0, dc = gpio_dc, res = gpio_res)
# display.init_display()
display.fill(1)
display.show()
display.test()