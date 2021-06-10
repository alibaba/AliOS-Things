import * as CHECKSUM from 'CHECKSUM'

export function md5(obj) {
    if (!obj) {
        throw new Error('params is invalid');
    }
    return CHECKSUM.md5(obj);
}

export function crc16(rawData) {
    if (!rawData) {
        throw new Error('params is invalid');
    }
    return CHECKSUM.crc16(rawData);
}

export function crc32(rawData) {
    if (!rawData) {
        throw new Error('params is invalid');
    }
    return CHECKSUM.crc32(rawData);
}

