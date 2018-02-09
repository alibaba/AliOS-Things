Websocket Client Example

Description:
A simple websocket client example which send "hello" and "world" to server.
The server will reply the message it received.
Once the client received "world", it will disconnect with server.

The SSL websocket server:
	wss://echo.websocket.org
	wss://sandbox.kaazing.net/echo
The websocket server without SSL:
	ws://echo.websocket.org
	ws://sandbox.kaazing.net/echo
	

Configuration:

[platform_opts.h]
	#define CONFIG_EXAMPLE_WEBSOCKET	1


    If using the WSS server:

	[wsclient_api.h]
		#define USING_SSL

	[config_rsa.h]
		#define SSL_MAX_CONTENT_LEN	5120

	[example_wsclient.c]
		wsclient_context *wsclient = create_wsclient("wss://sandbox.kaazing.net", 0, "echo", NULL);
		or
		wsclient_context *wsclient = create_wsclient("wss://echo.websocket.org", 0, NULL, NULL);


    If using the WS server:

	[example_wsclient.c]
		wsclient_context *wsclient = create_wsclient("ws://sandbox.kaazing.net", 0, "echo", NULL);
		or
		wsclient_context *wsclient = create_wsclient("ws://echo.websocket.org", 0, NULL, NULL);


Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A websocket client example thread will be started automatically when booting.
If using other websocekt server, modify the create_wsclient() API and the handle_message() function depending on the condition of the server.

