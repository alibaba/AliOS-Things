src = ['board.c']

component = aos_board_component('board_esp32devkitc', 'esp32', src)

if aos_global_config.get('hci_h4', 0):
    component.add_global_macros('CONFIG_BLE_HCI_H4_UART_PORT=1')


supported_targets="helloworld meshapp bluetooth.bleadv bluetooth.bleperipheral networkapp mqttapp"
platform_options="wifi=1"
