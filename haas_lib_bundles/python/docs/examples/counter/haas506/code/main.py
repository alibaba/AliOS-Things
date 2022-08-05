from ssd1306 import SSD1306_128_64
import utime as time
from driver import TIMER
from driver import GPIO
import aliyun

disp=SSD1306_128_64()
time_d = [0x32, 0x35, 0x3a, 0x30, 0x30] # 倒计时 25:00
print('time_d',time_d)

#model= 0 自动连续计时; model = 1 手动重启计时
model = 0
tomato_count = 0
count_data = {}
#实例化key1
key1=GPIO(10,10)
key1.open("KEY1")
#按键中断回调函数
def key1_callback(args):
    global time_d
    time_d = [0x32, 0x35, 0x3a, 0x30, 0x30]
    ret = timer.start()
    disp.oled_showstr(0,6,'                      ',1)
    print(ret)
    key1.disableIrq()
    key1.clearIrq()

def enable_key():
    #开启中断
    key1.enableIrq(key1_callback)

# 初始化
disp.begin()
#清屏    
disp.clear()
# 番茄时钟 OLED显示，每一秒刷一次
def down_time(args):
    global downtime_end,time_d,tomato_count,model
    if time_d[4] > 0x30:
        time_d[4] = time_d[4] - 1
    else:
        time_d[4] = 0x39
        if time_d[3] > 0x30:
            time_d[3] = time_d[3] - 1
        else:
            time_d[3] = 0x35
            if time_d[1] > 0x30:
                time_d[1] = time_d[1] - 1
            else:
                time_d[1] = 0x39
                if time_d[0] > 0x30:
                    time_d[0] = time_d[0] - 1
                else:
                    downtime_end = 1
                    time_d[0] = 0x30
                    time_d[1] = 0x30
                    time_d[3] = 0x30
                    time_d[4] = 0x30                     
                    tomato_count += 1
                    #界面显示
                    disp.oled_showstr(0,0,"count:",2)
                    disp.oled_showstr(55,0,str(tomato_count),2)
                    disp.oled_showstr(85,0,"md:",2)
                    disp.oled_showstr(110,0,str(model),2)

                    count_data['count'] =tomato_count
                    aliyun.up_data(count_data)
                    print(model)
                    print(type(model))
                    if model == 1:
                        disp.oled_showstr(0,6,'push key to begin',1)
                        timer.stop()
                    elif model ==0 :
                        time_d =  [0x32, 0x35, 0x3a, 0x30, 0x30]  #重置时间
    disp.oled_showmun(70,3,time_d,2)
    disp.oled_showstr(0,3,"remain:",2)

timer = TIMER(0)
timer.open(period=1000, mode=TIMER.PERIODIC, callback=down_time)


if __name__ == '__main__':
    global downtime_end
    models = model
    timer.stop()
    aliyun.connect()       #连接阿里云
    #打开按键使能
    enable_key()
    while aliyun.get_connect_state() == False:
        time.sleep(0.1)
    print('push key to begin')
    #界面显示
    disp.oled_showstr(0,6,'push key to begin',1)
    disp.oled_showstr(0,0,"count:",2)
    disp.oled_showstr(55,0,str(tomato_count),2)
    disp.oled_showstr(85,0,"md:",2)
    disp.oled_showstr(110,0,str(model),2)

    aliyun.up_data({'count':0})
    aliyun.up_data({'model':0})
    while True:      
        time.sleep_ms(500)
        model = aliyun.get_model()
        if models != model:
            disp.oled_showstr(110,0,str(model),2)
            models = model
