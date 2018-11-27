src =Split(''' 
''')
component =aos_component('ywss4linkkit', src)

dependencis =Split(''' 
    framework/ywss4linkkit/libywss
    utility/digest_algorithm
    framework/protocol/alink-ilop
    framework/connectivity/link-coap
    framework/connectivity/mqtt
''')
for i in dependencis:
    component.add_comp_deps(i)


global_macros =Split(''' 
    CONFIG_YWSS
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    ./libywss/os/
    ./libywss/os/platform/
''')
for i in includes:
    component.add_includes(i)
 
if aos_global_config.get("CONFIG_SYSINFO_DEVICE_NAME") == "ESP8266":
    component.add_global_macros("ESP8266_CONFIG")    


