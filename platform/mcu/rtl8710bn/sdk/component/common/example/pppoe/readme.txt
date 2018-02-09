LWIP PPPOE EXAMPLE

Description:
Example for lwip pppoe connection.

Configuration:
[lwipopts.h]
    #define PPP_SUPPORT                     1
    #define PPPOE_SUPPORT                   1
    #define PPPOS_SUPPORT                   0
    #define PAP_SUPPORT                     1
    #define CHAP_SUPPORT                    1
    #define MD5_SUPPORT                     1
[platform_opts.h]
    #define CONFIG_EXAMPLE_PPPOE            1

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A PPPOE example thread will be started automatically when booting.

Setup PPPOE_USER and PPPOE_PASSWD in example_pppoe.c for authentication. PPP addresses will be shown after PPP is connected
