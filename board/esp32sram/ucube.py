src = ['board.c']

component = aos_board_component('board_esp32sram', 'esp32', src)

if aos_global_config.get('hci_h4', 0):
    component.add_global_macros('CONFIG_BLE_HCI_H4_UART_PORT=1')


platform_options="wifi=1"
supported_targets="alinkapp bluetooth.bleadv bluetooth.bleperipheral helloworld networkapp acapp linkkitapp mqttapp coapapp linkkit_gateway linkkit_sched comboapp"
linux_only_targets="hdlcapp.hdlcserver tls meshapp smpapp linuxapp blink wifihalapp uDataapp netmgrapp nano helloworld_nocli athostapp comboapp"
