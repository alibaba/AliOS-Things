src =Split('''
    main.c
''')
component =aos_component('meshapp', src)
aos_global_config.set('no_with_lwip', 0)

dependencis =Split('''
    network/umesh
    kernel/cli
    network/netmgr
''')
for i in dependencis:
    component.add_comp_deps(i)


global_macros =Split('''
    TAPIF_DEFAULT_OFF
    DEBUG
''')
for i in global_macros:
    component.add_global_macros(i)

if aos_global_config.get('MESHAUTH') == 1:
    dependencis=Split('''
        utility/base64
        security/alicrypto
    ''')
    component.add_comp_deps(*dependencis)

if aos_global_config.get("ipv6") == 0:
    component.add_global_macros("LWIP_IPV6=0")

@post_config
def meshapp_post_config(component):

    comp_names = [comp.name for comp in aos_global_config.components]

    if "EMW3060" == aos_global_config.get("MODULE"):
        component.add_global_macros("MESHAPP_LIGHT_ENABLED")

meshapp_post_config(component)

