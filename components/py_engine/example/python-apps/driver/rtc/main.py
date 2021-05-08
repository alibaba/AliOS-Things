
from driver import RTC

print("-------------------rtc test--------------------")
rtc = RTC()
rtc.open()
rtc.setTime(21, 3, 19, 18, 25, 8)
value = rtc.getTime()
print(value)
rtc.close()
print("-------------------rtc test--------------------")
