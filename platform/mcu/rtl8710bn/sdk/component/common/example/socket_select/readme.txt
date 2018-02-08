LWIP SOCKET SELECT EXAMPLE

Description:
Use socket select() to handle socket read from clients or remote server.

Configuration:
Modify SERVER_PORT definition for listen port of created TCP server.
Can enable CONNECT_REMOTE to include TCP connection to remote server in example. Modify REMOTE_HOST and REMOTE_PORT for remote server.

[platform_opts.h]
    #define CONFIG_EXAMPLE_SOCKET_SELECT    1

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A socket select example thread will be started automatically when booting.
A local TCP server will be started to wait for connection. Can use a TCP client connecting to this server to send data.
If CONNECT_REMOTE is enabed in example. A remote TCP server is required and can send data to the created remote connection.
