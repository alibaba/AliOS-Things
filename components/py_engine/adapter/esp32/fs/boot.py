import kv
kv_ret = kv.get('enableOneMinuteOnCloud')
if (kv_ret != 'disable'):
    execfile('/lib/oneMinuteOnCloud.py')
