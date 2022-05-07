import * as fs from 'fs'

var path = '/data/jsamp/test.data';
var content = 'this is javascript fs test file';

/** filesystem total size */
console.log('fs total size: ' + fs.totalSize());

/** write file */
fs.writeSync(path, content);
console.log('fs write: ' + content);

/** used size */
console.log('fs used size: ' + fs.usedSize());

/** free size */
console.log('fs free size: ' + fs.freeSize());

/** read file */
var data = fs.readSync(path);
console.log('fs read: ' + data);

if (data === content) {
    console.log('congratulations!! fs test pass ..');
    fs.unlinkSync(path);
}
