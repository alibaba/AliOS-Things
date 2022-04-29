# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    读取HaaS EDU K1硬件和固件版本号，并显示在屏幕上
@Author     :    ethan.lcz
@version    :    1.0
'''
import uos
import utime                          # 延时API所在组件
from haaseduk1 import HAASEDUK1      # 引入haaseduk1库，目标用于区分K1版本
from driver import SPI
from driver import GPIO
import sh1106
import framebuf

oled = None

# OLED初始化
def oledInit():
    global oled

    # 字库文件存放于项目目录 font, 注意若用到了中英文字库则都需要放置
    framebuf.set_font_path(framebuf.FONT_HZK12, '/data/font/HZK12')
    framebuf.set_font_path(framebuf.FONT_HZK16, '/data/font/HZK16')
    framebuf.set_font_path(framebuf.FONT_HZK24, '/data/font/HZK24')
    framebuf.set_font_path(framebuf.FONT_HZK32, '/data/font/HZK32')
    framebuf.set_font_path(framebuf.FONT_ASC12_8, '/data/font/ASC12_8')
    framebuf.set_font_path(framebuf.FONT_ASC16_8, '/data/font/ASC16_8')
    framebuf.set_font_path(framebuf.FONT_ASC24_12, '/data/font/ASC24_12')
    framebuf.set_font_path(framebuf.FONT_ASC32_16, '/data/font/ASC32_16')

    oled_spi = SPI()
    oled_spi.open("oled_spi")

    oled_res = GPIO()
    oled_res.open("oled_res")

    oled_dc = GPIO()
    oled_dc.open("oled_dc")

    #oled像素132*64
    oled = sh1106.SH1106_SPI(132, 64, oled_spi, oled_dc, oled_res)

# OLED显示
# text:显示的文本
# x:水平坐标 y:垂直坐标
# color:颜色
# clear: True-清屏显示 False-不清屏显示
# sz:字体大小
def oledShowText(text, x, y, color, clear, sz):
    global oled
    if clear:
        oled.fill(0) # 清屏
    oled.text(text, x, y, color, size = sz)
    oled.show()

if __name__ == '__main__':
    fwVersion = 'K1 FW - ' + uos.version()[11:]
    board = HAASEDUK1()                  # 新建HAASEDUK1对象
    hwID = board.getHWID()               # 获取开发板ID
    print ("hwID:", hwID)
    hwVersion = "K1 HW - v1."+'01'[hwID]
    oledInit()

    while True:
        oledShowText(hwVersion, 6, 20, 1, True, 12)
        oledShowText(fwVersion, 6, 40, 1, False, 12)
        utime.sleep(10)                   # 每隔1秒钟进行一次温湿度信息测量和打印
