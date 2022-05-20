from driver import I2C
from driver import GPIO
from time import sleep
from uasyncio import run


class Gesture:
    WAVE = 0
    UP = 1
    DOWN = 2
    LEFT = 4
    RIGHT = 8
    FORWARD = 16
    BACKWARD = 32
    CLOCKWISE = 64
    ANTICLOCKWISE = 128

    _REG_INIT_LIST = [
        [0xEF, 0x00],
        [0x41, 0x00], [0x42, 0x00], [0x37, 0x07], [0x38, 0x17],
        [0x39, 0x06], [0x42, 0x01], [0x46, 0x2D], [0x47, 0x0F],
        [0x48, 0x3C], [0x49, 0x00], [0x4A, 0x1E], [0x4C, 0x22],
        [0x51, 0x10], [0x5E, 0x10], [0x60, 0x27], [0x80, 0x42],
        [0x81, 0x44], [0x82, 0x04], [0x8B, 0x01], [0x90, 0x06],
        [0x95, 0x0A], [0x96, 0x0C], [0x97, 0x05], [0x9A, 0x14],
        [0x9C, 0x3F], [0xA5, 0x19], [0xCC, 0x19], [0xCD, 0x0B],
        [0xCE, 0x13], [0xCF, 0x64], [0xD0, 0x21],

        [0xEF, 0x01],
        [0x02, 0x0F], [0x03, 0x10], [0x04, 0x02], [0x25, 0x01],
        [0x27, 0x39], [0x28, 0x7F], [0x29, 0x08], [0x3E, 0xFF],
        [0x5E, 0x3D], [0x65, 0x96], [0x67, 0x97], [0x69, 0xCD],
        [0x6A, 0x01], [0x6D, 0x2C], [0x6E, 0x01], [0x72, 0x01],
        [0x73, 0x35], [0x74, 0x00], [0x77, 0x01],

        [0xEF, 0x00],
        [0x41, 0xFF], [0x42, 0x01],

        [0xEF, 0x00],
        [0x41, 0x00], [0x42, 0x00], [0x48, 0x3C], [0x49, 0x00],
        [0x51, 0x10], [0x83, 0x20], [0x9f, 0xf9],

        [0xEF, 0x01],
        [0x01, 0x1E], [0x02, 0x0F], [0x03, 0x10], [0x04, 0x02],
        [0x41, 0x40], [0x43, 0x30], [0x65, 0x96], [0x66, 0x00],
        [0x67, 0x97], [0x68, 0x01], [0x69, 0xCD], [0x6A, 0x01],
        [0x6b, 0xb0], [0x6c, 0x04], [0x6D, 0x2C], [0x6E, 0x01],
        [0x74, 0x00],

        [0xEF, 0x00],
        [0x41, 0xFF], [0x42, 0x01]
    ]

    def __init__(self, i2c_name, irq_name, handler_func_map) -> None:
        self.i2c = I2C()
        ret = self.i2c.open(i2c_name)
        if ret < 0:
            raise Exception("Error open I2C ", i2c_name, " ret=", ret)
        self.irq = GPIO()
        ret = self.irq.open(irq_name)
        if ret < 0:
            raise Exception("Error open GPIO ", irq_name, " as IQR:", ret)
        self.handlerFuncMap = handler_func_map
        sleep(0.01)
        self._select_bank(0)
        self._select_bank(0)
        reg0 = self._read_reg(0)
        if reg0 != 0x20:
            raise Exception("Unexpected value", reg0, " in wake-up reg:0x00")
        for rvPair in self._REG_INIT_LIST:
            self._write_reg(rvPair)
        self.irq.on(self._irq_handler)

    def _select_bank(self, bank) -> None:
        ret = self.i2c.write(bytearray([0xEF, bank]))
        if ret < 0:
            raise Exception("selectBank: write data error, ret=", ret)

    def _write_reg(self, pair) -> None:
        ret = self.i2c.write(bytearray([pair[0], pair[1]]))
        if ret < 0:
            raise Exception("selectBank: write data error, ret=", ret)

    def _read_reg(self, addr) -> int:
        ret = self.i2c.write(bytearray([addr]))
        if ret < 0:
            raise Exception("readReg: write addr error, ret=", ret)
        value = bytearray(1)
        ret = self.i2c.read(value)
        if ret < 0:
            raise Exception("readReg: read data error, ret=", ret)
        return value[0]

    async def _cb(self, func):
        func()

    def _irq_handler(self, data) -> None:
        reg43 = self._read_reg(0x43)
        if reg43 == Gesture.WAVE:
            reg44 = self._read_reg(0x44)
            if reg44 != 0x01:
                return
        func = self.handlerFuncMap.get(reg43)
        if callable(func):
            run(self._cb(func))
