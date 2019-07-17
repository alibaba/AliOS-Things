/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var xd3144 = function(id) {
    this.handle = GPIO.open(id);
    this.cb = null;
    var self = this;
    this.getSpeed = function(cb){
        self.cb = cb;
        GPIO.count(self.handle,'falling',50,self.cb);
    };
};
module.exports = xd3144;




