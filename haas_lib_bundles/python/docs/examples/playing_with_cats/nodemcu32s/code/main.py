# -*- encoding: utf-8 -*-

from driver import GPIO
from uln2003 import *

if __name__ == '__main__':
    relay_dev = GPIO()
    relay_dev.open('relay')
    a = GPIO()
    a.open('uln2003_a')
    a_ = GPIO()
    a_.open('uln2003_a_')
    b = GPIO()
    b.open('uln2003_b')
    b_ = GPIO()
    b_.open('uln2003_b_')
    motor_dev= ULN2003(a, a_, b, b_)

    while True:
        relay_val = 0
        # 正转
        for i in range(0, 64):
            relay_dev.write(relay_val)
            if relay_val == 0:
                relay_val = 1
            else:
                relay_val = 0
            for j in range(0, 16):
                motor_dev.motorCw(4)
        # 反转
        for i in range(0, 64):
            relay_dev.write(relay_val)
            if relay_val == 0:
                relay_val = 1
            else:
                relay_val = 0
            for j in range(0, 16):
                motor_dev.motorCcw(4)
