/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


var encoder = function (irq_id, input_id) {
  this.irq_handle = GPIO.open(irq_id);
  this.input_handle = GPIO.open(input_id);
  this.interval = 500;
  this.speed = 0;
  this.turn = 0;
  this.cb = null;
  this.config = function (interval) {
    this.interval = interval;

  };
  var self = this;
  this.listen = function (val) {
    self.speed = val;
    if (null != self.cb) {
      self.cb(val);
    }
  };
  this.start = function (cb) {
    if (null != cb) {
      this.cb = cb;
    }
    GPIO.count(this.irq_handle, "falling", this.interval, this.listen);
  };
  this.getSpeed = function () {
    return this.speed;
  };

  this.reverseOrForward = function () {
    return this.turn;
  };

  this.setTurn = function (val) {
    this.turn = val;
  };
};

module.exports = encoder;


