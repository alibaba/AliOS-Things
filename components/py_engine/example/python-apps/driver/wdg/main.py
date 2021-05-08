
# coding=utf-8
# This is a sample Python script.
import utime
from driver import WDG

print("-------------------wdg test--------------------")
wdg = WDG()
wdg.open()
wdg.start(3000)
#utime.sleep_ms(4000)
wdg.feed()
wdg.close()
