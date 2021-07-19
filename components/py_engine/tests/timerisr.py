from driver import TIMER

tim0 = TIMER(0)
tim0.init(period=1000000, mode=TIMER.PERIODIC, callback=lambda t:print('timer_test_log2222'))

tim0.stop()
tim0.deinit()