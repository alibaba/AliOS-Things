#ifndef _DNS_H_
#define _DNS_H_

#define DNS_RESULT_COUNT             (3)
int dns_getaddrinfo(char *domain, char *ip[DNS_RESULT_COUNT]);

#endif