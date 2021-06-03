var uart = require('uart');

var msgbuf = 'this is amp uart test'
// led灯
var serial = uart.open({
  id: 'serial',
  success: function() {
    console.log('open spi success')
  },
  fail: function() {
    console.log('open spi failed')
  }
});

serial.write(msgbuf)
var value = serial.read()

console.log('sensor value is ' + value)

serial.on('data', function(len, data) {
  console.log('uart receive data len is : ' + len + '  data is:  ' + data);
})

serial.close();