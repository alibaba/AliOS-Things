from micropython import const
import utime
import framebuf
from driver import SPI
from driver import GPIO


# a few register definitions
_SET_CONTRAST = const(0x81)
_SET_NORM_INV = const(0xa6)
_SET_DISP = const(0xae)
_SET_SCAN_DIR = const(0xc0)
_SET_SEG_REMAP = const(0xa0)
_LOW_COLUMN_ADDRESS = const(0x00)
_HIGH_COLUMN_ADDRESS = const(0x10)
_SET_PAGE_ADDRESS = const(0xB0)


# register definitions
SET_CONTRAST        = const(0x81)
SET_ENTIRE_ON       = const(0xa4)
SET_NORM_INV        = const(0xa6)
SET_DISP            = const(0xae)
SET_MEM_ADDR        = const(0x20)
SET_COL_ADDR        = const(0x21)
SET_PAGE_ADDR       = const(0x22)
SET_DISP_START_LINE = const(0x40)
SET_SEG_REMAP       = const(0xa0)
SET_MUX_RATIO       = const(0xa8)
SET_COM_OUT_DIR     = const(0xc0)
SET_DISP_OFFSET     = const(0xd3)
SET_COM_PIN_CFG     = const(0xda)
SET_DISP_CLK_DIV    = const(0xd5)
SET_PRECHARGE       = const(0xd9)
SET_VCOM_DESEL      = const(0xdb)
SET_CHARGE_PUMP     = const(0x8d)

# dc 同步 led试试

class SH1106:
    def __init__(self, width, height, external_vcc):
        self.width = width
        self.height = height
        self.external_vcc = external_vcc
        self.pages = self.height // 8
        self.buffer = bytearray(self.pages * self.width)
        fb = framebuf.FrameBuffer(self.buffer, self.width, self.height, framebuf.MVLSB)
        self.framebuf = fb
# set shortcuts for the methods of framebuf
        self.fill = fb.fill
        self.fill_rect = fb.fill_rect
        self.hline = fb.hline
        self.vline = fb.vline
        self.line = fb.line
        self.rect = fb.rect
        self.pixel = fb.pixel
        self.scroll = fb.scroll
        # self.text = fb.text
        self.blit = fb.blit

        print("init done")

        self.init_display()

    def init_display(self):
        self.reset()
        for cmd in (
            SET_DISP | 0x00,    # 关闭显示
            SET_DISP_CLK_DIV, 0x80, # 设置时钟分频因子
            SET_MUX_RATIO, self.height - 1, # 设置驱动路数 路数默认0x3F（1/64）
            SET_DISP_OFFSET, 0x00,  # 设置显示偏移 偏移默认为0
            SET_DISP_START_LINE | 0x00, # 设置显示开始行[5:0]
            SET_CHARGE_PUMP, 0x14, # 电荷泵设置 bit2，开启/关闭
            SET_MEM_ADDR, 0x02, # 设置内存地址模式 [1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
            SET_SEG_REMAP | 0x01, # 段重定义设置,bit0:0,0->0;1,0->127;
            SET_COM_OUT_DIR | 0x08, # 设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
            SET_COM_PIN_CFG, 0x12, # 设置COM硬件引脚配置   [5:4]配置
            SET_PRECHARGE, 0xf1, # 设置预充电周期 [3:0],PHASE 1;[7:4],PHASE 2;
            SET_VCOM_DESEL, 0x30,  # 设置VCOMH 电压倍率 [6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
            SET_CONTRAST, 0xff, # 对比度设置 默认0x7F（范围1~255，越大越亮）
            SET_ENTIRE_ON, # 全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
            SET_NORM_INV, # 设置显示方式;bit0:1,反相显示;0,正常显示
            SET_DISP | 0x01): #  开启显示
            self.write_cmd(cmd)
        self.fill(1)
        self.show()

    def poweroff(self):
        self.write_cmd(_SET_DISP | 0x00)

    def poweron(self):
        self.write_cmd(_SET_DISP | 0x01)

    def rotate(self, flag, update=True):
        if flag:
            self.write_cmd(_SET_SEG_REMAP | 0x01)  # mirror display vertically
            self.write_cmd(_SET_SCAN_DIR | 0x08)  # mirror display hor.
        else:
            self.write_cmd(_SET_SEG_REMAP | 0x00)
            self.write_cmd(_SET_SCAN_DIR | 0x00)
        if update:
            self.show()

    def sleep(self, value):
        self.write_cmd(_SET_DISP | (not value))

    def contrast(self, contrast):
        self.write_cmd(_SET_CONTRAST)
        self.write_cmd(contrast)

    def invert(self, invert):
        self.write_cmd(_SET_NORM_INV | (invert & 1))

    def show(self):
        for page in range(self.height // 8):
            self.write_cmd(_SET_PAGE_ADDRESS | page)
            self.write_cmd(_LOW_COLUMN_ADDRESS)
            self.write_cmd(_HIGH_COLUMN_ADDRESS)
            page_buffer = bytearray(self.width)
            for i in range(self.width):
                page_buffer[i] = self.buffer[self.width * page + i]
            self.write_data(page_buffer)

    def reset(self, res):
        if res is not None:
            res.write(1)
            utime.sleep_ms(1)
            res.write(0)
            utime.sleep_ms(20)
            res.write(1)
            utime.sleep_ms(20)


class SH1106_I2C(SH1106):
    def __init__(self, width, height, i2c, res=None, addr=0x3c,
                 external_vcc=False):
        self.i2c = i2c
        self.addr = addr
        self.res = res
        self.temp = bytearray(2)
        if res is not None:
            res.init(res.OUT, value=1)
        super().__init__(width, height, external_vcc)

    def write_cmd(self, cmd):
        self.temp[0] = 0x80  # Co=1, D/C#=0
        self.temp[1] = cmd
        self.i2c.writeto(self.addr, self.temp)

    def write_data(self, buf):
        self.i2c.writeto(self.addr, b'\x40'+buf)

    def reset(self):
        super().reset(self.res)


class SH1106_SPI(SH1106):
    def __init__(self, width, height, spi, dc, res=None, cs=None,
                 external_vcc=False):
        # self.rate = 10 * 1000 * 1000
        # dc.init(dc.OUT, value=0)
        # if res is not None:
        #     res.init(res.OUT, value=0)
        # if cs is not None:
        #     cs.init(cs.OUT, value=1)
        self.spi = spi
        self.dc = dc
        # if res is not None:
        self.res = res
        # if cs is not None:
        self.cs = cs
        super().__init__(width, height, external_vcc)

    def write_cmd(self, cmd):
        # self.spi.init(baudrate=self.rate, polarity=0, phase=0)
        if self.cs is not None:
            self.cs.write(1)
            self.dc.write(0)
            self.cs.write(0)
            self.spi.write(bytearray([cmd]))
            self.cs.write(1)
        else:
            self.dc.write(0)
            self.spi.write(bytearray([cmd]))

    def write_data(self, buf):
        # self.spi.init(baudrate=self.rate, polarity=0, phase=0)
        if self.cs is not None:
            self.cs.write(1)
            self.dc.write(1)
            self.cs.write(0)
            self.spi.write(buf)
            self.cs.write(1)
        else:
            self.dc.write(1)
            self.spi.write(buf)            

    def reset(self):
        super().reset(self.res)

    def test(self):
        while True:
            self.fill(1)
            self.show()

            self.fill(0)
            self.show()
