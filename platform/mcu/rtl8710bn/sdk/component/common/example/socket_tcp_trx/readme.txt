LWIP SOCKET TCP TX/RX EXAMPLE

Description:
Example of TCP bidirectional transmission with use two threads for TCP tx/rx on one socket.
Example 1 uses non-blocking recv and semaphore for TCP send/recv mutex
Example 2 does not use any synchronization mechanism, but can only run correctly on lwip with TCPIP thread msg api patch

Configuration:
Modify SERVER_PORT in example_socket_tcp_trx.c for listen port

[platform_opts.h]
Run example 1 in example_socket_tcp_trx_1.c
	#define CONFIG_EXAMPLE_SOCKET_TCP_TRX    1

Run example 2 in example_socket_tcp_trx_2.c
	#define CONFIG_EXAMPLE_SOCKET_TCP_TRX    2

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A socket TCP trx example thread will be started automatically when booting.
A TCP server will be started to wait for connection.
Can use a TCP client connecting to this server to start a TCP bidirectional transmission