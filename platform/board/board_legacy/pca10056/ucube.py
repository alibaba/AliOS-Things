src =Split(''' 
''')
component =aos_component('board_pca10056', src)


global_includes =Split(''' 
    .
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    STDIO_UART=0
    CONFIG_NO_TCPIP
    BOARD_PCA10056
    CONFIG_GPIO_AS_PINRESET
    FLOAT_ABI_HARD
    NRF52840_XXAA
    CONFIG_SOC_SERIES_NRF52X
    CONFIG_CLOCK_CONTROL_NRF5_K32SRC_250PPM=y
    CONFIG_BOARD_NRF52840_PCA10056=1
    RHINO_CONFIG_WORKQUEUE=1
''')
for i in global_macros:
    component.add_global_macros(i)

linux_only_targets="blink bluetooth.bleadv bluetooth.blemesh_tmall bluetooth.bleperipheral bluetooth.breezeapp helloworld modbus_app udata_demo.sensor_local_demo udata_demo.udata_local_demo udataapp ulog_app yts"
