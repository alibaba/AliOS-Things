import * as kv from 'kv'

var key = 'key-test';
var value = 'this is amp kv test file';

// kv set
kv.setStorageSync(key, value);

// kv get
var val = kv.getStorageSync(key);
console.log('kv read: ' + val);

// kv remove
kv.removeStorageSync(key);
