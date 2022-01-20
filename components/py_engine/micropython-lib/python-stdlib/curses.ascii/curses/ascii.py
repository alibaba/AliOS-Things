"""Constants and membership tests for ASCII characters"""

NUL = 0x00  # ^@
SOH = 0x01  # ^A
STX = 0x02  # ^B
ETX = 0x03  # ^C
EOT = 0x04  # ^D
ENQ = 0x05  # ^E
ACK = 0x06  # ^F
BEL = 0x07  # ^G
BS = 0x08  # ^H
TAB = 0x09  # ^I
HT = 0x09  # ^I
LF = 0x0A  # ^J
NL = 0x0A  # ^J
VT = 0x0B  # ^K
FF = 0x0C  # ^L
CR = 0x0D  # ^M
SO = 0x0E  # ^N
SI = 0x0F  # ^O
DLE = 0x10  # ^P
DC1 = 0x11  # ^Q
DC2 = 0x12  # ^R
DC3 = 0x13  # ^S
DC4 = 0x14  # ^T
NAK = 0x15  # ^U
SYN = 0x16  # ^V
ETB = 0x17  # ^W
CAN = 0x18  # ^X
EM = 0x19  # ^Y
SUB = 0x1A  # ^Z
ESC = 0x1B  # ^[
FS = 0x1C  # ^\
GS = 0x1D  # ^]
RS = 0x1E  # ^^
US = 0x1F  # ^_
SP = 0x20  # space
DEL = 0x7F  # delete

controlnames = [
    "NUL",
    "SOH",
    "STX",
    "ETX",
    "EOT",
    "ENQ",
    "ACK",
    "BEL",
    "BS",
    "HT",
    "LF",
    "VT",
    "FF",
    "CR",
    "SO",
    "SI",
    "DLE",
    "DC1",
    "DC2",
    "DC3",
    "DC4",
    "NAK",
    "SYN",
    "ETB",
    "CAN",
    "EM",
    "SUB",
    "ESC",
    "FS",
    "GS",
    "RS",
    "US",
    "SP",
]


def _ctoi(c):
    if type(c) == type(""):
        return ord(c)
    else:
        return c


def isalnum(c):
    return isalpha(c) or isdigit(c)


def isalpha(c):
    return isupper(c) or islower(c)


def isascii(c):
    return _ctoi(c) <= 127  # ?


def isblank(c):
    return _ctoi(c) in (8, 32)


def iscntrl(c):
    return _ctoi(c) <= 31


def isdigit(c):
    return _ctoi(c) >= 48 and _ctoi(c) <= 57


def isgraph(c):
    return _ctoi(c) >= 33 and _ctoi(c) <= 126


def islower(c):
    return _ctoi(c) >= 97 and _ctoi(c) <= 122


def isprint(c):
    return _ctoi(c) >= 32 and _ctoi(c) <= 126


def ispunct(c):
    return _ctoi(c) != 32 and not isalnum(c)


def isspace(c):
    return _ctoi(c) in (9, 10, 11, 12, 13, 32)


def isupper(c):
    return _ctoi(c) >= 65 and _ctoi(c) <= 90


def isxdigit(c):
    return (
        isdigit(c) or (_ctoi(c) >= 65 and _ctoi(c) <= 70) or (_ctoi(c) >= 97 and _ctoi(c) <= 102)
    )


def isctrl(c):
    return _ctoi(c) < 32


def ismeta(c):
    return _ctoi(c) > 127


def ascii(c):
    if type(c) == type(""):
        return chr(_ctoi(c) & 0x7F)
    else:
        return _ctoi(c) & 0x7F


def ctrl(c):
    if type(c) == type(""):
        return chr(_ctoi(c) & 0x1F)
    else:
        return _ctoi(c) & 0x1F


def alt(c):
    if type(c) == type(""):
        return chr(_ctoi(c) | 0x80)
    else:
        return _ctoi(c) | 0x80


def unctrl(c):
    bits = _ctoi(c)
    if bits == 0x7F:
        rep = "^?"
    elif isprint(bits & 0x7F):
        rep = chr(bits & 0x7F)
    else:
        rep = "^" + chr(((bits & 0x7F) | 0x20) + 0x20)
    if bits & 0x80:
        return "!" + rep
    return rep
