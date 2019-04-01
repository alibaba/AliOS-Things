#include <stdio.h>
#include <unistd.h>
#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

#include "joylink_packets.h"
#include "joylink_crypt.h"
#include "joylink_auth_uECC.h"
#include "joylink_auth_crc.h"
#include "joylink.h"
#include "joylink_utils.h"

/**
 * brief: 
 *
 * @Param: p_in
 * @Param: in_len
 * @Param: key
 * @Param: key_type
 * @Param: p_out
 * @Param: maxlen
 *
 * @Returns: 
 */
int 
joylink_dencrypt_sub_dev_data(const uint8_t *p_in, int in_len, 
        const uint8_t *key, int key_type,
        uint8_t* p_out, int maxlen)
{
	int ret_len = 0;
    switch(key_type){
        case ET_NOTHING:
            break;
        case ET_ACCESSKEYAES:
            ret_len = device_aes_decrypt(key, 16, key+16, 
                    p_in, in_len, p_out, maxlen);
            break;
        case ET_ECDH:
        default:
            break;
    }
	return ret_len;
}

/**
 * brief: 
 *
 * @Param: pBuf
 * @Param: buflen
 * @Param: enctype
 * @Param: key
 * @Param: payload
 * @Param: length
 *
 * @Returns: 
 */
int 
joylink_encrypt_sub_dev_data(
        uint8_t* pBuf, 
        int buflen, 
        E_EncType enctype, 
        uint8_t* key, 
        const uint8_t* payload, 
        int length)
{
	char* psJson = (char*)payload;
	uint8_t* pOut = pBuf;
    int retlen = 0;
    uint8_t peerPubKey[uECC_BYTES * 2] = {0};
    uint8_t secret[uECC_BYTES] = {0};

	switch (enctype)
	{
	case ET_NOTHING:
		break;
	case ET_PSKAES:
		break;
	case ET_ACCESSKEYAES:
		retlen = device_aes_encrypt(
                key, 
                16, 
                key+16, 
                (uint8_t*)psJson, 
                length, 
                pOut,
                length + 16);
		break;
	case ET_ECDH:
		memcpy(pOut, __g_ekey.devPubKeyC, uECC_BYTES + 1);
		pOut += (uECC_BYTES + 1);
		uECC_decompress(key, peerPubKey);

		uECC_shared_secret(
                peerPubKey, 
                __g_ekey.priKey, 
                secret);

		retlen = device_aes_encrypt(
                secret,
                16, 
                key + 4, 
                (const uint8_t*)psJson, 
                length, 
                pOut, 
                length + 16);
		break;
	default:
		break;
	}

	return retlen;
}

/**
 * brief: 
 *
 * @Param: pBuf
 * @Param: buflen
 * @Param: enctype
 * @Param: cmd
 * @Param: key
 * @Param: payload
 * @Param: length
 *
 * @Returns: 
 */
int joylink_encrypt_lan_basic(
        uint8_t* pBuf, 
        int buflen, 
        E_EncType enctype, 
        E_PacketType cmd, 
        uint8_t* key, 
        const uint8_t* payload, 
        int length)
{
	JLPacketHead_t head = {
		.magic = 0x123455BB,
		.optlen = 0,
		.payloadlen = 0,
		.version = 1,
		.type = (char)cmd,
		.total = 0,
		.index = 0,
		.enctype = (char)enctype,
		.reserved = 0,
		.crc = 0		// Todo:У??CRC
	};

	char* psJson = (char*)payload;
	uint8_t* pOut = pBuf + sizeof(JLPacketHead_t);
    uint8_t peerPubKey[uECC_BYTES * 2] = {0};
	uint8_t secret[uECC_BYTES] = {0};

	switch (enctype)
	{
	case ET_NOTHING:
		memcpy(pOut, psJson, length);
		pOut += length;
		head.optlen = 0;
		head.payloadlen = (uint16_t)length;
		break;
	case ET_PSKAES:
		memcpy(pOut, psJson, length);
		pOut += length;
		head.optlen = 0;
		head.payloadlen = length;
		break;
	case ET_ACCESSKEYAES:
		head.optlen = 0;
		length = device_aes_encrypt(
                key, 
                16, 
                key+16, 
                (uint8_t*)psJson, 
                length, 
                pOut,
                length + 16);

		pOut += length;
		head.payloadlen = length;
		break;
	case ET_ECDH:
		memcpy(pOut, __g_ekey.devPubKeyC, uECC_BYTES + 1);
		pOut += (uECC_BYTES + 1);
		head.optlen = (uECC_BYTES + 1);
		uECC_decompress(key, peerPubKey);

		uECC_shared_secret(
                peerPubKey, 
                __g_ekey.priKey, 
                secret);

		length = device_aes_encrypt(
                secret,
                16, 
                key + 4, 
                (const uint8_t*)psJson, 
                length, 
                pOut, 
                length + 16);

		head.payloadlen = length;
		pOut += length;
		break;
	default:
		break;
	}

	length = pOut - pBuf;
	head.crc = CRC16(pBuf + sizeof(JLPacketHead_t), length - sizeof(JLPacketHead_t));
	memcpy(pBuf, &head, sizeof(head));

	return length;
}

/**
 * brief: 
 *
 * @Param: pBuf
 * @Param: buflen
 * @Param: cmd
 * @Param: key
 * @Param: payload
 * @Param: payloadLen
 *
 * @Returns: 
 */
int joylink_encypt_server_rsp(
        uint8_t* pBuf, 
        int buflen, 
        E_PacketType cmd, 
        uint8_t* key, 
        const uint8_t* payload, 
        int payloadLen)
{
	JLPacketHead_t head = {
		.magic = 0x123455CC,
		.optlen = 0,
		.payloadlen = 0,
		.version = 1,
		.type = (char)cmd,
		.total = 0,
		.index = 0,
		.enctype = (char)1,
		.reserved = 0,
		.crc = 0		// Todo:У??CRC
	};

	int len = 0;
	uint8_t* pOut = pBuf + sizeof(JLPacketHead_t);
    int feedid_len;
    JLAuth_t *ap = NULL;

	switch (cmd)
	{
	case PT_AUTH:
		head.enctype = ET_ACCESSKEYAES;
        ap = (JLAuth_t *)payload;

        feedid_len = strlen(_g_pdev->jlp.feedid);
        log_debug("feedid:%s:len:%d", _g_pdev->jlp.feedid, feedid_len);

		head.optlen = feedid_len + 4;
		memcpy(pOut, _g_pdev->jlp.feedid, feedid_len);
		pOut += feedid_len;

		memcpy(pOut, &ap->random_unm, 4);
		pOut += 4;

		len = device_aes_encrypt(
                key, 16, 
                key + 16, 
                (uint8_t*)payload, 
                payloadLen, 
                pOut, JL_MAX_PACKET_LEN);

		head.payloadlen = len;
		pOut += len;

		break;

	default:
		head.enctype = ET_SESSIONKEYAES;
		head.optlen = 0;
		len = device_aes_encrypt(
                key, 
                16, 
                key + 16, 
                (uint8_t*)payload, 
                payloadLen, 
                pOut, 
                JL_MAX_PACKET_LEN);

		head.payloadlen = len;
		pOut += len;
		break;
	}

	payloadLen = pOut - pBuf;
	head.crc = CRC16(pBuf + sizeof(JLPacketHead_t), payloadLen - sizeof(JLPacketHead_t));
	memcpy(pBuf, &head, sizeof(head));

	return payloadLen;
}

/**
 * brief: 
 *
 * @Param: pParam
 * @Param: pIn
 * @Param: length
 * @Param: pOut
 * @Param: maxlen
 *
 * @Returns: 
 */
int 
joylink_dencypt_server_req(
        JLPacketParam_t *pParam, 
        const uint8_t *pIn, 
        int length, 
        uint8_t* pOut, int maxlen)
{
	int ret = 0;
	int retLen = 0;
	uint8_t* p = NULL;
	JLPacketHead_t* pPack = (JLPacketHead_t*)pIn;

	if (0x123455CC != pPack->magic){
		return 0;
    }

	switch (pPack->type)
	{
	case PT_AUTH:
		pParam->version = 1;
		pParam->type = pPack->type;
		p = (uint8_t*)pIn + sizeof(JLPacketHead_t)+pPack->optlen;

		retLen = device_aes_decrypt(
                (const uint8_t*)_g_pdev->jlp.accesskey, 
                16, 
                (const uint8_t*)&_g_pdev->jlp.accesskey[0] + 16, 
                p,
                pPack->payloadlen, 
                p, 
                maxlen);

		if (retLen>0){
			ret = retLen;
			memcpy(pOut, p, retLen);
			pPack->payloadlen = retLen;
		}
		break;

	default:
		pParam->version = 1;
		pParam->type = pPack->type;
		p = (uint8_t*)pIn + sizeof(JLPacketHead_t)+pPack->optlen;

		retLen = device_aes_decrypt(
                _g_pdev->jlp.sessionKey, 
                16, 
                &_g_pdev->jlp.sessionKey[0] + 16, 
                p, 
                pPack->payloadlen, 
                p, 
                maxlen);

		if (retLen > 0){
			ret = retLen;
			memcpy(pOut, p, retLen);
			pPack->payloadlen = retLen;
		}
		break;
	}

	return ret;
}

/**
 * brief: 
 *
 * @Param: cmd
 * @Param: enctype
 *
 * @Returns: 
 */
int 
joylink_is_cmd_enctype_match(int cmd, int enctype)
{
    switch (cmd){
        case PT_SCAN:
            if(enctype == ET_NOTHING){
                return 1;
            }
            break;
        case PT_WRITE_ACCESSKEY:
            if(enctype == ET_ECDH){
                return 1;
            }
            break;
        case PT_JSONCONTROL:
        case PT_SCRIPTCONTROL:
        case PT_SUB_AUTH:
        case PT_SUB_LAN_JSON:
        case PT_SUB_LAN_SCRIPT:
        case PT_SUB_ADD:
            if(enctype == ET_ACCESSKEYAES){
                return 1;
            }
            break;
    }

    return 0;
}

/**
 * brief: 
 *
 * @Param: pParam
 * @Param: pIn
 * @Param: length
 * @Param: pOut
 * @Param: maxlen
 *
 * @Returns: 
 */
int 
joylink_dencypt_lan_req(JLPacketParam_t *pParam, const uint8_t *pIn, 
        int length, uint8_t* pOut, int maxlen)
{
	int retLen = 0;
    char* pJsonStr;
    uint8_t devPubKey[uECC_BYTES * 2];
    uint8_t secret[uECC_BYTES];
	JLCommonHeader_t* pCommon = (JLCommonHeader_t*)pIn;
	if (0x123455bb == pCommon->magic){
		JLPacketHead_t* pPack = (JLPacketHead_t*)pIn;
		if ((length != (pPack->optlen + pPack->payloadlen + sizeof(JLPacketHead_t))) ||
			(pPack->payloadlen > JL_MAX_PACKET_LEN)){
			return 0;
        }

        if(!joylink_is_cmd_enctype_match(pPack->type, pPack->enctype)){
            return 0;
        }

		pParam->version = pPack->version;
		switch (pPack->enctype)
		{
		case ET_NOTHING:
            pParam->type = pPack->type;
            pJsonStr = (char*)pIn + sizeof(JLPacketHead_t)+pPack->optlen;
            retLen = pPack->payloadlen;
            memcpy(pOut, pJsonStr, retLen);
            break;
		case ET_ACCESSKEYAES:
            pJsonStr = (char*)pIn + sizeof(JLPacketHead_t)+pPack->optlen;
            retLen = device_aes_decrypt((const uint8_t*)_g_pdev->jlp.localkey, 
                    16, 
                    (const uint8_t*)&_g_pdev->jlp.localkey[0]+ 16, 
                    pIn + sizeof(JLPacketHead_t)+pPack->optlen, 
                    pPack->payloadlen, 
                    (uint8_t*)pJsonStr, 
                    maxlen);

            pParam->type = pPack->type;
            memcpy(pOut, pJsonStr, retLen);
            break;
		case ET_ECDH:
            uECC_decompress(pIn + sizeof(JLPacketHead_t), devPubKey);
            uECC_shared_secret(devPubKey, __g_ekey.priKey, secret);
            pJsonStr = (char*)pIn + sizeof(JLPacketHead_t)+pPack->optlen;// malloc(pPack->payloadlen);
            retLen = device_aes_decrypt(secret,
                    16,
                    secret+4,
                    pIn + sizeof(JLPacketHead_t)+pPack->optlen,
                    pPack->payloadlen,
                    (uint8_t*)pJsonStr,
                    maxlen);

            pParam->type = pPack->type;
            memcpy(pOut, pJsonStr, retLen);
            break;

		default:
			break;
		}
	}

	return retLen;
}
