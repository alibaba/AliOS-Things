import ubluetooth
import json
import gc
import time
import network
from micropython import const

_wlan = network.WLAN(network.STA_IF)

_ble = ubluetooth.BLE()
_bleNetConfigStatus = None
_ble_adv_name = 'esp-node'
_ble_tx = None
_ble_rx = None
_ble_msg = ''

BLE_CONNECTED = const(0x00)
BLE_DISCONNECTED = const(0x01)
BLE_COMMINICATING = const(0x02)

NUS_UUID = 0xFFA0
RX_UUID = 0xFFA2
TX_UUID = 0xFFA3

BLE_NUS = ubluetooth.UUID(NUS_UUID)
BLE_RX = (ubluetooth.UUID(RX_UUID), ubluetooth.FLAG_WRITE)
BLE_TX = (ubluetooth.UUID(TX_UUID), ubluetooth.FLAG_NOTIFY | ubluetooth.FLAG_READ)

BLE_UART = (BLE_NUS, (BLE_TX, BLE_RX,))
SERVICES = [BLE_UART, ]

def send(data):
    _ble.gatts_notify(0, _ble_tx, data + '\n')

def advertiser(name):
    _name = bytes(name, 'UTF-8')
    _ble.gap_advertise(100, bytearray('\x02\x01\x02') + bytearray((len(_name) + 1, 0x09)) + _name)

def ble_irq(event, data):
    global _ble_msg, _bleNetConfigStatus

    if event == 1:
        _bleNetConfigStatus = BLE_CONNECTED

    elif event == 2:
        _bleNetConfigStatus = BLE_DISCONNECTED
        advertiser("esp-node")

    elif event == 3:
        buffer = _ble.gatts_read(_ble_rx)
        _ble_msg += buffer.decode('hex').strip()
        _ble_msg = '{"cmd":' + _ble_msg.split('{"cmd":')[-1]
        if(_ble_msg.count('{') == _ble_msg.count('}')):
            try:
                cmdd = json.loads(_ble_msg)
            except Exception as e:
                pass
            else:
                if(cmdd['cmd'] == 'WiFiCon'):
                    _wlan.active(True)
                    if(_wlan.isconnected()):
                        _wlan.disconnect()

                    _wlan.connect(cmdd['param']['ssid'], cmdd['param']['pswd'])
                    timeout = 5
                    if('timeout' in cmdd['param'].keys()):
                        timeout = int(cmdd['param']['timeout'])
                    while(True):
                        status = _wlan.status()
                        if(status == network.STAT_WRONG_PASSWORD):
                            _bleNetConfigStatus = BLE_COMMINICATING
                            ret = {'cmd':'WiFiCon', 'ret':{'state':'STAT_WRONG_PASSWORD'}}
                            send(json.dumps(ret).encode('hex'))
                            _bleNetConfigStatus = BLE_CONNECTED
                            break
                        if(status == network.STAT_NO_AP_FOUND):
                            _bleNetConfigStatus = BLE_COMMINICATING
                            ret = {'cmd':'WiFiCon', 'ret':{'state':'STAT_NO_AP_FOUND'}}
                            send(json.dumps(ret).encode('hex'))
                            _bleNetConfigStatus = BLE_CONNECTED
                            break
                        if(status == network.STAT_GOT_IP):
                            _bleNetConfigStatus = BLE_COMMINICATING
                            ret = {'cmd':'WiFiCon', 'ret':{'state':'STAT_GOT_IP', 'ifconfig':_wlan.ifconfig()}}
                            send(json.dumps(ret).encode('hex'))
                            _bleNetConfigStatus = BLE_CONNECTED
                            break
                        if(status == 1001):
                            pass
                        if(timeout < 0):
                            _bleNetConfigStatus = BLE_COMMINICATING
                            ret = {'cmd':'WiFiCon', 'ret':{'state':'STAT_CONNECT_TIMEOUT'}}
                            send(json.dumps(ret).encode('hex'))
                            _bleNetConfigStatus = BLE_CONNECTED
                            break
                        time.sleep(1)
                        timeout -= 1
                _ble_msg = ''
                gc.collect()


def start():
    global _ble,_ble_tx, _ble_rx, _bleNetConfigStatus
    _ble.active(True)
    ((_ble_tx, _ble_rx,), ) = _ble.gatts_register_services(SERVICES)
    _ble.irq(ble_irq)
    advertiser(_ble_adv_name)
    _bleNetConfigStatus = BLE_DISCONNECTED

def stop():
    global _ble,_bleNetConfigStatus
    _ble.active(False)
    _bleNetConfigStatus = BLE_DISCONNECTED

def getWLAN():
    return _wlan

def status():
    return _bleNetConfigStatus
