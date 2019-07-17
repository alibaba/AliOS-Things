/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var d5a2 = function(id) {
    this.handle = GPIO.open(id);
    var self = this;
    this.Open = function(){
        GPIO.write(self.handle,1);
    };
    this.Close = function(){
        GPIO.write(self.handle,0);
    }; 
};

module.exports = d5a2;




