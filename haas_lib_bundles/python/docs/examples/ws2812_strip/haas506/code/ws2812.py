import utime as time

class WS2812:
    """
    Driver for WS2812 RGB LEDs. May be used for controlling single LED or chain
    of LEDs.
    Example of use:
        chain = WS2812(spi_bus=1, led_count=4)
        data = [
            (255, 0, 0),    # red
            (0, 255, 0),    # green
            (0, 0, 255),    # blue
            (85, 85, 85),   # white
        ]
        chain.show(data)
    Version: 1.0
    """

    def __init__(self, spiObj, led_count=1, intensity=1):
        """
        Params:
        * spiObj
        * led_count: count of LEDs
        * intensity:light intensity (float up to 1)
        """
        self.led_count = led_count
        self.intensity = intensity
        self.buf_bytes = (0x88, 0x8e, 0xe8, 0xee)

        # prepare SPI data buffer (4 bytes for each color)
        self.buf_length = self.led_count * 3 * 4
        self.buf = bytearray(self.buf_length)
        # SPI init
        self.spiObj=spiObj
        # turn LEDs off
        # self.show([])


    def send_buf(self,data):
        """
        Send buffer over SPI.
        """
        self.spiObj.write(data)
    
  
    def update_buf(self, data, start=0):
        """
        Fill a part of the buffer with RGB data.
        Order of colors in buffer is changed from RGB to GRB because WS2812 LED
        has GRB order of colors. Each color is represented by 4 bytes in buffer
        (1 byte for each 2 bits).
        Returns the index of the first unfilled LED
        Note: If you find this function ugly, it's because speed optimisations
        beated purity of code.
        """
        mask = 0x03
        index = start * 12
        for red, green, blue in data:
            red = int(red * self.intensity)
            green = int(green * self.intensity)
            blue = int(blue * self.intensity)

            self.buf[index] = self.buf_bytes[green >> 6 & mask]
            self.buf[index+1] = self.buf_bytes[green >> 4 & mask]
            self.buf[index+2] = self.buf_bytes[green >> 2 & mask]
            self.buf[index+3] = self.buf_bytes[green & mask]

            self.buf[index+4] = self.buf_bytes[red >> 6 & mask]
            self.buf[index+5] = self.buf_bytes[red >> 4 & mask]
            self.buf[index+6] = self.buf_bytes[red >> 2 & mask]
            self.buf[index+7] = self.buf_bytes[red & mask]

            self.buf[index+8] = self.buf_bytes[blue >> 6 & mask]
            self.buf[index+9] = self.buf_bytes[blue >> 4 & mask]
            self.buf[index+10] = self.buf_bytes[blue >> 2 & mask]
            self.buf[index+11] = self.buf_bytes[blue & mask]

            index += 12

        return index // 12

    def fill_buf(self, data):
        """
        Fill buffer with RGB data.
        All LEDs after the data are turned off.
        """
        end = self.update_buf(data)

        # # turn off the rest of the LEDs
        off = self.buf_bytes[0]
        for index in range(end * 12, self.buf_length):
            self.buf[index] = off
            index += 1


    # 从头开始，到尾部点亮所有灯，时间可设置
    def animation_1(self):
        global wite_time
        num=len(self.buf)//12
        # print(self.buf)
        # print(len(self.buf))
        for i in range(num):
            data=self.buf[:12*(i+1)]
            self.send_buf(data)
            time.sleep_ms(wite_time)
    #从头到尾
    def animation_2(self):
        num=len(self.buf)//12
        data=self.buf[:]
        for i in range(30):
            # 右移12
            data=data[(num*12-12):]+data[0:(num*12-12)]            
            self.send_buf(data)
            time.sleep_ms(100)     

    # 从尾部开始，到头部点亮所有灯，
    def animation_3(self):
        num=len(self.buf)//12
        data1=self.buf[:]
        data2=self.buf[:]
        data3=[]
        # print(self.buf)
        # print(len(self.buf))
        for i in range(num):
            for j in range((num-i-1)*12):
                data1[j]=self.buf_bytes[0]
            data3=data1[:12*(num-i-1)]+data2[12*(num-1-i):]
            self.send_buf(data3)
            time.sleep_ms(1000) 

    # 从尾到头 循环，循环时间可调
    def animation_4(self):
        data=self.buf[:]
        for i in range(30):
            # 左移12
            data=data[12:]+data[0:12]            
            self.send_buf(data)
            time.sleep_ms(200)    


    # 关闭所有led灯
    def turn_off(self):
        self.fill_buf([])
        data=self.buf[:]
        # print(data)
        self.send_buf(data)
      


    def show(self, mode):
        global wite_time
        """
        Show RGB data on LEDs. Expected data = [(R, G, B), ...] where R, G and B
        are intensities of colors in range from 0 to 255. One RGB tuple for each
        LED. Count of tuples may be less than count of connected LEDs.
        """
        if mode==1 or mode == 3 or mode ==4:
            data=[(255,0,0),(0,255,0),(0,0,255),(255,0,0),(0,255,0),(0,0,255),(255,0,0),(0,255,0),(0,0,255),(255,0,0),(0,255,0),(0,0,255),
                (255,0,0),(0,255,0),(0,0,255),(255,0,0),(0,255,0),(0,0,255),(255,0,0),(0,255,0),(0,0,255),(255,0,0),(0,255,0),(0,0,255),
                (255,0,0),(0,255,0),(0,0,255),(255,0,0),(0,255,0),(0,0,255)]

            self.fill_buf(data)
            if mode == 1:
                wite_time = 100
                self.animation_1()
            if mode==3:
                self.animation_3()
            if mode==4:
                self.animation_4()  
        if mode==2:
            print("blue")#跑马灯
            data =[(0,0,255)]   
            self.fill_buf(data) 
            self.animation_2()
        if mode==5:
            data =[(0,0,255) for i in range(30)]#常亮
            self.fill_buf(data) 
            wite_time = 0
            self.animation_1()     





# from utime import time

# def rander(seed,n):
#     if n ==1:
#         return 0
#     seed = int(seed)
#     length = len(str(seed))
#     seed = int(seed**2/pow(10,(length/2))) % int(pow(10.0,length))
#     print(str(seed) +" " ,end='')
#     rander(seed,n-1)
# def main():
#     seed = time()
#     rander(seed,100)

# main()