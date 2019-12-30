/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var lm393 = function (id) {

  this.handle = GPIO.open(id);
  this.check = function () {
    var val = GPIO.read(this.handle);
    if (val) return 0;
    return 1;
  };
};

module.exports = lm393;