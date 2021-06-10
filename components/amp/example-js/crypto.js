import * as checksum from 'checksum' 
import * as crypto from 'crypto' 

function crypto_test()
{
	let enc_param = {
		'key': '000102030405060708090a0b0c0d0e0f',
		'in': '3132333435363738'
	}
	let enc_result = crypto.encrypt(enc_param);
	console.log('encrypt:'+enc_result)

	let dec_param = {
		'key': '000102030405060708090a0b0c0d0e0f',
		'in': enc_result
	}

	let dec_result = crypto.decrypt(dec_param);
	console.log('decrypt:'+dec_result)
}

function checksum_test()
{
	let check_param = {
		'src': '000102030405060708090a0b0c0d0e0f'
	}

	let crc16_result = checksum.crc16(check_param);
	console.log('crc16:'+crc16_result)

	let crc32_result = checksum.crc32(check_param);
	console.log('crc32:'+crc32_result)

	let md5 = checksum.md5(check_param);
	console.log('md5:'+md5)
}

//checksum_test()
crypto_test()

