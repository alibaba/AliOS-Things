import bluetooth
import struct
import json
import gc
import time
import network
from micropython import const

_wlan = network.WLAN(network.STA_IF)

_ble = bluetooth.BLE()
_bleNetConfigStatus = None
_ble_adv_name = 'esp-node'
_ble_tx = None
_ble_rx = None
_ble_msg = ''

BLE_CONNECTED = const(0x00)
BLE_DISCONNECTED = const(0x01)
BLE_COMMINICATING = const(0x02)

WIFI_IDLE = 1000
WIFI_CONNECTING = 1001
WIFI_GOT_IP = network.STAT_GOT_IP

NUS_UUID = 0xFFA0
RX_UUID = 0xFFA2
TX_UUID = 0xFFA3

BLE_NUS = bluetooth.UUID(NUS_UUID)
BLE_RX = (bluetooth.UUID(RX_UUID), bluetooth.FLAG_WRITE)
BLE_TX = (bluetooth.UUID(TX_UUID),
          bluetooth.FLAG_NOTIFY | bluetooth.FLAG_READ)

BLE_UART = (BLE_NUS, (BLE_TX, BLE_RX,))
SERVICES = [BLE_UART, ]

# Generate a payload to be passed to gap_advertise(adv_data=...).

# Helpers for generating BLE advertising payloads.


# Advertising payloads are repeated packets of the following form:
#   1 byte data length (N + 1)
#   1 byte type (see constants below)
#   N bytes type-specific data

_ADV_TYPE_FLAGS = const(0x01)
_ADV_TYPE_NAME = const(0x09)
_ADV_TYPE_UUID16_COMPLETE = const(0x3)
_ADV_TYPE_UUID32_COMPLETE = const(0x5)
_ADV_TYPE_UUID128_COMPLETE = const(0x7)
_ADV_TYPE_UUID16_MORE = const(0x2)
_ADV_TYPE_UUID32_MORE = const(0x4)
_ADV_TYPE_UUID128_MORE = const(0x6)
_ADV_TYPE_APPEARANCE = const(0x19)


def advertising_payload(limited_disc=False, br_edr=False, name=None, services=None, appearance=0):
    payload = bytearray()

    def _append(adv_type, value):
        nonlocal payload
        payload += struct.pack("BB", len(value) + 1, adv_type) + value

    _append(
        _ADV_TYPE_FLAGS,
        struct.pack("B", (0x01 if limited_disc else 0x02) +
                    (0x18 if br_edr else 0x04)),
    )

    if name:
        _append(_ADV_TYPE_NAME, name)

    if services:
        for uuid in services:
            b = bytes(uuid)
            if len(b) == 2:
                _append(_ADV_TYPE_UUID16_COMPLETE, b)
            elif len(b) == 4:
                _append(_ADV_TYPE_UUID32_COMPLETE, b)
            elif len(b) == 16:
                _append(_ADV_TYPE_UUID128_COMPLETE, b)

    # See org.bluetooth.characteristic.gap.appearance.xml
    if appearance:
        _append(_ADV_TYPE_APPEARANCE, struct.pack("<h", appearance))

    return payload

# region not used, but might be useful


def decode_field(payload, adv_type):
    i = 0
    result = []
    while i + 1 < len(payload):
        if payload[i + 1] == adv_type:
            result.append(payload[i + 2: i + payload[i] + 1])
        i += 1 + payload[i]
    return result


def decode_name(payload):
    n = decode_field(payload, _ADV_TYPE_NAME)
    return str(n[0], "utf-8") if n else ""


def decode_services(payload):
    services = []
    for u in decode_field(payload, _ADV_TYPE_UUID16_COMPLETE):
        services.append(bluetooth.UUID(struct.unpack("<h", u)[0]))
    for u in decode_field(payload, _ADV_TYPE_UUID32_COMPLETE):
        services.append(bluetooth.UUID(struct.unpack("<d", u)[0]))
    for u in decode_field(payload, _ADV_TYPE_UUID128_COMPLETE):
        services.append(bluetooth.UUID(u))
    return services

# endregion


def send(data):
    _ble.gatts_notify(0, _ble_tx, data + '\n')


def advertiser(name):
    payload = advertising_payload(
        name=name, services=[bluetooth.UUID(NUS_UUID)])
    _ble.gap_advertise(interval_us=100, adv_data=payload)


def ble_irq(event, data):
    global _ble_msg, _bleNetConfigStatus

    if event == 1:
        _bleNetConfigStatus = BLE_CONNECTED

    elif event == 2:
        _bleNetConfigStatus = BLE_DISCONNECTED
        advertiser(_ble_adv_name)

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
                            ret = {'cmd': 'WiFiCon', 'ret': {
                                'state': 'STAT_WRONG_PASSWORD'}}
                            send(json.dumps(ret).encode('hex'))
                            _bleNetConfigStatus = BLE_CONNECTED
                            break
                        if(status == network.STAT_NO_AP_FOUND):
                            _bleNetConfigStatus = BLE_COMMINICATING
                            ret = {'cmd': 'WiFiCon', 'ret': {
                                'state': 'STAT_NO_AP_FOUND'}}
                            send(json.dumps(ret).encode('hex'))
                            _bleNetConfigStatus = BLE_CONNECTED
                            break
                        if(status == network.STAT_GOT_IP):
                            _bleNetConfigStatus = BLE_COMMINICATING
                            ret = {'cmd': 'WiFiCon', 'ret': {
                                'state': 'STAT_GOT_IP', 'ifconfig': _wlan.ifconfig()}}
                            send(json.dumps(ret).encode('hex'))
                            _bleNetConfigStatus = BLE_CONNECTED
                            break
                        if(status == 1001):
                            pass
                        if(timeout < 0):
                            _bleNetConfigStatus = BLE_COMMINICATING
                            ret = {'cmd': 'WiFiCon', 'ret': {
                                'state': 'STAT_CONNECT_TIMEOUT'}}
                            send(json.dumps(ret).encode('hex'))
                            _bleNetConfigStatus = BLE_CONNECTED
                            break
                        time.sleep(1)
                        timeout -= 1
                _ble_msg = ''
                gc.collect()

# set name as start parm rather than another func like 'setName' to prevent user change name when ble processing


def start(name):
    global _ble, _ble_tx, _ble_rx, _bleNetConfigStatus, _ble_adv_name
    _ble_adv_name = name
    _ble.active(True)
    ((_ble_tx, _ble_rx,), ) = _ble.gatts_register_services(SERVICES)
    _ble.irq(ble_irq)
    advertiser(_ble_adv_name)
    _bleNetConfigStatus = BLE_DISCONNECTED


def stop():
    global _ble, _bleNetConfigStatus
    _ble.irq(None)
    _ble.active(False)
    _bleNetConfigStatus = BLE_DISCONNECTED


def getWLAN():
    return _wlan


def getBleStatus():
    return _bleNetConfigStatus


def getWiFiStatus():
    return _wlan.status()


def getWiFiConfig():
    return _wlan.ifconfig()
