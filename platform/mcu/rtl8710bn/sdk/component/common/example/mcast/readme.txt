LWIP MULTICAST EXAMPLE

Description:
Join multicast group of 224.0.0.251 and listen on port 5353. 
Send packet with the content of received packet to multicast group of 224.0.0.251.

Configuration:
[lwipopts.h]
    #define LWIP_UDP     1
    #define LWIP_IGMP    1
[platform_opts.h]
    #define CONFIG_EXAMPLE_MCAST    1
	
Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A multicast example thread will be started automatically when booting.

