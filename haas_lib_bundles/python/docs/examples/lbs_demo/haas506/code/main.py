import utime as time
import network
from lbs import LBS

g_connect_status = False


def on_4g_cb(args):
    global g_connect_status
    pdp = args[0]
    netwk_sta = args[1]
    if netwk_sta == 1:
        g_connect_status = True
    else:
        g_connect_status = False


def connect_network():
    global net,on_4g_cb,g_connect_status
    net = network.NetWorkClient()
    g_register_network = False
    if net._stagecode is not None and net._stagecode == 3 and net._subcode == 1:
        g_register_network = True
    else:
        g_register_network = False
    if g_register_network:
        net.on(1,on_4g_cb)
        net.connect(None)
    else:
        print('network register failed')
    while True:
        if g_connect_status:
            print('network register successed')
            break
        time.sleep_ms(20)

if __name__ == '__main__':
    connect_network()
    getlbs = LBS()
    lbstr = getlbs.get_lbs()
    print(lbstr)