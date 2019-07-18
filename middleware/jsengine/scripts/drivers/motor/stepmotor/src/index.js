/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var speedDelay = function (val) {
  var i = 0;
  var tmp = 0;
  for (i = 0; i < val; i++) {
    tmp = 0;
  }
};

var stepmotor = function (a_id, b_id, c_id, d_id) {

  this.ahandle = GPIO.open(a_id);
  this.bhandle = GPIO.open(b_id);
  this.chandle = GPIO.open(c_id);
  this.dhandle = GPIO.open(d_id);

  this.forwardRun = function (index) {
    if (0 == index) {
      GPIO.write(this.ahandle, 1);
      GPIO.write(this.bhandle, 1);
      GPIO.write(this.chandle, 0);
      GPIO.write(this.dhandle, 0);

    } else if (1 == index) {
      GPIO.write(this.ahandle, 0);
      GPIO.write(this.bhandle, 1);
      GPIO.write(this.chandle, 1);
      GPIO.write(this.dhandle, 0);
    } else if (2 == index) {

      GPIO.write(this.ahandle, 0);
      GPIO.write(this.bhandle, 0);
      GPIO.write(this.chandle, 1);
      GPIO.write(this.dhandle, 1);
    } else if (3 == index) {
      GPIO.write(this.ahandle, 1);
      GPIO.write(this.bhandle, 0);
      GPIO.write(this.chandle, 0);
      GPIO.write(this.dhandle, 1);

    } else {
      console.log('unknow index');

    }
  };

  this.reversRun = function (index) {

    if (0 == index) {
      GPIO.write(this.ahandle, 1);
      GPIO.write(this.bhandle, 0);
      GPIO.write(this.chandle, 0);
      GPIO.write(this.dhandle, 1);

    } else if (1 == index) {
      GPIO.write(this.ahandle, 0);
      GPIO.write(this.bhandle, 0);
      GPIO.write(this.chandle, 1);
      GPIO.write(this.dhandle, 1);


    } else if (2 == index) {

      GPIO.write(this.ahandle, 0);
      GPIO.write(this.bhandle, 1);
      GPIO.write(this.chandle, 1);
      GPIO.write(this.dhandle, 0);
    } else if (3 == index) {
      GPIO.write(this.ahandle, 1);
      GPIO.write(this.bhandle, 1);
      GPIO.write(this.chandle, 0);
      GPIO.write(this.dhandle, 0);

    } else {
      console.log('unknow index!');

    }

  };
};

module.exports = stepmotor;

