
import driver

print("-------------------spi test--------------------")
spiObj = driver.spi();
spiObj.open("/data/python/config/spi.json", "SPI0");
spi_read_buf = bytearray(6)
spi_write_buf = bytearray([1, 2, 3, 4, 5, 6])
value = spiObj.read(spi_read_buf);
print(value);
value = spiObj.write(spi_write_buf)
print(value);
value = spiObj.sendRecv(spi_write_buf, spi_read_buf)
print(value);
spiObj.close();
print("-------------------spi test--------------------")
