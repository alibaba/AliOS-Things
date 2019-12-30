/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var irf520 = function (id) {
  this.handle = PWM.start(id);
  this.Control = function (val) {
    PWM.setDuty(this.handle, val);
  };
};
module.exports = irf520;