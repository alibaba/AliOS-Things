/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var sw420 = function (id) {
  this.handle = GPIO.open(id);
  this.getVal = function () {
    var val = 0;
    val = GPIO.read(this.handle);
    return val;
  };

};

module.exports = sw420;


