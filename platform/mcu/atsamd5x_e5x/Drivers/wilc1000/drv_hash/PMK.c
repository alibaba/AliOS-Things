/**
 *
 * \file
 *
 * \brief Implementation of HASH algorithms.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "driver/include/m2m_types.h"
#include "drv_hash/crypto.h"

#define A_SHA_DIGEST_LEN                20

uint8 pbkdf2_sha1_f(uint8 *key, uint8 key_length,
		uint8 *data, uint32 data_len,
		uint32 iterations, uint32 count, uint8 *digest);

void Sha_HashFinish(tstrHashContext * pstrHashCxt,uint8 *pu8LengthPadding);

void Sha_HashUpdate(tstrHashContext * pstrHashCxt, uint8 * pu8Data, uint32 u32DataLength);

void SHA1ProcessBlock(uint32* pu32Sha1State, const uint8* pu8MessageBlock);

#define SHA_PAD_BYTE                    0x80

#define F1(x, y, z)                     (((x) & (y)) | (~(x) & (z)))
#define F2(x, y, z)                     (((x) & (z)) | ((y) & ~(z)))
#define F3(x, y, z)                     ((x) ^ (y) ^ (z))
#define F4(x, y, z)                     (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define F5(x, y, z)                     ((y) ^ ((x) | ~(z)))

/** SHA1 specific defines. */
#define SHA1_CH(x, y, z)                F1(x,y,z)
#define SHA1_MAJ(x, y, z)               F4(x,y,z)
#define SHA_PARITY(x, y, z)             F3(x,y,z)
#define SHA1_INERMEDIATE(A,B,C,D,E,W,K)		\
{	\
	W += SHA_ROTL(A,5) + E  + K;	\
	E = D;		\
	D = C;		\
	C = SHA_ROTL(B,30);	\
	B = A;				\
	A = W;				\
}

/** SHA256 specific defines. */
#define SHA256_SMALL_SIGMA0(x)          (SHA_ROTR(x, 7) ^ SHA_ROTR(x,18) ^  SHA_SHR(x, 3))
#define SHA256_SMALL_SIGMA1(x)          (SHA_ROTR(x,17) ^ SHA_ROTR(x,19) ^ SHA_SHR(x,10))
#define SHA256_CAP_SIGMA0(x)            (SHA_ROTR(x, 2) ^ SHA_ROTR(x,13) ^ SHA_ROTR(x,22))
#define SHA256_CAP_SIGMA1(x)            (SHA_ROTR(x, 6) ^ SHA_ROTR(x,11) ^ SHA_ROTR(x,25))
#define SHA256_MAJ(x,y,z)               ((x & y) | (z & (x | y)))
#define SHA256_CH(x,y,z)                (z ^ (x & (y ^ z)))

void SHA1ProcessBlock(uint32* pu32Sha1State, const uint8* pu8MessageBlock)
{
	/* Constants defined in FIPS-180-2, section 4.2.1 */
	const uint32 K[4] = {
		0x5A827999,
		0x6ED9EBA1,
		0x8F1BBCDC,
		0xCA62C1D6
	};
	int t;                  /* Loop counter */
	uint32 W[80];           /* Word sequence */
	uint32 A, B, C, D, E;   /* Word buffers */

	/* Initialize the first 16 words in the array W. */
	for (t = 0; t < 16; t++) 
		W[t] = GETU32(pu8MessageBlock, (t * 4));

	for (t = 16; t < 80; t++)
		W[t] = SHA_ROTL((W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]), 1);

	A = pu32Sha1State[0];
	B = pu32Sha1State[1];
	C = pu32Sha1State[2];
	D = pu32Sha1State[3];
	E = pu32Sha1State[4];

	for (t = 0; t < 20; t++) {
		W[t] += SHA1_CH(B, C, D);
		SHA1_INERMEDIATE(A, B, C, D, E, W[t], K[0])
	}

	for (t = 20; t < 40; t++) {
		W[t] += SHA_PARITY(B, C, D);
		SHA1_INERMEDIATE(A, B, C, D, E, W[t], K[1])
	}

	for (t = 40; t < 60; t++) {
		W[t] += SHA1_MAJ(B, C, D);
		SHA1_INERMEDIATE(A, B, C, D, E, W[t], K[2])
	}

	for (t = 60; t < 80; t++) {
		W[t] += SHA_PARITY(B, C, D);
		SHA1_INERMEDIATE(A, B, C, D, E, W[t], K[3])
	}

	pu32Sha1State[0] += A;
	pu32Sha1State[1] += B;
	pu32Sha1State[2] += C;
	pu32Sha1State[3] += D;
	pu32Sha1State[4] += E;
}

void Sha_HashUpdate(tstrHashContext * pstrHashCxt, uint8 * pu8Data, uint32 u32DataLength)
{
	if((pstrHashCxt != NULL) && (pu8Data != NULL) && (u32DataLength != 0))
	{
		if(pstrHashCxt->fpHash != NULL)
		{
			uint32 	u32ResidualBytes;
			uint32 	u32NBlocks;
			uint32 	u32Offset;
			uint32 	u32BlockCount;

			/* Get the remaining bytes from the previous update (if the length is not block alligned). */
			u32ResidualBytes = pstrHashCxt->u32TotalLength % pstrHashCxt->u8BlockSize;

			/* Update the total data length. */
			pstrHashCxt->u32TotalLength += u32DataLength;

			if(u32ResidualBytes != 0)
			{
				if((u32ResidualBytes + u32DataLength) >= pstrHashCxt->u8BlockSize)
				{
					u32Offset = pstrHashCxt->u8BlockSize - u32ResidualBytes;
					M2M_MEMCPY(&pstrHashCxt->au8CurrentBlock[u32ResidualBytes],pu8Data,u32Offset);
					pu8Data += u32Offset;
					u32DataLength -= u32Offset;
					pstrHashCxt->fpHash(pstrHashCxt->au32HashState, pstrHashCxt->au8CurrentBlock);
				}
				else
				{
					M2M_MEMCPY(&pstrHashCxt->au8CurrentBlock[u32ResidualBytes],pu8Data,u32DataLength);
					u32DataLength = 0;
				}
			}

			/* Get the number of HASH BLOCKs and the residual bytes. */
			u32NBlocks = u32DataLength / pstrHashCxt->u8BlockSize;
			u32ResidualBytes = u32DataLength % pstrHashCxt->u8BlockSize;
			for(u32BlockCount = 0 ; u32BlockCount < u32NBlocks ; u32BlockCount ++)
			{
				pstrHashCxt->fpHash(pstrHashCxt->au32HashState, pu8Data);
				pu8Data += pstrHashCxt->u8BlockSize;
			}
			
			if(u32ResidualBytes != 0)
			{
				M2M_MEMCPY(pstrHashCxt->au8CurrentBlock, pu8Data, u32ResidualBytes);
			}	
		}
	}
}

void Sha_HashFinish(tstrHashContext * pstrHashCxt,uint8 *pu8LengthPadding)
{
	if(pstrHashCxt != NULL)
	{
		if(pstrHashCxt->fpHash != NULL)
		{
			uint32 	u32Offset;
			uint32 	u32PaddingLength;

			/* Calculate the offset of the last data byte in the current block. */
			u32Offset = pstrHashCxt->u32TotalLength % pstrHashCxt->u8BlockSize;

			/* Add the padding byte 0x80. */
			pstrHashCxt->au8CurrentBlock[u32Offset ++] = SHA_PAD_BYTE;

			/* Calculate the required padding to complete one Hash Block Size. */
			u32PaddingLength = pstrHashCxt->u8BlockSize - u32Offset;
			M2M_MEMSET(&pstrHashCxt->au8CurrentBlock[u32Offset],0,u32PaddingLength);

			/* If the padding count is not enough to hold 64-bit representation of */
			/* the total input message length, one padding block is required. */
			if(u32PaddingLength < 8)
			{
				pstrHashCxt->fpHash(pstrHashCxt->au32HashState,pstrHashCxt->au8CurrentBlock);
				M2M_MEMSET(pstrHashCxt->au8CurrentBlock,0,pstrHashCxt->u8BlockSize);
			}
			M2M_MEMCPY(&pstrHashCxt->au8CurrentBlock[pstrHashCxt->u8BlockSize - 8],pu8LengthPadding,8);
			pstrHashCxt->fpHash(pstrHashCxt->au32HashState,pstrHashCxt->au8CurrentBlock);
		}
	}
}

void DRV_SHA1_Hash(tstrHashContext *pstrSha1Context, uint8 u8Flags, uint8 *pu8Data,
		uint32 u32DataLength, uint8 *pu8Digest)
{
	if(pstrSha1Context != NULL)
	{		
		/* SHA1 hash initialization. */
		if(u8Flags & SHA_FLAGS_INIT)
		{
			/* Initialize the Hash Context. */
			M2M_MEMSET((uint8 *)pstrSha1Context, 0, sizeof(tstrHashContext));

			/* Define SHA1 Specific parameters. */
			pstrSha1Context->fpHash = SHA1ProcessBlock;
			pstrSha1Context->u8BlockSize = SHA_BLOCK_SIZE;
			pstrSha1Context->u8DigestSize = SHA1_DIGEST_SIZE;
			
			/* Set the Hash state to the initial value (specified by the standard). */
			pstrSha1Context->au32HashState[0] = 0x67452301;
			pstrSha1Context->au32HashState[1] = 0xEFCDAB89;
			pstrSha1Context->au32HashState[2] = 0x98BADCFE;
			pstrSha1Context->au32HashState[3] = 0x10325476;
			pstrSha1Context->au32HashState[4] = 0xC3D2E1F0;
		}

		/* SHA1 hash update. */
		if((u8Flags & SHA_FLAGS_UPDATE) && (pu8Data != NULL))
		{
			Sha_HashUpdate(pstrSha1Context, pu8Data, u32DataLength);
		}

		/* SHA1 hash finish. */
		if(u8Flags & SHA_FLAGS_FINISH)
		{
			uint8	au8Tmp[8];

			/* Pack the length at the end of the padding block. */
			PUTU32(pstrSha1Context->u32TotalLength >> 29, au8Tmp,0);
			PUTU32(pstrSha1Context->u32TotalLength << 3, au8Tmp, 4);
			Sha_HashFinish(pstrSha1Context,au8Tmp);

			/* Compute digest. */
			PUTU32(pstrSha1Context->au32HashState[0], pu8Digest, 0);
			PUTU32(pstrSha1Context->au32HashState[1], pu8Digest, 4);
			PUTU32(pstrSha1Context->au32HashState[2], pu8Digest, 8);
			PUTU32(pstrSha1Context->au32HashState[3], pu8Digest, 12);
			PUTU32(pstrSha1Context->au32HashState[4], pu8Digest, 16);
		}
	}
}

/**
 * \brief Perform the HMAC operation on a given vector of data streams
 * using the hash function passed to the DRV_HMAC_Vector.
 */
void DRV_HMAC_Vector(tpfHash fpHash, uint8 *pu8Key, uint32 u32KeyLength,
		tstrBuffer *pstrInData, uint8 u8NumInputs, uint8 *pu8Out)
{
	if((pu8Key != NULL) && (pstrInData != NULL) && (pu8Out != NULL) && (fpHash != NULL))
	{
		tstrHashContext	strHashContext;
		uint32 		i;
		uint8			au8TmpBuffer[SHA_BLOCK_SIZE];
		uint8			au8TmpHash[SHA_MAX_DIGEST_SIZE];
		uint8			au8TmpKey[SHA_MAX_DIGEST_SIZE];
		/* HMAC initialize. */
		/* Adjust the key size. */
		if(u32KeyLength > SHA_BLOCK_SIZE)
		{
			fpHash(&strHashContext, SHA_FLAGS_FULL_HASH, pu8Key, u32KeyLength,pu8Key);
			u32KeyLength = strHashContext.u8DigestSize;
			pu8Key = au8TmpKey;
		}
		
		M2M_MEMSET(au8TmpBuffer, 0x36, SHA_BLOCK_SIZE);
		for(i = 0; i< u32KeyLength ; i++)
		{
			au8TmpBuffer[i] ^= pu8Key[i];
		}

		/* Start hashing. */
		fpHash(&strHashContext , SHA_FLAGS_INIT | SHA_FLAGS_UPDATE, au8TmpBuffer, SHA_BLOCK_SIZE, NULL);
		
		/* HMAC update. */
		for(i = 0 ; i < u8NumInputs ; i ++)
		{
			fpHash(
				&strHashContext , SHA_FLAGS_UPDATE , 
				pstrInData[i].pu8Data , pstrInData[i].u16BufferSize , NULL);
		}
		
		fpHash(
			&strHashContext , SHA_FLAGS_FINISH, 
			NULL, 0, au8TmpHash);
		
		/* HMAC finalize. */		
		M2M_MEMSET(au8TmpBuffer, 0x5C, SHA_BLOCK_SIZE);
		for(i = 0; i< u32KeyLength ; i++)
		{
			au8TmpBuffer[i] ^= pu8Key[i];
		}

		fpHash(&strHashContext, 	SHA_FLAGS_INIT | SHA_FLAGS_UPDATE, au8TmpBuffer, SHA_BLOCK_SIZE, NULL);
		fpHash(&strHashContext, SHA_FLAGS_UPDATE | SHA_FLAGS_FINISH, au8TmpHash, strHashContext.u8DigestSize, pu8Out);
	}
}

void DRV_hmac_sha1(uint8 *pu8Key, uint32 u32KeyLength, uint8 *pu8Data, uint32 u32DataLength, uint8 *pu8Digest)
{
	tstrBuffer	strIn;
	strIn.pu8Data = pu8Data;
	strIn.u16BufferSize = u32DataLength;
	DRV_HMAC_Vector(DRV_SHA1_Hash,pu8Key,u32KeyLength,&strIn,1,pu8Digest);
}

/**
 * \brief This function iterates the hashing of digest as per the key.
 *
 * \return 1 on success, 0 on failure.
 */
uint8 pbkdf2_sha1_f(uint8 *key, uint8 key_length, uint8 *data, uint32 data_len,
		uint32 iterations, uint32 count, uint8 *digest)
{
    uint8 tmp1[36] = {0};
    uint8 tmp2[A_SHA_DIGEST_LEN] = {0};
    uint16 i, j;
    
    for (i = 0; i < key_length ; i++)
    {
        if (key[i] < 32)
        {           
            return 0;
        }
        else if (key[i] > 126)
        {           
            return 0;
        }
    }


    /* U1 = PRF(P, S || int(i)) */
    m2m_memcpy(tmp1, data, data_len);

    tmp1[data_len]   = (uint8)((count>>24) & 0xFF);
    tmp1[data_len+1] = (uint8)((count>>16) & 0xFF);
    tmp1[data_len+2] = (uint8)((count>>8) & 0xFF);
    tmp1[data_len+3] = (uint8)(count & 0xFF);

    DRV_hmac_sha1(key, key_length, tmp1, (data_len + 4), tmp2);


    /* digest = U1 */
    m2m_memcpy(digest, tmp2, A_SHA_DIGEST_LEN);

    for (i = 1; i < iterations; i++)
    {
        /* Un = PRF(P, Un-1) */
        DRV_hmac_sha1(key, key_length, tmp2, A_SHA_DIGEST_LEN, tmp1);

        m2m_memcpy(tmp2, tmp1, A_SHA_DIGEST_LEN);

        /* digest = digest xor Un */
        for (j = 0; j < A_SHA_DIGEST_LEN; j++)
        {
            digest[j] ^= tmp1[j];
        }
    }   
    return 1;
}

/**
 * \brief This function runs the PBKDF2 on the data with a given key.
 * To obtain the PSK from the password, PBKDF2 is run on SSID (data) with password (key).
 */
uint8 pbkdf2_sha1(uint8 *key, uint8 key_len, uint8 *data, uint8 data_len, uint8 *digest)
{
    if ((key_len > 64) || (data_len > 32))
        return 0;

    if (pbkdf2_sha1_f(key, key_len, data, data_len, 4096, 1, digest) == 0) {
        return 0;
    }
    return pbkdf2_sha1_f(key, key_len, data, data_len, 4096, 2, &digest[A_SHA_DIGEST_LEN]);
}
