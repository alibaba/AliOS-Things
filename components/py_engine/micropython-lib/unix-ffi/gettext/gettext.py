import ffilib

libc = ffilib.libc()

gettext_ = libc.func("s", "gettext", "s")
ngettext_ = libc.func("s", "ngettext", "ssL")


def gettext(message):
    return gettext_(message)


def ngettext(singular, plural, n):
    return ngettext_(singular, plural, n)
