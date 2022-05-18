import kv
channel = kv.get('app_upgrage_channel')
if channel == "disable":
    pass
elif channel == "mqtt":
    execfile('/lib/appOta.py')
else:
    execfile('/lib/oneMinuteOnCloud.py')








