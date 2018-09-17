src =Split(''' 
    board.c
''')
component =aos_component('board_pca10040', src)


global_includes =Split(''' 
    .
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    STDIO_UART=0
    CONFIG_NO_TCPIP
    BOARD_PCA10040
    CONFIG_GPIO_AS_PINRESET
    FLOAT_ABI_HARD
    NRF52832_XXAA
    CONFIG_SOC_SERIES_NRF52X
    CONFIG_CLOCK_CONTROL_NRF5_K32SRC_250PPM=y
    CONFIG_BOARD_NRF52832_PCA10040=1
    RHINO_CONFIG_WORKQUEUE=1
''')
for i in global_macros:
    component.add_global_macros(i)

linux_only_targets="netmgrapp nano helloworld bluetooth.blemesh_srv wifihalapp bluetooth.blemesh_cli hdlcapp.hdlcserver acapp uDataapp bluetooth.bleperipheral vflashdemo bluetooth.breezeapp bluetooth.bleadv bluetooth.blemesh bluetooth.ble_bqb helloworld_nocli"
