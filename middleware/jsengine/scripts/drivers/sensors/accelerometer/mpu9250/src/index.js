/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var delay = function(val){
  var i = 0;
  var tmp = 0;
  for(i=0;i<val;i++){
    tmp = 0;
  }
};

var mpu9250 = function(id) {
  this.handle = I2C.open(id);
  this.openflag = 0;
  this.regRead = function(reg){

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

  this.init = function() {
    this.regWrite(0x6b,0x00); 
    delay(500);
    this.regWrite(0x6c,0x00);
    this.regWrite(0x1A,0x00);
    this.regWrite(0x1B,0x10);
    this.regWrite(0x1C,0x10);
    this.regWrite(0x19,0x07);
    this.regWrite(0x1D,0x01);
    this.regWrite(0x0A,0x02);

  };

  
  this.getAcc = function(){
    var buff = [0,0,0,0,0,0];
    var accVal = [0,0,0];
    var i = 0;
    var tmp = 0;
    if(0 == this.openflag){
      this.init();
      this.openflag = 1;
    }
    buff[0] =  this.regRead(0x3B); 
    buff[1] =  this.regRead(0x3C); 
    buff[2] =  this.regRead(0x3D); 
    buff[3] =  this.regRead(0x3E); 
    buff[4] =  this.regRead(0x3F); 
    buff[5] =  this.regRead(0x40); 

    accVal[0] = (buff[0]<<8)|buff[1];
    accVal[1] = (buff[2]<<8)|buff[3];
    accVal[2] = (buff[4]<<8)|buff[5];

    return accVal;
  };
  this.getGyro = function(){
    var buff = [0,0,0,0,0,0];
    var gyroVal = [0,0,0];
    var i = 0;
    var tmp = 0;
    if(0 == this.openflag){
      this.init();
      this.openflag = 1;
    }
    buff[0] =  this.regRead(0x43); 
    buff[1] =  this.regRead(0x44); 
    buff[2] =  this.regRead(0x45); 
    buff[3] =  this.regRead(0x46); 
    buff[4] =  this.regRead(0x47); 
    buff[5] =  this.regRead(0x48); 
    gyroVal[0] = (buff[0]<<8)|buff[1];
    gyroVal[1] = (buff[2]<<8)|buff[3];
    gyroVal[2] = (buff[4]<<8)|buff[5];
    return gyroVal;
  };

};

module.exports = mpu9250;