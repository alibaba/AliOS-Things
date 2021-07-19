/* 
 * PWM's options are configured in app.json.
 
{
    "version": "0.0.1",
    "io": {
        "pwm0": {
            "type": "PWM",
            "port": 0
        },
        "timer1": {
            "type":"TIMER",
            "port": 0
        }
    },
    "debugLevel": "DEBUG"
  }
*/
import * as pwm from 'pwm'
import * as timer from 'timer'

console.log('pwm: pwm test open ')

var pwm1 = pwm.open({
  id: 'pwm0',
  success: function() {
    console.log('pwm: open pwm success')
  },
  fail: function() {
    console.log('pwm: open pwm failed')
  }
});

var timer1 = timer.open({
    id: 'timer1'
  });

var freq = pwm1.get().freq
var duty = pwm1.get().duty

console.log('pwm: pwm default config freq is ' + freq + ' duty is ' + duty)

console.log('pwm: pwm test start ')

duty = 0;
var cnt = 10;
var loop = 10;

timer1.setInterval(function(){
  if (duty >= 100) {
    duty = 0;
  }
  duty  = duty + 10;

  pwm1.set({
    freq: 200, /* Configure 200 ～ 900 to make the buzzer work  */
    duty: duty
  })

  freq = pwm1.get().freq
  duty = pwm1.get().duty

  console.log('pwm: pwm timer config freq is ' + freq + ' duty is ' + duty)

  console.log('pwm: pwm test count ' + cnt)
  cnt = cnt - 1;
  if (cnt == 0) {

    console.log('pwm: pwm test finish loop ' + loop)
    loop--;
    if (loop == 0)
    {
      pwm1.close();
      timer1.clearInterval();
    }
    // else {
    //     pwm1 = pwm.open({
    //         id: 'pwm1',
    //         success: function() {
    //           console.log('pwm: open pwm success')
    //         },
    //         fail: function() {
    //           console.log('pwm: open pwm failed')
    //         }
    //       });
    // }

    cnt = 10;
  }

}, 1000000)
