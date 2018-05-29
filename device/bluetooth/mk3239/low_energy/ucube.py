src =Split(''' 
    ../BTE_platform/mico_bt_bus.c
    ../BTE_platform/mico_bt_hcd.c
    ../BTE_platform/mico_bt_logmsg.c
    ../BTE_platform/mico_bt_nvram_access.c
    ../BTE_platform/mico_upio.c
''')
component =aos_component('lib_ble_low_energy', src)

dependencis =Split(''' 
    ./device/bluetooth/mk3239/firmware
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split(''' 
    .
    ../include
''')
for i in global_includes:
    component.add_global_includes(i)

aos_global_config.set('BLUETOOTH_LIB_TYPE','low_energy')
BLUETOOTH_LIB_TYPE = aos_global_config.get('BLUETOOTH_LIB_TYPE')
HOST_ARCH = aos_global_config.get('HOST_ARCH')
TOOLCHAIN_NAME = aos_global_config.get('TOOLCHAIN_NAME')
component.add_prebuilt_libs('BTE_'+BLUETOOTH_LIB_TYPE+'.'+HOST_ARCH+'.'+TOOLCHAIN_NAME+'.release.a')

aos_global_config.set('VALID_PLATFORMS',['MK3238','MK3239'])

