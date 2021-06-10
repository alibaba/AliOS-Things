./qjsc -c -m src/gpio.js -N jslib_gpio -M GPIO -o bytecode/jslib_gpio.c
./qjsc -c -m src/events.js -N jslib_events -o bytecode/jslib_events.c
./qjsc -c -m src/crypto.js -N jslib_crypto -M events -M CRYPTO -o bytecode/jslib_crypto.c
./qjsc -c -m src/checksum.js -N jslib_checksum -M events -M CHECKSUM -o bytecode/jslib_checksum.c
./qjsc -c -m src/appota.js -N jslib_appota -M events -M APPOTA -o bytecode/jslib_appota.c
./qjsc -c -m src/iot.js -N jslib_iot -M events -M AIOT_DEVICE -o bytecode/jslib_iot.c
./qjsc -c -m src/bt_host.js -N jslib_bt_host -M events -M BT_HOST -o bytecode/jslib_bt_host.c
./qjsc -c src/gateway.js -N jslib_gateway -M AIOT_GATEWAY -M events -o bytecode/jslib_gateway.c
./qjsc -c src/repl.js -o bytecode/jslib_repl.c -N jslib_repl -M REPL -M os -M std
