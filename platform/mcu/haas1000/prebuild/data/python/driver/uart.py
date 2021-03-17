
import driver

print("-------------------uart test--------------------")
uartObj = driver.uart();
uartObj.open("/data/python/config/uart.json", "serial2");

regval  = bytearray([0x41, 0x42, 0x43, 0x44]);
uartObj.write(regval)

uartObj.close()
