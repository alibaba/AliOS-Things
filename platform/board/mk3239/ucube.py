src =Split(''' 
    board.c
    wifi_nvram.c
''')
component =aos_component('board_mk3239', src)


global_includes =Split(''' 
    ./
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    HSE_VALUE=26000000
    STDIO_UART=0
    CONFIG_NO_TCPIP
    RHINO_CONFIG_TICK_TASK=0
    RHINO_CONFIG_WORKQUEUE=0
    $(if
    $(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
''')
for i in global_macros:
    component.add_global_macros(i)




linux_only_targets="coapapp helloworld http2app httpapp jsengine_app linkkit_gateway linkkitapp lwm2mapp mqttapp otaapp  udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udata_demo.udata_cloud_demo udata_demo.udata_local_demo udataapp ulocation.baseapp yts"
