import * as uart from 'uart'
/* Uart's options are configured in app.json.
{
    "version": "0.0.1",
    "io": {
        "serial": {
            "type": "UART",
            "port": 2,
            "dataWidth":8,
            "baudRate":115200,
            "stopBits":0,
            "flowControl":"disable",
            "parity":"none"
        }
    },
    "debugLevel": "DEBUG"
}
*/

var msgbuf = 'this is amp uart test'

function ab2str(buf) {
  return String.fromCharCode.apply(null, new Uint8Array(buf));
}

console.log('uart open')
var serial = uart.open({
  id: 'serial',
  //mode: 'poll', //just for read mode
  success: function() {
    console.log('open uart success')
  },
  fail: function() {
    console.log('open uart failed')
  }
});
console.log('uart write')

serial.write(msgbuf);

sleepMs(1000);

console.log('uart read')
var rCnt = 0;
var rtrn = 0;

var value = ''

//just for read mode
// while(1)
// {
//   rtrn = serial.read()
//   if(0 != rtrn)
//   {
//     value += ab2str(rtrn);
//     rCnt++;
//   }

//   if(rCnt > 10)
//   {
//     break;
//   }

// }

// console.log('sensor value is ' + value)

serial.on('data', function(data, len) {
console.log('uart receive data len is : ' + len + '  data is:  ' + ab2str(data));
})

//serial.close();
