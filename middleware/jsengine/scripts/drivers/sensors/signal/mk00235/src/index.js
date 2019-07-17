/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var mk00235 = function(id) {
    this.handle = GPIO.open(id);
    this.cb = null;
    var self = this;
    var t = null;
    this.check = function(){
        var value = GPIO.read(self.handle);
        if(0 == value && null != self.cb) {
            self.cb(1);
        }
    };

    this.OnRain = function(cb){
        self.cb = cb;
        if(null != self.t) {
            clearInterval(self.t);
        }
        self.t = setInterval(self.check,1000);
    };
};
module.exports = mk00235;




