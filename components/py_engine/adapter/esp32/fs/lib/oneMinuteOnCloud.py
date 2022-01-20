print('enable OneMinuteOnCloud')
import ubluetooth
import uos as os
import uerrno as errno
import ujson as json
import uzlib
import upip_utarfile as tarfile
import gc
import time
import machine
import ussl
import usocket
import network

_wlan = network.WLAN(network.STA_IF)
_ble = ubluetooth.BLE()

_ble_adv_name = 'esp-node'

pull_code_state = []
file_buf = bytearray(128)

def download_file_task(filelist):
    global pull_code_state
    for file_task in filelist:
        if('needunpack' in file_task.keys() and file_task['needunpack'] == True):
            gc.collect()
            print(gc.mem_free())
            pull_code_state.append(install_pkg(file_task['url'], file_task['path']))
            gc.collect()
        else:
            gc.collect()
            print(gc.mem_free())
            pull_code_state.append(download_save_file(file_task['url'], file_task['path']+file_task['name']))
            gc.collect()

def download_save_file(file_url, fname):
    global file_buf
    f1 = url_open(file_url)
    if(isinstance(f1, (str, bytes, bytearray)) == True):
        print(f1)
        return f1

    print(fname)
    _makedirs(fname)
    with open(fname, "wb") as outf:
        while True:
            sz = f1.readinto(file_buf)
            if not sz:
                break
            outf.write(file_buf, sz)
        outf.close()
    f1.close()
    del f1
    print('download_save_file success')
    return 'SUCCESS'

def install_pkg(package_url, install_path):
    gzdict_sz = 16 + 15
    f1 = url_open(package_url)
    if(isinstance(f1, (str, bytes, bytearray)) == True):
        print(f1)
        return f1
    try:
        f2 = uzlib.DecompIO(f1, gzdict_sz)
        f3 = tarfile.TarFile(fileobj=f2)
        install_tar(f3, install_path)
    except Exception as e:
        print(e)
        return("UNTAR_FILE_FAIL")
    finally:
        f1.close()
    del f3
    del f2
    gc.collect()

    print('install_pkg success')
    return 'SUCCESS'

def url_open(url):
    proto, _, host, urlpath = url.split('/', 3)
    try:
        port = 443
        if ":" in host:
            host, port = host.split(":")
            port = int(port)
        ai = usocket.getaddrinfo(host, port, 0, usocket.SOCK_STREAM)
    except OSError as e:
        print("Error:", "Unable to resolve %s (no Internet?)" % host, e)
        return 'HOST_RESOLVED_FAIL'
    print("Address infos:", ai)
    ai = ai[0]
    s = usocket.socket(ai[0], ai[1], ai[2])
    try:
        s.connect(ai[-1])
        if proto == "https:":
            s = ussl.wrap_socket(s, server_hostname=host)
        s.write("GET /%s HTTP/1.0\r\nHost: %s:%s\r\n\r\n" % (urlpath, host, port))
        l = s.readline()
        protover, status, msg = l.split(None, 2)
        if status != b"200":
            if status == b"404" or status == b"301":
                return("Package not found")
            return(status)
        while True:
            l = s.readline()
            if not l:
                return("Unexpected EOF in HTTP headers")
            if l == b'\r\n':
                break
    except Exception as e:
        s.close()
        print(e)
        return('SOCKET_ERROR')
    return s

def save_file(fname, subf):
    global file_buf
    with open(fname, "wb") as outf:
        while True:
            sz = subf.readinto(file_buf)
            if not sz:
                break
            outf.write(file_buf, sz)
        outf.close()

def _makedirs(name, mode=0o777):
    ret = False
    s = ""
    comps = name.rstrip("/").split("/")[:-1]
    if comps[0] == "":
        s = "/"
    for c in comps:
        if s and s[-1] != "/":
            s += "/"
        s += c
        try:
            os.mkdir(s)
            ret = True
        except OSError as e:
            if e.args[0] != errno.EEXIST and e.args[0] != errno.EISDIR:
                print(e)
            ret = False
    return ret

def install_tar(f, prefix):
    for info in f:
        print(info)
        fname = info.name
        try:
            fname = fname[fname.index("/") + 1:]
        except ValueError:
            fname = ""

        outfname = prefix + fname
        if info.type != tarfile.DIRTYPE:
            print("Extracting " + outfname)
            _makedirs(outfname)
            subf = f.extractfile(info)
            save_file(outfname, subf)

def rmvdir(dir):
    for i in os.ilistdir(dir):
        if i[1] == 16384:
            rmvdir('{}/{}'.format(dir,i))
        elif i[1] == 32678:
            os.remove('{}/{}'.format(dir,i[0]))
    os.rmdir(dir)

def send(data):
    _ble.gatts_notify(0, _ble_tx, data + '\n')

def advertiser(name):
    _name = bytes(name, 'UTF-8')
    _ble.gap_advertise(100, bytearray('\x02\x01\x02') + bytearray((len(_name) + 1, 0x09)) + _name)

def ble_irq(event, data):
    global ble_msg

    if event == 1:
        print('Central connected')

        global pull_code_state
        if(pull_code_state!=[]):
            ret = {'cmd':'PullCode', 'ret':{'state':pull_code_state}}
            send(json.dumps(ret).encode('hex'))

    elif event == 2:
        print('Central disconnected')
        advertiser("esp-node")

    elif event == 3:
        buffer = _ble.gatts_read(_ble_rx)
        ble_msg += buffer.decode('hex').strip()

        ble_msg = '{"cmd":' + ble_msg.split('{"cmd":')[-1]
        # only save one cmd
        print(ble_msg)

        if(ble_msg.count('{') == ble_msg.count('}')):
            try:
                cmdd = json.loads(ble_msg)
                print(cmdd)

                if(cmdd['cmd'] == 'WiFiCon'):
                    _wlan.active(True)
                    if(_wlan.isconnected()):
                        _wlan.disconnect()
                    print(cmdd['param']['ssid'], cmdd['param']['pswd'])
                    _wlan.connect(cmdd['param']['ssid'], cmdd['param']['pswd'])
                    timeout = 5
                    if('timeout' in cmdd['param'].keys()):
                        timeout = int(cmdd['param']['timeout'])
                    while(True):
                        status = _wlan.status()
                        print(status)
                        if(status == network.STAT_WRONG_PASSWORD):
                            print('STAT_WRONG_PASSWORD')
                            ret = {'cmd':'WiFiCon', 'ret':{'state':'STAT_WRONG_PASSWORD'}}
                            send(json.dumps(ret).encode('hex'))
                            break
                        if(status == network.STAT_NO_AP_FOUND):
                            print('STAT_NO_AP_FOUND')
                            ret = {'cmd':'WiFiCon', 'ret':{'state':'STAT_NO_AP_FOUND'}}
                            send(json.dumps(ret).encode('hex'))
                            break
                        if(status == network.STAT_GOT_IP):
                            print('STAT_GOT_IP')
                            ret = {'cmd':'WiFiCon', 'ret':{'state':'STAT_GOT_IP', 'ifconfig':_wlan.ifconfig()}}
                            send(json.dumps(ret).encode('hex'))

                            wificonf = {"ssid":cmdd['param']['ssid'],"pswd":cmdd['param']['pswd'],"autoConnect":True}
                            with open('/WiFi.json', "w") as f:
                                f.write(json.dumps(wificonf) + "\n")
                            break
                        if(status == 1001):
                            print('scaning for ap ...')
                        if(timeout < 0):
                            print('STAT_CONNECT_TIMEOUT')
                            ret = {'cmd':'WiFiCon', 'ret':{'state':'STAT_CONNECT_TIMEOUT'}}
                            send(json.dumps(ret).encode('hex'))
                            break
                        time.sleep(1)
                        timeout -= 1

                if(cmdd['cmd'] == 'PullCode'):
                    global pull_code_state
                    if(_wlan.isconnected() is False):
                        print(_wlan.isconnected())
                        ret = {'cmd':'PullCode', 'ret':{'state':'NO_NETWORK'}}
                        send(json.dumps(ret).encode('hex'))
                    else:
                        # _thread.start_new_thread(download_file_task, (cmdd['param']['filelist'], ))
                        try:
                            f = open('/afterlife.json', "w")
                            f.write(json.dumps(cmdd) + "\n")
                            f.close()
                        except Exception as e:
                            print(e)
                            pass
                        else:
                            # see you afterlife
                            ret = {'cmd':'PullCode', 'ret':{'state':'START_DOWNLOAD'}}
                            send(json.dumps(ret).encode('hex'))

                if(cmdd['cmd'] == 'DeviceInfo'):
                    with open('/DeviceInfo.json', "w") as f:
                        f.write(cmdd['param'] + "\n")
                    ret = {'cmd':'DeviceInfo', 'ret':{'state':'DeviceInfoRecved'}}
                    send(json.dumps(ret).encode('hex'))

                if(cmdd['cmd'] == 'PullCodeCheck'):
                    ret = {'cmd':'PullCode', 'ret':{'state':pull_code_state}}
                    send(json.dumps(ret).encode('hex'))

                if(cmdd['cmd'] == 'Reset'):
                    machine.reset()

                ble_msg = ''
                gc.collect()

            except Exception as e:
                pass

if('WiFi.json' in os.listdir('/')):
    try:
        f = open('/WiFi.json', "r")
        wificonf = f.readline()
        wificonf = json.loads(wificonf)
        f.close()
        if('autoConnect' in wificonf.keys() and wificonf['autoConnect'] == True):
            print('autoConnect')
            _wlan.active(True)
            _wlan.connect(wificonf['ssid'], wificonf['pswd'],)
    except Exception as e:
        print('try WiFi autoConnect, found')
        print(e)
        pass

if('afterlife.json' in os.listdir('/')):
    try:
        f = open('/afterlife.json', "r")
        wish = f.readline()
        wish = json.loads(wish)
        f.close()
        print(wish)
        time.sleep(5)
        if(_wlan.isconnected() == False):
            pull_code_state = 'NO_NETWORK'
            print('NO_NETWORK')
            raise
        print('wifi connected')
        if('cmd' in wish.keys() and wish['cmd'] == 'PullCode'):
            download_file_task(wish['param']['filelist'])
    except Exception as e:
        raise (e)

_ble.active(True)
NUS_UUID = 0xFFA0
RX_UUID = 0xFFA2
TX_UUID = 0xFFA3

BLE_NUS = ubluetooth.UUID(NUS_UUID)
BLE_RX = (ubluetooth.UUID(RX_UUID), ubluetooth.FLAG_WRITE)
BLE_TX = (ubluetooth.UUID(TX_UUID), ubluetooth.FLAG_NOTIFY | ubluetooth.FLAG_READ)
BLE_UART = (BLE_NUS, (BLE_TX, BLE_RX,))
SERVICES = [BLE_UART, ]

_ble_tx = None
_ble_rx = None

ble_msg = ''

((_ble_tx, _ble_rx,), ) = _ble.gatts_register_services(SERVICES)

_ble.irq(ble_irq)
advertiser(_ble_adv_name)

if('afterlife.json' in os.listdir('/')):
    os.remove('/afterlife.json')
    time.sleep(10)
