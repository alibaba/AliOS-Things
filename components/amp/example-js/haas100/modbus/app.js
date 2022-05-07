import * as mb from './modbus.js';

var deviceAddr = 1;
var device = mb.Modbus(
  { id: 'UART1' },
  { id: 'D13', polarity: 1 },
  deviceAddr
);

/* read single coil */
setInterval(() => {
  device.read(deviceAddr, 'hr501-555', 1000).then((result) => {
    console.log('read data ' + result)

  }).catch((res) => {
    console.log('read fail ' + res)
  });
}, 5000);

