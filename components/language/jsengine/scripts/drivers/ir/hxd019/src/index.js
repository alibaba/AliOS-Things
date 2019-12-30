/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var hxd019 = function (scl_id, sda_id, busy_id) {
  this.sclHandle = GPIO.open(scl_id);
  this.sdaHandle = GPIO.open(sda_id);
  this.busyHandle = GPIO.open(busy_id);

  this.init = function () {
    GPIO.write(this.sdaHandle, 1);
    GPIO.write(this.sclHandle, 1);
  };

  this.send = function (buf) {
    this.init();
    IR.send(this.sdaHandle, this.sclHandle, buf);
  };

  this.learn = function () {
    var arr = IR.learn(this.sdaHandle, this.sclHandle, this.busyHandle);
    return arr;
  };

};

module.exports = hxd019;
