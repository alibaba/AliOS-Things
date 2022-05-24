from driver import UART
import utime

class URM37(object):

    def __init__(self, uart_obj):
        self.uart_obj = None
        if not isinstance(uart_obj, UART):
            raise ValueError("parameter is not a UART object")
        self.uart_obj = uart_obj

    def getRange(self):
        tx_buf = bytearray([0x22, 0x00, 0x00, 0x22])
        rx_buf = bytearray(4)
        while True:
            if self.uart_obj.read(rx_buf) <= 0:
                break
        self.uart_obj.write(tx_buf)
        utime.sleep_ms(300)
        if self.uart_obj.read(rx_buf) != 4:
            return 65535
        return rx_buf[1] * 256 + rx_buf[2]

    def getTemperature(self):
        tx_buf = bytearray([0x11, 0x00, 0x00, 0x11])
        rx_buf = bytearray(4)
        while True:
            if self.uart_obj.read(rx_buf) <= 0:
                break
        self.uart_obj.write(tx_buf)
        utime.sleep_ms(300)
        if self.uart_obj.read(rx_buf) != 4:
            return -4095.0
        ret = rx_buf[1] * 256 + rx_buf[2]
        if (ret & 4096) == 0:
            ret = (ret & 4095) * 0.1
        else:
            ret = (ret & 4095) * (-0.1)
        return ret
