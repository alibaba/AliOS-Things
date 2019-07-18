/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var ne555 = function (id) {
  this.handle = GPIO.open(id);
  this.cb = null;
  var self = this;
  this.getHz = function (cb) {
    self.cb = cb;
    GPIO.count(self.handle, 'falling', 1000, self.cb);
  };
};
module.exports = ne555;




