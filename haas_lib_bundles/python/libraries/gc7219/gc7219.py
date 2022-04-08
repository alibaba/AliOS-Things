from driver import SPI, GPIO

# register address map for each GC7219 (opCodes)
OP_NOOP = 0
OP_DIGIT0 = 1 # == one row (byte)
OP_DIGIT1 = 2
OP_DIGIT2 = 3
OP_DIGIT3 = 4
OP_DIGIT4 = 5
OP_DIGIT5 = 6
OP_DIGIT6 = 7
OP_DIGIT7 = 8
OP_DECODEMODE = 9  # no decode == 0 (useful only for 7-segment display)
OP_INTENSITY = 10  # 0 - 15 (different steps for each GC7219)
OP_SCANLIMIT = 11  # all rows == 7 (sets number of active rows / digits)
OP_SHUTDOWN = 12  # turn display on == 1
OP_DISPLAYTEST = 15  # turn all LEDs on == 1

ALL_DISPLAYS = -1

class Bitmap(object):
    def __init__(self, width=8, height=8):
        self.width = width
        self.height = height
        self.array = [0] * (width * height // 8)

    def setPixel(self, x, y, val=True):
        ret = self.containsPixel(x, y)
        if not ret:
            return
        (bytePos, posInByte) = ret
        if val:
            self.array[bytePos] |= 128 >> posInByte
        else:
            self.array[bytePos] &= ~(128 >> posInByte)

    def getPixel(self, x, y):
        ret = self.containsPixel(x, y)
        if not ret:
            return 0
        (bytePos, posInByte) = ret
        return self.array[bytePos] & (128 >> posInByte)

    def containsPixel(self, x, y):
        if x >= self.width or y >= self.height or x < 0 or y < 0:
            return None

        pos1D = self.width * y + x

        posInByte = pos1D % 8

        bytePos = pos1D // 8

        return (bytePos, posInByte)

class Matrix(object):
    # = 8x8 matrix / one chip
    def __init__(self, x, y, rotation, mirrorX, mirrorY):
        # 0, 90, 180, 270
        self.rotation = rotation
        self.mirrorX = mirrorX
        self.mirrorY = mirrorY
        # pixel [0, 0] is [firstX, firstY] for the main bitmap
        self.firstX = x
        self.firstY = y
        # bitmap 8x8 - it's a copy of the part of the main bitmap
        # before transforming and flushing
        self.bitmap = Bitmap(8, 8)

class MatrixRender(object):
    def __init__(self, horizDisplays, vertDisplays, rotation, mirrorX, mirrorY):
        self.horizDisplays = horizDisplays
        self.vertDisplays = vertDisplays
        self.matrixList = []
        for row in range(self.horizDisplays):
            for col in range(self.vertDisplays):
                self.matrixList.append(Matrix(row*8, col*8, rotation, mirrorX, mirrorY))

    @property
    def matrixCount(self):
        return len(self.matrixList)

    def flushBitmap(self, bitmap):
        for index, matrix in enumerate(self.matrixList):
            self.transform(bitmap, index, matrix)

    def transform(self, bitmap, index, matrix):
        xx = x = matrix.firstX
        yy = y = matrix.firstY

        for i in range(8):
            for j in range(8):
                val = bitmap.getPixel(x, y)
                if matrix.rotation == 90:
                    xx = 7-j
                    yy = i
                elif matrix.rotation == 180:
                    xx = 7-i
                    yy = 7-j
                elif matrix.rotation == 270:
                    xx = j
                    yy = 7-i

                # 0 and invalid values
                else:
                    xx = i
                    yy = j

                if matrix.mirrorX:
                    xx = 7-xx
                if matrix.mirrorY:
                    yy = 7-yy

                matrix.bitmap.setPixel(xx, yy, val)

                y += 1

            # increase x before next loop
            x += 1
            y = 0

class GC7219(object):
    def __init__(self, DIN, CS, horizDisplays=4, vertDisplays=1, rotation=0, mirrorX=False, mirrorY=False):
        self.DIN = None
        self.CS = None

        if not isinstance(DIN, SPI):
            raise ValueError('parameter DIN is not an SPI object')
        if not isinstance(CS, GPIO):
            raise ValueError('parameter CS should be GPIO object')

        self.DIN = DIN
        self.CS = CS

        # the self.bitmap will transform to self.render.matrixList
        self.render = MatrixRender(horizDisplays, vertDisplays, rotation, mirrorX, mirrorY)
        self.bitmap = Bitmap(horizDisplays*8, vertDisplays*8)

        self.init()

    def init(self):
        # deactivate displaytest
        self.spiTransfer(OP_DISPLAYTEST, 0)
        # activate all rows
        self.spiTransfer(OP_SCANLIMIT, 7)
        # we don't use 7-segment display
        self.spiTransfer(OP_DECODEMODE, 0)
        # average light intensity
        self.spiTransfer(OP_INTENSITY, 7)
        # turn off all LEDs
        for i in range(8):
            self.spiTransfer(OP_DIGIT0 + i, 0)
        # activate displays
        self.spiTransfer(OP_SHUTDOWN, 1)

    # 1 则点亮所有 LED，0 则进入正常显示模式
    def displayTest(self, enable):
        self.spiTransfer(OP_DISPLAYTEST, enable)

    # 0 则关闭所有 LED，1 则正常显示
    def display(self, on):
        self.spiTransfer(OP_SHUTDOWN, on)

    # 设置 LED 亮度，0-15
    def setIntensity(self, intensity):
        intensity = max(0, min(15, intensity))
        self.spiTransfer(OP_INTENSITY, intensity)

    # 将 bitmap 中的点阵数据刷到 LED 面板上
    def flush(self):
        self.render.flushBitmap(self.bitmap)
        for index, matrix in enumerate(self.render.matrixList):
            self.sendMatrix(matrix, index)

    def sendMatrix(self, matrix, index):
        for i, byte in enumerate(matrix.bitmap.array):
            self.spiTransfer(OP_DIGIT0 + i, byte, index)

    # SPI 底层传输函数
    # byte opCode, byte data, int display
    def spiTransfer(self, opCode, data, display=ALL_DISPLAYS):
        # begin writing
        self.CS.write(0)

        for i in range(self.render.matrixCount, -1, -1):
            # send opcode and data or only no-op (skip display)?
            write = display == ALL_DISPLAYS or display == i

            buf = bytearray((write and opCode or OP_NOOP, write and data or OP_NOOP))
            self.DIN.write(buf)

        # stop writing - let chips process new commands
        self.CS.write(1)
