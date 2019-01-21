/**
 *	@file    symmetric.h
 *	@version 33ef80f (HEAD, tag: MATRIXSSL-3-7-2-OPEN, tag: MATRIXSSL-3-7-2-COMM, origin/master, origin/HEAD, master)
 *
 *	Header for internal symmetric key cryptography support.
 */
/*
 *	Copyright (c) 2013-2015 INSIDE Secure Corporation
 *	Copyright (c) PeerSec Networks, 2002-2011
 *	All Rights Reserved
 *
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software
 *	into proprietary programs.  If you are unable to comply with the GPL, a
 *	commercial license for this software may be purchased from INSIDE at
 *	http://www.insidesecure.com/eng/Company/Locations
 *
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *	See the GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#ifndef _h_PS_SYMMETRIC
#define _h_PS_SYMMETRIC

/******************************************************************************/
#ifdef USE_AES
/******************************************************************************/


#ifndef USE_AES_CBC_EXTERNAL
typedef struct {
	uint32 eK[64], dK[64];
	int32 Nr;
} psAesKey_t;

typedef struct {
	int32			blocklen;
	unsigned char	IV[16];
	psAesKey_t		key;
#if defined(USE_AES_GCM) || defined(USE_AES_CCM)
	unsigned char	EncCtr[16];
	unsigned char	CtrBlock[16];
#endif
#ifdef USE_AES_GCM
	unsigned char	gInit[16];
	uint32			TagTemp[4];
	unsigned char	Hash_SubKey[16];
	uint32			ProcessedBitCount[4];
	uint32			InputBufferCount;
	uint32			OutputBufferCount;
	union
	{
		unsigned char Buffer[128];
		uint32 BufferAlignment;
	} Input;
#endif /* USE_AES_GCM */
#ifdef USE_AES_CCM
	uint32_t ccmTagTemp[16 / sizeof(uint32_t)]; /* 32 */
	union
	{
		/* Used for formatting IV. */
		uint8_t Temporary[16];
		/* Used for processing Mac. */
		uint8_t Y0[16];
	} u; /* 48 */
#endif /* USE_AES_CCM */
} psAesCipher_t;
#endif /* USE_AES_CBC_EXTERNAL */

#endif /* USE_AES */

#ifdef USE_IDEA
#define SSL_IDEA_KEY_LEN	16
#define SSL_IDEA_IV_LEN		8
#define SSL_IDEA_BLOCK_LEN	8

typedef struct {
	uint16	key_schedule[52];
} psIdeaKey_t;

typedef struct {
	psIdeaKey_t		key;
	uint32			IV[2];
	short			for_encryption;
	short			inverted;
} idea_CBC;
#endif
/******************************************************************************/

/******************************************************************************/
#ifdef USE_SEED
/******************************************************************************/
#define SSL_SEED_KEY_LEN	16
#define SSL_SEED_IV_LEN		16


typedef struct {
	uint32 K[32], dK[32];
} psSeedKey_t;

typedef struct {
	int32			blocklen;
	unsigned char	IV[16];
	psSeedKey_t		key;
} seed_CBC;

#endif /* USE_SEED */
/******************************************************************************/

/******************************************************************************/
#if defined(USE_3DES) || defined(USE_DES)
/******************************************************************************/
#define DES3_KEY_LEN	24
#define DES3_IV_LEN		8
#define DES_KEY_LEN		8

typedef struct {
	uint32 ek[3][32], dk[3][32];
} psDes3Key_t;

/*
	A block cipher CBC structure
 */
typedef struct {
	int32				blocklen;
	unsigned char		IV[8];
	psDes3Key_t			key;
} des3_CBC;

#endif /* USE_3DES || USE_DES */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_ARC4
typedef struct {
	unsigned char	state[256];
	uint32	byteCount;
	unsigned char	x;
	unsigned char	y;
} psRc4Key_t;
#endif /* USE_ARC4 */
/******************************************************************************/
#ifdef USE_RC2
typedef struct {
	unsigned xkey[64];
} psRc2Key_t;

typedef struct {
	int32				blocklen;
	unsigned char		IV[8];
	psRc2Key_t			key;
} rc2_CBC;
#endif /* USE_RC2 */
/******************************************************************************/
/*	Universal types and defines */
/******************************************************************************/
#define MAXBLOCKSIZE	24

typedef union {
#ifdef USE_RC2
	rc2_CBC		rc2;
#endif
#ifdef USE_ARC4
	psRc4Key_t	arc4;
#endif
#ifdef USE_3DES
	des3_CBC	des3;
#endif
#ifdef USE_AES
	psAesCipher_t	aes;
#endif
#ifdef USE_SEED
	seed_CBC	seed;
#endif
#ifdef USE_IDEA
	idea_CBC	idea;
#endif
} psCipherContext_t;

#define byte(x, n) (((x) >> (8 * (n))) & 255)

#endif /* _h_PS_SYMMETRIC */
/******************************************************************************/
