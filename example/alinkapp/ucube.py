src = Split('''
        alink_sample.c
''')

component = aos_component('alinkapp', src)
component.add_component_dependencis('utility/log', 'framework/protocol/alink', 'tools/cli',
                                    'framework/fota', 'framework/netmgr', 'framework/common')

if aos_global_config.get_aos_global_config('ywss') != 0:
    component.add_component_dependencis('framework/ywss')

if aos_global_config.get_aos_global_config('at_adapter') == 1:
    component.add_component_dependencis('tools/at_adapter/at_adapter')
    aos_global_config.set_aos_global_config('LWIP', 1)

if aos_global_config.get_aos_global_config('sal') == 1:
    aos_global_config.set_aos_global_config('gateway', 0)

if aos_global_config.mcu_family.startswith('linux'):
    if not aos_global_config.get_aos_global_config('gateway'):
        aos_global_config.set_aos_global_config('gateway', 0)
elif aos_global_config.mcu_family == 'stm32f4xx' \
        or aos_global_config.mcu_family == 'xr871':
    if not aos_global_config.get_aos_global_config('gateway'):
        aos_global_config.set_aos_global_config('gateway', 0)
else:
    if not aos_global_config.get_aos_global_config('gateway'):
        aos_global_config.set_aos_global_config('gateway', 1)


all_components = [component.name for component in aos_global_config.components]

if aos_global_config.get_aos_global_config('gateway') == 1:
    component.add_component_dependencis('framework/gateway')
    if 'linuxhost' in all_components:
        if aos_global_config.get_aos_global_config('DDA') == None:
            aos_global_config.set_aos_global_config('DDA', 1)

if 'armhflinux' in all_components:
    if aos_global_config.get_aos_global_config('DDA') == None:
        aos_global_config.set_aos_global_config('DDA', 1)

if aos_global_config.get_aos_global_config('LWIP') == 1:
    component.add_component_dependencis('protocols/net')
    aos_global_config.set_aos_global_config('no_with_lwip', 1)

MESHDEBUG = 0
if aos_global_config.get_aos_global_config('meshdebug') == 1:
    MESHDEBUG =1
aos_global_config.set_aos_global_config('MESHDEBUG', MESHDEBUG)

MESHSUPER = 0
if aos_global_config.get_aos_global_config('meshsuper') == 1:
    MESHSUPER =1
aos_global_config.set_aos_global_config('MESHSUPER', MESHSUPER)

if aos_global_config.get_aos_global_config('DDA') == 1:
    component.add_component_dependencis('tools/dda')

if aos_global_config.get_aos_global_config('sds') == 1:
    component.add_macro('CONFIG_SDS')