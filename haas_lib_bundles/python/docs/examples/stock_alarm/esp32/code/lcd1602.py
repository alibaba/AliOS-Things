import utime

# 命令标志(配合后面的参数传递)
LCD_CLEARDISPLAY = 0x01
LCD_RETURNHOME = 0x02
LCD_ENTRYMODESET = 0x04
LCD_DISPLAYCONTROL = 0x08
LCD_CURSORSHIFT = 0x10
LCD_FUNCTIONSET = 0x20
LCD_SETCGRAMADDR = 0x40
LCD_SETDDRAMADDR = 0x80

# 打印项标志
# 光标移动模式(打印时)
LCD_ENTRY_DEC = 0x00 # Decrease 模式，光标向左减少，即从右到左打印
LCD_ENTRY_INC = 0x02 # Increase 模式，光标向右增加，即从左到右打印
# 文字移动模式(打印时)
LCD_ENTRY_SHIFT = 0x01  # 文字向左移动
LCD_ENTRY_NOSHIFT = 0x00  # 文字不移动

# 显示开关、光标开关、光标闪烁开关标志
LCD_DISPLAYON = 0x04
LCD_DISPLAYOFF = 0x00
LCD_CURSORON = 0x02
LCD_CURSOROFF = 0x00
LCD_BLINKON = 0x01
LCD_BLINKOFF = 0x00

# 屏幕移动标志
LCD_DISPLAYMOVE = 0x08
LCD_MOVERIGHT = 0x04
LCD_MOVELEFT = 0x00

# 功能设置标志
LCD_8BITMODE = 0x10
LCD_4BITMODE = 0x00
LCD_2LINE = 0x08
LCD_1LINE = 0x00
LCD_5x10DOTS = 0x04
LCD_5x8DOTS = 0x00

class LCD1602(object):
    def __init__(self, RS, RW, E, D4, D5, D6, D7):
        self.RS = RS
        self.RW = RW
        self.E  = E

        self.D4 = D4
        self.D5 = D5
        self.D6 = D6
        self.D7 = D7

        if not self.D4 or not self.D5 or not self.D6 or not self.D7:
            raise Exception('should give d4-d7 pins')
        if not self.RS or not self.E:
            raise Exception('should give RS E pins')

        self._displayfunction = 0
        self._displaycontrol = 0
        self._displaymode = 0

        self.init()

    def init(self):
        # 等待 4.5v 上电
        utime.sleep_ms(50)
        # 参考初始化时序，RS RW 拉低
        self.RS.write(0)
        self.RW.write(0)
        # 第一次发送初始化指令
        self.write4bits(0x03)
        utime.sleep_us(4500)  # 等待至少 4.1ms

        # 第二次发送初始化指令
        self.write4bits(0x03)
        utime.sleep_us(4500)  # 等待至少 100us

        # 第三次发送初始化指令
        self.write4bits(0x03)
        utime.sleep_us(150)

        # 设置屏幕为 4bits 接线模式
        self.write4bits(0x02)

        self._displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS
        # 功能设置，默认4bits模式，2行，每个字符5x8点阵
        self.command(LCD_FUNCTIONSET | self._displayfunction)

        # 设置显示模式，默认为显示、无光标、无光标闪烁
        self._displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF
        self.display()

        # 清空屏幕指令
        self.clear()

        # 设置打印项模式 LCD_ENTRY_INC 光标向右移动, LCD_ENTRY_NOSHIFT 打印文字不移动
        self._displaymode = LCD_ENTRY_INC | LCD_ENTRY_NOSHIFT
        # set the entry mode
        self.command(LCD_ENTRYMODESET | self._displaymode)

    # ## 一些简单的命令封装
    # 清空屏幕，设置指针到左上角，并重置 display 左右偏移，并重置 display 方向
    def clear(self):
        self.command(LCD_CLEARDISPLAY)
        utime.sleep_us(2000)

    # 指针回到左上角起始位置，scrollDisplayLeft/Right 也会 rest
    def home(self):
        self.command(LCD_RETURNHOME)
        utime.sleep_us(2000)

    # 设置当前指针位置，支持 16x2
    def setCursor(self, row, col):
        rowArr = [0x00, 0x40]
        if row >= 2: row = 1
        if col >= 16: col = 15

        self.command(LCD_SETDDRAMADDR | (col + rowArr[row]))

    # 临时关闭显示，再次 display 时内容重新绘制
    def noDisplay(self):
        self._displaycontrol &= ~LCD_DISPLAYON
        self.command(LCD_DISPLAYCONTROL | self._displaycontrol)

    # 显示绘制内容
    def display(self):
        self._displaycontrol |= LCD_DISPLAYON
        self.command(LCD_DISPLAYCONTROL | self._displaycontrol)

    # 关闭光标显示
    def noCursor(self):
        self._displaycontrol &= ~LCD_CURSORON
        self.command(LCD_DISPLAYCONTROL | self._displaycontrol)

    # 开启光标显示，即一个下划线，表示下次字符绘制起始位置
    def cursor(self):
        self._displaycontrol |= LCD_CURSORON
        self.command(LCD_DISPLAYCONTROL | self._displaycontrol)

    # 关闭闪烁光标
    def noBlink(self):
        self._displaycontrol &= ~LCD_BLINKON
        self.command(LCD_DISPLAYCONTROL | self._displaycontrol)

    # 开启闪烁光标
    def blink(self):
        self._displaycontrol |= LCD_BLINKON
        self.command(LCD_DISPLAYCONTROL | self._displaycontrol)

    # 将整个屏幕显示内容向左滚动一列
    def scrollDisplayLeft(self):
        self.command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT)

    # 将整个屏幕显示内容向右滚动一列
    def scrollDisplayRight(self):
        self.command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT)

    # 显示模式从左到右，print字符串时会按照这个方向打印
    def leftToRight(self):
        self._displaymode |= LCD_ENTRY_INC
        self.command(LCD_ENTRYMODESET | self._displaymode)

    # 显示模式从右到左，print字符串时会按照这个方向打印
    def rightToLeft(self):
        self._displaymode &= ~LCD_ENTRY_INC
        self.command(LCD_ENTRYMODESET | self._displaymode)

    # 开启该状态后，文字打印时同时向左滚动，当前 cursor 位置保持屏幕不变
    def autoscroll(self):
        self._displaymode |= LCD_ENTRY_SHIFT
        self.command(LCD_ENTRYMODESET | self._displaymode)

    # 关闭该状态时，文字打印正常，光标向右移动
    def noAutoscroll(self):
        self._displaymode &= ~LCD_ENTRY_NOSHIFT
        self.command(LCD_ENTRYMODESET | self._displaymode)

    # 命令发送，value 为一个字节大小
    def command(self, value):
        self.send(value, 0)

    # 数据发送，value 为一个字节大小
    def write(self, value):
        self.send(value, 1)
        return 1  # assume success

    # 发送数据，底层封装，value 为一个字节大小
    def send(self, value, mode):
        self.RS.write(mode)

        # RW 拉低表示写
        self.RW.write(0)

        self.write4bits(value >> 4)
        self.write4bits(value)

    # 目前只支持 4 Data 线接入方法
    def write4bits(self, value):
        ret = 0
        ret += self.D4.write(value >> 0 & 0x01)
        ret += self.D5.write(value >> 1 & 0x01)
        ret += self.D6.write(value >> 2 & 0x01)
        ret += self.D7.write(value >> 3 & 0x01)
        if ret != 0:
            print('!!ERROR write4bits ret %s' % ret)

        self.pulseEnable()

    # E pin 产生下降沿，表示数据已准备好通知 LCD 读取
    def pulseEnable(self):
        self.E.write(0)
        utime.sleep_us(1)
        self.E.write(1)
        utime.sleep_us(1)  # 产生下降沿时，高电平保持(Enable Pules Width ( High level )) >450 ns
        self.E.write(0)
        # 数据保持时间(Data Hold Time) >10ns，但是写指令时间至少 40us （见datasheet，按照 osc 250Hz计算）
        # 等待指令整体完成，这样就省略 busy flag 检查了
        utime.sleep_us(100)

    # 打印字符
    def print(self, text):
        n = 0
        for idx in range(len(text)):
            if self.write(ord(text[idx])): n += 1
            else: break

        return n

    # 在指定位置打印字符
    def displayString(self, row, col, text):
        self.setCursor(row, col)
        return self.print(text)

def main():
    from driver import GPIO
    print('GPIO init begin')
    RS = GPIO(); RS.open('RS')
    RW = GPIO(); RW.open('RW')
    E = GPIO(); E.open('E')
    D4 = GPIO(); D4.open('D4')
    D5 = GPIO(); D5.open('D5')
    D6 = GPIO(); D6.open('D6')
    D7 = GPIO(); D7.open('D7')
    print('GPIO init done')

    drv = LCD1602(RS, RW, E, D4, D5, D6, D7)
    drv.init()
    utime.sleep(1)
    drv.print('Hello')

    drv.print(' HaaS')
    return drv

if __name__ == '__main__':
    main()
