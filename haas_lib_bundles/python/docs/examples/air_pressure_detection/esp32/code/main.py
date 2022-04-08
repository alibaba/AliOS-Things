import lvgl as lv
from display import LVGL as disp
from bmp280 import *
import ujson

lv.init()
disp.init()


def set_value(indic, v):
    meter.set_indicator_value(indic, v)

# 压力值转海拔高度


def convertPressure2Altitude(p, t):
    height = ((1-((p/101325)**(1/5.255)))*(t+273.15)/0.0065)
    return height


#
# A simple meter
#
meter = lv.meter(lv.scr_act())
meter.center()
meter.set_size(200, 200)

# Add a scale first
scale = meter.add_scale()
meter.set_scale_ticks(scale, 81, 2, 10, lv.palette_main(lv.PALETTE.GREY))
meter.set_scale_major_ticks(scale, 10, 4, 15, lv.color_black(), 10)
meter.set_scale_range(scale, 300, 1100, 270, 135)

indic = lv.meter_indicator_t()

# Add arc color for 300-400
indic = meter.add_arc(scale, 3, lv.color_hex(0x0000CD), 0)
meter.set_indicator_start_value(indic, 300)
meter.set_indicator_end_value(indic, 400)
# Add the tick lines color for 300-400
indic = meter.add_scale_lines(scale, lv.color_hex(
    0x0000CD), lv.color_hex(0x0000CD), False, 0)
meter.set_indicator_start_value(indic, 300)
meter.set_indicator_end_value(indic, 400)

# Add arc color for 400-500
indic = meter.add_arc(scale, 3, lv.color_hex(0x0000FF), 0)
meter.set_indicator_start_value(indic, 400)
meter.set_indicator_end_value(indic, 500)
# Add the tick lines color for 400-500
indic = meter.add_scale_lines(scale, lv.color_hex(
    0x0000FF), lv.color_hex(0x0000FF), False, 0)
meter.set_indicator_start_value(indic, 400)
meter.set_indicator_end_value(indic, 500)

# Add arc color for 500-600
indic = meter.add_arc(scale, 3, lv.color_hex(0x1E90FF), 0)
meter.set_indicator_start_value(indic, 500)
meter.set_indicator_end_value(indic, 600)
# Add the tick lines color for 500-600
indic = meter.add_scale_lines(scale, lv.color_hex(
    0x1E90FF), lv.color_hex(0x1E90FF), False, 0)
meter.set_indicator_start_value(indic, 500)
meter.set_indicator_end_value(indic, 600)

# Add arc color for 600-700
indic = meter.add_arc(scale, 3, lv.color_hex(0x00BFFF), 0)
meter.set_indicator_start_value(indic, 600)
meter.set_indicator_end_value(indic, 700)
# Add the tick lines color for 600-700
indic = meter.add_scale_lines(scale, lv.color_hex(
    0x00BFFF), lv.color_hex(0x00BFFF), False, 0)
meter.set_indicator_start_value(indic, 600)
meter.set_indicator_end_value(indic, 700)

# Add arc color for 700-800
indic = meter.add_arc(scale, 3, lv.color_hex(0x7FFFAA), 0)
meter.set_indicator_start_value(indic, 700)
meter.set_indicator_end_value(indic, 800)
# Add the tick lines color for 700-800
indic = meter.add_scale_lines(scale, lv.color_hex(
    0x7FFFAA), lv.color_hex(0x7FFFAA), False, 0)
meter.set_indicator_start_value(indic, 700)
meter.set_indicator_end_value(indic, 800)

# Add arc color for 800-900
indic = meter.add_arc(scale, 3, lv.color_hex(0x00FA9A), 0)
meter.set_indicator_start_value(indic, 800)
meter.set_indicator_end_value(indic, 900)
# Add the tick lines color for 800-900
indic = meter.add_scale_lines(scale, lv.color_hex(
    0x00FA9A), lv.color_hex(0x00FA9A), False, 0)
meter.set_indicator_start_value(indic, 800)
meter.set_indicator_end_value(indic, 900)

# Add arc color for 900-1000
indic = meter.add_arc(scale, 3, lv.color_hex(0x3CB371), 0)
meter.set_indicator_start_value(indic, 900)
meter.set_indicator_end_value(indic, 1000)
# Add the tick lines color for 900-1000
indic = meter.add_scale_lines(scale, lv.color_hex(
    0x3CB371), lv.color_hex(0x3CB371), False, 0)
meter.set_indicator_start_value(indic, 900)
meter.set_indicator_end_value(indic, 1000)

# Add arc color for 1000-1100
indic = meter.add_arc(scale, 3, lv.color_hex(0x2E8B57), 0)
meter.set_indicator_start_value(indic, 1000)
meter.set_indicator_end_value(indic, 1100)
# Add the tick lines color for 1000-1100
indic = meter.add_scale_lines(scale, lv.color_hex(
    0x2E8B57), lv.color_hex(0x2E8B57), False, 0)
meter.set_indicator_start_value(indic, 1000)
meter.set_indicator_end_value(indic, 1100)

# Add a needle line indicator
indic = meter.add_needle_line(scale, 4, lv.palette_main(lv.PALETTE.GREY), -10)

style = lv.style_t()
style.init()
style.set_pad_all(2)
style.set_x(70)
style.set_y(220)
style.set_text_color(lv.palette_main(lv.PALETTE.RED))

spans = lv.spangroup(lv.scr_act())
spans.set_width(300)
spans.set_height(300)
spans.add_style(style, 0)
spans.set_overflow(lv.SPAN_OVERFLOW.CLIP)
spans.set_mode(lv.SPAN_MODE.EXPAND)
span = spans.new_span()

if __name__ == "__main__":
    try:
        print("Testing bmp280")
        i2cDev = I2C()
        i2cDev.open("bmp280")
        baroDev = BMP280(i2cDev)
        while 1:
            # "pressure" - 代表气压传感器测量到的气压值
            pressure = baroDev.getPressure()
            # "temprature" - 代表气压传感器测量到的温度值
            temprature = baroDev.getTemperature()
            print(str(temprature) + ' 摄氏度')
            set_value(indic, int(int(pressure) / 100))
            span.set_text(str(int(pressure) / 100) + ' hPa, ' +
                          str(convertPressure2Altitude(pressure, temprature)) + ' m')
            print(str(int(pressure) / 100) + ' hPa')
            print(str(convertPressure2Altitude(pressure, temprature)) + ' m')
            # 每2秒钟上报一次
            sleep_ms(2000)
        i2cDev.close()
        del baroDev
        print("Test bmp280 done")
    except OSError:
        print("make sure bmp280.py is in libs folder")
