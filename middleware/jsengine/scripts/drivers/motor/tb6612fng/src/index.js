/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var codedmotor = function (ain0_id, ain1_id, pwma) {

  this.ain0_handle = GPIO.open(ain0_id);
  this.ain1_handle = GPIO.open(ain1_id);
  this.pwma_handle = PWM.start(pwma);
  this.set = function (cw, pwm_val) {

    if (cw) {
      GPIO.write(this.ain0_handle, 0);
      GPIO.write(this.ain1_handle, 1);
    } else {
      GPIO.write(this.ain0_handle, 1);
      GPIO.write(this.ain1_handle, 0);
    }
    if (pwm_val > 100) {
      pwm_val = 100;
    }
    if (pwm_val < 0) {
      pwm_val = 0;
    }
    PWM.setDuty(this.pwma_handle, pwm_val);
  };

};

module.exports = codedmotor;
