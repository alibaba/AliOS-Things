src = ['board.c']

component = aos_board_component('board_esp32sram', 'esp32', src)

if aos_global_config.get('hci_h4', 0):
    component.add_global_macros('CONFIG_BLE_HCI_H4_UART_PORT=1')


linux_only_targets="athostapp blink bluetooth.bleadv bluetooth.bleperipheral bluetooth.breezeapp coapapp helloworld linkkit_gateway linkkitapp meshapp mqttap phttp2app otaapp tls udataapp udevapp yts"
