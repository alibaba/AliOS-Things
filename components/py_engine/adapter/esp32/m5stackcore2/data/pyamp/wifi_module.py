import utime
import sys

def wifi_scan():
    print('start wifi scan ...')
    import network
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)
    return(sta_if.scan())

def wifi_connect(wifiSSID, wifiPassword):
    print('start wifi connect ', wifiSSID)
    retry = 0
    if (sys.platform == "esp32"):
        import network
        sta_if = network.WLAN(network.STA_IF)
        sta_if.active(True)
        if not sta_if.isconnected():
            sta_if.connect(wifiSSID, wifiPassword)
            while not sta_if.isconnected():
                utime.sleep_ms(1000)
                retry = retry + 1
                if (retry > 10):
                    return False
        print('Wi-Fi connected', sta_if.ifconfig())
        return True
    else:
        import netmgr as nm
        nm.init()
        wifi_connected = nm.getStatus()
        if wifi_connected != 5:
            nm.connect(wifiSSID, wifiPassword)
        while True :
            if wifi_connected == 5:               # nm.getStatus()返回5代表连线成功
                break
            else:
                wifi_connected = nm.getStatus() # 获取Wi-Fi连接路由器的状态信息
                utime.sleep_ms(1000)
                retry = retry + 1
                if (retry > 10):
                    return False
        print('Wi-Fi connected', nm.getInfo()['ip'])
        return True

def is_wifi_connected():
    if (sys.platform == "esp32"):
        import network
        sta_if = network.WLAN(network.STA_IF)
        sta_if.active(True)
        if sta_if.isconnected():
            return True
        else:
            return False
    else:
        import netmgr as nm
        nm.init()
        wifi_connected = nm.getStatus()
        if wifi_connected == 5:
            return True
        else:
            return False
