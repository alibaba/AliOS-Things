/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef KV_ADAPT_H
#define KV_ADAPT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read KV flash partition
 *
 * @param[in]   offset the offset of the partition
 * @param[out]  buf    the store buffer for read
 * @param[in]   nbytes the length to read
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t kv_flash_read(uint32_t offset, void *buf, uint32_t nbytes);

/**
 * @brief Write KV flash partition
 *
 * @param[in]   offset the offset of the partition
 * @param[in]   buf    the store buffer for write
 * @param[in]   nbytes the length to write
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t kv_flash_write(uint32_t offset, void *buf, uint32_t nbytes);

/**
 * @brief Erase KV flash partition
 *
 * @param[in]  offset the offset of the partition
 * @param[in]  size   the length to erase
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t kv_flash_erase(uint32_t offset, uint32_t size);

/**
 * @brief Create OS lock
 *
 * @return lock pointer for success, NULL is failed
 */
void *kv_lock_create(void);

/**
 * @breif Free OS lock
 *
 * @param[in]  lock pointer to the os lock
 *
 * @return 0 on success, otherwise is failed
 *
 */
int32_t kv_lock_free(void *lock);

/**
 * @brief Lock the lock
 *
 * @param[in]  lock   pointer to the os lock
 *
 * @return 0 on success, otherwise is failed
 *
 */
int32_t kv_lock(void *lock);

/**
 * @brief Unlock the lock
 *
 * @param[in]  lock pointer to the os lock
 *
 * @return 0 on success, otherwise is failed
 *
 */
int32_t kv_unlock(void *lock);

/**
 * @brief Create OS semaphore
 *
 * @return semaphore pointer for success, NULL is failed
 *
 */
void *kv_sem_create(void);

/**
 * @brief Delete OS semaphore
 *
 * @param[in]  sem pointer to the os semaphore
 *
 * @return 0 on success, otherwise is failed
 *
 */
int32_t kv_sem_free(void *sem);

/**
 * @brief Take a semaphore
 *
 * @param[in]  sem     pointer to the semaphore
 *
 * @return 0 on success, otherwise is failed
 *
 */
int32_t kv_sem_wait(void *sem);

/**
 * @brief Give a semaphore and wakeup all waiting task
 *
 * @param[in]  sem pointer to the semaphore
 *
 * @return 0 on success, otherwise is failed
 *
 */
int32_t kv_sem_post_all(void *sem);

/**
 * @brief Create a task
 *
 * @param[in] name name of the task
 * @param[in] fn   the entry function of task
 * @param[in] arg  the parameter of task entry function
 * @param[in] stack the size of task stack
 *
 * @return 0 on success, otherwise is failed
 */
int32_t kv_start_task(const char *name, void (*fn)(void *), void *arg,
                      uint32_t stack);

/**
 * @brief Delete a task
 *
 * @return none
 */
void kv_delete_task(void);

/**
 * @brief wrapper of MM allocation
 *
 * @param[in]  size size of the mem to alloc
 *
 * @return NULL is error, other is memory address
 *
 */
void *kv_malloc(uint32_t size);

/**
 * @brief wrapper of MM free
 *
 * @param[in] ptr address point of the mem
 *
 */
void kv_free(void *ptr);

/**
 * @brief Encrypt data
 *
 * @param[in]  input     the pointer to the input data
 * @param[out] output    the pointer to the encryption result
 * @param[in]  input_len the length of the input data
 *
 * @return 0 on success, otherwise is failed
 */
int32_t kv_secure_encrypt(uint8_t *input, uint8_t *output, uint32_t input_len);

/**
 * @brief Decrypt data
 *
 * @param[in]  input     the pointer to the input data
 * @param[out] output    the pointer to the decryption result
 * @param[in]  input_len the length of the input data
 *
 * @return 0 on success, otherwise is failed
 */
int32_t kv_secure_decrypt(uint8_t *input, uint8_t *output, uint32_t input_len);

/**
 * @brief Get the key for encrypt/decrypt algorithm
 *
 * @param[in] key_len the length of the key
 *
 * @return key buffer pointer, NULL is error
 */
uint8_t *kv_secure_get_key(uint32_t key_len);

/**
 * @brief Get the iv for encrypt/decrypt algorithm
 *
 * @param[in] iv_len the length of the iv
 *
 * @return iv buffer pointer, NULL is error
 */
uint8_t *kv_secure_get_iv(uint32_t iv_len);

#ifdef __cplusplus
}
#endif

#endif  /* KV_ADAPT_H */

