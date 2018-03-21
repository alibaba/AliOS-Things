HTTPC EXAMPLE

Description:
Based on HTTPC API, an HTTP/HTTPS client example to access httpbin.org for test are provided

Configuration for HTTP: None
Configuration for HTTPS to httpbin.org (TLS-ECDHE-RSA-WITH-AES-256-CBC-SHA384):
[config_rsa.h]
	#define POLARSSL_KEY_EXCHANGE_ECDHE_RSA_ENABLED
	#define POLARSSL_ECDH_C
	#define POLARSSL_ECP_C
	#define SSL_MAX_CONTENT_LEN          (8 * 1024)
[platform_opts.h]
	#define CONFIG_EXAMPLE_HTTPC         1

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A httpc example thread will be started automatically when booting.
GET to http://httpbin.org/get and POST to https://httpbin.org/post will be verified.
Should link PolarSSL bignum.c to SRAM to speed up SSL handshake for HTTPS client.