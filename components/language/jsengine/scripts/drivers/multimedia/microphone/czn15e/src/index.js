/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var czn15e = function (id) {

  this.handle = GPIO.open(id);
  this.get = function () {
    var val = GPIO.read(this.handle);
    return val;
  };
};

module.exports = czn15e;