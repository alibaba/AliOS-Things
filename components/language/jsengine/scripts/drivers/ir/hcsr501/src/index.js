/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
// Infrared sensor
// need to connect GPIO t0 hcsr501 OUT pin

var hcsr501 = function (id) {
  this.handle = GPIO.open(id);
  this.cb = null;

  this.check = function () {
    var val = GPIO.read(this.handle);
    return val;
  };
};

module.exports = hcsr501;