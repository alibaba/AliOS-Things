from driver import GPIO

DS1302_REG_SECOND = (0x80)
DS1302_REG_MINUTE = (0x82)
DS1302_REG_HOUR   = (0x84)
DS1302_REG_DAY    = (0x86)
DS1302_REG_MONTH  = (0x88)
DS1302_REG_WEEKDAY= (0x8A)
DS1302_REG_YEAR   = (0x8C)
DS1302_REG_WP     = (0x8E)
DS1302_REG_CTRL   = (0x90)
DS1302_REG_RAM    = (0xC0)

class DS1302:
    def __init__(self, o_clk, o_rst, b_di, b_do):
        self.clk = o_clk
        self.cs = o_rst
        self.di = b_di
        self.do = b_do
        self.gpiodio = GPIO()
        self.gpiodio.open(self.do)

    def _dectohex(self, dat):
        return (dat//10) * 16 + (dat % 10)

    def _hextodec(self, dat):
        return (dat//16) * 10 + (dat % 16)

    def _writeByte(self, dat):
        for i in range(8):
            self.gpiodio.write((dat >> i) & 1)
            self.clk.write(1)
            self.clk.write(0)

    def _readByte(self):
        d = 0
        self.gpiodio.close()
        self.gpiodio.open(self.di)
        for i in range(8):
            d = d | (self.gpiodio.read() << i)
            self.clk.write(1)
            self.clk.write(0)
        self.gpiodio.close()
        self.gpiodio.open(self.do)
        return d

    def _getReg(self, reg):
        self.cs.write(1)
        self._writeByte(reg)
        t = self._readByte()
        self.cs.write(0)
        return t

    def _setReg(self, reg, dat):
        self.cs.write(1)
        self._writeByte(reg)
        self._writeByte(dat)
        self.cs.write(0)

    def _wr(self, reg, dat):
        self._setReg(DS1302_REG_WP, 0)
        self._setReg(reg, dat)
        self._setReg(DS1302_REG_WP, 0x80)

    def start(self):
        t = self._getReg(DS1302_REG_SECOND + 1)
        self._wr(DS1302_REG_SECOND, t & 0x7f)

    def stop(self):
        t = self._getReg(DS1302_REG_SECOND + 1)
        self._wr(DS1302_REG_SECOND, t | 0x80)

    def second(self, second=None):
        if second == None:
            return self._hextodec(self._getReg(DS1302_REG_SECOND+1)) % 60
        else:
            self._wr(DS1302_REG_SECOND, self._dectohex(second % 60))

    def minute(self, minute=None):
        if minute == None:
            return self._hextodec(self._getReg(DS1302_REG_MINUTE+1))
        else:
            self._wr(DS1302_REG_MINUTE, self._dectohex(minute % 60))

    def hour(self, hour=None):
        if hour == None:
            return self._hextodec(self._getReg(DS1302_REG_HOUR+1))
        else:
            self._wr(DS1302_REG_HOUR, self._dectohex(hour % 24))

    def weekday(self, weekday=None):
        if weekday == None:
            return self._hextodec(self._getReg(DS1302_REG_WEEKDAY+1))
        else:
            self._wr(DS1302_REG_WEEKDAY, self._dectohex(weekday % 8))

    def day(self, day=None):
        if day == None:
            return self._hextodec(self._getReg(DS1302_REG_DAY+1))
        else:
            self._wr(DS1302_REG_DAY, self._dectohex(day % 32))

    def month(self, month=None):
        if month == None:
            return self._hextodec(self._getReg(DS1302_REG_MONTH+1))
        else:
            self._wr(DS1302_REG_MONTH, self._dectohex(month % 13))

    def year(self, year=None):
        if year == None:
            return self._hextodec(self._getReg(DS1302_REG_YEAR+1)) + 2000
        else:
            self._wr(DS1302_REG_YEAR, self._dectohex(year % 100))

    def getDatetime(self):
        return [self.year(), self.month(), self.day(), self.hour(), self.minute(), self.second(),self.weekday()]

    def setDatetime(self, data):
        if data is None:
            raise ValueError("invalid data")
        else:
            self.year(data[0])
            self.month(data[1])
            self.day(data[2])
            self.hour(data[3])
            self.minute(data[4])
            self.second(data[5])
            self.weekday(data[6])

    def ram(self, reg, dat=None):
        if dat == None:
            return self._getReg(DS1302_REG_RAM + 1 + (reg % 31)*2)
        else:
            self._wr(DS1302_REG_RAM + (reg % 31)*2, dat)
