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

supported_targets="helloworld"
linux_only_targets="bluetooth.ble_bqb bluetooth.blemesh_srv hdlcapp.hdlcserver bluetooth.breezeapp bluetooth.bleadv bluetooth.blemesh helloworld_nocli wifihalapp bluetooth.blemesh_cli acapp nano bluetooth.bleperipheral"
