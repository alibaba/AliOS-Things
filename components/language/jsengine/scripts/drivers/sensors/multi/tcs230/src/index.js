/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var cbHandle = null;
var tcs230 = null;

var tcs230S = function (s2_id, s3_id, out_id) {
  this.s2Handle = GPIO.open(s2_id);
  this.s3Handle = GPIO.open(s3_id);
  this.outHandle = GPIO.open(out_id);
  this.index = 0;
  this.rval = 0;
  this.gval = 0;
  this.bval = 0;
  this.count = 0;

  this.redFilter = function () {

    GPIO.write(this.s2Handle, 0);
    GPIO.write(this.s3Handle, 0);
  };

  this.greenFilter = function () {

    GPIO.write(this.s2Handle, 1);
    GPIO.write(this.s3Handle, 1);
  };


  this.blueFilter = function () {

    GPIO.write(this.s2Handle, 1);
    GPIO.write(this.s3Handle, 0);
  };

  this.register = function (obj, cb) {
    cbHandle = cb;
    tcs230 = obj;
  };

}


var tHandle = function () {
  if (null == tcs230) {
    return;
  }

  if (0 == tcs230.index) {
    tcs230.redFilter();
    tcs230.index = tcs230.index + 1;
    tcs230.count = 0;
  } else if (1 == tcs230.index) {
    tcs230.rval = tcs230.count;
    tcs230.index = tcs230.index + 1;
    tcs230.greenFilter();
    tcs230.count = 0;

  } else if (2 == tcs230.index) {
    tcs230.gval = tcs230.count;
    tcs230.index = tcs230.index + 1;
    tcs230.blueFilter();
    tcs230.count = 0;
  } else if (3 == tcs230.index) {
    tcs230.bval = tcs230.count;
    tcs230.index = tcs230.index + 1;
    tcs230.count = 0;
  } else {
    tcs230.index = 0;
    cbHandle(tcs230.rval, tcs230.gval, tcs230.bval);
    tcs230.rval = 0;
    tcs230.gval = 0;
    tcs230.bval = 0;
    setTimeout(tHandle, 2000);
    return;
  }
  setTimeout(tHandle, 500);
};


var cHandle = function () {

  if (null == tcs230) {
    return;
  }
  var val = GPIO.read(tcs230.outHandle);
  if (0 == val) {
    tcs230.count = tcs230.count + 1;
  }
  process.nextTick(cHandle);
};

process.nextTick(cHandle);
setTimeout(tHandle, 500);




module.exports = tcs230S;
