import os
import utime
import micropython
import ujson
from driver import GPIO
micropython.alloc_emergency_exception_buf(100)

class IRREMOTE(object):
    CODE = {207: "1", 231: "2", 133: "3", 239: "4", 199: "5", 165: "6", 189: "7", 181: "8", 173: "9",
            151: "0", 221: "PREV", 253: "NEXT", 31: "VOL-", 87: "VOL+", 103: "100+", 79: "200+", 61: "START/STOP"}

    def __init__(self, gpioobj):
        self.irrecv = None
        if not isinstance(gpioobj, GPIO):
            raise ValueError("parameter is not a GPIO object")

        self.irrecv = gpioobj
        self.irrecv.on(self.__irProcess)

        self.ir_step = 0
        self.ir_count = 0
        self.buf64 = [0 for i in range(64)]
        self.recived_ok = False
        self.cmd = None
        self.cmd_last = None
        self.repeat = 0
        self.repeat_last = None
        self.t_ok = None
        self.t_ok_last = None
        self.start = 0
        self.start_last = 0
        self.changed = False

    def __irProcess(self, source):
        thisComeInTime = utime.ticks_us()

        # 更新时间
        curtime = utime.ticks_diff(thisComeInTime, self.start)
        self.start = thisComeInTime
        if curtime >= 8500 and curtime <= 9500:
            print(1,curtime)
            self.ir_step = 1
            return

        if self.ir_step == 1:
            print(2,curtime)
            if curtime >= 4000 and curtime <= 5000:
                self.ir_step = 2
                self.recived_ok = False
                self.ir_count = 0
                self.repeat = 0
            elif curtime >= 2000 and curtime <= 3000:  # 长按重复接收
                self.ir_step = 3
                self.repeat += 1

        elif self.ir_step == 2:  # 接收4个字节
            self.buf64[self.ir_count] = curtime
            self.ir_count += 1
            if self.ir_count >= 64:
                self.recived_ok = True
                self.t_ok = self.start #记录最后ok的时间
                self.ir_step = 0

        elif self.ir_step == 3:  # 重复
            if curtime >= 500 and curtime <= 650:
                self.repeat += 1

    def __check_cmd(self):
        byte4 = 0
        for i in range(32):
            x = i * 2
            t = self.buf64[x] + self.buf64[x+1]
            byte4 <<= 1
            if t >= 1800 and t <= 2800:
                byte4 += 1
        user_code_hi = (byte4 & 0xff000000) >> 24
        user_code_lo = (byte4 & 0x00ff0000) >> 16
        data_code = (byte4 & 0x0000ff00) >> 8
        data_code_r = byte4 & 0x000000ff
        self.cmd = data_code_r
        print(self.buf64)

    def getRemoteObject(self):
        return self.buf64

    def getRemoteChar(self):
        # 接收到数据
        if self.recived_ok:
            self.__check_cmd()
            self.recived_ok = False

        #数据有变化
        if self.cmd != self.cmd_last:
            self.changed = True
        else:
            self.changed = False

        #更新
        self.cmd_last = self.cmd
        self.repeat_last = self.repeat
        self.t_ok_last = self.t_ok
        #对应按钮字符
        s = self.CODE.get(self.cmd)
        return self.changed, s
