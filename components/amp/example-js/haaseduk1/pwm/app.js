import * as pwm from 'pwm'
import * as timer from 'timer'

console.log('pwm: pwm test open ')

var pwm0 = pwm.open({
  id: 'PWM0',
  success: function() {
    console.log('pwm: open pwm0 success')
  },
  fail: function() {
    console.log('pwm: open pwm0 failed')
  }
});

var timer0 = timer.open({
    id: 'TIMER0'
  });

var freq = pwm0.get().freq
var duty = pwm0.get().duty

console.log('pwm: pwm default config freq is ' + freq + ' duty is ' + duty)

console.log('pwm: pwm test start ')

duty = 0;
var cnt = 10;
var loop = 10;

timer0.setInterval(function(){
  if (duty >= 100) {
    duty = 0;
  }
  duty  = duty + 10;

  pwm0.set({
    freq: 200, /* Configure 200 ～ 900 to make the buzzer work  */
    duty: duty
  })

  freq = pwm0.get().freq
  duty = pwm0.get().duty

  console.log('pwm: pwm timer config freq is ' + freq + ' duty is ' + duty)

  console.log('pwm: pwm test count ' + cnt)
  cnt = cnt - 1;
  if (cnt == 0) {

    console.log('pwm: pwm test finish loop ' + loop)
    loop--;
    if (loop == 0)
    {
      pwm0.close();
      timer0.clearInterval();
    }
    cnt = 10;
  }

}, 1000000)