/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var shtc1 = function (id) {

  this.isOn = 1;
  this.handle = I2C.open(id);
  this.getTempHumi = function () {

    var val = [0, 0];
    var Reg = [0x80, 0x5D];
    I2C.write(this.handle, Reg);
    Reg[0] = 0x7C;
    Reg[1] = 0xA2;
    I2C.write(this.handle, Reg);
    var TempHumm = I2C.read(this.handle, 6);

    var temp = TempHumm[0] << 8;
    temp = temp | TempHumm[1];
    var humi = TempHumm[3] << 8;
    humi = humi | TempHumm[4];

    temp = temp * 175;
    temp = temp / 65536;
    temp = temp - 45;
    humi = humi * 100;
    humi = humi / 65536;

    val[0] = temp;
    val[1] = humi;
    return val;
  };


};

module.exports = shtc1;



