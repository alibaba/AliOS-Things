src =Split(''' 
    src/CoAPDeserialize.c
    src/CoAPExport.c
    src/CoAPMessage.c
    src/CoAPNetwork.c
    src/CoAPObserve.c
    src/CoAPPlatform.c
    src/CoAPResource.c
    src/CoAPSerialize.c
    src/CoAPServer.c
''')
component =aos_component('link-coap', src)

dependencis =Split(''' 
    framework/protocol/alink-ilop
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split(''' 
    ./src
''')
for i in global_includes:
    component.add_global_includes(i)

    
macros =Split(''' 
    DEBUG
    USE_LPTHREAD
''')
for i in macros:
    component.add_macros(i)


if aos_global_config.get('LINK_COAP_ALONE') == 1:
    includes = Split(''' 
        ./platform
        ../../protocol/alink-ilop/iotkit-system
    ''')
    for i in includes:
        component.add_includes(i)
    
    sources = Split(''' 
        platform/HAL_UDP_rhino.c
        platform/HAL_OS_rhino.c 
    ''')
    for i in sources:
        component.add_sources(i)
else:
    component.add_comp_deps("framework/protocol/alink-ilop")


    