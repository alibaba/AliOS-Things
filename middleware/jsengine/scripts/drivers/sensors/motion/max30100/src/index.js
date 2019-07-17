/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var max30100 = function(id) {

    this.handle = I2C.open(id);
    this.init_flag = 0;
    var self = this;
    this.write_one = function(addr,val){
        var data = [0,0];
        data[0]=addr; 
        data[1]=val; 
        I2C.write(self.handle,data);
    };

    this.read_one = function(addr){
        var reg=[0];
        reg[0] = addr;
        I2C.write(self.handle,reg);
        var val = I2C.read(self.handle,1); 
        return val[0];
    };

    this.reset = function(){

        self.write_one(0x02,0x00);
        self.write_one(0x03,0x00);
        self.write_one(0x04,0x00);
    };

    this.config = function(){

        var tmp = 0;
        if(1 == self.init_flag) {
            return;
        }
        self.write_one(0x06,0x0B);

        tmp = self.read_one(0x07);
        tmp = tmp & 0xFC;
        tmp = tmp | 0x03;
        self.write_one(0x07,tmp);

        tmp = self.read_one(0x07);
        tmp = tmp & 0xE3;
        tmp = tmp | (0x01<<2);
        self.write_one(0x07,tmp);
        self.write_one(0x09,(0x0F<<4) | 0x0F);
        self.write_one(0x01,0xF0);
        self.reset();
        self.init_flag = 1;
    };

    this.readData = function() {
        var data = [-1,-1];
        self.config();
        var tmp = self.read_one(0x00);
        if(tmp & 0x20) {
            var reg=[0];
            reg[0] = 0x05;
            I2C.write(self.handle,reg);
            var val = I2C.read(self.handle,5); 
            data[0] = val[2]*256+val[3];
            data[1] = val[0]*256+val[1];
        }
        return  data;
    };

};

module.exports = max30100;
