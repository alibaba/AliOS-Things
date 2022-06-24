"""2.9 inch E-paper display black/red version 2."""
from math import cos, sin, pi, radians
from micropython import const
from framebuf import FrameBuffer, GS8, MONO_HLSB, MONO_HMSB
from utime import sleep_ms


class Display(object):
    """Serial interface for 2.9 inch E-paper display.

    Note:  All coordinates are zero based.
    """

    # Command constants from display datasheet
    PANEL_SETTING = const(0x00)
    POWER_OFF = const(0x02)
    POWER_ON = const(0x04)
    DEEP_SLEEP = const(0x07)
    DATA_START_TRANSMISSION_1 = const(0x10)
    DISPLAY_REFRESH = const(0x12)
    DATA_START_TRANSMISSION_2 = const(0x13)
    VCOM_AND_DATA_INTERVAL_SETTING = const(0x50)
    TCON_RESOLUTION = const(0x61)
    GET_STATUS = const(0x71)

    def __init__(self, spi, cs, dc, rst, busy, width=128, height=296):
        """Constructor for Display.

        Args:
            spi (Class Spi):  SPI interface for display
            cs (Class Pin):  Chip select pin
            dc (Class Pin):  Data/Command pin
            rst (Class Pin):  Reset pin
            busy (Class Pin):  Busy pin
            width (Optional int): Screen width (default 128)
            height (Optional int): Screen height (default 296)
        """
        self.spi = spi
        self.cs = cs
        self.dc = dc
        self.rst = rst
        self.busy = busy
        self.width = width
        self.height = height
        self.byte_width = -(-width // 8)  # Ceiling division
        self.buffer_length = self.byte_width * height
        # Buffers (black image and red or yellow image)
        self.blackimage = bytearray(self.buffer_length)
        self.ryimage = bytearray(self.buffer_length)
        # Frame Buffers (black image and red or yellow image)
        self.blackFB = FrameBuffer(self.blackimage, width, height, MONO_HLSB)
        self.ryFB = FrameBuffer(self.ryimage, width, height, MONO_HLSB)
        self.clear_buffers()
        # Initialize GPIO pins
        self.cs.init(self.cs.OUT, value=1)
        self.dc.init(self.dc.OUT, value=0)
        self.rst.init(self.rst.OUT, value=1)
        self.busy.init(self.busy.IN)
        self.reset()
        # Send initialization commands
        self.write_cmd(self.POWER_ON)
        self.ReadBusy()  # Wait for display to indicate idle
        self.write_cmd(self.PANEL_SETTING, 0x0F, 0x89)
        self.write_cmd(self.TCON_RESOLUTION, 0x80, 0x01, 0x28)
        self.write_cmd(self.VCOM_AND_DATA_INTERVAL_SETTING, 0x77)

    def cleanup(self):
        """Clean up resources."""
        self.clear()
        self.sleep()
        # self.spi.deinit()
        self.spi.close()
        print('display off')

    def clear(self, red=True, black=True):
        """Clear display.

        Args:
            red (bool): True (default) = clear red
            black (bool): True (default) = clear black
        """
        self.ReadBusy()
        self.clear_buffers(red, black)
        self.present(red, black)
        self.write_cmd(self.DISPLAY_REFRESH)
        sleep_ms(200)
        self.ReadBusy()

    def clear_buffers(self, red=True, black=True):
        """Clear buffer.

        Args:
            red (bool): True (default) = clear red buffer
            black (bool): True (default) = clear black buffer
        """
        if red:
            self.ryFB.fill(0xFF)

        if black:
            self.blackFB.fill(0xFF)

    def draw_bitmap(self, x, y, w, h, path='', bytebuf=bytearray(b''), red=False, invert=False,
                    rotate=0):
        """Load MONO_HMSB bitmap from disc and draw to screen.

        Args:
            path (string): Image file path.
            x (int): x-coord of image.
            y (int): y-coord of image.
            w (int): Width of image.
            h (int): Height of image.
            red (bool): True = red image, False (Default) = black image.
            invert (bool): True = invert image, False (Default) = normal image.
            rotate(int): 0, 90, 180, 270
        Notes:
            w x h cannot exceed 2048
        """
        if path == '' and bytebuf == bytearray(b''):
            return
        array_size = w * h
        buf = bytearray(b'')

        if path != '':
            with open(path, "rb") as f:
                buf = bytearray(f.read(array_size))
        else:
            buf = bytebuf

        fb = FrameBuffer(buf, w, h, MONO_HMSB)

        if rotate == 0 and invert is True:  # 0 degrees
            fb2 = FrameBuffer(bytearray(array_size), w, h, MONO_HMSB)
            for y1 in range(h):
                for x1 in range(w):
                    fb2.pixel(x1, y1, fb.pixel(x1, y1) ^ 0x01)
            fb = fb2
        elif rotate == 90:  # 90 degrees
            byte_width = (w - 1) // 8 + 1
            adj_size = h * byte_width
            fb2 = FrameBuffer(bytearray(adj_size), h, w, MONO_HMSB)
            for y1 in range(h):
                for x1 in range(w):
                    if invert is True:
                        fb2.pixel(y1, x1,
                                    fb.pixel(x1, (h - 1) - y1) ^ 0x01)
                    else:
                        fb2.pixel(y1, x1, fb.pixel(x1, (h - 1) - y1))
            fb = fb2
        elif rotate == 180:  # 180 degrees
            fb2 = FrameBuffer(bytearray(array_size), w, h, MONO_HMSB)
            for y1 in range(h):
                for x1 in range(w):
                    if invert is True:
                        fb2.pixel(x1, y1, fb.pixel((w - 1) - x1,
                                                    (h - 1) - y1) ^ 0x01)
                    else:
                        fb2.pixel(x1, y1,
                                    fb.pixel((w - 1) - x1, (h - 1) - y1))
            fb = fb2
        elif rotate == 270:  # 270 degrees
            byte_width = (w - 1) // 8 + 1
            adj_size = h * byte_width
            fb2 = FrameBuffer(bytearray(adj_size), h, w, MONO_HMSB)
            for y1 in range(h):
                for x1 in range(w):
                    if invert is True:
                        fb2.pixel(y1, x1,
                                    fb.pixel((w - 1) - x1, y1) ^ 0x01)
                    else:
                        fb2.pixel(y1, x1, fb.pixel((w - 1) - x1, y1))
            fb = fb2

        if red:
            self.ryFB.blit(fb, x, y)
        else:
            self.blackFB.blit(fb, x, y)

    def draw_bitmap_raw(self, path, x, y, w, h, red=False, invert=False,
                        rotate=0):
        """Load raw bitmap from disc and draw to screen.

        Args:
            path (string): Image file path.
            x (int): x-coord of image.
            y (int): y-coord of image.
            w (int): Width of image.
            h (int): Height of image.
            red (bool): True = red image, False (Default) = black image.
            invert (bool): True = invert image, False (Default) = normal image.
            rotate(int): 0, 90, 180, 270
        Notes:
            w x h cannot exceed 2048
        """
        if rotate == 90 or rotate == 270:
            w, h = h, w  # Swap width & height if landscape

        buf_size = w * h
        with open(path, "rb") as f:
            if rotate == 0:
                buf = bytearray(f.read(buf_size))
            elif rotate == 90:
                buf = bytearray(buf_size)
                for x1 in range(w - 1, -1, -1):
                    for y1 in range(h):
                        index = (w * y1) + x1
                        buf[index] = f.read(1)[0]
            elif rotate == 180:
                buf = bytearray(buf_size)
                for index in range(buf_size - 1, -1, -1):
                    buf[index] = f.read(1)[0]
            elif rotate == 270:
                buf = bytearray(buf_size)
                for x1 in range(1, w + 1):
                    for y1 in range(h - 1, -1, -1):
                        index = (w * y1) + x1 - 1
                        buf[index] = f.read(1)[0]
            if invert:
                for i, _ in enumerate(buf):
                    buf[i] ^= 0xFF

            fbuf = FrameBuffer(buf, w, h, GS8)
            if red:
                self.ryFB.blit(fbuf, x, y)
            else:
                self.blackFB.blit(fbuf, x, y)

    def draw_circle(self, x0, y0, r, red=False, invert=False):
        """Draw a circle.

        Args:
            x0 (int): X coordinate of center point.
            y0 (int): Y coordinate of center point.
            r (int): Radius.
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
        """
        f = 1 - r
        dx = 1
        dy = -r - r
        x = 0
        y = r
        self.draw_pixel(x0, y0 + r, red, invert)
        self.draw_pixel(x0, y0 - r, red, invert)
        self.draw_pixel(x0 + r, y0, red, invert)
        self.draw_pixel(x0 - r, y0, red, invert)
        while x < y:
            if f >= 0:
                y -= 1
                dy += 2
                f += dy
            x += 1
            dx += 2
            f += dx
            self.draw_pixel(x0 + x, y0 + y, red, invert)
            self.draw_pixel(x0 - x, y0 + y, red, invert)
            self.draw_pixel(x0 + x, y0 - y, red, invert)
            self.draw_pixel(x0 - x, y0 - y, red, invert)
            self.draw_pixel(x0 + y, y0 + x, red, invert)
            self.draw_pixel(x0 - y, y0 + x, red, invert)
            self.draw_pixel(x0 + y, y0 - x, red, invert)
            self.draw_pixel(x0 - y, y0 - x, red, invert)

    def draw_ellipse(self, x0, y0, a, b, red=False, invert=False):
        """Draw an ellipse.

        Args:
            x0, y0 (int): Coordinates of center point.
            a (int): Semi axis horizontal.
            b (int): Semi axis vertical.
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
        Note:
            The center point is the center of the x0,y0 pixel.
            Since pixels are not divisible, the axes are integer rounded
            up to complete on a full pixel.  Therefore the major and
            minor axes are increased by 1.
        """
        a2 = a * a
        b2 = b * b
        twoa2 = a2 + a2
        twob2 = b2 + b2
        x = 0
        y = b
        px = 0
        py = twoa2 * y
        # Plot initial points
        self.draw_pixel(x0 + x, y0 + y, red, invert)
        self.draw_pixel(x0 - x, y0 + y, red, invert)
        self.draw_pixel(x0 + x, y0 - y, red, invert)
        self.draw_pixel(x0 - x, y0 - y, red, invert)
        # Region 1
        p = round(b2 - (a2 * b) + (0.25 * a2))
        while px < py:
            x += 1
            px += twob2
            if p < 0:
                p += b2 + px
            else:
                y -= 1
                py -= twoa2
                p += b2 + px - py
            self.draw_pixel(x0 + x, y0 + y, red, invert)
            self.draw_pixel(x0 - x, y0 + y, red, invert)
            self.draw_pixel(x0 + x, y0 - y, red, invert)
            self.draw_pixel(x0 - x, y0 - y, red, invert)
        # Region 2
        p = round(b2 * (x + 0.5) * (x + 0.5) +
                  a2 * (y - 1) * (y - 1) - a2 * b2)
        while y > 0:
            y -= 1
            py -= twoa2
            if p > 0:
                p += a2 - py
            else:
                x += 1
                px += twob2
                p += a2 - py + px
            self.draw_pixel(x0 + x, y0 + y, red, invert)
            self.draw_pixel(x0 - x, y0 + y, red, invert)
            self.draw_pixel(x0 + x, y0 - y, red, invert)
            self.draw_pixel(x0 - x, y0 - y, red, invert)

    def draw_hline(self, x, y, w, red=False, invert=False):
        """Draw a horizontal line.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            w (int): Width of line.
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
        """
        if self.is_off_grid(x, y, x + w - 1, y):
            return
        if red:
            self.ryFB.hline(x, y, w, int(invert))
        else:
            self.blackFB.hline(x, y, w, int(invert))

    def draw_letter(self, x, y, letter, font, red=False, invert=False,
                    rotate=False):
        """Draw a letter.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            letter (string): Letter to draw.
            font (XglcdFont object): Font.
            red (bool): True = red font, False (Default) = black font
            invert (bool): Invert color
            rotate (int): Rotation of letter
        """
        fbuf, w, h = font.get_letter(letter, invert=invert, rotate=rotate)
        # Check for errors
        if w == 0:
            return w, h
        # Offset y for 270 degrees and x for 180 degrees
        if rotate == 180:
            x -= w
        elif rotate == 270:
            y -= h
        if red:
            self.ryFB.blit(fbuf, x, y)
        else:
            self.blackFB.blit(fbuf, x, y)
        return w, h

    def draw_line(self, x1, y1, x2, y2, red=False, invert=False):
        """Draw a line using Bresenham's algorithm.

        Args:
            x1, y1 (int): Starting coordinates of the line
            x2, y2 (int): Ending coordinates of the line
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
        """
        # Check for horizontal line
        if y1 == y2:
            if x1 > x2:
                x1, x2 = x2, x1
            self.draw_hline(x1, y1, x2 - x1 + 1, red, invert)
            return
        # Check for vertical line
        if x1 == x2:
            if y1 > y2:
                y1, y2 = y2, y1
            self.draw_vline(x1, y1, y2 - y1 + 1, red, invert)
            return
        # Confirm coordinates in boundary
        if self.is_off_grid(min(x1, x2), min(y1, y2),
                            max(x1, x2), max(y1, y2)):
            return
        if red:
            self.ryFB.line(x1, y1, x2, y2, int(invert))
        else:
            self.blackFB.line(x1, y1, x2, y2, int(invert))

    def draw_lines(self, coords, red=False, invert=False):
        """Draw multiple lines.

        Args:
            coords ([[int, int],...]): Line coordinate X, Y pairs
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
        """
        # Starting point
        x1, y1 = coords[0]
        # Iterate through coordinates
        for i in range(1, len(coords)):
            x2, y2 = coords[i]
            self.draw_line(x1, y1, x2, y2, red, invert)
            x1, y1 = x2, y2

    def draw_pixel(self, x, y, red=False, invert=False):
        """Draw a single pixel.

        Args:
            x (int): X position.
            y (int): Y position.
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
        """
        if self.is_off_grid(x, y, x, y):
            return
        if red:
            self.ryFB.pixel(x, y, int(invert))
        else:
            self.blackFB.pixel(x, y, int(invert))

    def draw_polygon(self, sides, x0, y0, r, red=False, invert=False,
                     rotate=0):
        """Draw an n-sided regular polygon.

        Args:
            sides (int): Number of polygon sides.
            x0, y0 (int): Coordinates of center point.
            r (int): Radius.
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
            rotate (Optional float): Rotation in degrees relative to origin.
        Note:
            The center point is the center of the x0,y0 pixel.
            Since pixels are not divisible, the radius is integer rounded
            up to complete on a full pixel.  Therefore diameter = 2 x r + 1.
        """
        coords = []
        theta = radians(rotate)
        n = sides + 1
        for s in range(n):
            t = 2.0 * pi * s / sides + theta
            coords.append([int(r * cos(t) + x0), int(r * sin(t) + y0)])

        # Cast to python float first to fix rounding errors
        self.draw_lines(coords, red, invert)

    def draw_rectangle(self, x, y, w, h, red=False, invert=False):
        """Draw a rectangle.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            w (int): Width of rectangle.
            h (int): Height of rectangle.
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
        """
        if red:
            self.ryFB.rect(x, y, w, h, int(invert))
        else:
            self.blackFB.rect(x, y, w, h, int(invert))

    def draw_text(self, x, y, text, font, red=False,  invert=False,
                  rotate=0, spacing=1):
        """Draw text.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            text (string): Text to draw.
            font (XglcdFont object): Font.
            red (bool): True = red font, False (Default) = black font
            invert (bool): Invert color
            rotate (int): Rotation of letter
            spacing (int): Pixels between letters (default: 1)
        """
        for letter in text:
            # Get letter array and letter dimensions
            w, h = self.draw_letter(x, y, letter, font, red, invert, rotate)
            # Stop on error
            if w == 0 or h == 0:
                return
            if rotate == 0:
                # Fill in spacing
                if spacing:
                    self.fill_rectangle(x + w, y, spacing, h, red, not invert)
                # Position x for next letter
                x += (w + spacing)
            elif rotate == 90:
                # Fill in spacing
                if spacing:
                    self.fill_rectangle(x, y + h, w, spacing, red, not invert)
                # Position y for next letter
                y += (h + spacing)
            elif rotate == 180:
                # Fill in spacing
                if spacing:
                    self.fill_rectangle(x - w - spacing, y, spacing, h,
                                        red, not invert)
                # Position x for next letter
                x -= (w + spacing)
            elif rotate == 270:
                # Fill in spacing
                if spacing:
                    self.fill_rectangle(x, y - h - spacing, w, spacing,
                                        red, not invert)
                # Position y for next letter
                y -= (h + spacing)
            else:
                print("Invalid rotation.")
                return

    def draw_vline(self, x, y, h, red=False, invert=False):
        """Draw a vertical line.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            h (int): Height of line.
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
        """
        # Confirm coordinates in boundary
        if self.is_off_grid(x, y, x, y + h):
            return
        if red:
            self.ryFB.vline(x, y, h, int(invert))
        else:
            self.blackFB.vline(x, y, h, int(invert))

    def fill_circle(self, x0, y0, r, red=False, invert=False):
        """Draw a filled circle.

        Args:
            x0 (int): X coordinate of center point.
            y0 (int): Y coordinate of center point.
            r (int): Radius.
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
        """
        f = 1 - r
        dx = 1
        dy = -r - r
        x = 0
        y = r
        self.draw_vline(x0, y0 - r, 2 * r + 1, red, invert)
        while x < y:
            if f >= 0:
                y -= 1
                dy += 2
                f += dy
            x += 1
            dx += 2
            f += dx
            self.draw_vline(x0 + x, y0 - y, 2 * y + 1, red, invert)
            self.draw_vline(x0 - x, y0 - y, 2 * y + 1, red, invert)
            self.draw_vline(x0 - y, y0 - x, 2 * x + 1, red, invert)
            self.draw_vline(x0 + y, y0 - x, 2 * x + 1, red, invert)

    def fill_ellipse(self, x0, y0, a, b, red=False, invert=False):
        """Draw a filled ellipse.

        Args:
            x0, y0 (int): Coordinates of center point.
            a (int): Semi axis horizontal.
            b (int): Semi axis vertical.
            red (bool): True = red line, False (Default) = black line.
            invert (bool): True = clear line, False (Default) = draw line.
        Note:
            The center point is the center of the x0,y0 pixel.
            Since pixels are not divisible, the axes are integer rounded
            up to complete on a full pixel.  Therefore the major and
            minor axes are increased by 1.
        """
        a2 = a * a
        b2 = b * b
        twoa2 = a2 + a2
        twob2 = b2 + b2
        x = 0
        y = b
        px = 0
        py = twoa2 * y
        # Plot initial points
        self.draw_line(x0, y0 - y, x0, y0 + y, red, invert)
        # Region 1
        p = round(b2 - (a2 * b) + (0.25 * a2))
        while px < py:
            x += 1
            px += twob2
            if p < 0:
                p += b2 + px
            else:
                y -= 1
                py -= twoa2
                p += b2 + px - py
            self.draw_line(x0 + x, y0 - y, x0 + x, y0 + y, red, invert)
            self.draw_line(x0 - x, y0 - y, x0 - x, y0 + y, red, invert)
        # Region 2
        p = round(b2 * (x + 0.5) * (x + 0.5) +
                  a2 * (y - 1) * (y - 1) - a2 * b2)
        while y > 0:
            y -= 1
            py -= twoa2
            if p > 0:
                p += a2 - py
            else:
                x += 1
                px += twob2
                p += a2 - py + px
            self.draw_line(x0 + x, y0 - y, x0 + x, y0 + y, red, invert)
            self.draw_line(x0 - x, y0 - y, x0 - x, y0 + y, red, invert)

    def fill_rectangle(self, x, y, w, h, red=False, invert=False):
        """Draw a filled rectangle.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            w (int): Width of rectangle.
            h (int): Height of rectangle.
            red (bool): True = red line, False (Default) = black line.
            visble (bool): True (Default) = draw line, False = clear line.
        """
        if self.is_off_grid(x, y, x + w - 1, y + h - 1):
            return
        if red:
            self.ryFB.fill_rect(x, y, w, h, int(invert))
        else:
            self.blackFB.fill_rect(x, y, w, h, int(invert))

    def fill_polygon(self, sides, x0, y0, r, red=False, invert=False,
                     rotate=0):
        """Draw a filled n-sided regular polygon.

        Args:
            sides (int): Number of polygon sides.
            x0, y0 (int): Coordinates of center point.
            r (int): Radius.
            red (bool): True = red line, False (Default) = black line.
            visble (bool): True (Default) = draw line, False = clear line.
            rotate (Optional float): Rotation in degrees relative to origin.
        Note:
            The center point is the center of the x0,y0 pixel.
            Since pixels are not divisible, the radius is integer rounded
            up to complete on a full pixel.  Therefore diameter = 2 x r + 1.
        """
        # Determine side coordinates
        coords = []
        theta = radians(rotate)
        n = sides + 1
        for s in range(n):
            t = 2.0 * pi * s / sides + theta
            coords.append([int(r * cos(t) + x0), int(r * sin(t) + y0)])
        # Starting point
        x1, y1 = coords[0]
        # Minimum Maximum X dict
        xdict = {y1: [x1, x1]}
        # Iterate through coordinates
        for row in coords[1:]:
            x2, y2 = row
            xprev, yprev = x2, y2
            # Calculate perimeter
            # Check for horizontal side
            if y1 == y2:
                if x1 > x2:
                    x1, x2 = x2, x1
                if y1 in xdict:
                    xdict[y1] = [min(x1, xdict[y1][0]), max(x2, xdict[y1][1])]
                else:
                    xdict[y1] = [x1, x2]
                x1, y1 = xprev, yprev
                continue
            # Non horizontal side
            # Changes in x, y
            dx = x2 - x1
            dy = y2 - y1
            # Determine how steep the line is
            is_steep = abs(dy) > abs(dx)
            # Rotate line
            if is_steep:
                x1, y1 = y1, x1
                x2, y2 = y2, x2
            # Swap start and end points if necessary
            if x1 > x2:
                x1, x2 = x2, x1
                y1, y2 = y2, y1
            # Recalculate differentials
            dx = x2 - x1
            dy = y2 - y1
            # Calculate error
            error = dx >> 1
            ystep = 1 if y1 < y2 else -1
            y = y1
            # Calcualte minimum and maximum x values
            for x in range(x1, x2 + 1):
                if is_steep:
                    if x in xdict:
                        xdict[x] = [min(y, xdict[x][0]), max(y, xdict[x][1])]
                    else:
                        xdict[x] = [y, y]
                else:
                    if y in xdict:
                        xdict[y] = [min(x, xdict[y][0]), max(x, xdict[y][1])]
                    else:
                        xdict[y] = [x, x]
                error -= abs(dy)
                if error < 0:
                    y += ystep
                    error += dx
            x1, y1 = xprev, yprev
        # Fill polygon
        for y, x in xdict.items():
            self.draw_hline(x[0], y, x[1] - x[0] + 2, red, invert)

    def is_off_grid(self, xmin, ymin, xmax, ymax):
        """Check if coordinates extend past display boundaries.

        Args:
            xmin (int): Minimum horizontal pixel.
            ymin (int): Minimum vertical pixel.
            xmax (int): Maximum horizontal pixel.
            ymax (int): Maximum vertical pixel.
        Returns:
            boolean: False = Coordinates OK, True = Error.
        """
        if xmin < 0:
            print('x-coordinate: {0} below minimum of 0.'.format(xmin))
            return True
        if ymin < 0:
            print('y-coordinate: {0} below minimum of 0.'.format(ymin))
            return True
        if xmax >= self.width:
            print('x-coordinate: {0} above maximum of {1}.'.format(
                xmax, self.width - 1))
            return True
        if ymax >= self.height:
            print('y-coordinate: {0} above maximum of {1}.'.format(
                ymax, self.height - 1))
            return True
        return False

    def present(self, red=True, black=True):
        """Present image to display.

        Args:
            red (bool): True (default) = present red buffer
            black (bool): True (default) = present black buffer
        """
        if (black and self.blackimage is not None):
            self.write_cmd(self.DATA_START_TRANSMISSION_1)
            self.write_data(self.blackimage)
        if (red and self.ryimage is not None):
            self.write_cmd(self.DATA_START_TRANSMISSION_2)
            self.write_data(self.ryimage)

        self.write_cmd(self.DISPLAY_REFRESH)
        sleep_ms(200)
        self.ReadBusy()

    def sleep(self):
        """Put display to sleep."""
        self.write_cmd(self.POWER_OFF)
        self.ReadBusy()
        self.write_cmd(self.DEEP_SLEEP, 0xA5)

    # def ReadBusy(self):
    #     """Check if display busy."""
    #     self.write_cmd(self.GET_STATUS)
    #     while self.busy.read() == 0:      # 0: busy, 1: idle
    #         self.write_cmd(self.GET_STATUS)
    #         sleep_ms(200)

    # def reset(self):
    #     """Perform reset."""
    #     self.rst.write(1)
    #     sleep_ms(200)
    #     self.rst.write(0)
    #     sleep_ms(10)
    #     self.rst.write(1)
    #     sleep_ms(200)

    # def sleep(self):
    #     """Put display to sleep."""
    #     self.write_cmd(self.POWER_OFF)
    #     self.ReadBusy()
    #     self.write_cmd(self.DEEP_SLEEP, 0xA5)

    # def write_cmd(self, command, *args):
    #     """Write command to display.

    #     Args:
    #         command (byte): Display command code.
    #         *args (optional bytes): Data to transmit.
    #     """
    #     self.dc.write(0)
    #     self.cs.write(0)
    #     self.spi.write(bytearray([command]))
    #     self.cs.write(1)
    #     # Handle any passed data
    #     if len(args) > 0:
    #         self.write_data(bytearray(args))

    # def write_data(self, data):
    #     """Write data to display.

    #     Args:
    #         data (bytes): Data to transmit.
    #     """
    #     self.dc.write(1)
    #     self.cs.write(0)
    #     self.spi.write(data)
    #     self.cs.write(1)

    def ReadBusy(self):
        """Check if display busy."""
        self.write_cmd(self.GET_STATUS)
        while self.busy.value() == 0:      # 0: busy, 1: idle
            self.write_cmd(self.GET_STATUS)
            sleep_ms(200)

    def reset(self):
        """Perform reset."""
        self.rst(1)
        sleep_ms(200)
        self.rst(0)
        sleep_ms(10)
        self.rst(1)
        sleep_ms(200)

    def write_cmd(self, command, *args):
        """Write command to display.

        Args:
            command (byte): Display command code.
            *args (optional bytes): Data to transmit.
        """
        self.dc(0)
        self.cs(0)
        # print("send cmd")
        # print(command)
        self.spi.write(bytearray([command]))
        self.cs(1)
        # Handle any passed data
        if len(args) > 0:
            self.write_data(bytearray(args))

    def write_data(self, data):
        """Write data to display.

        Args:
            data (bytes): Data to transmit.
        """
        self.dc(1)
        self.cs(0)
        # print("send data")
        # print(data)
        self.spi.write(data)
        self.cs(1)
