import network
from machine import Pin
from machine import SPI

pinRst = Pin("F4", Pin.OUT)
pinCS = Pin("B12", Pin.OUT)
spi = SPI(2, 80000)

nic = network.WIZNET5K(spi, pinRst, pinCS)
nic.ifconfig()