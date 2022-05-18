from driver import I2C

CSS811_STATUS = 0x00
CSS811_MEAS_MODE = 0x01
CSS811_ALG_RESULT_DATA = 0x02
CSS811_RAW_DATA = 0x03
CSS811_ENV_DATA = 0x05
CSS811_NTC = 0x06
CSS811_THRESHOLDS = 0x10
CSS811_BASELINE = 0x11
CSS811_HW_ID = 0x20
CSS811_HW_VERSION = 0x21
CSS811_FW_BOOT_VERSION = 0x23
CSS811_FW_APP_VERSION = 0x24
CSS811_ERROR_ID = 0xE0
CSS811_APP_START = 0xF4
CSS811_SW_RESET = 0xFF

class CCS811:
    def __init__(self,i2cDev):
        self._i2c = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        self._i2c = i2cDev
        self.tVOC = 0
        self.CO2 = 0
        self.setup()

    def print_error(self):
        buf = bytearray(1)
        self._i2c.memRead(buf,CSS811_ERROR_ID,8)
        error = buf[0]

        message = 'Error: '
        if error & 1 << 5:
            message += 'HeaterSupply '
        elif error & 1 << 4:
            message += 'HeaterFault '
        elif error & 1 << 3:
            message += 'MaxResistance '
        elif error & 1 << 2:
            message += 'MeasModeInvalid '
        elif error & 1 << 1:
            message += 'ReadRegInvalid '
        elif error & 1 << 0:
            message += 'MsgInvalid '

        print(message)

    def check_for_error(self):
        buf = bytearray(1)
        self._i2c.memRead(buf,CSS811_STATUS,8)
        value = buf[0]

        return value & 1 << 0

    def app_valid(self):
        buf = bytearray(1)
        self._i2c.memRead(buf,CSS811_STATUS,8)
        value = buf[0]

        return value & 1 << 4

    def set_drive_mode(self, mode):
        if mode > 4:
            mode = 4

        setting = bytearray(1)
        self._i2c.memRead(setting,CSS811_MEAS_MODE,8)

        setting[0] &= ~(0b00000111 << 4)
        setting[0] |= (mode << 4)
        self._i2c.memWrite(setting,CSS811_MEAS_MODE,8)

    def configure_ccs811(self):
        hardware_id = bytearray(1)
        self._i2c.memRead(hardware_id,CSS811_HW_ID,8)

        if hardware_id[0] != 0x81:
            raise ValueError('CCS811 not found. Please check wiring.',hardware_id[0])

        if self.check_for_error():
            self.print_error()
            raise ValueError('Error at Startup.')

        if not self.app_valid():
            raise ValueError('Error: App not valid.')

        buf = bytearray(1)
        buf[0] = CSS811_APP_START
        self._i2c.write(buf)

        if self.check_for_error():
            self.print_error()
            raise ValueError('Error at AppStart.')
        self.set_drive_mode(1)
        if self.check_for_error():
            self.print_error()
            raise ValueError('Error at setDriveMode.')

    def setup(self):
        self.configure_ccs811()

    def get_base_line(self):
        b = bytearray(2)
        self._i2c.memRead(b,CSS811_BASELINE,8)

        baselineMSB = b[0]
        baselineLSB = b[1]
        baseline = (baselineMSB << 8) | baselineLSB
        return baseline

    def data_available(self):
        buf = bytearray(1)
        self._i2c.memRead(buf,CSS811_STATUS,8)
        value = buf[0]
        return value & 1 << 3

    def read_logorithm_results(self):
        d = bytearray(4)
        self._i2c.memRead(d,CSS811_ALG_RESULT_DATA,8)
        co2MSB = d[0]
        co2LSB = d[1]
        tvocMSB = d[2]
        tvocLSB = d[3]
        self.CO2 = (co2MSB << 8) | co2LSB
        self.tVOC = (tvocMSB << 8) | tvocLSB

    def geteCO2(self):
        if self.data_available():
            d = bytearray(4)
            self._i2c.memRead(d,CSS811_ALG_RESULT_DATA,8)

            co2MSB = d[0]
            co2LSB = d[1]
            tvocMSB = d[2]
            tvocLSB = d[3]

            self.CO2 = (co2MSB << 8) | co2LSB
            self.tVOC = (tvocMSB << 8) | tvocLSB
            return 1,self.CO2
        else:
            return 0,0

    def getTVOC(self):
        if self.data_available():
            d = bytearray(4)
            self._i2c.memRead(d,CSS811_ALG_RESULT_DATA,8)

            co2MSB = d[0]
            co2LSB = d[1]
            tvocMSB = d[2]
            tvocLSB = d[3]

            self.CO2 = (co2MSB << 8) | co2LSB
            self.tVOC = (tvocMSB << 8) | tvocLSB
            return 1,self.tVOC
        else:
            return 0,0

    def geteCO2TVOC(self):
        if self.data_available():
            d = bytearray(4)
            self._i2c.memRead(d,CSS811_ALG_RESULT_DATA,8)

            co2MSB = d[0]
            co2LSB = d[1]
            tvocMSB = d[2]
            tvocLSB = d[3]

            self.CO2 = (co2MSB << 8) | co2LSB
            self.tVOC = (tvocMSB << 8) | tvocLSB
            return 1,self.CO2,self.tVOC
        else:
            return 0,0,0
