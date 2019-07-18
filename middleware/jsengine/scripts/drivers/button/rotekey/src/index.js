/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


var rotekey = function (swId, dtId, clkId) {
  this.swHandle = GPIO.open(swId);
  this.dtHandle = GPIO.open(dtId);
  this.clkHandle = GPIO.open(clkId);
  var self = this;
  this.pressCb = null;
  this.dirCb = null;
  this.OnPress = function (cb) {
    self.pressCb = cb;
    GPIO.on(self.swHandle, 'falling', self.pressCb);
  };

  this.check = function () {
    var val = GPIO.read(self.dtHandle);
    if (null != self.dirCb) {
      self.dirCb(val);
    }
  };

  this.OnDir = function (cb) {
    self.dirCb = cb;
    GPIO.on(self.clkHandle, 'falling', self.check);
  };
};

module.exports = rotekey;

