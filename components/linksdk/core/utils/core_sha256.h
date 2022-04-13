#ifndef _CORE_SHA256_H_
#define _CORE_SHA256_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"

#define CORE_SHA256_DIGEST_LENGTH            (32)
#define CORE_SHA256_BLOCK_LENGTH             (64)
#define CORE_SHA256_SHORT_BLOCK_LENGTH       (CORE_SHA256_BLOCK_LENGTH - 8)
#define CORE_SHA256_DIGEST_STRING_LENGTH     (CORE_SHA256_DIGEST_LENGTH * 2 + 1)

/**
 * \brief          SHA-256 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    uint8_t is224;                  /*!< 0 => SHA-256, else SHA-224 */
} core_sha256_context_t;

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
void core_sha256_init(core_sha256_context_t *ctx);

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
void core_sha256_free(core_sha256_context_t *ctx);


/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 */
void core_sha256_starts(core_sha256_context_t *ctx);

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void core_sha256_update(core_sha256_context_t *ctx, const unsigned char *input, uint32_t ilen);

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-256 checksum result
 */
void core_sha256_finish(core_sha256_context_t *ctx, uint8_t output[32]);

/* Internal use */
void core_sha256_process(core_sha256_context_t *ctx, const unsigned char data[64]);

/**
 * \brief          Output = SHA-256( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-256 checksum result
 */
void core_sha256(const uint8_t *input, uint32_t ilen, uint8_t output[32]);

void core_hmac_sha256(const uint8_t *msg, uint32_t msg_len, const uint8_t *key, uint32_t key_len, uint8_t output[32]);

#if defined(__cplusplus)
}
#endif

#endif

