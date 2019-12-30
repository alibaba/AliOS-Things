/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var mq7 = function (ao_id, do_id) {
  this.Ahandle = ADC.open(ao_id);
  this.Dhandle = GPIO.open(do_id);
  this.GetHeavy = function () {
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

module.exports = mq7;




