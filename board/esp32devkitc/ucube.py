src = ['board.c']

component = aos_board_component('board_esp32devkitc', 'esp32', src)

if aos_global_config.get('hci_h4', 0):
    component.add_global_macros('CONFIG_BLE_HCI_H4_UART_PORT=1')


linux_only_targets="athostapp blink bluetooth.bleadv bluetooth.bleperipheral bluetooth.breezeapp coapapp helloworld http2app httpapp jsengine_app linkkit_gateway linkkitapp lwm2mapp meshapp mqttapp otaapp udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udataapp ulocation.baseapp yts"
