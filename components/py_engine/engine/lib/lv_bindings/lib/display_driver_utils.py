import usys as sys
sys.path.append('') # See: https://github.com/micropython/micropython/issues/6419

import lvgl as lv
import lv_utils

ORIENT_LANDSCAPE = False
ORIENT_PORTRAIT  = True

class driver:
    
    def __init__(self,width=420,height=320,orientation=ORIENT_PORTRAIT):

        if not lv.is_initialized():
            lv.init()

        self.width = width
        self.height = height
        self.orientation = orientation
        self.disp = None
        self.touch = None
        self.type = None
        if not lv_utils.event_loop.is_running():
            self.init_gui()
        
    def init_gui_SDL(self):

        import SDL
        SDL.init(w=self.width, h=self.height, auto_refresh=False)
        self.event_loop = lv_utils.event_loop(refresh_cb = SDL.refresh)

        # Register SDL display driver.

        disp_buf1 = lv.disp_draw_buf_t()
        buf1_1 = bytearray(self.width*10)
        disp_buf1.init(buf1_1, None, len(buf1_1)//4)
        disp_drv = lv.disp_drv_t()
        disp_drv.init()
        disp_drv.draw_buf = disp_buf1
        disp_drv.flush_cb = SDL.monitor_flush
        disp_drv.hor_res = self.width
        disp_drv.ver_res = self.height
        disp_drv.register()

        # Register SDL mouse driver

        indev_drv = lv.indev_drv_t()
        indev_drv.init() 
        indev_drv.type = lv.INDEV_TYPE.POINTER;
        indev_drv.read_cb = SDL.mouse_read;
        indev_drv.register();

        self.type = "SDL"
        print("Running the SDL lvgl version")
        
    def init_gui_ili9341(self):

        # Initialize ILI9341 display

        from ili9XXX import ili9341,LANDSCAPE
        from xpt2046 import xpt2046
        import espidf as esp

        if self.orientation == ORIENT_PORTRAIT:
            print ("Running the ili9341 lvgl version in portait mode")

            # Initialize ILI9341 display in prtrait mode
            # the following are the settings for the Lolin tft 2.4 display
            # self.disp = ili9341(miso=19,mosi=23,clk=18, cs=26, dc=5, rst=-1, power=-1, backlight=-1, spihost=esp.VSPI_HOST)
            # self.touch = xpt2046(spihost=esp.VSPI_HOST,cal_x0=3751, cal_x1 = 210, cal_y0=3906, cal_y1 = 283, transpose=True)
            
            self.disp = ili9341()
            self.touch = xpt2046()

        elif self.orientation == ORIENT_LANDSCAPE:
            print ("Running the ili9341 lvgl version in landscape mode")
            # Initialize ILI9341 display
            # the following are the settings for the Lolin tft 2.4 display
            # self.disp = ili9341(miso=19,mosi=23,clk=18, cs=26, dc=5, rst=-1, power=-1, backlight=-1, backlight_on=0,
            #                     spihost=esp.VSPI_HOST, width=320, height=240, rot=LANDSCAPE)
            # self.touch = xpt2046(spihost=esp.VSPI_HOST,cal_x0=3799, cal_x1 = 353, cal_y0=220, cal_y1 = 3719, transpose=False)
            
            # Register xpt2046 touch driver
            self.disp = ili9341(width=320, height=240, rot=LANDSCAPE)
            self.touch = xpt2046(cal_x0=3799, cal_x1 = 353, cal_y0=220, cal_y1 = 3719,transpose = False)

        else:
            raise RuntimeError("Invalid orientation")

        self.type="ili9341"
        
        '''
        # Register raw resistive touch driver (remove xpt2046 initialization first)
        import rtch
        touch = rtch.touch(xp = 32, yp = 33, xm = 25, ym = 26, touch_rail = 27, touch_sense = 33)
        touch.init()
        indev_drv = lv.indev_drv_t()
        lv.indev_drv_init(indev_drv) 
        indev_drv.type = lv.INDEV_TYPE.POINTER;
        indev_drv.read_cb = touch.read;
        lv.indev_drv_register(indev_drv);
        '''

    def init_gui_twatch(self):

            import ttgo
            from axp_constants import AXP202_VBUS_VOL_ADC1,AXP202_VBUS_CUR_ADC1,AXP202_BATT_CUR_ADC1,AXP202_BATT_VOL_ADC1
                
            watch = ttgo.Watch()
            tft = watch.tft
            power = watch.pmu
            power.adc1Enable(AXP202_VBUS_VOL_ADC1
                             | AXP202_VBUS_CUR_ADC1 
                             | AXP202_BATT_CUR_ADC1
                             | AXP202_BATT_VOL_ADC1, True)
            watch.lvgl_begin()
            watch.tft.backlight_fade(100)

            self.type="t-watch"            
            print("Running lvgl on the LilyGo t-watch 2020")
            
    def init_gui(self):
        
        # Identify platform and initialize it

        try:
            self.init_gui_twatch()
            return
        except:
            pass

        try:
            self.init_gui_ili9341()
            return
        except ImportError:
            pass
            
        try:
            self.init_gui_SDL()
            return
        except ImportError:
            pass

        raise RuntimeError("Could not find a suitable display driver!")

