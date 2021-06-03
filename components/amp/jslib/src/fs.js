import * as FS from 'FS'

export function writeSync(path, data, options) {
    if (!path || !data) {
        throw new Error('params is invalid');
    }
    options = options || {flag: 'w'};
    FS.write(path, data, options);
}

export function readSync(path) {
    if(!path) {
        throw new Error("invalid params");
    }
    var content = FS.read(path);
    if (!content) {
        throw 'file open error';
    }
    return content;
}

export function unlinkSync(path) {
    if(!path) {
        throw new Error("invalid params");
    }
    FS.delete(path);
}

export function totalSize() {
    return FS.totalsize();
}

export function usedSize() {
    return FS.usedsize();
}

export function freeSize() {
    return FS.freesize();
}
