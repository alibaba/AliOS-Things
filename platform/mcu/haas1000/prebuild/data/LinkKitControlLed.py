from linkkit import linkkit
from haascv import driver
import ujson
import netmgr as nm
import utime

connected = 0
PRODUCT_KEY = "xxxx"
PRODUCT_SECRET = "xxxx"
DEVICE_NAME  =    "xxxx"
DEVICE_SECRET  =   "xxxx"

def connect_wifi():
    nm.init()
    nm.connect("KIDS","12345678")



def on_connect():
    global  connected
    print('linkkit is connected ')
    connected =  1

def control_led(status):
    pass

def on_prop_set(request):
    global callback_num
    callback_num += 1
    print("on_prop_set: request is %s  callback num is %d " %(request,callback_num))



connect_wifi()
lk = linkkit.LinkKit(host_name="cn-shanghai",
    product_key=PRODUCT_KEY,
    device_name=DEVICE_NAME,
    device_secret=DEVICE_SECRET,
    product_secret=PRODUCT_SECRET)

lk.on_connect = on_connect
lk.on_prop_set  = on_prop_set

lk.connect_async()
if(connected == 0):
    print(" wait for linkkit conneted callback ")
    utime.sleep_ms(100)

while(True):
    lk.do_yield(200)

