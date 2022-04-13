/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_security.h
 * @brief:   vendor security interface
 * @author:  
 * @date:    2019/12/16
 * @version: 1.0
 */
#include "tg_security.h"
#include "tg_ota.h"
#include "hal_trace.h"
#include "secure_boot/mincrypt/sha256.h"
#include "hal_norflash.h"
#include "boot_struct/tool_msg.h"
#include "cmsis.h"
#include "pmu.h"

#define HASH_SECURE_CHECK_ENABLE
#define NORFLASH_OTP_LOCK_ENABLE

/**
 * @brief Get current secure boot status
 *
 * @returns 0: secure boot is not enabled; 1: secure boot enabled
 */
int tg_secure_get_secboot_status(void)
{
    return 1;
}

void hex_to_str( char *src, char *dst, int src_en )
{
    int num = 0;
    char szTmp[3];

    for( num = 0; num < src_en; num ++ )
    {
        sprintf( szTmp, "%02x", (unsigned char) src[num] );
        memcpy( &dst[num * 2], szTmp, 2 );
    }
}

/**
 * @brief check secure boot root public key hash
 * @read pub_otp.bin from OTP flash, computer hash and compare with pubkey_hash.
 * @param[in] pubkey_hash: The hash of root public key
 * @returns 0: succeed; -1: failed
 */
int tg_secure_set_secboot_hash(const char* pubkey_hash)
{
    int32_t ret = 0;
	uint32_t lock = 0;
	int32_t num = 0;
	int32_t pub_otp_len = 0;
    unsigned char k_hash[SHA256_DIGEST_SIZE] = {0};
	char hash_string[SHA256_DIGEST_SIZE*2 + 2] = {0};
	struct sec_key_struct_t ota_bes_sec_key;
	unsigned char pub_otp[1024] = {0}; //pub_otp.bin length short than 1024
	unsigned char pub_otp_default_header[PUB_OPT_HEADER_LEN] = {0xff, 0xff, 0xff, 0xff};
	unsigned short efuse = 0;

	TRACE("%s %d, pubkey_hash:%s\r\n", __func__, __LINE__, pubkey_hash);

#ifdef HASH_SECURE_CHECK_ENABLE
	lock = int_lock();
	ret = hal_norflash_security_register_read(HAL_NORFLASH_ID_0, 0, (unsigned char *)&ota_bes_sec_key, sizeof(ota_bes_sec_key));
	if (ret) {
		TRACE("error hal_norflash_security_register_read return: %d", ret);
		TRACE("secureboot disable\r\n");
		return -1;
	}
	int_unlock(lock);

	memcpy(pub_otp, (unsigned char *)&ota_bes_sec_key, sizeof(ota_bes_sec_key));
	memcpy(pub_otp, pub_otp_default_header, PUB_OPT_HEADER_LEN);
	pub_otp_len = sizeof(ota_bes_sec_key) + PUB_OPT_END_LEN; //pub_otp.bin end of 00,00,00,00

	printf_raw("\n");
	printf_raw("pub_otp.bin file data:\n");
	for (num = 0; num < pub_otp_len; num ++) {
		printf_raw("%02x ", *(pub_otp + num));
		if ((num + 1) % 16 == 0) {
			printf_raw("\n");
		}
	}
	printf_raw("\n");

	SHA256_hash((void *)pub_otp, pub_otp_len, k_hash);
	hex_to_str(k_hash, hash_string, SHA256_DIGEST_SIZE);

	printf_raw("pub_otp.bin hash data:\n");
	for (num = 0; num < SHA256_DIGEST_SIZE; num ++) {
		printf_raw("%02x ", *(k_hash + num));
		if ((num + 1) % 16 == 0) {
			printf_raw("\n");
		}
	}

	TRACE("%s %d, pubkey_hash:%s", __func__, __LINE__, pubkey_hash);
	TRACE("%s %d, hash_string:%s", __func__, __LINE__, hash_string);

	if (strcmp(hash_string, pubkey_hash)) {
		TRACE("%s %d, hash_string != pubkey_hash", __func__, __LINE__);
		TRACE("secureboot disable\r\n");
        return -1;
	}

	TRACE("%s %d, hash_string == pubkey_hash", __func__, __LINE__);
#endif

	//copy ota_boot1_sec.bin to ota_boot1.bin zone
	ret = ota_adapt_copy_partitionA_to_partitionB(HAL_PARTITION_BOOTLOADER, HAL_PARTITION_OTA_TEMP);
	if (ret) {
		TRACE("error %s %d, ret:%d\r\n", __func__, __LINE__, ret);
		TRACE("secureboot disable\r\n");
		return -1;
	}

	//write efuse page0 to 0xb001
	ret = pmu_get_efuse(PMU_EFUSE_PAGE_SECURITY, &efuse);
	TRACE("%s %d, efuse:0x%x, ret:%d\r\n", __func__, __LINE__, efuse, ret);
	if (efuse != 0xb001) {
		for (num = 0; num < 3; num ++) {
			ret = pmu_set_security_value_to_efuse();
			ret = pmu_get_efuse(PMU_EFUSE_PAGE_SECURITY, &efuse);
	
			if (efuse == 0xb001) {
				TRACE("%s %d, efuse:0x%x, ret:%d\r\n", __func__, __LINE__, efuse, ret);
				break;
			}
		}
	
		if (num == 3) {
			TRACE("error %s %d, efuse:0x%x, ret:%d\r\n", __func__, __LINE__, efuse, ret);
			TRACE("secureboot disable\r\n");
			return -1;
		}
	}

#ifdef NORFLASH_OTP_LOCK_ENABLE
	lock = int_lock();
	for (num = 0; num < 3; num ++) {
		ret = hal_norflash_security_register_lock(HAL_NORFLASH_ID_0, 0, 1024);
		if (!ret) {
			break;
		}
	}
	int_unlock(lock);

	if (num == 3) {
		TRACE("%s %d, hal_norflash_security_register_lock fail, ret:%d", __func__, __LINE__, ret);
		TRACE("secureboot disable\r\n");
		return -1;
	}
#endif

	TRACE("secureboot enable\r\n");
	return 0; 
}

/**
 * @brief Set secure boot root public key hash into efuse
 *
 * @param[in] buffer: The buffer to store the hash
 * @param[in] buffer_len: length of the buffer
 * @returns 0: succeed; -1: failed
 */
int tg_secure_get_secboot_hash(char* buffer, int buffer_len)
{
    int32_t ret = 0;
	uint32_t lock = 0;
	int32_t num = 0;
	int32_t pub_otp_len = 0;
    unsigned char k_hash[SHA256_DIGEST_SIZE] = {0};
	char hash_string[SHA256_DIGEST_SIZE*2 + 2] = {0};
	struct sec_key_struct_t ota_bes_sec_key;
	unsigned char pub_otp[1024] = {0}; //pub_otp.bin length short than 1024
	unsigned char pub_otp_default_header[PUB_OPT_HEADER_LEN] = {0xff, 0xff, 0xff, 0xff};
	unsigned short efuse = 0;

	if (buffer_len <= SHA256_DIGEST_SIZE*2) {
		TRACE("error %s %d, buffer_len(%d) <= SHA256_DIGEST_SIZE*2", __func__, __LINE__, buffer_len);
		TRACE("secureboot disable\r\n");
		return -1;
	}

	for (num = 0; num < 3; num ++) {
		ret = pmu_get_efuse(PMU_EFUSE_PAGE_SECURITY, &efuse);

		if (efuse == 0xb001) {
			TRACE("%s %d, efuse:0x%x, ret:%d\r\n", __func__, __LINE__, efuse, ret);
			break;
		}
	}

	if (num == 3) {
		TRACE("%s %d, efuse:0x%x, ret:%d\r\n", __func__, __LINE__, efuse, ret);
		memset(buffer, '0', SHA256_DIGEST_SIZE*2);
		TRACE("secureboot disable\r\n");
		return 0;
	}

	lock = int_lock();
	ret = hal_norflash_security_register_read(HAL_NORFLASH_ID_0, 0, (unsigned char *)&ota_bes_sec_key, sizeof(ota_bes_sec_key));
	if (ret) {
		TRACE("error hal_norflash_security_register_read return: %d", ret);
		TRACE("secureboot disable\r\n");
		return -1;
	}
	int_unlock(lock);

	memcpy(pub_otp, (unsigned char *)&ota_bes_sec_key, sizeof(ota_bes_sec_key));
	memcpy(pub_otp, pub_otp_default_header, PUB_OPT_HEADER_LEN);
	pub_otp_len = sizeof(ota_bes_sec_key) + PUB_OPT_END_LEN; //pub_otp.bin end of 00,00,00,00

	SHA256_hash((void *)pub_otp, pub_otp_len, k_hash);
	hex_to_str(k_hash, hash_string, SHA256_DIGEST_SIZE);

	TRACE("%s %d, hash_string:%s", __func__, __LINE__, hash_string);
	strcpy(buffer, hash_string);
	TRACE("secureboot enable\r\n");

    return ret;
}

