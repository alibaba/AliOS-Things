##############################################################################
# Pure/Hybrid micropython lvgl display driver for ili9341 and ili9488 on ESP32
#
# For ili9341 display:
#
#   Build micropython with
#     LV_CFLAGS="-DLV_COLOR_DEPTH=16 -DLV_COLOR_16_SWAP=1"
#   (make parameter) to configure LVGL use the same color format as ili9341
#   and prevent the need to loop over all pixels to translate them.
#
# For ili9488 display:
#
#   Build micropython with
#     LV_CFLAGS="-DLV_COLOR_DEPTH=32"
#   (make parameter) to configure LVGL use the ARGB color format, which can be
#   easily converted to RGB format used by ili9488 display.
#
#   Default SPI freq is set to 40MHz. This means cca 9fps of full screen
#   redraw. to increase FPS, you can use 80MHz SPI - easily add parameter
#   mhz=80 in initialization of driver.
#
# For gc9a01 display:
#
#   Build micropython with
#     LV_CFLAGS="-DLV_COLOR_DEPTH=16 -DLV_COLOR_16_SWAP=1"
#   (make parameter) to configure LVGL use the same color format as ili9341
#   and prevent the need to loop over all pixels to translate them.
#
#   Default SPI freq is set to 60MHz as that is the maximum the tested dislay
#   would suport despite the datasheet suggesting that higher freqs would be
#   supported
#
# Critical function for high FPS are flush and ISR.
# when "hybrid=True", use C implementation for these functions instead of
# pure python implementation. This improves each frame in about 15ms!
#
# When hybrid=False driver is pure micropython.
# Pure Micropython could be viable when ESP32 supports Viper code emitter.
#
# ili9488 driver DO NOT support pure micropython now (because of required
# color convert). Pure micropython is only supported the for ili9341 and
# gc9a01 displays!
##############################################################################

import espidf as esp
import lvgl as lv
import lv_utils
import micropython
import gc

micropython.alloc_emergency_exception_buf(256)
# gc.threshold(0x10000) # leave enough room for SPI master TX DMA buffers

# Constants

COLOR_MODE_RGB = const(0x00)
COLOR_MODE_BGR = const(0x08)

MADCTL_MH = const(0x04)
MADCTL_ML = const(0x10)
MADCTL_MV = const(0x20)
MADCTL_MX = const(0x40)
MADCTL_MY = const(0x80)

PORTRAIT = MADCTL_MX
LANDSCAPE = MADCTL_MV

DISPLAY_TYPE_ILI9341 = const(1)
DISPLAY_TYPE_ILI9488 = const(2)
DISPLAY_TYPE_GC9A01 = const(3)

class ili9XXX:


    TRANS_BUFFER_LEN = const(16)

    display_name = 'ili9XXX'
    display_type = 0

    init_cmds = [ ]

    # Default values of "power" and "backlight" are reversed logic! 0 means ON.
    # You can change this by setting backlight_on and power_on arguments.

    def __init__(self,
        miso=5, mosi=18, clk=19, cs=13, dc=12, rst=4, power=14, backlight=15, backlight_on=0, power_on=0,
        spihost=esp.HSPI_HOST, mhz=40, factor=4, hybrid=True, width=240, height=320,
        colormode=COLOR_MODE_BGR, rot=PORTRAIT, invert=False, double_buffer=True, half_duplex=True, display_type=0,
        asynchronous=False, initialize=True
    ):

        # Initializations

        if not lv.is_initialized():
            lv.init()

        self.asynchronous = asynchronous
        self.initialize = initialize

        self.width = width
        self.height = height

        self.miso = miso
        self.mosi = mosi
        self.clk = clk
        self.cs = cs
        self.dc = dc
        self.rst = rst
        self.power = power
        self.backlight = backlight
        self.backlight_on = backlight_on
        self.power_on = power_on
        self.spihost = spihost
        self.mhz = mhz
        self.factor = factor
        self.hybrid = hybrid
        self.half_duplex = half_duplex

        self.buf_size = (self.width * self.height * lv.color_t.__SIZE__) // factor

        if invert:
            self.init_cmds.append({'cmd': 0x21})

        # Register display driver 

        self.buf1 = esp.heap_caps_malloc(self.buf_size, esp.MALLOC_CAP.DMA)
        self.buf2 = esp.heap_caps_malloc(self.buf_size, esp.MALLOC_CAP.DMA) if double_buffer else None
        
        if self.buf1 and self.buf2:
            print("Double buffer")
        elif self.buf1:
            print("Single buffer")
        else:
            raise RuntimeError("Not enough DMA-able memory to allocate display buffer")

        self.disp_buf = lv.disp_draw_buf_t()
        self.disp_drv = lv.disp_drv_t()

        self.disp_buf.init(self.buf1, self.buf2, self.buf_size // lv.color_t.__SIZE__)
        self.disp_drv.init()
        self.disp_spi_init()

        self.disp_drv.user_data = {'dc': self.dc, 'spi': self.spi, 'dt': self.display_type}
        self.disp_drv.draw_buf = self.disp_buf
        self.disp_drv.flush_cb = esp.ili9xxx_flush if hybrid and hasattr(esp, 'ili9xxx_flush') else self.flush
        self.disp_drv.monitor_cb = self.monitor
        self.disp_drv.hor_res = self.width
        self.disp_drv.ver_res = self.height
        
        if self.initialize:
            self.init()

        if not lv_utils.event_loop.is_running():
            self.event_loop = lv_utils.event_loop(asynchronous=self.asynchronous)


    ######################################################

    def disp_spi_init(self):

        # TODO: Register finalizer callback to deinit SPI.
        # That would get called on soft reset.

        buscfg = esp.spi_bus_config_t({
            "miso_io_num": self.miso,
            "mosi_io_num": self.mosi,
            "sclk_io_num": self.clk,
            "quadwp_io_num": -1,
            "quadhd_io_num": -1,
            "max_transfer_sz": self.buf_size,
        })

        devcfg_flags = esp.SPI_DEVICE.NO_DUMMY
        if self.half_duplex:
            devcfg_flags |= esp.SPI_DEVICE.HALFDUPLEX

        devcfg = esp.spi_device_interface_config_t({
            "clock_speed_hz": self.mhz*1000*1000,   # Clock out at DISP_SPI_MHZ MHz
            "mode": 0,                              # SPI mode 0
            "spics_io_num": self.cs,                # CS pin
            "queue_size": 2,
            "flags": devcfg_flags,
            "duty_cycle_pos": 128,
        })

        if self.hybrid and hasattr(esp, 'ili9xxx_post_cb_isr'):
            devcfg.pre_cb = None
            devcfg.post_cb = esp.ili9xxx_post_cb_isr
        else:
            devcfg.pre_cb = esp.ex_spi_pre_cb_isr
            devcfg.post_cb = esp.ex_spi_post_cb_isr

        esp.gpio_pad_select_gpio(self.cs)

        # Initialize the SPI bus, if needed.

        if buscfg.miso_io_num >= 0 and \
           buscfg.mosi_io_num >= 0 and \
           buscfg.sclk_io_num >= 0:

                esp.gpio_pad_select_gpio(self.miso)
                esp.gpio_pad_select_gpio(self.mosi)
                esp.gpio_pad_select_gpio(self.clk)

                esp.gpio_set_direction(self.miso, esp.GPIO_MODE.INPUT)
                esp.gpio_set_pull_mode(self.miso, esp.GPIO.PULLUP_ONLY)
                esp.gpio_set_direction(self.mosi, esp.GPIO_MODE.OUTPUT)
                esp.gpio_set_direction(self.clk, esp.GPIO_MODE.OUTPUT)

                ret = esp.spi_bus_initialize(self.spihost, buscfg, 1)
                if ret != 0: raise RuntimeError("Failed initializing SPI bus")

        self.trans_buffer = esp.heap_caps_malloc(TRANS_BUFFER_LEN, esp.MALLOC_CAP.DMA)
        self.cmd_trans_data = self.trans_buffer.__dereference__(1)
        self.word_trans_data = self.trans_buffer.__dereference__(4)

        # Attach the LCD to the SPI bus

        ptr_to_spi = esp.C_Pointer()
        ret = esp.spi_bus_add_device(self.spihost, devcfg, ptr_to_spi)
        if ret != 0: raise RuntimeError("Failed adding SPI device")
        self.spi = ptr_to_spi.ptr_val

        self.bytes_transmitted = 0
        completed_spi_transaction = esp.spi_transaction_t()
        cast_spi_transaction_instance = esp.spi_transaction_t.__cast_instance__

        def post_isr(arg):
            reported_transmitted = self.bytes_transmitted
            if reported_transmitted > 0:
                print('- Completed DMA of %d bytes (mem_free=0x%X)' % (reported_transmitted , gc.mem_free()))
                self.bytes_transmitted -= reported_transmitted

        # Called in ISR context!
        def flush_isr(spi_transaction_ptr):
            self.disp_drv.flush_ready()
            # esp.spi_device_release_bus(self.spi)
            esp.get_ccount(self.end_time_ptr)

            # cast_spi_transaction_instance(completed_spi_transaction, spi_transaction_ptr)
            # self.bytes_transmitted += completed_spi_transaction.length
            # try:
            #     micropython.schedule(post_isr, None)
            # except RuntimeError:
            #     pass

        self.spi_callbacks = esp.spi_transaction_set_cb(None, flush_isr)

    #
    # Deinitialize SPI device and bus, and free memory
    # This function is called from finilizer during gc sweep - therefore must not allocate memory!
    #

    trans_result_ptr = esp.C_Pointer()

    def deinit(self):

        print('Deinitializing {}..'.format(self.display_name))

        # Prevent callbacks to lvgl, which refer to the buffers we are about to delete

        if lv_utils.event_loop.is_running():
            self.event_loop.deinit()

        self.disp_drv.remove()

        if self.spi:

            # Pop all pending transaction results

            ret = 0
            while ret == 0:
                ret = esp.spi_device_get_trans_result(self.spi, self.trans_result_ptr , 1)

            # Remove device

            esp.spi_bus_remove_device(self.spi)
            self.spi = None

            # Free SPI bus

            esp.spi_bus_free(self.spihost)
            self.spihost = None

        # Free RAM

        if self.buf1:
            esp.heap_caps_free(self.buf1)
            self.buf1 = None

        if self.buf2:
            esp.heap_caps_free(self.buf2)
            self.buf2 = None

        if self.trans_buffer:
            esp.heap_caps_free(self.trans_buffer)
            self.trans_buffer = None


    ######################################################

    trans = esp.spi_transaction_t() # .__cast__(
#                esp.heap_caps_malloc(
#                    esp.spi_transaction_t.__SIZE__, esp.MALLOC_CAP.DMA))

    def spi_send(self, data):
        self.trans.length = len(data) * 8   # Length is in bytes, transaction length is in bits. 
        self.trans.tx_buffer = data         # data should be allocated as DMA-able memory
        self.trans.user = None
        esp.spi_device_polling_transmit(self.spi, self.trans)

    def spi_send_dma(self, data):
        self.trans.length = len(data) * 8   # Length is in bytes, transaction length is in bits. 
        self.trans.tx_buffer = data         # data should be allocated as DMA-able memory
        self.trans.user = self.spi_callbacks
        esp.spi_device_queue_trans(self.spi, self.trans, -1)

    ######################################################
    ######################################################

    def send_cmd(self, cmd):
        esp.gpio_set_level(self.dc, 0)	    # Command mode
        self.cmd_trans_data[0] = cmd
        self.spi_send(self.cmd_trans_data)

    def send_data(self, data):
        esp.gpio_set_level(self.dc, 1)	    # Data mode
        if len(data) > TRANS_BUFFER_LEN: raise RuntimeError('Data too long, please use DMA!')
        trans_data = self.trans_buffer.__dereference__(len(data))
        trans_data[:] = data[:]
        self.spi_send(trans_data)

    def send_trans_word(self):
        esp.gpio_set_level(self.dc, 1)	    # Data mode
        self.spi_send(self.word_trans_data)

    def send_data_dma(self, data):          # data should be allocated as DMA-able memory
        esp.gpio_set_level(self.dc, 1)      # Data mode
        self.spi_send_dma(data)

    ######################################################

    async def _init(self, sleep_func):

        # Initialize non-SPI GPIOs

        esp.gpio_pad_select_gpio(self.dc)
        if self.rst != -1: esp.gpio_pad_select_gpio(self.rst)
        if self.backlight != -1: esp.gpio_pad_select_gpio(self.backlight)
        if self.power != -1: esp.gpio_pad_select_gpio(self.power)

        esp.gpio_set_direction(self.dc, esp.GPIO_MODE.OUTPUT)
        if self.rst != -1: esp.gpio_set_direction(self.rst, esp.GPIO_MODE.OUTPUT)
        if self.backlight != -1: esp.gpio_set_direction(self.backlight, esp.GPIO_MODE.OUTPUT)
        if self.power != -1: esp.gpio_set_direction(self.power, esp.GPIO_MODE.OUTPUT)

        # Power the display

        if self.power != -1:
            esp.gpio_set_level(self.power, self.power_on)
            await sleep_func(100)

        # Reset the display

        if self.rst != -1:
            esp.gpio_set_level(self.rst, 0)
            await sleep_func(100)
            esp.gpio_set_level(self.rst, 1)
            await sleep_func(100)

        # Send all the commands

        for cmd in self.init_cmds:
            self.send_cmd(cmd['cmd'])
            if 'data' in cmd:
                self.send_data(cmd['data'])
            if 'delay' in cmd:
                await sleep_func(cmd['delay'])

        print("{} initialization completed".format(self.display_name))

        # Enable backlight

        if self.backlight != -1:
            print("Enable backlight")
            esp.gpio_set_level(self.backlight, self.backlight_on)

        # Register the driver
        self.disp_drv.register()

    
    def init(self):
        import utime
        generator = self._init(lambda ms:(yield ms))
        try:
            while True:
                ms = next(generator)
                utime.sleep_ms(ms)
        except StopIteration:
            pass

    async def init_async(self):
        import uasyncio
        await self._init(uasyncio.sleep_ms)

    def power_down(self):

        if self.power != -1:
            esp.gpio_set_level(self.power, 1 - self.power_on)

        if self.backlight != -1:
            esp.gpio_set_level(self.backlight, 1 - self.backlight_on)

    
    ######################################################

    start_time_ptr = esp.C_Pointer()
    end_time_ptr = esp.C_Pointer()
    flush_acc_setup_cycles = 0
    flush_acc_dma_cycles = 0

    def flush(self, disp_drv, area, color_p):

        if self.end_time_ptr.int_val and self.end_time_ptr.int_val > self.start_time_ptr.int_val:
            self.flush_acc_dma_cycles +=  self.end_time_ptr.int_val - self.start_time_ptr.int_val

        esp.get_ccount(self.start_time_ptr)

        # esp.spi_device_acquire_bus(self.spi, esp.ESP.MAX_DELAY)

        # Column addresses

        self.send_cmd(0x2A);

        self.word_trans_data[0] = (area.x1 >> 8) & 0xFF
        self.word_trans_data[1] = area.x1 & 0xFF
        self.word_trans_data[2] = (area.x2 >> 8) & 0xFF
        self.word_trans_data[3] = area.x2 & 0xFF
        self.send_trans_word()

        # Page addresses

        self.send_cmd(0x2B);

        self.word_trans_data[0] = (area.y1 >> 8) & 0xFF
        self.word_trans_data[1] = area.y1 & 0xFF
        self.word_trans_data[2] = (area.y2 >> 8) & 0xFF
        self.word_trans_data[3] = area.y2 & 0xFF
        self.send_trans_word()

        # Memory write by DMA, disp_flush_ready when finished

        self.send_cmd(0x2C)

        size = (area.x2 - area.x1 + 1) * (area.y2 - area.y1 + 1)
        data_view = color_p.__dereference__(size * lv.color_t.__SIZE__)

        esp.get_ccount(self.end_time_ptr)
        if self.end_time_ptr.int_val > self.start_time_ptr.int_val:
            self.flush_acc_setup_cycles += self.end_time_ptr.int_val - self.start_time_ptr.int_val
        esp.get_ccount(self.start_time_ptr)

        self.send_data_dma(data_view)
        
    ######################################################

    monitor_acc_time = 0
    monitor_acc_px = 0
    monitor_count = 0

    cycles_in_ms = esp.esp_clk_cpu_freq() // 1000

    def monitor(self, disp_drv, time, px):
        self.monitor_acc_time += time
        self.monitor_acc_px += px
        self.monitor_count += 1

    def stat(self):
        if self.monitor_count == 0:
            return None

        time = self.monitor_acc_time // self.monitor_count
        setup = self.flush_acc_setup_cycles // (self.monitor_count * self.cycles_in_ms)
        dma = self.flush_acc_dma_cycles // (self.monitor_count * self.cycles_in_ms)
        px = self.monitor_acc_px // self.monitor_count

        self.monitor_acc_time = 0
        self.monitor_acc_px = 0
        self.monitor_count = 0
        self.flush_acc_setup_cycles = 0
        self.flush_acc_dma_cycles = 0

        return time, setup, dma, px



class ili9341(ili9XXX):

    def __init__(self,
        miso=5, mosi=18, clk=19, cs=13, dc=12, rst=4, power=14, backlight=15, backlight_on=0, power_on=0,
        spihost=esp.HSPI_HOST, mhz=40, factor=4, hybrid=True, width=240, height=320,
        colormode=COLOR_MODE_BGR, rot=PORTRAIT, invert=False, double_buffer=True, half_duplex=True,
        asynchronous=False, initialize=True
    ):

        # Make sure Micropython was built such that color won't require processing before DMA

        if lv.color_t.__SIZE__ != 2:
            raise RuntimeError('ili9341 micropython driver requires defining LV_COLOR_DEPTH=16')
        if colormode == COLOR_MODE_BGR and not hasattr(lv.color_t().ch, 'green_l'):
            raise RuntimeError('ili9341 BGR color mode requires defining LV_COLOR_16_SWAP=1')

        self.display_name = 'ILI9341'
        self.display_type = DISPLAY_TYPE_ILI9341

        self.init_cmds = [
            {'cmd': 0xCF, 'data': bytes([0x00, 0x83, 0X30])},
            {'cmd': 0xED, 'data': bytes([0x64, 0x03, 0X12, 0X81])},
            {'cmd': 0xE8, 'data': bytes([0x85, 0x01, 0x79])},
            {'cmd': 0xCB, 'data': bytes([0x39, 0x2C, 0x00, 0x34, 0x02])},
            {'cmd': 0xF7, 'data': bytes([0x20])},
            {'cmd': 0xEA, 'data': bytes([0x00, 0x00])},
            {'cmd': 0xC0, 'data': bytes([0x26])},               # Power control
            {'cmd': 0xC1, 'data': bytes([0x11])},               # Power control
            {'cmd': 0xC5, 'data': bytes([0x35, 0x3E])},	        # VCOM control
            {'cmd': 0xC7, 'data': bytes([0xBE])},               # VCOM control
            {'cmd': 0x36, 'data': bytes([rot | colormode])},    # Memory Access Control
            {'cmd': 0x3A, 'data': bytes([0x55])},               # Pixel Format Set
            {'cmd': 0xB1, 'data': bytes([0x00, 0x1B])},
            {'cmd': 0xF2, 'data': bytes([0x08])},
            {'cmd': 0x26, 'data': bytes([0x01])},
            {'cmd': 0xE0, 'data': bytes([0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00])},
            {'cmd': 0XE1, 'data': bytes([0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F])},
            {'cmd': 0x2A, 'data': bytes([0x00, 0x00, 0x00, 0xEF])},
            {'cmd': 0x2B, 'data': bytes([0x00, 0x00, 0x01, 0x3f])},
            {'cmd': 0x2C, 'data': bytes([0])},
            {'cmd': 0xB7, 'data': bytes([0x07])},
            {'cmd': 0xB6, 'data': bytes([0x0A, 0x82, 0x27, 0x00])},
            {'cmd': 0x11, 'data': bytes([0]), 'delay':100},
            {'cmd': 0x29, 'data': bytes([0]), 'delay':100}
        ]

        super().__init__(miso, mosi, clk, cs, dc, rst, power, backlight, backlight_on, power_on,
            spihost, mhz, factor, hybrid, width, height, colormode, rot, invert, double_buffer, half_duplex,
            asynchronous=asynchronous, initialize=initialize)


class ili9488(ili9XXX):

    def __init__(self,
        miso=5, mosi=18, clk=19, cs=13, dc=12, rst=4, power=14, backlight=15, backlight_on=0, power_on=0,
        spihost=esp.HSPI_HOST, mhz=40, factor=8, hybrid=True, width=320, height=480,
        colormode=COLOR_MODE_RGB, rot=PORTRAIT, invert=False, double_buffer=True, half_duplex=True,
        asynchronous=False, initialize=True
    ):

        if lv.color_t.__SIZE__ != 4:
            raise RuntimeError('ili9488 micropython driver requires defining LV_COLOR_DEPTH=32')
        if not hybrid:
            raise RuntimeError('ili9488 micropython driver do not support non-hybrid driver')

        self.display_name = 'ILI9488'
        self.display_type = DISPLAY_TYPE_ILI9488

        self.init_cmds = [
            {'cmd': 0x01, 'data': bytes([0]), 'delay': 200},
            {'cmd': 0x11, 'data': bytes([0]), 'delay': 120},
            {'cmd': 0xE0, 'data': bytes([0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78, 0x4C, 0x09, 0x0A, 0x08, 0x16, 0x1A, 0x0F])},
            {'cmd': 0xE1, 'data': bytes([0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45, 0x46, 0x04, 0x0E, 0x0D, 0x35, 0x37, 0x0F])},
            {'cmd': 0xC0, 'data': bytes([0x17, 0x15])}, ### 0x13, 0x13
            {'cmd': 0xC1, 'data': bytes([0x41])},       ### 
            {'cmd': 0xC2, 'data': bytes([0x44])},       ### 
            {'cmd': 0xC5, 'data': bytes([0x00, 0x12, 0x80])},
            #{'cmd': 0xC5, 'data': bytes([0x00, 0x0, 0x0, 0x0])},
            {'cmd': 0x36, 'data': bytes([rot | colormode])},    # Memory Access Control
            {'cmd': 0x3A, 'data': bytes([0x66])},
            {'cmd': 0xB0, 'data': bytes([0x00])},
            {'cmd': 0xB1, 'data': bytes([0xA0])},
            {'cmd': 0xB4, 'data': bytes([0x02])},
            {'cmd': 0xB6, 'data': bytes([0x02, 0x02])},
            {'cmd': 0xE9, 'data': bytes([0x00])},
            {'cmd': 0x53, 'data': bytes([0x28])},
            {'cmd': 0x51, 'data': bytes([0x7F])},
            {'cmd': 0xF7, 'data': bytes([0xA9, 0x51, 0x2C, 0x02])},
            {'cmd': 0x29, 'data': bytes([0]), 'delay': 120}
        ]

        super().__init__(miso, mosi, clk, cs, dc, rst, power, backlight, backlight_on, power_on,
            spihost, mhz, factor, hybrid, width, height, colormode, rot, invert, double_buffer, half_duplex, display_type=DISPLAY_TYPE_ILI9488,
            asynchronous=asynchronous, initialize=initialize)


class gc9a01(ili9XXX):
    # On the tested display the write direction and colormode appear to be
    # reversed from how they are presented in the datasheet

    def __init__(self,
        miso=5, mosi=18, clk=19, cs=13, dc=12, rst=4, power=14, backlight=15, backlight_on=0, power_on=0,
        spihost=esp.HSPI_HOST, mhz=60, factor=4, hybrid=True, width=240, height=240,
        colormode=COLOR_MODE_RGB, rot=PORTRAIT, invert=False, double_buffer=True, half_duplex=True,
        asynchronous=False, initialize=True
    ):

        if lv.color_t.SIZE != 2:
            raise RuntimeError('gc9a01 micropython driver requires defining LV_COLOR_DEPTH=16')

        # This is included as the color mode appears to be reversed from the
        # datasheet and the ili9XXX driver values
        if colormode == COLOR_MODE_RGB:
            self.colormode = COLOR_MODE_BGR
        elif colormode == COLOR_MODE_BGR:
            self.colormode = COLOR_MODE_RGB

        self.display_name = 'GC9A01'
        self.display_type = DISPLAY_TYPE_GC9A01

        self.init_cmds = [
            {'cmd': 0xEF, 'data': bytes([0])},
            {'cmd': 0xEB, 'data': bytes([0x14])},
            {'cmd': 0xFE, 'data': bytes([0])},
            {'cmd': 0xEF, 'data': bytes([0])},
            {'cmd': 0xEB, 'data': bytes([0x14])},
            {'cmd': 0x84, 'data': bytes([0x40])},
            {'cmd': 0x85, 'data': bytes([0xFF])},
            {'cmd': 0x86, 'data': bytes([0xFF])},
            {'cmd': 0x87, 'data': bytes([0xFF])},
            {'cmd': 0x88, 'data': bytes([0x0A])},
            {'cmd': 0x89, 'data': bytes([0x21])},
            {'cmd': 0x8A, 'data': bytes([0x00])},
            {'cmd': 0x8B, 'data': bytes([0x80])},
            {'cmd': 0x8C, 'data': bytes([0x01])},
            {'cmd': 0x8D, 'data': bytes([0x01])},
            {'cmd': 0x8E, 'data': bytes([0xFF])},
            {'cmd': 0x8F, 'data': bytes([0xFF])},
            {'cmd': 0xB6, 'data': bytes([0x00, 0x00])}, 
            {'cmd': 0x36, 'data': bytes([rot | self.colormode])},
            {'cmd': 0x3A, 'data': bytes([0x05])},
            {'cmd': 0x90, 'data': bytes([0x08, 0x08, 0x08, 0x08])},
            {'cmd': 0xBD, 'data': bytes([0x06])},
            {'cmd': 0xBC, 'data': bytes([0x00])},
            {'cmd': 0xFF, 'data': bytes([0x60, 0x01, 0x04])},
            {'cmd': 0xC3, 'data': bytes([0x13])},
            {'cmd': 0xC4, 'data': bytes([0x13])},
            {'cmd': 0xC9, 'data': bytes([0x22])},
            {'cmd': 0xBE, 'data': bytes([0x11])},
            {'cmd': 0xE1, 'data': bytes([0x10, 0x0E])},
            {'cmd': 0xDF, 'data': bytes([0x21, 0x0c, 0x02])},
            {'cmd': 0xF0, 'data': bytes([0x45, 0x09, 0x08, 0x08, 0x26, 0x2A])},
            {'cmd': 0xF1, 'data': bytes([0x43, 0x70, 0x72, 0x36, 0x37, 0x6F])},
            {'cmd': 0xF2, 'data': bytes([0x45, 0x09, 0x08, 0x08, 0x26, 0x2A])},
            {'cmd': 0xF3, 'data': bytes([0x43, 0x70, 0x72, 0x36, 0x37, 0x6F])},
            {'cmd': 0xED, 'data': bytes([0x1B, 0x0B])},
            {'cmd': 0xAE, 'data': bytes([0x77])},
            {'cmd': 0xCD, 'data': bytes([0x63])},
            {'cmd': 0x70, 'data': bytes([0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03])},
            {'cmd': 0xE8, 'data': bytes([0x34])},
            {'cmd': 0x62, 'data': bytes([0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70])},
            {'cmd': 0x63, 'data': bytes([0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70])},
            {'cmd': 0x64, 'data': bytes([0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07])},
            {'cmd': 0x66, 'data': bytes([0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00])},
            {'cmd': 0x67, 'data': bytes([0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98])},
            {'cmd': 0x74, 'data': bytes([0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00])},
            {'cmd': 0x98, 'data': bytes([0x3e, 0x07])},
            {'cmd': 0x35, 'data': bytes([0])},
            {'cmd': 0x21, 'data': bytes([0])},
            {'cmd': 0x11, 'data': bytes([0]), 'delay': 20},
            {'cmd': 0x29, 'data': bytes([0]), 'delay': 120}
        ]
        
        super().__init__(miso, mosi, clk, cs, dc, rst, power, backlight, backlight_on, power_on,
            spihost, mhz, factor, hybrid, width, height, self.colormode, rot, invert, double_buffer, half_duplex, display_type=self.display_type,
            asynchronous=asynchronous, initialize=initialize)
