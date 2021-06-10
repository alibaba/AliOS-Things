
import * as uart from 'uart'

/* PWM's options are configured in app.json.
{
    "version": "0.0.1",
    "io": {
        "serial": {
            "type": "UART",
            "port": 2
        }
    },
    "debugLevel": "DEBUG"
}
*/

var msgbuf = 'this is amp uart test'
// led灯

console.log('uart open')
var serial = uart.open({
  id: 'serial',
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

while(1)
{
  rtrn = serial.read()
  if(0 != rtrn)
  {
    value += rtrn;
    rCnt++;
  }

  if(rCnt > 10)
  {
    break;
  }

}
console.log('sensor value is ' + value)
serial.on('data', function(len, data) {
console.log('uart receive data len is : ' + len + '  data is:  ' + data);
})

serial.close();
