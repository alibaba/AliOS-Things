import kv
import time
import _thread
import uos
def _on_get_url(url):
    kv.set('_amp_pyapp_url',url)
    execfile('/lib/appOta.py')

def _connect_wifi():
    time.sleep(5)
    ssid = kv.get('_amp_wifi_ssid')
    passwd = kv.get('_amp_wifi_passwd')
    if isinstance(ssid,str) and isinstance(passwd,str):
        import network
        sta_if = network.WLAN(network.STA_IF)
        if not sta_if.isconnected():
            sta_if.active(True)
            sta_if.connect(ssid,passwd)

bt_disabled = kv.get('disable_bt')
if bt_disabled != "no":
    uos.plussys_mm()

channel = kv.get('app_upgrade_channel')
if channel == "disable":
    pass
else:
    import online_upgrade
    online_upgrade.on(_on_get_url)
    try:
        _thread.start_new_thread(_connect_wifi, ())
        #_thread.stack_size(10 * 1024)
    except Exception as e:
        print(e)








