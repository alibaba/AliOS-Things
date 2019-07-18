/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var bigkey = function (key_id) {

  this.Handle = GPIO.open(key_id);
  var self = this;
  this.listen = function (edge, cb) {
    GPIO.on(self.Handle, edge, cb);
  };
};

module.exports = bigkey;

