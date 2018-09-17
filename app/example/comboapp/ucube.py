src =Split('''
    linkkit_sample_single.c
    comboapp.c
''')
component =aos_component('comboapp', src)

dependencis =Split('''
    feature.linkkit
    framework/netmgr
    framework/common
    framework/ywss4linkkit
    utility/cjson
    tools/cli
    bluetooth/breeze/ais_ilop/ali_lib
    bluetooth/breeze/ais_ilop/ble_app_ali
    bluetooth/profile
    bluetooth/breeze/ais_ilop_impl/zephyr
''')
for i in dependencis:
    component.add_comp_deps(i)

global_macros =Split('''
    MQTT_DIRECT
    ALIOT_DEBUG
    IOTX_DEBUG
    USE_LPTHREAD
    CONFIG_DM_DEVTYPE_SINGLE
    TEST_ALCS
    CONFIG_AOS_CLI
    CONFIG_COMBO_AWSS
    AWSS_NEED_REBOOT
    BLE_APP_RECONFIG_AISILOP
''')
for i in global_macros:
    component.add_global_macros(i)

if aos_global_config.get('LWIP') == 1:
    component.add_comp_deps("kernel/protocols/net")
    aos_global_config.set('no_with_lwip', 0)

if aos_global_config.get('print_heap') == 1:
    component.add_global_macros('CONFIG_PRINT_HEAP')

if aos_global_config.mcu_family == 'esp8266':
    component.add_global_macros('FOTA_RAM_LIMIT_MODE')
    component.add_global_macros('ESP8266_CHIPSET')
else :
    component.add_global_macros('CONFIG_AOS_CLI')
    component.add_comp_deps("cli")

aos_global_config.set('ble', 1)
