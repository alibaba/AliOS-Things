/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var hs0038 = function (id) {

  this.handle = IR.open(id);
  this.user_callback = null;
  var ref = this;
  this.ir_code = function (val) {
    if ('error' == val) {
      return;
    }
    if (null != ref.user_callback) {
      ref.user_callback(val);
    }
  }
  this.on = function (cb) {
    ref.user_callback = cb;
    IR.on(ref.handle, ref.ir_code);
  };
};

module.exports = hs0038;