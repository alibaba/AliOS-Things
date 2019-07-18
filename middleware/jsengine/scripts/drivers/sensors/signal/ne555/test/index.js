var NE555 = require('ne555/src');
var handle = new NE555('ne555');

handle.getHz(function (count) {
  console.log('hz:' + count + 'n/s');
});
