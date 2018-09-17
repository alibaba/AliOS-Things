/************************************************************************/
/**	\file	http_test.c
 *	\brief	Simple HTTP server test
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include "http_test.h"
#include "lwip/sockets.h"
#include "lwip/stats.h"
#include "lwip/netif.h"
#include <stdarg.h>

/* Macros */

/* Types */

/* Externals */

/* Variables */

#pragma default_section(ALLDATA, "sdram0")
char http_buffer[2048];
char* http_url;

const char http_error_400[] = "HTTP/1.1 400 Bad Request\r\n\r\n";
const char http_error_404[] = "HTTP/1.1 404 Not Found\r\n\r\n";
const char http_error_405[] = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
const char http_ok_nocache[] =	"HTTP/1.1 200 OK\r\n"
								"Cache-Control: no-store, no-cache\r\n";
const char http_content_type_html[] = "Content-Type: text/html\r\n\r\n";
const char http_content_length[] = "Content-Length: %u\r\n";
const char http_content_type_bin[] = "Content-Type: application/octet-stream\r\n\r\n";

const char html_index[] =
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\r\n"
"<html>\r\n"
"<head>\r\n"
"<title>LwIP blackfin platform test</title>\r\n"
"</head>\r\n"
"<body>\r\n"
"<h2>Hello LwIP world</h2>\r\n"
"Run wget http://%s/speedtest.bin for a simple TCP speed test.<br>\r\n"
"You can also right-click this <a href=\"/speedtest.bin\">link</a> and save with a fast browser.<br>\r\n"
"The 300 MHz BF536 is able to push 11.3 MByte/s of TCP useful payload.<br>\r\n"
"TCP MSS is 1460 bytes -> 1500 bytes MTU + 7 preamble + 1 start + 2*6 addresses + 2 type + 4 CRC + 12 intergap = 1538 bytes on the wire.<br>\r\n"
"11.3*1024*1024*8*1538/1460 = 99.85 Mbit/s, so the physical link is saturated.<br>\r\n"
"<br>\r\n"
"You can also try netio -t %s for TCP RX speed test.<br>\r\n"
"<br>\r\n"
"And don't forget to test SNMP.\r\n";

const char html_net_table[] =
"<h3>LwIP %s statistics</h3>\r\n"
"<table>\r\n";

const char html_net_sys_header[] =
"<tr><td></td><td>used</td><td>max</td><td>err</td></tr>\r\n";

const char html_net_sys_data[] =
"<tr><td>%s:</td><td>%d</td><td>%d</td><td>%d</td></tr>\r\n";

const char html_net_mem_header[] =
"<tr><td></td><td>available</td><td>used</td><td>max</td><td>err</td><td>illegal</td></tr>\r\n";

const char html_net_mem_data[] =
"<tr><td>%s:</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\r\n";

const char html_net_proto_header[] =
"<tr><td></td><td>xmit</td><td>recv</td><td>fw</td><td>drop</td>"
"<td>chkerr</td><td>lenerr</td><td>memerr</td><td>rterr</td><td>proterr</td><td>opterr</td><td>err</td></tr>\r\n";

const char html_net_proto_data[] =
"<tr><td>%s:</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\r\n";

const char html_net_igmp_header[] =
"<tr><td>xmit</td><td>recv</td><td>drop</td>"
"<td>chkerr</td><td>lenerr</td><td>memerr</td><td>proterr</td>"
"<td>rx_v1</td><td>rx_general</td><td>rx_group</td><td>rx_report</td>"
"<td>tx_join</td><td>tx_leave</td><td>tx_report</td></tr>\r\n";

const char html_net_igmp_data[] =
"<tr><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td>"
"<td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\r\n";

const char html_net_table_end[] =
"</table>\r\n";

const char html_end[] =
"</body>\r\n"
"</html>\r\n";

static const char *http_memp_desc[MEMP_MAX] = {
#define LWIP_MEMPOOL(name,num,size,desc)  (desc),
#include "lwip/memp_std.h"
};

/* Functions */

char* strnstr(const char* s1, size_t n, const char* s2)
{
	const char *p1, *p2;
	int i, j, l = strlen(s2);
	for (i=0; i<=n-l; i++)
	{
		if (*s1 == *s2)
		{
			p1 = s1;
			p2 = s2;
			j = 0;
			while (*p1++ == *p2++)
			{
				j++;
				if (j==l)	return (char*)s1;
			}
		}
		s1++;
	}
	return NULL;
}

int http_download(int s)
{
	int n;
	char* ptr = http_buffer;
	char* headerend = 0;
	int length = 0;
	do
	{
		if (length >= sizeof(http_buffer))	return 400;

		int r = read(s, ptr, sizeof(http_buffer)-length);
    	if (r<=0)	return 400;
		ptr += r;
		length += r;
		headerend = strnstr(http_buffer, length, "\r\n\r\n");
	}
	while (!headerend);
	*headerend = 0;

	ptr = strtok(http_buffer, " ");
	if (strcmp(ptr, "GET"))	return 405;

	http_url = strtok(0, " ");
	return 200;
}

void http_write(int s, const char* str)
{
	write(s, str, strlen(str));
}

void http_format(int s, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int n = vsprintf(http_buffer, format, args);
	va_end(args);
	write(s, http_buffer, n);
}

void http_speedtest_bin(int s)
{
	int i;
	http_write(s, http_ok_nocache);
	http_format(s, http_content_length, 71821*1460);
	http_write(s, http_content_type_bin);
	for (i=0; i<71821; i++)
	{
		int err = write(s, http_buffer, 1460);
		if (err <= 0)	break;
	}
}

void http_index_html(int s)
{
	int p;

	http_write(s, http_ok_nocache);
	http_write(s, http_content_type_html);
	const char* a = ipaddr_ntoa(&netif_default->ip_addr);
	http_format(s, html_index, a, a);

	#if SYS_STATS
	http_format(s, html_net_table, "System");
	http_write(s, html_net_sys_header);
	http_format(s, html_net_sys_data, "Semaphore",	lwip_stats.sys.sem.used, lwip_stats.sys.sem.max, lwip_stats.sys.sem.err);
	http_format(s, html_net_sys_data, "Mutex",		lwip_stats.sys.mutex.used, lwip_stats.sys.mutex.max, lwip_stats.sys.mutex.err);
	http_format(s, html_net_sys_data, "Mailbox",	lwip_stats.sys.mbox.used, lwip_stats.sys.mbox.max, lwip_stats.sys.mbox.err);
	http_write(s, html_net_table_end);
	#endif
	
	#if MEM_STATS || MEMP_STATS
	http_format(s, html_net_table, "Memory");
	http_write(s, html_net_mem_header);
	#if MEM_STATS
	http_format(s, html_net_mem_data, "MEM",		lwip_stats.mem.avail, lwip_stats.mem.used, lwip_stats.mem.max, lwip_stats.mem.err, lwip_stats.mem.illegal);
	#endif
	#if MEMP_STATS
	for (p=0; p<MEMP_MAX; p++)
		http_format(s, html_net_mem_data, http_memp_desc[p],	lwip_stats.memp[p].avail, lwip_stats.memp[p].used, lwip_stats.memp[p].max, lwip_stats.memp[p].err, lwip_stats.memp[p].illegal);
	#endif
	http_write(s, html_net_table_end);
	#endif

	http_format(s, html_net_table, "Protocol");
	http_write(s, html_net_proto_header);
	#if LINK_STATS
	http_format(s, html_net_proto_data, "LINK",	lwip_stats.link.xmit, lwip_stats.link.recv, lwip_stats.link.fw, lwip_stats.link.drop,
											lwip_stats.link.chkerr, lwip_stats.link.lenerr, lwip_stats.link.memerr, lwip_stats.link.rterr, lwip_stats.link.proterr, lwip_stats.link.opterr, lwip_stats.link.err);
	#endif
	#if ETHARP_STATS
	http_format(s, html_net_proto_data, "ARP",	lwip_stats.etharp.xmit, lwip_stats.etharp.recv, lwip_stats.etharp.fw, lwip_stats.etharp.drop,
											lwip_stats.etharp.chkerr, lwip_stats.etharp.lenerr, lwip_stats.etharp.memerr, lwip_stats.etharp.rterr, lwip_stats.etharp.proterr, lwip_stats.etharp.opterr, lwip_stats.etharp.err);
	#endif
	#if IP_STATS
	http_format(s, html_net_proto_data, "IP",	lwip_stats.ip.xmit, lwip_stats.ip.recv, lwip_stats.ip.fw, lwip_stats.ip.drop,
											lwip_stats.ip.chkerr, lwip_stats.ip.lenerr, lwip_stats.ip.memerr, lwip_stats.ip.rterr, lwip_stats.ip.proterr, lwip_stats.ip.opterr, lwip_stats.ip.err);
	#endif
	#if ICMP_STATS
	http_format(s, html_net_proto_data, "ICMP",	lwip_stats.icmp.xmit, lwip_stats.icmp.recv, lwip_stats.icmp.fw, lwip_stats.icmp.drop,
											lwip_stats.icmp.chkerr, lwip_stats.icmp.lenerr, lwip_stats.icmp.memerr, lwip_stats.icmp.rterr, lwip_stats.icmp.proterr, lwip_stats.icmp.opterr, lwip_stats.icmp.err);
	#endif
	#if UDP_STATS
	http_format(s, html_net_proto_data, "UDP",	lwip_stats.udp.xmit, lwip_stats.udp.recv, lwip_stats.udp.fw, lwip_stats.udp.drop,
											lwip_stats.udp.chkerr, lwip_stats.udp.lenerr, lwip_stats.udp.memerr, lwip_stats.udp.rterr, lwip_stats.udp.proterr, lwip_stats.udp.opterr, lwip_stats.udp.err);
	#endif
	#if TCP_STATS
	http_format(s, html_net_proto_data, "TCP",	lwip_stats.tcp.xmit, lwip_stats.tcp.recv, lwip_stats.tcp.fw, lwip_stats.tcp.drop,
											lwip_stats.tcp.chkerr, lwip_stats.tcp.lenerr, lwip_stats.tcp.memerr, lwip_stats.tcp.rterr, lwip_stats.tcp.proterr, lwip_stats.tcp.opterr, lwip_stats.tcp.err);
	#endif
	http_write(s, html_net_table_end);

	#if IGMP_STATS
	http_format(s, html_net_table, "IGMP");
	http_write(s, html_net_igmp_header);
	http_format(s, html_net_igmp_data,	lwip_stats.igmp.xmit, lwip_stats.igmp.recv, lwip_stats.igmp.drop,
									lwip_stats.igmp.chkerr, lwip_stats.igmp.lenerr, lwip_stats.igmp.memerr, lwip_stats.igmp.proterr,
									lwip_stats.igmp.rx_v1, lwip_stats.igmp.rx_general, lwip_stats.igmp.rx_group, lwip_stats.igmp.rx_report,
									lwip_stats.igmp.tx_join, lwip_stats.igmp.tx_leave, lwip_stats.igmp.tx_report);
	http_write(s, html_net_table_end);
	#endif

	http_write(s, html_end);
}

void http_request(int s)
{
	// Download request
	int err = http_download(s);

	// Manage error or return content
   	switch (err)
	{
		case 400:
		{
			http_write(s, http_error_400);
			break;
		}
		case 405:
		{
			http_write(s, http_error_405);
			break;
		}
		case 200:
		{
			if (strcmp(http_url, "/") == 0)
			{
				http_index_html(s);
			}
			else if (strcmp(http_url, "/speedtest.bin") == 0)
			{
				http_speedtest_bin(s);
			}
			else
			{
    			http_write(s, http_error_404);
			}
			break;
		}
	}

	// Close the connection
	close(s);
}

void httpd()
{
 	// Bind TCP socket on port 80
	int listensocket = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(80);
	bind(listensocket, (struct sockaddr *)&sa, sizeof(sa));

	// Listen
	listen(listensocket, 2);

	for (;;)
	{
		// Wait for new connection
		socklen_t addr_size = sizeof(sa);
	    int s = accept(listensocket, (struct sockaddr *)&sa, &addr_size);
	    if (s<0)	continue;

		// Handle request
	    http_request(s);
	}
}
