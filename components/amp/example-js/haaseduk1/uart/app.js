import * as uart from 'uart'

var msgbuf = 'this is amp uart test'

function ab2str(buf) {
  return String.fromCharCode.apply(null, new Uint8Array(buf));
}

console.log('please short haaseduk1 UART2_TXD & UART2_RXD to finish this test !!!')
var serial = uart.open({
  id: 'serial',
  success: function() {
    console.log('open uart success')
  },
  fail: function() {
    console.log('open uart failed')
  }
});
console.log('uart write: ' + msgbuf)

serial.write(msgbuf);

sleepMs(1000);

serial.on('data', function(data, len) {
    console.log('uart receive data len is : ' + len + '  data is:  ' + ab2str(data));
    serial.close();
})
