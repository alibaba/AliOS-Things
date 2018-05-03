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


#endif