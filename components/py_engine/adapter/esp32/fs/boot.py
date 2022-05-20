import kv
import time
import _thread

def _on_get_url(url):
    kv.set('_amp_pyapp_url',url)
    execfile('/lib/appOta.py')

def _connect_wifi():
    ssid = kv.get('_amp_wifi_ssid')
    passwd = kv.get('_amp_wifi_passwd')
    if isinstance(ssid,str) and isinstance(passwd,str):
        import network
        sta_if = network.WLAN(network.STA_IF)
        if not sta_if.isconnected():
            sta_if.active(True)
            sta_if.scan()
            sta_if.connect(ssid,passwd)

channel = kv.get('app_upgrade_channel')
if channel == "disable":
    pass
elif channel == "bt":
    execfile('/lib/oneMinuteOnCloud.py')
else:
    import online_upgrade
    online_upgrade.on(_on_get_url)
    try:
        _thread.start_new_thread(_connect_wifi, ())
        #_thread.stack_size(10 * 1024)
    except Exception as e:
        print(e)








