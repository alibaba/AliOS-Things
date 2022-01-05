import axp192
try:
    # for m5stack-core2 only
    axp = axp192.Axp192()
    axp.powerAll()
    axp.setLCDBrightness(80) # 设置背光亮度 0～100
except OSError:
    print("make sure axp192.py is in libs folder")

