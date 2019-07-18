var BigKey = require('bigkey/src');
var handle = new BigKey('bigkey');
var keycb = function (val) {
  console.log('key press:' + val);
};
handle.listen('both', keycb);
