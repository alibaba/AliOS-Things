from linkkit import linkkit
import ujson
import netmgr as nm
import utime
callback_num = 0
connected = 0
PRODUCT_KEY = "xxxx"
PRODUCT_SECRET = "xxxx"
DEVICE_NAME  =    "xxxx"
DEVICE_SECRET  =   "xxxx"
def connect_wifi():
    nm.init()
    nm.connect("KIDS","12345678")

def on_device_dynamic_register(rc, value, userdata):
    if rc == 0:
        print("dynamic register device success, rc:%d, value:%s" % (rc, value))
    else:
        print("dynamic register device fail,rc:%d, value:%s" % (rc, value))

def on_connect():
    global  connected
    print('linkkit is connected ')
    connected =  1

def on_disconnect():
    print('linkkit is disconnect ')

def on_prop_set():
    print('linkkit is  on prop set  ')

def on_thing_prop_post(id,code):
    global callback_num
    callback_num += 1
    print(' on report reply: id is %d , code is %d , callback_num is %d ' %(id,code,callback_num))

def on_trigger_event_reply(id,code):
    global callback_num
    callback_num += 1
    print("on_trigger_event_reply: id is %d ,code is %d, callback num is %d " %(id,code,callback_num))


def on_service_request(request,response):
    global callback_num
    callback_num += 1
    print("on_service_request: request is %s ,response is %s, callback num is %d " %(request,response,callback_num))

def on_prop_set(request):
    global callback_num
    callback_num += 1
    print("on_prop_set: request is %s  callback num is %d " %(request,callback_num))

def property_test():
    num = 100
    while(num > 0):
        property_test = {
                "test_prop": num
        }
        payload = ujson.dumps(property_test)
        request_id = lk.thing_post_property(payload)
        num = num -20

def do_post_event_test():
    num = 100
    while(num > 0):
        event_test = {
            "test_event": num
        }
        payload = ujson.dumps(event_test)
        print(payload)
        lk.thing_trigger_event(("EventTest",payload))
        print("***** num is %d " %(num))
        num = num - 20

connect_wifi()
lk = linkkit.LinkKit(host_name="cn-shanghai",
    product_key=PRODUCT_KEY,
    device_name=DEVICE_NAME,
    device_secret=DEVICE_SECRET,
    product_secret=PRODUCT_SECRET)
lk.on_device_dynamic_register = on_device_dynamic_register
lk.on_connect = on_connect
lk.on_disconnect = on_disconnect
lk.on_thing_prop_post = on_thing_prop_post
lk.on_thing_event_post = on_trigger_event_reply
lk.on_prop_set  = on_prop_set
lk.on_service_request = on_service_request

lk.connect_async()
if(connected == 0):
    print(" wait for linkkit conneted callback ")
    utime.sleep_ms(100)

do_post_event_test()
#do_post_prop_test()
while(True):
    lk.do_yield(200)

