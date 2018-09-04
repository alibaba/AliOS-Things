var config = {port: 1, baudRate: 921600, dataBits: Uart.DATA_WIDTH_8BIT, parity: Uart.NO_PARITY, stopBits: Uart.STOP_BITS_1, flowControl: Uart.FLOW_CONTROL_DISABLED};
Uart.open(config);

setInterval(function() {
  Uart.write(1, 'hello world\n');
}, 3000);