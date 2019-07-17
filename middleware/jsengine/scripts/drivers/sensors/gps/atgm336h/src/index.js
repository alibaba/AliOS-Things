/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var atgm336h = function(id) {
    this.handle = UART.open(id);
    this.cb = null;
    var self = this;
    this.recv = function(start,end,cb){
        self.cb = cb;
        UART.on(self.handle,start,end,self.cb);
    };

    this.send = function(str){
        UART.write(this.handle,str);
    };
};
module.exports = atgm336h;




