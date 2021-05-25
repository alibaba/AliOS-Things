#include "rom_ssl_ram_map.h"
#include <diag.h>

extern struct _rom_ssl_ram_map rom_ssl_ram_map;

//AES HW CRYPTO
extern int rtl_crypto_aes_ecb_init(IN const u8* key, IN const u32 keylen);
extern int rtl_crypto_aes_ecb_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_aes_ecb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_aes_cbc_init(IN const u8* key, 	IN const u32 keylen);
extern int rtl_crypto_aes_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_aes_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);

//DES HW CRYPTO
extern int rtl_crypto_des_cbc_init(IN const u8* key, IN const u32 keylen);
extern int rtl_crypto_des_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_des_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_3des_cbc_init(IN const u8* key, IN const u32 keylen);
extern int rtl_crypto_3des_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_3des_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);

int platform_set_malloc_free( void * (*malloc_func)( size_t ),
                              void (*free_func)( void * ) )
{
	/* OS interface */
	rom_ssl_ram_map.ssl_malloc = malloc_func;
	rom_ssl_ram_map.ssl_free = free_func;
	rom_ssl_ram_map.ssl_printf = (int (*)(char const *, ...))DiagPrintf;

	//AES HW CRYPTO
	rom_ssl_ram_map.hw_crypto_aes_ecb_init = rtl_crypto_aes_ecb_init;
	rom_ssl_ram_map.hw_crypto_aes_ecb_decrypt = rtl_crypto_aes_ecb_decrypt;
	rom_ssl_ram_map.hw_crypto_aes_ecb_encrypt = rtl_crypto_aes_ecb_encrypt;
	rom_ssl_ram_map.hw_crypto_aes_cbc_init = rtl_crypto_aes_cbc_init;
	rom_ssl_ram_map.hw_crypto_aes_cbc_decrypt = rtl_crypto_aes_cbc_decrypt;
	rom_ssl_ram_map.hw_crypto_aes_cbc_encrypt = rtl_crypto_aes_cbc_encrypt;

	//DES HW CRYPTO
	rom_ssl_ram_map.hw_crypto_des_cbc_init = rtl_crypto_des_cbc_init;
	rom_ssl_ram_map.hw_crypto_des_cbc_decrypt = rtl_crypto_des_cbc_decrypt;
	rom_ssl_ram_map.hw_crypto_des_cbc_encrypt = rtl_crypto_des_cbc_encrypt;
	rom_ssl_ram_map.hw_crypto_3des_cbc_init = rtl_crypto_3des_cbc_init;
	rom_ssl_ram_map.hw_crypto_3des_cbc_decrypt = rtl_crypto_3des_cbc_decrypt;
	rom_ssl_ram_map.hw_crypto_3des_cbc_encrypt = rtl_crypto_3des_cbc_encrypt;

	/* Variables */
	rom_ssl_ram_map.use_hw_crypto_func = 1;

	return 0;
}
