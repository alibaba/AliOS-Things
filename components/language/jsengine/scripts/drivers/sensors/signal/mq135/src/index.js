/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var mq135 = function (ao_id, do_id) {
  this.Ahandle = ADC.open(ao_id);
  this.Dhandle = GPIO.open(do_id);
  this.GetQuarity = function () {
    var tmp;
    tmp = ADC.read(this.Ahandle);
    return tmp;
  };
  this.GetFlags = function () {
    var val;
    val = GPIO.read(this.Dhandle);
    return val;
  };
};

module.exports = mq135;




