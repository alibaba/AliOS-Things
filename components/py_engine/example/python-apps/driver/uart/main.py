
# coding=utf-8
# This is a sample Python script.
import utime
from driver import UART

print("-------------------uart test--------------------")
print("-----How to test: connect PIN10 and PIN12-------")
uart = UART();
uart.open("serial2")
utime.sleep_ms(1000)
uart.setBaudRate(9600)
writeBuf  = bytearray([0x41, 0x42, 0x43, 0x44]);
for i in range(10):
    uart.write(writeBuf)
    utime.sleep_ms(1000)
    readBuf = bytearray(4)
    recvSize = uart.read(readBuf)
    utime.sleep_ms(100)
    print(recvSize)
    print(readBuf)
uart.close()
