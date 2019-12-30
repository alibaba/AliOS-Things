/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var I74hcx = function (id) {
  this.handle = GPIO.open(id);
  this.SetVal = function (val) {
    var tmp = val;
    if (tmp) {
      tmp = 1;
    } else {
      tmp = 0;
    }
    GPIO.write(this.handle, tmp);
  };
};

module.exports = I74hcx;




