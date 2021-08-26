./qjsc_linux -c -m src/gpio.js -N jslib_gpio -M GPIO -o bytecode/jslib_gpio.c
./qjsc_linux -c -m src/events.js -N jslib_events -o bytecode/jslib_events.c
./qjsc_linux -c -m src/crypto.js -N jslib_crypto -M events -M CRYPTO -o bytecode/jslib_crypto.c
./qjsc_linux -c -m src/checksum.js -N jslib_checksum -M events -M CHECKSUM -o bytecode/jslib_checksum.c
./qjsc_linux -c -m src/appota.js -N jslib_appota -M events -M APPOTA -o bytecode/jslib_appota.c
./qjsc_linux -c -m src/iot.js -N jslib_iot -M events -M AIOT_DEVICE -M AIOT_GATEWAY -o bytecode/jslib_iot.c
./qjsc_linux -c -m src/bt_host.js -N jslib_bt_host -M events -M BT_HOST -o bytecode/jslib_bt_host.c
./qjsc_linux -c -m src/repl.js -o bytecode/jslib_repl.c -N jslib_repl -M REPL -M os -M std
./qjsc_linux -c -m src/uart.js -o bytecode/jslib_uart.c -N jslib_uart -M events -M UART
./qjsc_linux -c -m src/fs.js -o bytecode/jslib_fs.c -N jslib_fs -M events -M FS
./qjsc_linux -c -m src/audioplayer.js -o bytecode/jslib_audioplayer.c -N jslib_audioplayer -M events -M AUDIOPLAYER
./qjsc_linux -c -m src/device.js -o bytecode/jslib_device.c -N jslib_device -M kv
./qjsc_linux -c -m src/network.js -N jslib_network -M events -M NETWORK -M NETMGR -M CELLULAR -o bytecode/jslib_network.c
./qjsc_linux -c -m src/netmgr.js -N jslib_netmgr -M events -M NETMGR -o bytecode/jslib_netmgr.c
./qjsc_linux -c -m src/mqtt.js -N jslib_mqtt -M MQTT -M events -o bytecode/jslib_mqtt.c
./qjsc_linux -c -m src/tcp.js -N jslib_tcp -M events -M TCP -o bytecode/jslib_tcp.c
./qjsc_linux -c -m src/location.js -N jslib_location -M NETWORK -M NETMGR -M CELLULAR -o bytecode/jslib_location.c
