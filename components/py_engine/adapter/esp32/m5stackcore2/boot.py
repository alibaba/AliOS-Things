import axp192
import kv

try:
    # for m5stack-core2 only
    axp = axp192.Axp192()
    axp.powerAll()
    axp.setLCDBrightness(80)  # 设置背光亮度 0～100
except OSError:
    print("make sure axp192.py is in libs folder")


def _on_get_url(url):
    kv.set('_amp_pyapp_url', url)
    execfile('/lib/appOta.py')


def _connect_wifi(ssid, passwd):
    import network
    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        sta_if.active(True)
        sta_if.scan()
        sta_if.connect(ssid, passwd)


channel = kv.get('app_upgrade_channel')
if channel == "disable":
    pass
else:
    ssid = kv.get('_amp_wifi_ssid')
    passwd = kv.get('_amp_wifi_passwd')
    if isinstance(ssid, str) and isinstance(passwd, str):
        _connect_wifi(ssid, passwd)
    import online_upgrade
    online_upgrade.on(_on_get_url)
