 --This example is contributed by Ian--
 
 In order to test this example, you can utilize vlc rtp streaming feature to feed pcmu data for ameba to play audio.
 
 For better wlan performance please make the following modification before compiling this example:
 
 in lwipopts.h, 
			PBUF_POOL_SIZE 20 => 40 (just an example)
			PBUF_POOL_BUFSIZE 500 => 250
			DEFAULT_UDP_RECVMBOX_SIZE 6 => 24 (just an example)			
 in opt.h,			
			MEMP_NUM_NETBUF 2 => 24 (just an example)
			TCPIP_MBOX_SIZE 6 => 6 (just an example)
