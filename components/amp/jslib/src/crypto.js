import * as CRYPTO from 'CRYPTO'

export function encrypt(param) {
    let result = CRYPTO.encrypt(
						{'crypt_mode': "aes_cbc"},
						param
					);
	if (result != '') {
		console.log('encrypt success:'+result);
	} else {
		console.log('encrypt failed');
	}
	return result;
}

export function decrypt(param) {
    let result = CRYPTO.decrypt(
						{'crypt_mode': "aes_cbc"},
						param
					);
	if (result != '') {
		console.log('decrypt success:'+result);
	} else {
		console.log('decrypt failed');
	}
	return result;
}

