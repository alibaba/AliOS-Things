/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
function byteTobcd(val){
    var tmp0 = val / 10;
    tmp0 = tmp0 << 4;
    var tmp1 = val % 10;
    tmp0 = tmp0 | tmp1;
    return tmp0;
}

function bcdTobyte(val){
    var tmp0 = val & 0x0F;
    tmp1 = val & 0xF0;
    tmp1 = tmp1 /16;
    tmp1 = tmp1 * 10;
    tmp1 = tmp1 + tmp0;
    return tmp1;
}

var ds1302 = function(id) {

  this.isInited = 0;
  this.handle = I2C.open(id);
  this.read_one = function(addr){
    var reg=[0];
    reg[0] = addr;
    I2C.write(this.handle,reg);
    var val = I2C.read(this.handle,1); 
    return val[0];
  };

  this.write_one = function(addr,val){
      var data = [0,0];
      data[0]=addr; 
      data[1]=val; 
      I2C.write(this.handle,data);
  };

  this.init_config = function(){

      var data = this.read_one(0x00);
      data = data & 0x7F;
      this.write_one(0x00,data); 

      /*24 hour mode*/
      data = this.read_one(0x02);
      data = data & 0xBF;
      this.write_one(0x02,data);
  };

  this.getTime = function(){

      var time = [-1,-1,-1,-1,-1,-1];
      if(0 == this.isInited){
          this.init_config();
          this.isInited = 1;
      }
      var reg=[0];
      reg[0] = 0x00;
      I2C.write(this.handle,reg);
      var val = I2C.read(this.handle,7); 
      time[0] = bcdTobyte(val[0]);
      time[1] = bcdTobyte(val[1]);
      time[2] = bcdTobyte(val[2]);
      time[3] = bcdTobyte(val[3]);
      time[4] = bcdTobyte(val[4]);
      time[5] = bcdTobyte(val[5]);
      time[6] = bcdTobyte(val[6]);
      return time;
  };
};

module.exports = ds1302;