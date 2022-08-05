
import utime as time
import codetab

# Constants
SSD1306_I2C_ADDRESS = 0x3C    # 011110+SA0+RW - 0x3C or 0x3D
SSD1306_SETCONTRAST = 0x81
SSD1306_DISPLAYALLON_RESUME = 0xA4
SSD1306_DISPLAYALLON = 0xA5
SSD1306_NORMALDISPLAY = 0xA6
SSD1306_INVERTDISPLAY = 0xA7
SSD1306_DISPLAYOFF = 0xAE
SSD1306_DISPLAYON = 0xAF
SSD1306_SETDISPLAYOFFSET = 0xD3
SSD1306_SETCOMPINS = 0xDA
SSD1306_SETVCOMDETECT = 0xDB
SSD1306_SETDISPLAYCLOCKDIV = 0xD5
SSD1306_SETPRECHARGE = 0xD9
SSD1306_SETMULTIPLEX = 0xA8
SSD1306_SETLOWCOLUMN = 0x00
SSD1306_SETHIGHCOLUMN = 0x10
SSD1306_SETSTARTLINE = 0x40
SSD1306_MEMORYMODE = 0x20
SSD1306_COLUMNADDR = 0x21
SSD1306_PAGEADDR = 0x22
SSD1306_COMSCANINC = 0xC0
SSD1306_COMSCANDEC = 0xC8
SSD1306_SEGREMAP = 0xA0
SSD1306_CHARGEPUMP = 0x8D
SSD1306_EXTERNALVCC = 0x1
SSD1306_SWITCHCAPVCC = 0x2

# Scrolling constants
SSD1306_ACTIVATE_SCROLL = 0x2F
SSD1306_DEACTIVATE_SCROLL = 0x2E
SSD1306_SET_VERTICAL_SCROLL_AREA = 0xA3
SSD1306_RIGHT_HORIZONTAL_SCROLL = 0x26
SSD1306_LEFT_HORIZONTAL_SCROLL = 0x27
SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29
SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL = 0x2A


class SSD1306Base(object):
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self._pages = height//8
        self._buffer = [0]*(width*self._pages)
        # Handle hardware I2C
        from driver import I2C
        self._i2c=I2C()
        self._i2c.open('OLED')


    def _initialize(self):
        raise NotImplementedError

    def writeCmd(self, command):
        """Send command byte to display."""
        # I2C write.
        control = 0x00   # Co = 0, DC = 0
        # writeBuf=bytearray(2)
        # writeBuf[0]=control
        # writeBuf[1]=command
        # self._i2c.write(writeBuf)

        writeBuf=bytearray(1)
        writeBuf[0]=command
        self._i2c.memWrite(writeBuf,control,8)


    def writeDat(self, data):
        """Send byte of data to display."""
        # I2C write.
        control = 0x40   # Co = 0, DC = 0
        # writeBuf=bytearray(2)
        # writeBuf[0]=control
        # writeBuf[1]=data
        # self._i2c.write(writeBuf)

        writeBuf=bytearray(1)
        writeBuf[0]=data
        self._i2c.memWrite(writeBuf,control,8)

    def begin(self, vccstate=SSD1306_SWITCHCAPVCC):
        """Initialize display."""
        # Save vcc state.
        self._vccstate = vccstate
        # Reset and initialize display.
        # self.reset()
        self._initialize()
        # Turn on the display.
        self.writeCmd(SSD1306_DISPLAYON)

# --------------------------------------------------------------
#  Prototype      : oled_fill(fill_data）
#  Parameters     : fill_data,范围0x00-0xff
#  Description    : 全屏填充，例如 0x00-全黑，0xff全亮
# --------------------------------------------------------------
    def oled_fill(self,fill_data):
        for i in range(8):
            #page0-page1
            self.writeCmd(0xb0+i)
            # low colum start address
            self.writeCmd(0x00)
            #high colum start address
            self.writeCmd(0x10)
        for i in range(128*64):
            self.writeDat(fill_data)  
      
# --------------------------------------------------------------
#  Prototype      : clear()
#  Parameters     : none
#  Description    : 全黑
# --------------------------------------------------------------
    def clear(self):
        self.oled_fill(0x00)



# --------------------------------------------------------------
#  Prototype      : oled_setPos(x,y)
#  Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7)
#  Description    : 设置起始坐标
# --------------------------------------------------------------
    def oled_setPos(self,x,y):
        self.writeCmd(0xb0+y)
        self.writeCmd(((x&0xf0)>>4)|0x10)
        self.writeCmd((x&0x0f)|0x01)

# --------------------------------------------------------------
#  Prototype      : oled_showCN(x,y,n)
#  Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
#  Description    : 显示codetab.py中的汉字,16*16点阵
# --------------------------------------------------------------
    def oled_showCN(self,x,y,n):
        self.oled_setPos(x,y)
        adder=32*n
        for i in range(16):
            self.writeDat(codetab.F1[adder])
            adder+=1
        self.oled_setPos(x,y+1)
        for i in range(16):
            self.writeDat(codetab.F1[adder])
            adder+=1

# --------------------------------------------------------------
#  Prototype      : oled_showStr(x,y,ch,TextSize)
#  Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
#  Description    : 显示codetab.py中的ASCII字符,有6*8和8*16可选择
# --------------------------------------------------------------    

    def oled_showmun(self,x,y,ch,TextSize):
        c=0
        j=0
        if TextSize==1:
            while ch[j]!='\0':
                #ord()将字符转换成十进制，如'a'->97
                c=ch[j]-32
                if x>126:
                    x=0
                    y+=1
                self.oled_setPos(x,y)
                for i in range(6):
                    self.writeDat(codetab.F6x8[c][i]) 
                x+=6
                j+=1
                #防止index out of range 
                if j==len(ch):
                    break
        if TextSize==2:
             while ch[j]!='\0':
                #ord()将字符转换成十进制
                c=ch[j]-32
                if x>120:
                    x=0
                    y+=1
                self.oled_setPos(x,y)
                for i in range(8):
                    self.writeDat(codetab.F8X16[c*16+i]) 
                self.oled_setPos(x,y+1)
                for i in range(8):
                    self.writeDat(codetab.F8X16[c*16+i+8])    
                x+=8
                j+=1    
                #防止index out of range                                    
                if j==len(ch):
                    break


    def oled_showstr(self,x,y,ch,TextSize):
        c2=0
        j=0
        if TextSize==1:
            while ch[j]!='\0':
                #ord()将字符转换成十进制，如'a'->97
                c2=ord(ch[j])-32
                if x>126:
                    x=0
                    y+=1
                self.oled_setPos(x,y)
                for i in range(6):
                    self.writeDat(codetab.F6x8[c2][i]) 
                x+=6
                j+=1
                #防止index out of range 
                if j==len(ch):
                    break
        if TextSize==2:
             while ch[j]!='\0':
                #ord()将字符转换成十进制，如'a'->97
                c2=ord(ch[j])-32
                if x>120:
                    x=0
                    y+=1
                self.oled_setPos(x,y)
                for i in range(8):
                    self.writeDat(codetab.F8X16[c2*16+i]) 
                self.oled_setPos(x,y+1)
                for i in range(8):
                    self.writeDat(codetab.F8X16[c2*16+i+8])    
                x+=8
                j+=1    
                #防止index out of range                                    
                if j==len(ch):
                    break
# # --------------------------------------------------------------
# Prototype      : oled_showPicture(x0,y0,x1,y1,BMP)
# Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:128)
# Description    : 显示BMP位图
# --------------------------------------------------------------
    def oled_showPicture(self,x0,y0,x1,y1,BMP):
        i=0
        if y1%8==0:
            y=y1/8
        else:
            y=y1/8+1
        for y in range(y0,y1):
            self.oled_setPos(x0,y)   
            for x in range(x0,x1):
                self.writeDat(BMP[i])  
                i+=1    
            if i==len(BMP) :
                break

# --------------------------------------------------------------
# Prototype      : set_contrast(contrast)
# Parameters     : coontrast，取值范围为0-255
# Description    : 对比度/亮度调节
# --------------------------------------------------------------    
    def set_contrast(self, contrast):
        if contrast < 0 or contrast > 255:
            raise ValueError('Contrast must be a value from 0 to 255 (inclusive).')
        self.writeCmd(SSD1306_SETCONTRAST)
        self.writeCmd(contrast)


class SSD1306_128_64(SSD1306Base):
    def __init__(self):
        super(SSD1306_128_64, self).__init__(128, 64)

    def _initialize(self):
        # 128x64 pixel specific initialization.
        self.writeCmd(SSD1306_DISPLAYOFF)                    # 0xAE
        self.writeCmd(SSD1306_SETDISPLAYCLOCKDIV)            # 0xD5
        self.writeCmd(0x80)                                  # the suggested ratio 0x80
        self.writeCmd(SSD1306_SETMULTIPLEX)                  # 0xA8
        self.writeCmd(0x3F)
        self.writeCmd(SSD1306_SETDISPLAYOFFSET)              # 0xD3
        self.writeCmd(0x0)                                   # no offset
        self.writeCmd(SSD1306_SETSTARTLINE | 0x0)            # line #0
        self.writeCmd(SSD1306_CHARGEPUMP)                    # 0x8D
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.writeCmd(0x10)
        else:
            self.writeCmd(0x14)
        self.writeCmd(SSD1306_MEMORYMODE)                    # 0x20
        self.writeCmd(0x00)                                  # 0x0 act like ks0108
        self.writeCmd(SSD1306_SEGREMAP | 0x1)
        self.writeCmd(SSD1306_COMSCANDEC)
        self.writeCmd(SSD1306_SETCOMPINS)                    # 0xDA
        self.writeCmd(0x12)
        self.writeCmd(SSD1306_SETCONTRAST)                   # 0x81
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.writeCmd(0x9F)
        else:
            self.writeCmd(0xCF)
        self.writeCmd(SSD1306_SETPRECHARGE)                  # 0xd9
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.writeCmd(0x22)
        else:
            self.writeCmd(0xF1)
        self.writeCmd(SSD1306_SETVCOMDETECT)                 # 0xDB
        self.writeCmd(0x40)
        self.writeCmd(SSD1306_DISPLAYALLON_RESUME)           # 0xA4
        self.writeCmd(SSD1306_NORMALDISPLAY)                 # 0xA6


class SSD1306_128_32(SSD1306Base):
    def __init__(self):
        super(SSD1306_128_32, self).__init__(128, 32)

    def _initialize(self):
        self.writeCmd(SSD1306_DISPLAYOFF)                    # 0xAE
        self.writeCmd(SSD1306_SETDISPLAYCLOCKDIV)            # 0xD5
        self.writeCmd(0x80)                                  # the suggested ratio 0x80
        self.writeCmd(SSD1306_SETMULTIPLEX)                  # 0xA8
        self.writeCmd(0x1F)
        self.writeCmd(SSD1306_SETDISPLAYOFFSET)              # 0xD3
        self.writeCmd(0x0)                                   # no offset
        self.writeCmd(SSD1306_SETSTARTLINE | 0x0)            # line #0
        self.writeCmd(SSD1306_CHARGEPUMP)                    # 0x8D
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.writeCmd(0x10)
        else:
            self.writeCmd(0x14)
        self.writeCmd(SSD1306_MEMORYMODE)                    # 0x20
        self.writeCmd(0x00)                                  # 0x0 act like ks0108
        self.writeCmd(SSD1306_SEGREMAP | 0x1)
        self.writeCmd(SSD1306_COMSCANDEC)
        self.writeCmd(SSD1306_SETCOMPINS)                    # 0xDA
        self.writeCmd(0x02)
        self.writeCmd(SSD1306_SETCONTRAST)                   # 0x81
        self.writeCmd(0x8F)
        self.writeCmd(SSD1306_SETPRECHARGE)                  # 0xd9
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.writeCmd(0x22)
        else:
            self.writeCmd(0xF1)
        self.writeCmd(SSD1306_SETVCOMDETECT)                 # 0xDB
        self.writeCmd(0x40)
        self.writeCmd(SSD1306_DISPLAYALLON_RESUME)           # 0xA4
        self.writeCmd(SSD1306_NORMALDISPLAY)                 # 0xA6


class SSD1306_96_16(SSD1306Base):
    def __init__(self):
        super(SSD1306_96_16, self).__init__(96, 16)

    def _initialize(self):
        self.writeCmd(SSD1306_DISPLAYOFF)                    # 0xAE
        self.writeCmd(SSD1306_SETDISPLAYCLOCKDIV)            # 0xD5
        self.writeCmd(0x60)                                  # the suggested ratio 0x60
        self.writeCmd(SSD1306_SETMULTIPLEX)                  # 0xA8
        self.writeCmd(0x0F)
        self.writeCmd(SSD1306_SETDISPLAYOFFSET)              # 0xD3
        self.writeCmd(0x0)                                   # no offset
        self.writeCmd(SSD1306_SETSTARTLINE | 0x0)            # line #0
        self.writeCmd(SSD1306_CHARGEPUMP)                    # 0x8D
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.writeCmd(0x10)
        else:
            self.writeCmd(0x14)
        self.writeCmd(SSD1306_MEMORYMODE)                    # 0x20
        self.writeCmd(0x00)                                  # 0x0 act like ks0108
        self.writeCmd(SSD1306_SEGREMAP | 0x1)
        self.writeCmd(SSD1306_COMSCANDEC)
        self.writeCmd(SSD1306_SETCOMPINS)                    # 0xDA
        self.writeCmd(0x02)
        self.writeCmd(SSD1306_SETCONTRAST)                   # 0x81
        self.writeCmd(0x8F)
        self.writeCmd(SSD1306_SETPRECHARGE)                  # 0xd9
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.writeCmd(0x22)
        else:
            self.writeCmd(0xF1)
        self.writeCmd(SSD1306_SETVCOMDETECT)                 # 0xDB
        self.writeCmd(0x40)
        self.writeCmd(SSD1306_DISPLAYALLON_RESUME)           # 0xA4
        self.writeCmd(SSD1306_NORMALDISPLAY)                 # 0xA6

