# Stub Timer implementation for the JS port.
#
# This does absolutely nothing beyond provide a class, so it will
# not be suitable for any application which needs a timer.
#
# MIT license; Copyright (c) 2021 embeddedt


class Timer:

    PERIODIC = 0
    ONE_SHOT = 1

    def __init__(self, id):
        pass

    def init(self, mode=PERIODIC, period=-1, callback=None):
        pass

    def deinit(self):
        pass
