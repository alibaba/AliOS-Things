#include "tls.h"

//#define MBEDTLS_CUSTOM_CA

#if defined(MBEDTLS_CUSTOM_CA)
#define CUSTOM_CA_CRT_RSA                                                 \
"-----BEGIN CERTIFICATE-----\r\n"                                       \
"MIICtDCCAZwCAQAwDQYJKoZIhvcNAQELBQAwIDEeMBwGA1UEAxMVQ2VydGlmaWNh\r\n"  \
"dGUgQXV0aG9yaXR5MB4XDTE3MDcxOTExMzU1NVoXDTIyMDcxODExMzU1NVowIDEe\r\n"  \
"MBwGA1UEAxMVQ2VydGlmaWNhdGUgQXV0aG9yaXR5MIIBIjANBgkqhkiG9w0BAQEF\r\n"  \
"AAOCAQ8AMIIBCgKCAQEA3FPtvNnMiETM5qprK4625nf8z39HnM5pdkyjW3a4JWt4\r\n"  \
"RTuLv6x7b56OnJttZPL0hYyVwGsxt3LeuoXD3pBlLn61iwUK6Fb4NX5Xo04LaKl7\r\n"  \
"gQFQ+lENPPSu/JdzcERly0d1JYIQ4Z11732yCdblf5oZJ/0skUzhTVCusWnYvY3Z\r\n"  \
"xs/O3wjvbuCucxgZoyDv6AZ8ZQ0xKZ3JKjm8URD4yrRYEkQ+gBeTkNC3nVFJ/u8X\r\n"  \
"nrNT/qzhhI6HS8Lf88dkQu3W5gvIVVy5qv49hqpWGXwbEkrIsMgC9OJCZ5qoo17Y\r\n"  \
"iUj/SBH6xVA9ikaFOlVeH9rD1euzwgjIm+X2Wu7S5wIDAQABMA0GCSqGSIb3DQEB\r\n"  \
"CwUAA4IBAQCt9EYWA2vTVJmEajB87MzHHvjTV/cTWRGLKnLoBHL3OKf+Lembmu6Q\r\n"  \
"YxoN8OCqRjrcvh0sgQ1H6jpfmVSIoLKoT9BVy16t5PZ8x0XSSrMlXQKz+pAuOZBc\r\n"  \
"sinSoRPDMr0M92m2CAnJ8mIpr6o0lTtWJfY1xeuT2+LbFzvaI6dtWnQYmN1mxPqA\r\n"  \
"JLhAlQhCqmRiDhgFPfKSwKsmEPdUtrA2InOsgxDGa0utawK2BWgQc6hkhT9uZ4dF\r\n"  \
"DxLkNG8w4QFnHXcm8pdpg5zbO7GSrtPRs2CU2fMpE79CMPKSH3ErxV2F4aPtHFP5\r\n"  \
"sV1Ai+iyFoUKCzjW4iUDTJux2gUTzpmH\r\n"  \
"-----END CERTIFICATE-----\r\n"


/* Concatenation of all available CA certificates */
const char mbedtls_custom_cas_pem[] = CUSTOM_CA_CRT_RSA;
const size_t mbedtls_custom_cas_pem_len = sizeof( mbedtls_custom_cas_pem );
#endif
