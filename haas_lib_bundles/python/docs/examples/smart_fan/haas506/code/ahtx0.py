import utime as time
from driver import I2C

#CONSTANTS
AHT10_ADDRESS = 0x38     # 0111000 (7bit address)
AHT10_READ_DELAY_MS = 75 # Time it takes for AHT to collect data
AHT_TEMPERATURE_CONST = 200
AHT_TEMPERATURE_OFFSET = 50
KILOBYTE_CONST = 1048576
CMD_INITIALIZE = bytearray([0xE1, 0x08, 0x00])
CMD_MEASURE = bytearray([0xAC, 0x33, 0x00])
FARENHEIT_MULTIPLIER = 9/5
FARENHEIT_OFFSET = 32

class AHT10:
    def __init__(self,  mode=0, address=AHT10_ADDRESS):
        self.i2c = I2C()
        self.i2c.open('aht10')

        self.address = address
        writeBuf=bytearray(4)
        writeBuf[0]=self.address
        writeBuf[1]=CMD_INITIALIZE[0]
        writeBuf[2]=CMD_INITIALIZE[1]
        writeBuf[3]=CMD_INITIALIZE[2]
        self.i2c.write(writeBuf)

        self.readings_raw = bytearray(8)
        self.results_parsed = [0, 0]
        self.mode = mode      # 0 for Celsius(摄氏度), 1 for Farenheit(华氏度)

    def read_raw(self):
        writeBuf=bytearray(4)
        writeBuf[0]=self.address
        writeBuf[1]=CMD_MEASURE[0]
        writeBuf[2]=CMD_MEASURE[1]
        writeBuf[3]=CMD_MEASURE[2]
        self.i2c.write(writeBuf)

        time.sleep_ms(AHT10_READ_DELAY_MS)
        readBuf=bytearray(6)
        readBuf[0]=AHT10_ADDRESS
        self.i2c.read(readBuf)
        self.readings_raw[0]=readBuf[0]
        self.readings_raw[1]=readBuf[1]
        self.readings_raw[2]=readBuf[2]
        self.readings_raw[3]=readBuf[3]
        self.readings_raw[4]=readBuf[4]
        self.readings_raw[5]=readBuf[5]

        self.results_parsed[0] = self.readings_raw[1] << 12 | self.readings_raw[2] << 4 | self.readings_raw[3] >> 4
        self.results_parsed[1] = (self.readings_raw[3] & 0x0F) << 16 | self.readings_raw[4] << 8 | self.readings_raw[5]

    def humidity(self):
        self.read_raw()
        return (self.results_parsed[0] / KILOBYTE_CONST) * 100 

    def temperature(self):
        self.read_raw()
        if self.mode is 0:
            return (self.results_parsed[1] / KILOBYTE_CONST) * AHT_TEMPERATURE_CONST - AHT_TEMPERATURE_OFFSET
        else:
            return ((self.results_parsed[1] / KILOBYTE_CONST) * AHT_TEMPERATURE_CONST - AHT_TEMPERATURE_OFFSET) * FARENHEIT_MULTIPLIER + FARENHEIT_OFFSET

    def set_mode(self, mode):
        if mode==0 or mode==1:
            self.mode = mode
        else:    
            raise ValueError('Mode must be either 0 for Celsius or 1 Farenheit')
