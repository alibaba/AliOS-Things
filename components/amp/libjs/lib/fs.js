'use strict';

function writeSync(path, data, options) {
    if (!path || !data) {
        throw new Error('params is invalid');
    }
    options = options || {flag: 'w'};
    __native.FS.write(path, data, options);
}

function readSync(path) {
    if(!path) {
        throw new Error("invalid params");
    }
    var content = __native.FS.read(path);
    if (!content) {
        throw 'file open error';
    }
    
    return content;
}

function unlinkSync(path) {
    if(!path) {
        throw new Error("invalid params");
    }
    __native.FS.delete(path);
}

module.exports = {
    writeSync,
    readSync,
    unlinkSync
}