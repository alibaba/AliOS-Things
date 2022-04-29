import time
from driver import I2C

class TEA5767:
    FREQ_RANGE = (87.5, 108.0)
    ADC = (0, 5, 7, 10)
    ADC_BIT = (0, 1, 2, 3)
    def __init__(self, i2cDev):
        self._i2c = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        self._i2c = i2cDev
        self.frequency = 0.0
        self.standby_mode = False
        self.mute_mode = False
        self.soft_mute_mode = True
        self.search_mode = False
        self.search_direction = 1
        self.search_adc_level = 7
        self.stereo_mode = True
        self.stereo_noise_cancelling_mode = True
        self.high_cut_mode = True
        self.is_ready = False
        self.is_stereo = False
        self.signal_adc_level = 0
        self.update()

    def setFrequency(self, freq):
        self.frequency = freq
        self.update()

    def changeFreqency(self, change):
        self.frequency += change
        self.search_direction = 1 if change >= 0 else 0
        self.update()

    def search(self, mode, dir=1, adc=7):
        self.search_mode = mode
        self.search_direction = dir
        self.search_adc_level = adc if adc in TEA5767.ADC else 7
        self.update()

    def mute(self, mode):
        self.mute_mode = mode
        self.update()

    def standby(self, mode):
        self.standby_mode = mode
        self.update()

    def read(self):
        buf = bytearray(5)
        self._i2c.read(buf)
        freqB = int((buf[0] & 0x3f) << 8 | buf[1])
        self.frequency = round((freqB * 32768 / 4 - 225000) / 1000000, 1)
        self.is_ready = int(buf[0] >> 7) == 1
        self.is_stereo = int(buf[2] >> 7) == 1
        self.signal_adc_level = int(buf[3] >> 4)

    def update(self):
        self.frequency = min(max(self.frequency, TEA5767.FREQ_RANGE[0]), TEA5767.FREQ_RANGE[1])
        freqB = 4 * (self.frequency * 1000000 + 225000) / 32768
        buf = bytearray(5)
        buf[0] = int(freqB) >> 8 | self.mute_mode << 7 | self.search_mode << 6
        buf[1] = int(freqB) & 0xff
        buf[2] = self.search_direction << 7 | 1 << 4 | self.stereo_mode << 3
        try:
            buf[2] += TEA5767.ADC_BIT[TEA5767.ADC.index(self.search_adc_level)] << 5
        except:
            pass
        buf[3] = self.standby_mode << 6 | 0 << 5 | 1 << 4
        buf[3] += self.soft_mute_mode << 3 | self.high_cut_mode << 2 | self.stereo_noise_cancelling_mode << 1
        buf[4] = 0
        self._i2c.write(buf)
        time.sleep_ms(50)
        self.read()
