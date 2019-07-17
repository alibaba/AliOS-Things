/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var ps2 = function(x_id,y_id,z_id) {
  this.xhandle = ADC.open(x_id);
  this.yhandle = ADC.open(y_id);
  this.zhandle = GPIO.open(z_id);
  this.zfun = null;
  
  this.get_xy = function(){
    var tmp = [0,0];
    tmp[0] = ADC.read(this.xhandle);
    tmp[1] = ADC.read(this.yhandle);
    return tmp;
  };

  this.onz = function(funz){
      this.zfun =   funz; 
      GPIO.on(this.zhandle,'falling',zfun);
  };

};

module.exports = ps2;