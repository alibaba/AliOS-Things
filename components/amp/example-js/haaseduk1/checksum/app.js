import * as checksum from 'checksum'

let check_param = {
    'src': '000102030405060708090a0b0c0d0e0f'
}

let crc16_result = checksum.crc16(check_param);
console.log('crc16:'+crc16_result)

let crc32_result = checksum.crc32(check_param);
console.log('crc32:'+crc32_result)

let md5 = checksum.md5(check_param);
console.log('md5:'+md5)