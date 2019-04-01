/**
 *
 * @page ATCMD
 *
 * @section ATCMD_socket Socket AT Command
 * 
 * @subsection ATCMD_socket_1 Create TCP connection.
 *
 * | command     | AT+TCPCONNECT=&lt;RemoteIP&gt;,&lt;Port&gt; |
 * |---------------|-------------------|
 * | param         | &lt;RemoteIP&gt;: The remote IP address for TCP connection<br>&lt;Port&gt;: The port number for TCP connection. |
 * | return value  | AT+NEWTCPCONNECT OK:&lt;socket&gt;<br>+ERROR:error number |
 *
 * @subsection ATCMD_socket_2 Transmit IP data to Internet.
 *
 * | command     | AT+TCPSEND=&lt;socket&gt;,&lt;payload&gt; |
 * |---------------|-------------------|
 * | param         | &lt;socket&gt;: socket index<br>&lt;payload&gt;: Data for sending to remote connection |
 * | return value  | +OK<br>+ERROR:error number |
 *
 * @subsection ATCMD_socket_3 Disconnect TCP connection.
 *
 * | command     | AT+TCPDISCONNECT=&lt;socket&gt; |
 * |---------------|-------------------|
 * | param         | &lt;socket&gt;: socket index |
 * | return value  | +OK<br>+ERROR:error number |
 *
 *
 */
#ifndef ATCMD_SOCKET_H__
#define ATCMD_SOCKET_H__


//TCP
#define ATCMD_TCPCONNECT            "AT+TCPCONNECT"
#define ATCMD_TCPSEND               "AT+TCPSEND"
#define ATCMD_TCPSEND6              "AT+IPV6SEND"
#define ATCMD_TCPDISCONNECT         "AT+TCPDISCONNECT"
#define ATCMD_TCPLISTEN	            "AT+TCPLISTEN"
#define ATCMD_TCPLISTEN6	        "AT+IPV6LISTEN"
#define ATCMD_TCPUNLISTEN           "AT+TCPUNLISTEN"

//SSL
#define ATCMD_SSLTEST               "AT+SSLTEST"
#define ATCMD_SSLCONNECT            "AT+SSLCONNECT"
#define ATCMD_SSLSEND               "AT+SSLSEND"
#define ATCMD_SSLDISCONNECT         "AT+SSLDISCONNECT"

#define ATCMD_UDPCREATE             "AT+UDPCREATE"
#define ATCMD_UDPSEND               "AT+UDPSEND"
#define ATCMD_UDPCLOSE              "AT+UDPCLOSE"

//ota cmd
#define ATCMD_OTASETSERVER          "AT+OTASETSERVER"
#define ATCMD_OTASETPARAM           "AT+OTASETPARAM"
#define ATCMD_OTASTART              "AT+OTASTART"

#define ATCMD_NSLOOKUP              "AT+NSLOOKUP"

#define ATCMD_IPERF                     "iperf"

//jd cloud cmd
#define ATCMD_JD_DEVICE_REMOVE              "AT+JD_DEVICE_REMOVE"
#define ATCMD_JD_START              "AT+JD_START"


#endif
