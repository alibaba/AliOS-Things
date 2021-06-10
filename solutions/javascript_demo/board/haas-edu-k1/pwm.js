/* 
 * PWM's options are configured in app.json.
 
{
  "version": "0.0.1",
  "io": {
      "pwm1": {
          "type": "PWM",
          "port": 1
      },
      "timer1": {
          "type":"TIMER",
          "port": 1
      }
  },
  "debugLevel": "DEBUG"
}
*/
import * as pwm from 'pwm'
import * as timer from 'TIMER'

console.log('pwm: pwm test open ')

var pwm1 = pwm.open({
  id: 'pwm1',
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
  duty  = duty + 20;

  pwm1.set({
    freq: 100,
    duty: duty
  })

  console.log('pwm: pwm test count ' + cnt)
  cnt = cnt - 1;
  if (cnt == 0) {
    pwm1.close();
    console.log('pwm: pwm test finish loop ' + loop)
    loop--;
    if (loop == 0) {
      timer1.clearInterval();
    }
    else {
        pwm1 = pwm.open({
            id: 'pwm1',
            success: function() {
              console.log('pwm: open pwm success')
            },
            fail: function() {
              console.log('pwm: open pwm failed')
            }
          });
    }

    cnt = 10;
  }

}, 1000)
