import os
import tty, termios
import select

COLOR_BLACK = 0
COLOR_RED = 1
COLOR_GREEN = 2
COLOR_YELLOW = 3
COLOR_BLUE = 4
COLOR_MAGENTA = 5
COLOR_CYAN = 6
COLOR_WHITE = 7

A_NORMAL = 0
A_BOLD = 1
A_UNDERLINE = 2
A_REVERSE = 4
A_STANDOUT = A_REVERSE

ATTRMAP = {
    A_NORMAL: b"\x1b[0m",
    A_BOLD: b"\x1b[1m",  # Some terminal emulators don't render bold by default, then use 4m for underline
    A_REVERSE: b"\x1b[7m",
}

# Use http://www.utf8-chartable.de/unicode-utf8-table.pl
# for utf-8 pseudographic reference
# "─"
ACS_HLINE = b"\xe2\x94\x80"
# "│"
ACS_VLINE = b"\xe2\x94\x82"
# "┌"
ACS_ULCORNER = b"\xe2\x94\x8c"
# "┐"
ACS_URCORNER = b"\xe2\x94\x90"
# "└"
ACS_LLCORNER = b"\xe2\x94\x94"
# "┘"
ACS_LRCORNER = b"\xe2\x94\x98"

KEY_F1 = 1031
KEY_RESIZE = 1100
KEY_MOUSE = 1101
KEY_BTAB = 1090

KEY_UP = 1001
KEY_DOWN = 1002
KEY_LEFT = 1003
KEY_RIGHT = 1004
KEY_HOME = 1005
KEY_END = 1006
KEY_PGUP = 1007
KEY_PGDN = 1008
KEY_QUIT = 1009
KEY_ENTER = 1010
KEY_BACKSPACE = 1011
KEY_DELETE = 1012
KEY_ESC = 0x1B

KEY_DC = KEY_DELETE
KEY_PPAGE = KEY_PGUP
KEY_NPAGE = KEY_PGDN

KEYMAP = {
    b"\x1b[A": KEY_UP,
    b"\x1b[B": KEY_DOWN,
    b"\x1b[D": KEY_LEFT,
    b"\x1b[C": KEY_RIGHT,
    b"\x1bOH": KEY_HOME,
    b"\x1bOF": KEY_END,
    b"\x1b[1~": KEY_HOME,
    b"\x1b[4~": KEY_END,
    b"\x1b[5~": KEY_PGUP,
    b"\x1b[6~": KEY_PGDN,
    b"\x03": KEY_QUIT,
    b"\r": KEY_ENTER,
    b"\x7f": KEY_BACKSPACE,
    b"\x1b[3~": KEY_DELETE,
    b"\x1bOA": KEY_UP,
    b"\x1bOB": KEY_DOWN,
    b"\x1bOD": KEY_LEFT,
    b"\x1bOC": KEY_RIGHT,
    b"\x1bOP": KEY_F1,
    b"\x1b": KEY_ESC,
    b"\x1b[Z": KEY_BTAB,
}

ALL_MOUSE_EVENTS = 0xFF


def _wr(s):
    # TODO: When Python is 3.5, update this to use only bytes
    if isinstance(s, str):
        s = bytes(s, "utf-8")
    os.write(1, s)


def _move(row, col):
    # TODO: When Python is 3.5, update this to use bytes
    _wr("\x1b[%d;%dH" % (row + 1, col + 1))


# Clear specified number of positions
def _clear_num_pos(num):
    if num > 0:
        _wr("\x1b[%dX" % num)


def _draw_box(left, top, width, height):
    bottom = top + height - 1
    _move(top, left)
    _wr(ACS_ULCORNER)
    hor = ACS_HLINE * (width - 2)
    _wr(hor)
    _wr(ACS_URCORNER)

    _move(bottom, left)
    _wr(ACS_LLCORNER)
    _wr(hor)
    _wr(ACS_LRCORNER)

    top += 1
    while top < bottom:
        _move(top, left)
        _wr(ACS_VLINE)
        _move(top, left + width - 1)
        _wr(ACS_VLINE)
        top += 1


class error(Exception):
    pass


class Window:
    def __init__(self, lines, cols, y, x):
        self.lines = lines
        self.cols = cols
        self.y = y
        self.x = x
        self.bkgattr = A_NORMAL
        self.keybuf = None
        self.keyi = 0
        self.keydelay = -1

    def _goto(self, row, col):
        _move(self.y + row, self.x + col)

    def move(self, y, x):
        # Maybe need to cache coords?
        self._goto(y, x)

    def getmaxyx(self):
        return (self.lines, self.cols)

    def addstr(self, y, x, str, attr=A_NORMAL):
        self._goto(y, x)
        # TODO: Should be "ORed"
        if attr == A_NORMAL:
            attr = self.bkgattr
        if attr != A_NORMAL:
            _wr(ATTRMAP[attr])
            _wr(str)
            _wr(ATTRMAP[A_NORMAL])
        else:
            _wr(str)

    def addnstr(self, y, x, str, n, attr=A_NORMAL):
        self.addstr(y, x, str[:n], attr)

    def addch(self, y, x, ch, attr=A_NORMAL):
        if isinstance(ch, int):
            ch = chr(ch)
        self.addstr(y, x, ch, attr)

    def attron(self, attr):
        pass

    def attroff(self, attr):
        pass

    def attrset(self, attr):
        pass

    def bkgdset(self, ch, attr=A_NORMAL):
        self.bkgattr = attr

    def erase(self):
        for i in range(self.lines):
            self._goto(i, 0)
            _clear_num_pos(self.cols)

    def border(self):
        _draw_box(self.x, self.y, self.cols, self.lines)

    def hline(self, y, x, ch, n):
        self.move(y, x)
        _wr(ch * n)

    def vline(self, y, x, ch, n):
        for i in range(n):
            self.move(y + i, x)
            _wr(ch)

    def refresh(self):
        pass

    def redrawwin(self):
        pass

    def keypad(self, yes):
        pass

    def timeout(self, delay):
        self.keydelay = delay

    def nodelay(self, yes):
        if yes:
            self.keydelay = 0
        else:
            self.keydelay = -1

    def getch(self):
        if self.keybuf and self.keyi < len(self.keybuf):
            c = self.keybuf[self.keyi]
            self.keyi += 1
            return c

        if self.keydelay >= 0:
            USE_EPOLL = 1
            if USE_EPOLL:
                poll = select.epoll()
                poll.register(0, select.EPOLLIN)
                res = poll.poll(self.keydelay / 1000)
                poll.unregister(0)
                poll.close()
            else:
                res = select.select([0], [], [], self.keydelay / 1000)[0]
            if not res:
                return -1

        key = os.read(0, 32)
        if key[0] != 0x1B:
            self.keybuf = key
            self.keyi = 1
            key = key[0]

        else:
            if key in KEYMAP:
                key = KEYMAP[key]
            else:
                assert False, repr(key)
        return key


SCREEN = Window(24, 80, 0, 0)
org_termios = None


def wrapper(func):
    global org_termios
    org_termios = termios.tcgetattr(0)
    res = func()
    endwin()
    return res


def initscr():
    global org_termios
    org_termios = termios.tcgetattr(0)
    return SCREEN


def doupdate():
    pass


def endwin():
    global org_termios
    _wr(b"\r")
    termios.tcsetattr(0, termios.TCSANOW, org_termios)


def raw():
    tty.setraw(0)


def cbreak():
    # TODO
    pass


def nocbreak():
    # TODO
    pass


def echo():
    # TODO
    pass


def noecho():
    # TODO
    pass


def meta(yes):
    # TODO
    pass


def mousemask(mask):
    # Mouse reporting - X10 compatbility mode
    _wr(b"\x1b[?9h")


def has_colors():
    return False


def can_change_color():
    return False


def curs_set(visibility):
    if visibility > 0:
        _wr(b"\x1b[?25h")
    else:
        _wr(b"\x1b[?25l")


def beep():
    _wr(b"\x07")


def newwin(lines, cols, y=0, x=0):
    # print("newwin(%d, %d, %d, %d)" % (lines, cols, y, x))
    cols = cols or SCREEN.cols
    lines = lines or SCREEN.lines
    return Window(lines, cols, y, x)
