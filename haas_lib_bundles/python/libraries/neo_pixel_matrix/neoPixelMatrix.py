import neopixel
import framebuf


class NeoPixelMatrix:
    def __init__(self, pin, width, hight, linedir):
        self.width = width
        self.height = hight
        self.linedir = linedir
        self.buffer = bytearray(self.width * self.height * 3)
        fb = framebuf.FrameBuffer(
            self.buffer, self.width, self.height, framebuf.RGB888)
        self.framebuf = fb
        self.fill = fb.fill
        self.fill_rect = fb.fill_rect
        self.hline = fb.hline
        self.vline = fb.vline
        self.line = fb.line
        self.rect = fb.rect
        self.pixel = fb.pixel
        self.scroll = fb.scroll
        self.text = fb.text
        self.blit = fb.blit
        self.brightness = 255
        self.np = neopixel.NeoPixel(pin, width*hight, bpp=3, timing=1)

    def show(self):
        for i in range(0, self.width * self.height):
            x = i // self.height
            y = (self.height - i % self.height - 1) if (x %
                                                        2 == 1) else (i % self.height)
            self.np[i] = (
                self.buffer[(x + self.width * y) * 3] * self.brightness >> 8,
                (self.buffer[(x + self.width * y) * 3 + 1] * self.brightness) >> 8,
                (self.buffer[(x + self.width * y) * 3 + 2] * self.brightness) >> 8)

        self.np.write()

    def setBuffer(self, buffer):
        self.buffer = buffer[:]

    def setBrightness(self, bright):
        # for i in range(self.width * self.height * 3):
        #     self.buffer[i] = (self.buffer[i] * bright) >> 8
        self.brightness = bright
