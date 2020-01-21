src =Split('''
    board.c
''')
component =aos_component('board_pca10040e', src)


global_includes =Split('''
    .
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split('''
    STDIO_UART=0
    CONFIG_NO_TCPIP
    BOARD_PCA10040e
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

linux_only_targets="blink bluetooth.bleadv bluetooth.blemesh_tmall bluetooth.bleperipheral bluetooth.breezeapp helloworld udata_demo.sensor_local_demo udata_demo.udata_local_demo udataapp yts"
