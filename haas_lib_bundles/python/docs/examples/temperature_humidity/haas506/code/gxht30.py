from driver import I2C
class GXHT30(object):

    # init i2cDev
    def __init__(self,i2cObj):
        self.i2cObj=None
        if not isinstance(i2cObj,I2C):
            raise ValueError("parameter is not an I2C object")

        self.i2cObj=i2cObj
    
    # write cmd to register
    # commands:0x2c、0x06
    def write(self,cmd1,cmd2):
        writeBuf=bytearray([cmd1,cmd2])
        self.i2cObj.write(writeBuf,2)

    # read data from register
    # read data from :0x00
    # len(data) are 6 bytes : cTemp MSB, cTemp LSB, cTemp CRC, Humididty MSB, Humidity LSB, Humidity CRC 
    def read(self,cmd,len):
        readBuf=bytearray(len)
        readBuf[0]=cmd
        self.i2cObj.read(readBuf,6)
        return readBuf
        
    # convert the data
    def covert_data(self,data):
        cTemp = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45
        fTemp = cTemp * 1.8 + 32
        humidity = 100 * (data[3] * 256 + data[4]) / 65535.0
        return cTemp,fTemp,humidity

    # measure temperature and humidity
    def measure(self):
        if self.i2cObj is None:
            raise ValueError("invalid I2C object")
        self.write(0x2c,0x06)
        data=self.read(0x00,6)
        cTemp,fTemp,humidity=self.covert_data(data)
        return  cTemp,fTemp,humidity

