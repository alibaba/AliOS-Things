/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var delay = function(val) {
  var i = 0;
  var tmp = 0;
  for (i=0;i < val;i++) {
    tmp = 0;
  }
};

var hmc5883 = function(id) {
  this.handle = I2C.open(id);
  this.openflag = 0;
  this.regRead = function(reg) {

    var data = [0x00];
    data[0] = reg;
    I2C.write(this.handle,data);
    var val = I2C.read(this.handle,1);
    return val[0];

  };
  this.regWrite = function(reg,val){
    var data = [0x00,0x00];
    data[0] = reg;
    data[1] = val;
    I2C.write(this.handle,data);
  };

  this.getId = function() {
    var id = [0,0,0]
    id[0] = this.regRead(10);
    id[1] = this.regRead(11);
    id[2] = this.regRead(12);
    return id;
  };
  this.init = function(){
    this.regWrite(0x02,0x00);
  };

  this.getAcc = function(){

    var data = [0,0,0,0,0,0];
    var i = 0;
    var status = 0;
    if (0 == this.openflag){
      this.init();
      this.openflag = 1;
    }
    for(i=0;i<50;i++){
      status = this.regRead(9);
      if(status&0x01){
        break;
      }
      delay(30);
    }
    if(0 == status&0x01){
      return data;
    }
   
    var cmd = [0x03];
    I2C.write(this.handle,cmd);
    var val = I2C.read(this.handle,6);
    data[0] = val[0];
    data[1] = val[0];
    data[2] = val[2];
    data[3] = val[3];
    data[4] = val[4];
    data[5] = val[5];
    return data;
  };

};

module.exports = hmc5883;