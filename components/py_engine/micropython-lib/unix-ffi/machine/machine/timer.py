import ffilib
import uctypes
import array
import uos
import os
import utime
from signal import *

libc = ffilib.libc()
librt = ffilib.open("librt")

CLOCK_REALTIME = 0
CLOCK_MONOTONIC = 1
SIGEV_SIGNAL = 0

sigval_t = {
    "sival_int": uctypes.INT32 | 0,
    "sival_ptr": (uctypes.PTR | 0, uctypes.UINT8),
}

sigevent_t = {
    "sigev_value": (0, sigval_t),
    "sigev_signo": uctypes.INT32 | 8,
    "sigev_notify": uctypes.INT32 | 12,
}

timespec_t = {
    "tv_sec": uctypes.INT32 | 0,
    "tv_nsec": uctypes.INT64 | 8,
}

itimerspec_t = {
    "it_interval": (0, timespec_t),
    "it_value": (16, timespec_t),
}


__libc_current_sigrtmin = libc.func("i", "__libc_current_sigrtmin", "")
SIGRTMIN = __libc_current_sigrtmin()

timer_create_ = librt.func("i", "timer_create", "ipp")
timer_settime_ = librt.func("i", "timer_settime", "PiPp")


def new(sdesc):
    buf = bytearray(uctypes.sizeof(sdesc))
    s = uctypes.struct(uctypes.addressof(buf), sdesc, uctypes.NATIVE)
    return s


def timer_create(sig_id):
    sev = new(sigevent_t)
    # print(sev)
    sev.sigev_notify = SIGEV_SIGNAL
    sev.sigev_signo = SIGRTMIN + sig_id
    timerid = array.array("P", [0])
    r = timer_create_(CLOCK_MONOTONIC, sev, timerid)
    os.check_error(r)
    # print("timerid", hex(timerid[0]))
    return timerid[0]


def timer_settime(tid, hz):
    period = 1000000000 // hz
    new_val = new(itimerspec_t)
    new_val.it_value.tv_nsec = period
    new_val.it_interval.tv_nsec = period
    # print("new_val:", bytes(new_val))
    old_val = new(itimerspec_t)
    # print(new_val, old_val)
    r = timer_settime_(tid, 0, new_val, old_val)
    os.check_error(r)
    # print("old_val:", bytes(old_val))
    # print("timer_settime", r)


class Timer:
    def __init__(self, id, freq):
        self.id = id
        self.tid = timer_create(id)
        self.freq = freq

    def callback(self, cb):
        self.cb = cb
        timer_settime(self.tid, self.freq)
        org_sig = signal(SIGRTMIN + self.id, self.handler)
        # print("Sig %d: %s" % (SIGRTMIN + self.id, org_sig))

    def handler(self, signum):
        # print('Signal handler called with signal', signum)
        self.cb(self)
