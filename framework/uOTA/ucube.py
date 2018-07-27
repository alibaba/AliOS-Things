src = Split('''
    src/service/ota_service.c
    src/service/ota_version.c
    src/transport/ota_transport_mqtt.c
    src/transport/ota_transport_coap.c
    src/download/ota_manifest.c
    src/download/ota_download_http.c
    src/download/ota_download_coap.c
    src/download/ota_socket.c
    src/verify/ota_util.c
    src/verify/ota_verify.c
''')
component = aos_component('uOTA', src)

dependencis = Split('''
    utility/digest_algorithm 
''')
for i in dependencis:
    component.add_comp_deps(i)
    
global_macros =Split(''' 
    OTA_BREAKPOINT_SUPPORT
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    hal
    inc
    src/service
    src/transport
    src/download
    src/verify
''')
for i in includes:
    component.add_includes(i)

