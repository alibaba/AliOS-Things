#if !defined (CONFIG_PLATFORM_8721D)
#define CONFIG_SSL_RSA          1
#endif

#include "rom_ssl_ram_map.h"
#include "platform_opts.h"
#define RTL_HW_CRYPTO
//#define SUPPORT_HW_SW_CRYPTO

/* RTL_CRYPTO_FRAGMENT should be less than 16000, and should be 16bytes-aligned */
#if defined (CONFIG_PLATFORM_8195A)
#define RTL_CRYPTO_FRAGMENT                4096
#else
#define RTL_CRYPTO_FRAGMENT               15360
#endif

#if defined(CONFIG_SSL_ROM)
#include <section_config.h>
#include "platform_stdlib.h"
#include "mbedtls/config_rom.h"
#define SUPPORT_HW_SW_CRYPTO
#elif defined(CONFIG_BAIDU_DUER) && CONFIG_BAIDU_DUER
#define CONFIG_SSL_RSA          0
#include "baidu_ca_mbedtls_config.h"
#elif defined(CONFIG_SSL_RSA) && CONFIG_SSL_RSA
#include "platform_stdlib.h"
#include "mbedtls/config_rsa.h"
#else
#include "platform_stdlib.h"
#include "mbedtls/config_all.h"
#endif
