src = ['board.c']

component = aos_board_component('board_esp32devkitc', 'esp32', src)

if aos_global_config.get('hci_h4', 0):
    component.add_global_macros('CONFIG_BLE_HCI_H4_UART_PORT=1')


platform_options="wifi=1"
linux_only_targets="bluetooth.bleadv bluetooth.bleperipheral helloworld networkapp acapp linkkitapp mqttapp coapapp linkkit_gateway comboapp hdlcapp.hdlcserver tls meshapp bluetooth.blemesh smpapp bluetooth.breezeapp bluetooth.blemesh_cli blink wifihalapp udataapp netmgrapp nano helloworld_nocli bluetooth.blemesh_srv athostapp comboapp cryptotest yts bluetooth.blemesh_tmall i2c_hts221_test otaapp kernel_test http2app devkit.dk_ld tinyengine_app"
