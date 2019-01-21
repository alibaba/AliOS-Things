/*
 * SHA1 hash implementation and interface functions
 * Copyright (c) 2003-2009, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef SHA1_H
#define SHA1_H



#define SHA1_MAC_LEN 20

int hmac_sha1_vector(const u8 *key, size_t key_len, size_t num_elem,
		     const u8 *addr[], const size_t *len, u8 *mac);
int hmac_sha1(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
	       u8 *mac);
int sha1_prf(const u8 *key, size_t key_len, const char *label,
	     const u8 *data, size_t data_len, u8 *buf, size_t buf_len);
int sha1_t_prf(const u8 *key, size_t key_len, const char *label,
	       const u8 *seed, size_t seed_len, u8 *buf, size_t buf_len);
int tls_prf(const u8 *secret, size_t secret_len,
			 const char *label, const u8 *seed, size_t seed_len,
			 u8 *out, size_t outlen);
int pbkdf2_sha1(const char *passphrase, const char *ssid, size_t ssid_len,
		int iterations, u8 *buf, size_t buflen);
int pbkdf2_sha1_flatten(const char *passphrase, const char *ssid, size_t ssid_len,
		int iterations, u8 *buf, size_t buflen);
int exec_hmac_sha1_vector(u8 (*k_pad[2])[64], size_t num_elem,
                          const u8 *addr[], const size_t *len, u8 *mac);
int prepare_hmac_sha1_vector(const u8 *key, size_t key_len, u8 (*k_pad[2])[64]);

// For PSK's repeat loop.
typedef struct __HMAC_SHA1_LOOP_DATA {
    u8 *mac; // result;
} HMAC_SHA1_LOOP_DATA_S;

size_t get_hmac_sha1_loop_data_size (void);

#ifdef ENABLE_DYNAMIC_HMAC_SHA1_BUF
int init_hmac_sha1_loop (HMAC_SHA1_LOOP_DATA_S *hmac_sha1_loop_data, 
                         const u8 *pass, u32 passphrase_len, size_t num_elem,
                         const u8 *addr[], const size_t *len);
#else // ENABLE_DYNAMIC_HMAC_SHA1_BUF
HMAC_SHA1_LOOP_DATA_S *init_hmac_sha1_loop (const u8 *key, size_t key_len, 
                                            size_t num_elem,
                                            const u8 *addr[], const size_t *len);
#endif // ENABLE_DYNAMIC_HMAC_SHA1_BUF

int loop_hmac_sha1 (HMAC_SHA1_LOOP_DATA_S *sha1_data);
int stop_hmac_sha1_loop (HMAC_SHA1_LOOP_DATA_S *sha1_data);

#if 1 // def ENABLE_BACKGROUND_PMK_CALC
typedef struct _PMK_CALC_DATA_S {
    union {
    u8      pmk[SHA1_MAC_LEN*2]; // PMK requires two PBKDF2 operations.
    u32     pmk_u32_block[2][SHA1_MAC_LEN/4];
    };
    void   *pbkdf2_sha1_data;
    void   *custom_data;
    
    #ifdef PROFILE_SUPPLICANT
    Time_T  pmk_begin_time;
    #endif // PROFILE_SUPPLICANT
} PMK_CALC_DATA_S;

PMK_CALC_DATA_S *init_pbkdf2_sha1_calc (const u8 *passphrase, const u8 *ssid,
                                        u32 ssid_len, u32 iterations);
void reinit_hmac_sha1_loop (HMAC_SHA1_LOOP_DATA_S *sha1_data,  size_t num_elem,
                            const u8 *addr[], const size_t *len);
int pbkdf2_sha1_calc_func (PMK_CALC_DATA_S * pmk_calc_data, u32 iterations);
void deinit_pbkdf2_sha1_calc (PMK_CALC_DATA_S * pmk_calc_data);
#endif // ENABLE_BACKGROUND_PMK_CALC
extern int sha1_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac);
extern int sha1_simple_vector(u8 data[][64], u32 block_num, u8 *mac);

#endif /* SHA1_H */

