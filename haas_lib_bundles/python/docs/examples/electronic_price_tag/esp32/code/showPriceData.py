
"""ESP2in9b v.2."""
from time import sleep
from machine import SPI, Pin
# from driver import SPI
from xglcd_font import XglcdFont
from esp2in9bv2 import Display

# #############################################################################
# ###############        HaaS Python price tag     ############################
# #############################################################################
# ##|              |####|                        |##|    OFFERTA      |########
# ##| product pic  |####|  ￥ 100.00 (sal)       |##|    -30%         |########
# ##| (url)        |####|            (name)      |##|   (offerta)     |########
# #############################################################################
# ---85pixel---                   ---135pixel---                  ---76pixel---

class ShowData():

    # init hw params
    def __init__(self, baudrate, dc, cs, rst, busy):
        self.BAUDRATE = baudrate
        # self.SCK = sck
        # self.MOSI = mosi
        self.DC = dc
        self.CS = cs
        self.RST = rst
        self.BUSY = busy
        # self.epaperSPI = SPI()
        # self.epaperSPI.open("epaper")

    def show(self, name='AliOS-Things', sel='$ 108.50', offerta='-30%', byteBuf=bytearray(b'')):
        # init epaper
        spi = SPI(2, baudrate=self.BAUDRATE, sck=Pin(18), mosi=Pin(23))
        display = Display(spi, dc=self.DC, cs=self.CS, rst=self.RST, busy=self.BUSY)
        # display = Display(spi=self.epaperSPI, dc=self.DC, cs=self.CS, rst=self.RST, busy=self.BUSY)

        # start draw Banner USE red color
        display.fill_rectangle(100, 0, 28, 296, red=True)
        # load fonts and draw Banner font
        unispace = XglcdFont('data/pyamp/fonts/Unispace12x24.c', 12, 24)
        text_width = unispace.measure_text("HaaS Python price tag")
        # print(text_width)
        display.draw_text(100, (296-text_width)//2, "HaaS Python price tag", unispace, red=True, invert=True, rotate=90)

        # start draw offerta Banner use black color
        display.fill_rectangle(0, 220, 100, 76)
        # load fonts and draw banner fonts
        ArcadePix = XglcdFont('data/pyamp/fonts/ArcadePix9x11.c', 9, 11)
        text_width = ArcadePix.measure_text("OFFERTA")
        # print(text_width)
        display.draw_text(70, (220+(76-text_width)//2), "OFFERTA", ArcadePix, invert=True, rotate=90)

        # start draw Split line
        display.fill_rectangle(30, 85, 5, 135)
        # load fonts
        # change name to ArcadePix
        # wendy = XglcdFont('fonts/Wendy7x8.c', 7, 8)
        # text_width = wendy.measure_text(name)
        # print(text_width)
        # display.draw_text(13, (220-text_width-4), name, wendy, rotate=90)
        text_width = ArcadePix.measure_text(name)
        # print(text_width)
        display.draw_text(13, (220-text_width-4), name, ArcadePix, rotate=90)

        # start load Product pic
        # this pic can update by cloud push
        if byteBuf != bytearray(b''):
            display.draw_bitmap(0, 0, 85, 85, bytebuf=byteBuf, rotate=90)
        else:
            display.draw_bitmap(0, 0, 85, 85, path="data/pyamp/images/alios.mono", rotate=90)

        # start load price
        # this price can update by cloud push
        espresso = XglcdFont('data/pyamp/fonts/EspressoDolce18x24.c', 18, 24)
        text_width = espresso.measure_text(sel)
        # print(text_width)
        display.draw_text(50, 110, sel, espresso, rotate=90)

        # start load offerta price
        espresso = XglcdFont('data/pyamp/fonts/EspressoDolce18x24.c', 18, 24)
        text_width = espresso.measure_text(offerta)
        # print(text_width)
        display.draw_text(30, 230, offerta, espresso, invert=True, rotate=90)

        # display show
        display.present()

        # sleep(10)
        # display.sleep()
        sleep(5)
        # display.clear()
        display.sleep()
        print('Done.')

    def close(self):
        display = Display(self.epaperSPI, dc=self.DC, cs=self.CS, rst=self.RST, busy=self.BUSY)
        display.cleanup()
        print('clean and close SPI')
