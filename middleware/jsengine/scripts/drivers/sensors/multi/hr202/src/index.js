/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var hr202 = function(id) {
    this.Dhandle = GPIO.open(id);
    this.cb = null;
    this.t = null;

    var self = this;

    this.check = function(){
        var val = GPIO.read(self.Dhandle);
        if(0 == val && null != self.cb){
            self.cb(val);  
        }
    };
    this.OnHum = function(cb){
        self.cb = cb
        if (null != self.t) {
            clearInterval(self.t);
        }
        self.t = setInterval(self.check,100);
    };
    
};

module.exports = hr202;




