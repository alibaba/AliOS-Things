/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var a4950 = function (pwma_id, pwmb_id) {

  this.pwma_handle = PWM.start(pwma_id);
  this.pwmb_handle = PWM.start(pwmb_id);

  this.stop = function () {
    PWM.setDuty(this.pwma_handle, 100);
    PWM.setDuty(this.pwmb_handle, 100);
  };

  this.forward = function (pwm_val) {
    var val = pwm_val;
    if (val < 0) {
      val = 0;
    }
    if (val > 100) {
      val = 100;
    }

    PWM.setDuty(this.pwma_handle, 100);
    PWM.setDuty(this.pwmb_handle, 100 - val);
  };
  this.reverse = function (pwm_val) {

    var val = pwm_val;
    if (val < 0) {
      val = 0;
    }
    if (val > 100) {
      val = 100;
    }

    PWM.setDuty(this.pwmb_handle, 100);
    PWM.setDuty(this.pwma_handle, 100 - val);
  };
};

module.exports = a4950;


