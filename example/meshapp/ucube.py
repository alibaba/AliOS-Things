src =Split(''' 
    main.c
''')
component =aos_component('meshapp', src)
aos_global_config.set('no_with_lwip', 0) 

dependencis =Split(''' 
    kernel/protocols/mesh
    tools/cli
    framework/netmgr
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
        utility/digest_algorithm
        security/tfs
        security/libid2
        security/libkm
        security/alicrypto
    ''')
    component.add_comp_deps(*dependencis) 

if aos_global_config.get("LWIP") == None:
    aos_global_config.set("LWIP",1)
if aos_global_config.get("LWIP") == 1:
    component.add_comp_deps("kernel/protocols/net") 

if aos_global_config.get("ipv6") == 0:
    component.add_global_macros("LWIP_IPV6=0")

@post_config
def meshapp_post_config(component):
    
    comp_names = [comp.name for comp in aos_global_config.components]
    if 'linuxhost' in comp_names:
        if aos_global_config.get("DDA") == None:
            aos_global_config.set("DDA",1)
    
    if 'armhflinux' in comp_names:
        if aos_global_config.get("DDA") == None:
            aos_global_config.set("DDA",1)
            
    if aos_global_config.get("DDA") == 1:        
        component.add_comp_deps("tools/dda")
    
    if "EMW3060" == aos_global_config.get("MODULE"):
        component.add_global_macros("MESHAPP_LIGHT_ENABLED") 
    
    if aos_global_config.get("mesh_hal_test") == 1:
        component.add_global_macros("MESH_HAL_TEST_ENABLED") 

meshapp_post_config(component)

