from driver import TIMER
tim0 = TIMER(0)

def cb(test):
    print('timer_test_log1111')

tim0.init(period=1000000, mode=TIMER.PERIODIC, callback=cb)
tim0.stop()
tim0.deinit()


tim1 = TIMER(1)
tim1.init(period=1000000, mode=TIMER.ONE_SHOT, callback=lambda t:print('timer_test_log2222'))

tim1.stop()
tim1.deinit()
