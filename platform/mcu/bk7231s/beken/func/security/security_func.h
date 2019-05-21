
#define USE_SEC_TEST_CMD 0


extern int hal_sha256_self_test( int  );

extern void *aes_encrypt_init(const u8 *key, size_t len);
extern void aes_encrypt(void *ctx, const u8 *plain, u8 *crypt);
extern void aes_encrypt_deinit(void *ctx);
extern void *aes_decrypt_init(const u8 *key, size_t len);
extern void aes_decrypt(void *ctx, const u8 *crypt, u8 *plain);
extern void aes_decrypt_deinit(void *ctx);
