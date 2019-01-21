/**
 *	@file    cryptoConfig.h
 *	@version 33ef80f (HEAD, tag: MATRIXSSL-3-7-2-OPEN, tag: MATRIXSSL-3-7-2-COMM, origin/master, origin/HEAD, master)
 *
 *	Configuration file for crypto features.
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

#ifndef _h_PS_CRYPTOCONFIG
#define _h_PS_CRYPTOCONFIG

/******************************************************************************/
/* Configurable features */
/******************************************************************************/
/*
	Enable psTraceCrypto family of APIs for debugging the crypto module
*/
/* #define USE_CRYPTO_TRACE */


/******************************************************************************/
/*
	Public-Key Algorithms and performance settings
*/
#define USE_RSA /* Enable/Disable RSA */
#define USE_NATIVE_RSA /* Default built-in software support */

/* #define USE_ECC */	/* Enable/Disable ECC */

/* #define USE_DH */	/* Enable Diffie-Hellman */

/*
	Optimize public/private kay operations speed or smaller ram usage.
	Only one may be defined.
	The speed gain for optimizing for speed is around 5%
	The memory savings for optimizing for ram is around 50%
	By default below, these will be enabled on an optimized build that is
		not optimized for size. Eg. for -O[1-3,fast], but not for -Os
*/
#if defined(__OPTIMIZE__)
#if defined(__OPTIMIZE_SIZE__)
#define PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM
#else
#define PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED
#endif /* OPTIMIZE_SIZE */
#endif /* OPTIMIZE */


/******************************************************************************/
/*
	Symmetric bock ciphers (including CBC mode) and performance settings
*/
#define USE_AES /* Enable/Disable AES */
/* #define USE_AES_CTR */ /* Counter Mode.  USE_AES must be enabled */
/* #define USE_AES_GCM */ /* Galois Counter Mode.  USE_AES must be enabled */

/* This is defined if the -maes compiler flag us used on Intel platforms */
#ifdef __AES__
#define USE_AES_CBC_EXTERNAL
#define USE_AES_GCM_EXTERNAL
#define USE_AESNI_CRYPTO
#endif

/* #define USE_3DES */
/* #define USE_DES */
/* #define USE_SEED */
/* #define USE_IDEA */

/*
	Improve block cipher performance, but produce larger code.
	Platforms vary, but ciphers will generally see a 5%-10% performance
		boost at the cost of 10-20 kilobytes (per algorithm).
	By default below, these will be enabled on an optimized build that is
		not optimized for size. Eg. for -O[1-3,fast], but not for -Os
*/
#if defined(__OPTIMIZE__) && !defined(__OPTIMIZE_SIZE__)
#define PS_AES_IMPROVE_PERF_INCREASE_CODESIZE
#define PS_3DES_IMPROVE_PERF_INCREASE_CODESIZE
#endif /* OPTIMIZE */

/******************************************************************************/
/*
	Symmetric stream ciphers
	@security These are generally deemed insecure and not enabled by default.
*/
 #define USE_ARC4 
 #define USE_RC2    /* Only PKCS#12 parse should ever want this algorithm */

/******************************************************************************/
/*
	Digest algorithms
	@note SHA256 and above are used with TLS 1.2, and also used for 
	certificate signatures on some certificates regardless of TLS version.
	@security MD5 is deprecated, but still required in combination with SHA-1
	for TLS handshakes before TLS 1.2, meaning that the strength is at least
	that of SHA-1 in this usage. The only other usage of MD5 by TLS is for
	certificate signatures and MD5 based cipher suites. Both of which are
	disabled at compile time by default.
*/
#define USE_SHA1
#define USE_MD5				/* Required for < TLS 1.2 Handshake */
/* #define USE_SHA224 */	/* Must enable SHA256 to use */
#define USE_SHA256			/* Required for >= TLS 1.2 */
#define USE_SHA384			/* Must enable SHA512 to use */
#define USE_SHA512
/* #define USE_MD2 */		/* @security INSECURE */
/* #define USE_MD4 */ 		/* @security INSECURE */

#define USE_HMAC /* Requires USE_MD5 and/or USE_SHA1 */

/*
	Improve hashing performance, but produce larger code.
	Platforms vary, but digests will generally see a 5%-10% performance
		boost at the cost of 1-10 kilobytes (per algorithm).
	By default below, these will be enabled on an optimized build that is
		not optimized for size. Eg. for -O[1-3,fast], but not for -Os
*/
#if defined(__OPTIMIZE__) && !defined(__OPTIMIZE_SIZE__)
#define PS_MD5_IMPROVE_PERF_INCREASE_CODESIZE
#define PS_SHA1_IMPROVE_PERF_INCREASE_CODESIZE
#endif /* OPTIMIZE */

/******************************************************************************/
/*
	X.509 Certificate and DH Params
*/
#define USE_X509
#define ENABLE_MD5_SIGNED_CERTS /* Accept MD5 signed certs if enabled */
#define USE_CERT_PARSE /* Usually required.  USE_X509 must be enabled */
#define USE_FULL_CERT_PARSE /* USE_CERT_PARSE must be enabled */
/* #define USE_CRL */ /* Must define USE_FULL_CERT_PARSE */
#define USE_BASE64_DECODE

/******************************************************************************/
/*
	Minimum supported key sizes in bits.
	@security Weaker keys will be rejected.
*/
#define MIN_RSA_BITS	1024
#define MIN_ECC_BITS	192
#define MIN_DH_BITS		1024

/******************************************************************************/
/*
	PKCS support
*/
#define USE_PRIVATE_KEY_PARSING
/* #define USE_PKCS5 */		/* v2.0 PBKDF encrypted priv keys, reqs USE_3DES */
/* #define USE_PKCS8 */		/* Alternative private key storage format */
/* #define USE_PKCS12 */	/* Requires USE_PKCS8 */

/******************************************************************************/
/*
  	PRNG Algorithms
	@security By default the OS PRNG will be used directly.
*/
/* #define USE_YARROW */

/******************************************************************************/
/*
	All below here are configurable tweaks (do not need to touch, in general)
*/
/*
	USE_1024_KEY_SPEED_OPTIMIZATIONS
	USE_2048_KEY_SPEED_OPTIMIZATIONS
	Optimizations for 1024/2048 bit key size multiplication and squaring math.
	The library size can increase significantly if enabled
	By default below, these will be enabled on an optimized build that is
		not optimized for size. Eg. for -O[1-3,fast], but not for -Os
*/
#if defined(__OPTIMIZE__) && !defined(__OPTIMIZE_SIZE__)
#define USE_1024_KEY_SPEED_OPTIMIZATIONS
#define USE_2048_KEY_SPEED_OPTIMIZATIONS
#endif /* OPTIMIZE */

/* @security Zero the stack of functions operating on secret data */
//#define USE_BURN_STACK


#endif /* _h_PS_CRYPTOCONFIG */
/******************************************************************************/

