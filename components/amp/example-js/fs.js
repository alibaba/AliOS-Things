var fs = require('fs');

var path = './test.data';
var file_content = 'this is amp fs test file';

// write file
fs.writeSync(path, file_content);

// read file
var data = fs.readSync(path);
console.log('fs read: ' + data);

fs.unlinkSync(path);