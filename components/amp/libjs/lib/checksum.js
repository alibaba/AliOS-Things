'use strict';

function md5(obj) {
    if (!obj) {
        throw new Error('params is invalid');
    }
    return __native.CHECKSUM.md5(obj);
}

function crc16(rawData) {
    if (!rawData) {
        throw new Error('params is invalid');
    }
    return __native.CHECKSUM.crc16(rawData);
}

function crc32(rawData) {
    if (!rawData) {
        throw new Error('params is invalid');
    }
    return __native.CHECKSUM.crc32(rawData);
}

module.exports = {
    md5,
    crc16,
    crc32,
}