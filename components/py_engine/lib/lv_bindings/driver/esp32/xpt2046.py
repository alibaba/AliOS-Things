from machine import Pin
import espidf as esp
import lvgl as lv

# TODO: Viper/native emmitters don't behave well when module is frozen.

class xpt2046:
    
    # Command is 8 bit, but we add another bit as a space before xpt2046 stats sending the response, See Figure 12 on the datasheet

    CMD_X_READ  = const(0b100100000)
    CMD_Y_READ  = const(0b110100000)
    CMD_Z1_READ = const(0b101100000)
    CMD_Z2_READ = const(0b110000000)

    MAX_RAW_COORD = const((1<<12) - 1)

    def __init__(self, miso=-1, mosi=-1, clk=-1, cs=25,
                 spihost=esp.HSPI_HOST, half_duplex=True, mhz=5, max_cmds=16,
                 cal_x0 = 3783, cal_y0 = 3948, cal_x1 = 242, cal_y1 = 423, 
                 transpose = True, samples = 3):

        # Initializations

        if not lv.is_initialized():
            lv.init()

        disp = lv.disp_t.__cast__(None)
        self.screen_width = disp.get_hor_res()
        self.screen_height = disp.get_ver_res()
        self.miso = miso
        self.mosi = mosi
        self.clk = clk
        self.cs = cs
        self.spihost = spihost
        self.half_duplex = half_duplex
        self.mhz = mhz
        self.max_cmds = max_cmds
        self.cal_x0 = cal_x0
        self.cal_y0 = cal_y0
        self.cal_x1 = cal_x1
        self.cal_y1 = cal_y1
        self.transpose = transpose
        self.samples = samples

        self.touch_count = 0
        self.touch_cycles = 0
        
        self.spi_init()

        indev_drv = lv.indev_drv_t()
        indev_drv.init()
        indev_drv.type = lv.INDEV_TYPE.POINTER
        indev_drv.read_cb = self.read
        indev_drv.register()
        
    def calibrate(self, x0, y0, x1, y1):
        self.cal_x0 = x0
        self.cal_y0 = y0
        self.cal_x1 = x1
        self.cal_y1 = y1

    def spi_init(self):
	buscfg = esp.spi_bus_config_t({
            "miso_io_num": self.miso,
	    "mosi_io_num": self.mosi,
	    "sclk_io_num": self.clk,
	    "quadwp_io_num": -1,
	    "quadhd_io_num": -1,
	    "max_transfer_sz": 4,
	})

        devcfg_flags = 0 # esp.SPI_DEVICE.NO_DUMMY
        if self.half_duplex:
            devcfg_flags |= esp.SPI_DEVICE.HALFDUPLEX

	devcfg = esp.spi_device_interface_config_t({
            "command_bits": 9,                      # Actually 8, but need another cycle before xpt starts transmitting response, see Figure 12 on the datasheet.
            "clock_speed_hz": self.mhz*1000*1000,   
            "mode": 0,                              # SPI mode 0
            "spics_io_num": self.cs,                # CS pin
            "queue_size": self.max_cmds,
            "flags": devcfg_flags,
            "duty_cycle_pos": 128,
	})

        esp.gpio_pad_select_gpio(self.cs)

	# Initialize the SPI bus, if needed

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

	# Attach the xpt2046 to the SPI bus

        ptr_to_spi = esp.C_Pointer()
	ret = esp.spi_bus_add_device(self.spihost, devcfg, ptr_to_spi)
        if ret != 0: raise RuntimeError("Failed adding SPI device")
        self.spi = ptr_to_spi.ptr_val
        
        # Prepare transactions. Each response is 16bit long

        self.trans = [esp.spi_transaction_t({
            'rx_buffer': bytearray(2),
            'length': 0 if self.half_duplex else 16,
            'rxlength': 16
            }) for i in range(0, self.max_cmds)]

    trans_result_ptr = esp.C_Pointer()

    #
    # Deinitalize SPI device and bus
    #

    def deinit(self):

        print('Deinitializing XPT2046...')

        if self.spi:

            # Pop all pending transaction results
            ret = 0
            while ret == 0:
                ret = esp.spi_device_get_trans_result(self.spi, self.trans_result_ptr , 1)

            # Remove device
            esp.spi_bus_remove_device(self.spi)

            # Free SPI bus
            esp.spi_bus_free(self.spihost)


    # @micropython.viper
    def xpt_cmds(self, cmds):
        cmd_count = int(len(cmds))
        for i in range(0, cmd_count):
            self.trans[i].cmd = cmds[i]
            esp.spi_device_queue_trans(self.spi, self.trans[i], -1)
        result = []
        for i in range(0, cmd_count):
            esp.spi_device_get_trans_result(self.spi, self.trans_result_ptr , -1)
            buf = self.trans[i].rx_buffer.__dereference__(2)
            value = (int(buf[0]) << 4) + (int(buf[1]) >> 4) # value is in the 12 higher bits, network order
            if value == int(self.MAX_RAW_COORD):
                value = 0
            result.append(value)
        return tuple(result)
    
    # @micropython.viper
    def get_med_coords(self, count : int):
        mid = count//2
        values = []
        for i in range(0, count):
            values.append(self.xpt_cmds([self.CMD_X_READ, self.CMD_Y_READ]))
        # values = self.xpt_cmds([self.CMD_X_READ]*count + [self.CMD_Y_READ]*count)
        # x_values = sorted(values[:count])
        # y_values = sorted(values[count:])
        x_values = sorted([x for x,y in values])
        y_values = sorted([y for x,y in values])
        if int(x_values[0]) == 0 or int(y_values[0]) == 0 : return None
        return x_values[mid], y_values[mid]

    # @micropython.viper
    def get_coords(self):
        med_coords = self.get_med_coords(int(self.samples))
        if not med_coords: return None
        if self.transpose:
            raw_y, raw_x = med_coords
        else:
            raw_x, raw_y = med_coords

        if int(raw_x) != 0 and int(raw_y) != 0:
            x = ((int(raw_x) - int(self.cal_x0)) * int(self.screen_width)) // (int(self.cal_x1) - int(self.cal_x0))
            y = ((int(raw_y) - int(self.cal_y0)) * int(self.screen_height)) // (int(self.cal_y1) - int(self.cal_y0))
            # print('(%d, %d) ==> (%d, %d)' % (raw_x, raw_y, x, y))
            return x,y
        else: return None

    # @micropython.native
    def get_pressure(self, factor : int) -> int:
        z1, z2, x = self.xpt_cmds([self.CMD_Z1_READ, self.CMD_Z2_READ, self.CMD_X_READ])
        if int(z1) == 0: return -1
        return ( (int(x)*factor) / 4096)*( int(z2)/int(z1) - 1)

    start_time_ptr = esp.C_Pointer()
    end_time_ptr = esp.C_Pointer()
    cycles_in_ms = esp.esp_clk_cpu_freq() // 1000

    # @micropython.native
    def read(self, indev_drv, data) -> int:

        esp.get_ccount(self.start_time_ptr)
        coords = self.get_coords()
        esp.get_ccount(self.end_time_ptr)

        if self.end_time_ptr.int_val > self.start_time_ptr.int_val:
            self.touch_cycles +=  self.end_time_ptr.int_val - self.start_time_ptr.int_val
            self.touch_count += 1

        if coords:
            data.point.x ,data.point.y = coords
            data.state = lv.INDEV_STATE.PRESSED
            return False
        data.state = lv.INDEV_STATE.RELEASED
        return False

    def stat(self):
        return self.touch_cycles / (self.touch_count * self.cycles_in_ms) 


