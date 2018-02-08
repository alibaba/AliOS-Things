#define CONFIG_SSL_RSA          1

#include "rom_ssl_ram_map.h"
#define RTL_HW_CRYPTO
//#define SUPPORT_HW_SW_CRYPTO
#define RTL_CRYPTO_FRAGMENT               15360 /* 15*1024 < 16000 */

#if CONFIG_SSL_RSA
#include "platform_stdlib.h"
#include "mbedtls/config_rsa.h"
#else
#include "platform_stdlib.h"
#include "mbedtls/config_all.h"
#endif