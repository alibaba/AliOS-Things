src = Split('''
        alink_sample.c
''')

component = aos_component('alinkapp', src)
component.add_comp_deps('utility/log', 'framework/protocol/alink', 'tools/cli',
                                    'framework/fota', 'framework/netmgr', 'framework/common')

if aos_global_config.get('ywss') != 0:
    component.add_comp_deps('framework/ywss')

if aos_global_config.get('at_adapter') == 1:
    component.add_comp_deps('tools/at_adapter/at_adapter')
    aos_global_config.set('LWIP', 1)

if aos_global_config.get('sal') == 1:
    aos_global_config.set('gateway', 0)

if aos_global_config.mcu_family.startswith('linux'):
    if aos_global_config.get('gateway') == None:
        aos_global_config.set('gateway', 0)
elif aos_global_config.mcu_family == 'stm32f4xx' \
        or aos_global_config.mcu_family == 'xr871':
    if aos_global_config.get('gateway') == None:
        aos_global_config.set('gateway', 0)
else:
    if aos_global_config.get('gateway') == None:
        aos_global_config.set('gateway', 1)


all_components = [component.name for component in aos_global_config.components]

if aos_global_config.get('gateway') == 1:
    component.add_comp_deps('framework/gateway')
    if 'linuxhost' in all_components:
        if aos_global_config.get('DDA') == None:
            aos_global_config.set('DDA', 1)

if 'armhflinux' in all_components:
    if aos_global_config.get('DDA') == None:
        aos_global_config.set('DDA', 1)

if aos_global_config.get('LWIP') == 1:
    component.add_comp_deps('protocols/net')
    aos_global_config.set('no_with_lwip', 1)

MESHDEBUG = 0
if aos_global_config.get('meshdebug') == 1:
    MESHDEBUG =1
aos_global_config.set('MESHDEBUG', MESHDEBUG)

MESHSUPER = 0
if aos_global_config.get('meshsuper') == 1:
    MESHSUPER =1
aos_global_config.set('MESHSUPER', MESHSUPER)

if aos_global_config.get('DDA') == 1:
    component.add_comp_deps('tools/dda')

if aos_global_config.get('sds') == 1:
    component.add_macros('CONFIG_SDS')