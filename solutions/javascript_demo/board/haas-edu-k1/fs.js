import * as fs from 'fs'

var path = '/workspace/dev_amp/test.data';
//var path = '/tmp/test.data';
var content = 'this is amp fs test file';

console.log('testing fs write...');
// write file
fs.writeSync(path, content);

console.log('testing fs read...');
// read file
var data = fs.readSync(path);
console.log('fs read: ' + data);

console.log('testing fs delete...');
fs.unlinkSync(path);
