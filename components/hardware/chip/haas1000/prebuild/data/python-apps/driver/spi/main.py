
from driver import SPI

print("-------------------spi test--------------------")
spi = SPI()
spi.open("SPI0")
readBuf = bytearray(3)
writeBuf = bytearray([0x9f])
print(writeBuf)
print(readBuf)
value = spi.sendRecv(writeBuf, readBuf)
print(value)
print(writeBuf)
print(readBuf)
spi.close()
print("-------------------spi test--------------------")
