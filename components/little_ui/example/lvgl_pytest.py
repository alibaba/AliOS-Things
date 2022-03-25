
#lvgl python test case
import lvgl as lv
lv.init()
import display
display.init()
draw_buf = lv.disp_draw_buf_t()
buf1_1 = bytearray(51200)
buf1_2 = bytearray(51200)
draw_buf.init(buf1_1, buf1_2, 25600)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = draw_buf
disp_drv.flush_cb = display.flush
disp_drv.hor_res = 320
disp_drv.ver_res = 240
disp_drv.register()
indev_drv=lv.indev_drv_t()
indev_drv.init()
indev_drv.read_cb = display.touch_read
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.register()
scr = lv.obj()
btn = lv.btn(scr)
btn.align_to(lv.scr_act(), lv.ALIGN.CENTER, 0, 0)
label = lv.label(btn)
label.set_text("Hello World!")
lv.scr_load(scr)
