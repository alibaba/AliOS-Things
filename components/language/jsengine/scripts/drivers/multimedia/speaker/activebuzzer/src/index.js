/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var activebuzzer = function (id) {

  this.Handle = GPIO.open(id);
  var self = this;
  this.open = function () {
    GPIO.write(self.Handle, 1);
  };

  this.close = function () {
    GPIO.write(self.Handle, 0);
  };

  this.set = function (val) {
    GPIO.on(self.Handle, val);
  };

};

module.exports = activebuzzer;

