
# Timer that matches machine.Timer (https://docs.micropython.org/en/latest/library/machine.Timer.html)
# for the unix port.
#
# MIT license; Copyright (c) 2021 Amir Gonnen
# 
# Based on timer.py from micropython-lib (https://github.com/micropython/micropython-lib/blob/master/unix-ffi/machine/machine/timer.py)


import ffi
import uctypes
import array
import os
import sys

# FFI libraries

libc = ffi.open("libc.so.6")
librt = ffi.open("librt.so")

# C constants

CLOCK_REALTIME = 0
CLOCK_MONOTONIC = 1
SIGEV_SIGNAL = 0

# C structs

sigaction_t = {
    "sa_handler" : (0 | uctypes.UINT64),
    "sa_mask"    : (8 | uctypes.ARRAY, 16 | uctypes.UINT64),
    "sa_flags"   : (136 | uctypes.INT32),
    "sa_restorer": (144 |uctypes.PTR, uctypes.UINT8), 
}

sigval_t = {
    "sival_int": 0 | uctypes.INT32,
    "sival_ptr": (0 | uctypes.PTR, uctypes.UINT8),
}

sigevent_t = {
    "sigev_value": (0, sigval_t),
    "sigev_signo": 8 | uctypes.INT32,
    "sigev_notify": 12 | uctypes.INT32,
}

timespec_t = {
    "tv_sec": 0 | uctypes.INT32,
    "tv_nsec": 8 | uctypes.INT64,
}

itimerspec_t = {
    "it_interval": (0, timespec_t),
    "it_value": (16, timespec_t),
}

# C functions

__libc_current_sigrtmin = libc.func("i", "__libc_current_sigrtmin", "")
SIGRTMIN = __libc_current_sigrtmin()

timer_create_ = librt.func("i", "timer_create", "ipp")
timer_delete_ = librt.func("i", "timer_delete", "i")
timer_settime_ = librt.func("i", "timer_settime", "PiPp")

sigaction_ = libc.func("i", "sigaction", "iPp")

# Create a new C struct

def new(sdesc):
    buf = bytearray(uctypes.sizeof(sdesc))
    s = uctypes.struct(uctypes.addressof(buf), sdesc, uctypes.NATIVE)
    return s

# Posix Signal handling


def sigaction(signum, handler, flags=0):
    sa = new(sigaction_t)
    sa_old = new(sigaction_t)
    cb = ffi.callback("v", handler, "i", lock=True)
    sa.sa_handler = cb.cfun()
    sa.sa_flags = flags
    r = sigaction_(signum, sa, sa_old)
    if r != 0:
        raise RuntimeError("sigaction_ error: %d (errno = %d)" % (r, os.errno()))
    return cb # sa_old.sa_handler

# Posix Timer handling

def timer_create(sig_id):
    sev = new(sigevent_t)
    # print(sev)
    sev.sigev_notify = SIGEV_SIGNAL
    sev.sigev_signo = SIGRTMIN + sig_id
    timerid = array.array("P", [0])
    r = timer_create_(CLOCK_MONOTONIC, sev, timerid)
    if r != 0:
        raise RuntimeError("timer_create_ error: %d (errno = %d)" % (r, os.errno()))
    # print("timerid", hex(timerid[0]))
    return timerid[0]

def timer_delete(tid):
    r = timer_delete_(tid)
    if r != 0:
        raise RuntimeError("timer_delete_ error: %d (errno = %d)" % (r, os.errno()))

def timer_settime(tid, period_ms, periodic):
    period_ns = (period_ms * 1000000) % 1000000000
    period_sec = (period_ms * 1000000) // 1000000000

    new_val = new(itimerspec_t)
    new_val.it_value.tv_sec = period_sec
    new_val.it_value.tv_nsec = period_ns
    if periodic:
        new_val.it_interval.tv_sec = period_sec
        new_val.it_interval.tv_nsec = period_ns
    # print("new_val:", bytes(new_val))
    old_val = new(itimerspec_t)
    # print(new_val, old_val)
    r = timer_settime_(tid, 0, new_val, old_val)
    if r != 0:
        raise RuntimeError("timer_settime_ error: %d (errno = %d)" % (r, os.errno()))
    # print("old_val:", bytes(old_val))

# Timer class

class Timer:

    PERIODIC = 0
    ONE_SHOT = 1

    def __init__(self, id):
        self.id = id
        self._valid = False

    def init(self, mode=PERIODIC, period=-1, callback=None):
        self.tid = timer_create(self.id)
        self.mode = mode
        self.period = period
        self.cb = callback
        timer_settime(self.tid, self.period, self.mode == Timer.PERIODIC)
        self.handler_ref = self.handler
        # print("Sig %d: %s" % (SIGRTMIN + self.id, self.org_sig))
        self.action = sigaction(SIGRTMIN + self.id, self.handler_ref)
        self._valid = True

    def deinit(self):
        if self._valid:
            timer_settime(self.tid, 0, self.mode == Timer.PERIODIC)
            # timer_delete(self.tid)
            self._valid = False

    def handler(self, signum=-1):
        # print('Signal handler called with signal', signum)
        try:
            self.cb(self)
        except:
            self.deinit()
            raise

