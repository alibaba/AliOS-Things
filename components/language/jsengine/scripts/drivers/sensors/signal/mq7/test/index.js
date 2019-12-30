var MQ7 = require('mq7/src');
var handle = new MQ7('mq7.ao', 'mq7.do');

setInterval(function () {
  var tmp_val = handle.GetHeavy();
  console.log('heavy:' + tmp_val);
  tmp_val = handle.GetFlags();
  if (0 == tmp_val) {
    console.log('Warning! CO gas out of limit!')
  }
}, 2000);
