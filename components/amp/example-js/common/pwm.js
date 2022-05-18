console.log('pwm: pwm requre ')
var pwm = require('pwm');
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

var freq = pwm1.getOption().freq
var duty = pwm1.getOption().duty

console.log('pwm: pwm default config freq is ' + freq + ' duty is ' + duty)

console.log('pwm: pwm test start ')

duty = 0;
var cnt = 10;
var loop = 10;
var timer = setInterval(function(){
  if (duty >= 100) {
    duty = 0;
  }
  duty  = duty + 20;

  pwm1.setOption({
    freq: 100,
    duty: duty
  })

  console.log('pwm: pwm test count ' + cnt)
  cnt = cnt - 1;
  if (cnt == 0) {
    pwm1.close();
    console.log('pwm: pwm test finish ')
    loop--;
    if (loop == 0) {
      clearInterval(timer);
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
