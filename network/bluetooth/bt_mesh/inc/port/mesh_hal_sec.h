/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _MESH_HAL_SEC_H_
#define _MESH_HAL_SEC_H_

#include <stdint.h>

struct bt_mesh_sg {
	const void *data;
	size_t len;
};

typedef void (*bt_mesh_dh_key_cb_t)(const uint8_t key[32]);

/*  @brief Container for public key callback */
struct bt_mesh_pub_key_cb {
        /** @brief Callback type for Public Key generation.
         *
         *  Used to notify of the local public key or that the local key is not
         *  available (either because of a failure to read it or because it is
         *  being regenerated).
         *
         *  @param key The local public key, or NULL in case of no key.
         */
        void (*func)(const uint8_t key[64]);

        struct bt_mesh_pub_key_cb *_next;
};

/** @brief Generate random data.
 *
 *  A random number generation helper which utilizes the Bluetooth
 *  controller's own RNG.
 *
 *  @param buf Buffer to insert the random data
 *  @param len Length of random data to generate
 *
 *  @return Zero on success or error code otherwise, positive in case
 *  of protocol error or negative (POSIX) in case of stack internal error
 */
int bt_mesh_rand(void *buf, size_t len);

/** @brief AES-128 ECB
 *
 *  A function used to perform AES-128 ECB encryption.
 *  See Core Spec V4.2, Vol 3, Part H, Section 2.2.1.
 *
 *  @param key AES key
 *  @param plaintext The data before encryption
 *  @param enc_data The data after encryption
 *
 *  @return 0 on sucess, otherwise negative number
 */
int bt_mesh_aes_encrypt(const uint8_t key[16], const uint8_t plaintext[16],
                        uint8_t enc_data[16]);

/** @brief AES-CMAC
 *
 * Function used to do the AES-CMAC calculation.
 * See Core Spec V4.2, Vol 3, Part H, Section 2.2.5.
 *
 * @param key The 128-bit key
 * @param sg The variable length data to be authenticated,
 *           each data is any entry of bt_mesh_sg type.
 * @param sg_len The number of the entries.
 * @param mac The output buffer.
 *
 * @return 0 on success, otherwise negative number.
 */
int bt_mesh_aes_cmac(const uint8_t key[16], struct bt_mesh_sg *sg,
                     size_t sg_len, uint8_t mac[16]);

/*  @brief Get the current Public Key.
 *
 *  Get the current ECC Public Key.
 *
 *  @return Current key, or NULL if not available.
 */
const uint8_t *bt_mesh_pub_key_get(void);

/*  @brief Calculate a DH Key from a remote Public Key.
 *
 *  Calculate a DH Key from the remote Public Key.
 *
 *  @param remote_pk Remote Public Key.
 *  @param cb Callback to notify the calculated key.
 *
 *  @return Zero on success or negative error code otherwise
 */
int bt_mesh_dh_key_gen(const uint8_t remote_pk[64], bt_mesh_dh_key_cb_t cb);

/*  @brief Generate a new Public Key.
 *
 *  Generate a new ECC Public Key. The callback will persist even after the
 *  key has been generated, and will be used to notify of new generation
 *  processes (NULL as key).
 *
 *  @param cb Callback to notify the new key, or NULL to request an update
 *            without registering any new callback.
 *
 *  @return Zero on success or negative error code otherwise
 */
int bt_mesh_pub_key_gen(struct bt_mesh_pub_key_cb *cb);

#endif /* _MESH_HAL_SEC_H_ */
