import kv
import netmgr as nm
import _thread
import time

def _connect_wifi():
    time.sleep_ms(1000 * 2)
    ssid = kv.get('_amp_wifi_ssid')
    passwd = kv.get('_amp_wifi_passwd')
    if isinstance(ssid, str) and isinstance(passwd, str):
        nm.init()
        if nm.getStatus() == 5:
            pass
        else:
            nm.connect(ssid, passwd)


def _on_get_url(url):
    kv.set('_amp_pyapp_url', url)
    execfile('/lib/appOta.py')


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



