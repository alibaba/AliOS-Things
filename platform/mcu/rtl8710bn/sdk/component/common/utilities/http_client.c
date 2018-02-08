//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include "platform/platform_stdlib.h"

#include "FreeRTOS.h"

const char * http_strstr(const char *str1, const char *str2) {
	char *a, *b;

	/* First scan quickly through the two strings looking for a
	* single-character match.  When it's found, then compare the
	* rest of the substring.
	*/

	b = (char *)str2;
	if (*b == 0) {
		return str1;
	}
	for ( ; *str1 != 0; str1 += 1) {
		if (*str1 != *b) {
			continue;
		}
		a = (char *)str1;
		while (1) {
			if (*b == 0) {
				return str1;
			}
			if (*a++ != *b++) {
				break;
			}
		}
		b = (char *)str2;
	}
	return (char *) 0;
}

static void* http_malloc(unsigned int size)
{
	return pvPortMalloc(size);
}

void http_free(void *buf)
{
	vPortFree(buf);
}

static char *http_itoa(int value)
{
	char *val_str;
	int tmp = value, len = 1;

	while((tmp /= 10) > 0)
		len ++;

	val_str = (char *) http_malloc(len + 1);
	sprintf(val_str, "%d", value);

	return val_str;
}

char *http_post_header(char *host, char *resource, char *type, int data_len)
{
	char *len_str = http_itoa(data_len);
	char *header = (char *) http_malloc(strlen("POST ") + strlen(resource) + strlen(" HTTP/1.1\r\nHost: ") + strlen(host) + 
		strlen("\r\nContent-Type: ") + strlen(type) + strlen("\r\nContent-Length: ") + strlen(len_str) + strlen("\r\n\r\n") + 1);
	sprintf(header, "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Type: %s\r\nContent-Length: %s\r\n\r\n", resource, host, type, len_str);
	http_free(len_str);

	return header;
}

char *http_get_header(char *host, char *resource)
{
	char *header = (char *) http_malloc(strlen("GET ") + strlen(resource) + strlen(" HTTP/1.1\r\nHost: ") + strlen(host) + strlen("\r\n\r\n") + 1);
	sprintf(header, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);

	return header;
}

char *http_response_header(char *buf, int response_len)
{
	char *http_response, *http_header = NULL, *header_end;
	int header_len;

	http_response = (char *) http_malloc(response_len + 1);
	memcpy(http_response, buf, response_len);
	http_response[response_len] = '\0';

	if(strncmp(http_response, "HTTP", 4) == 0) {
		if((header_end = (char *)http_strstr(http_response, "\r\n\r\n")) != NULL) {
			header_end += 4;
			header_len = header_end - http_response;
			http_header = (char *) http_malloc(header_len + 1);
			memcpy(http_header, http_response, header_len);
			http_header[header_len] = '\0';
		}
	}

	http_free(http_response);

	return http_header;
}

char *http_response_body(char *buf, int response_len)
{
	char *http_response, *http_body = NULL, *body_start;
	int body_len;

	http_response = (char *) http_malloc(response_len + 1);
	memcpy(http_response, buf, response_len);
	http_response[response_len] = '\0';

	if(strncmp(http_response, "HTTP", 4) == 0) {
		if((body_start = (char *)http_strstr(http_response, "\r\n\r\n")) != NULL) {
			body_start += 4;
			body_len = http_response + response_len - body_start;

			if(body_len > 0) {
				http_body = (char *) http_malloc(body_len + 1);
				memcpy(http_body, body_start, body_len);
				http_body[body_len] = '\0';
			}

			http_free(http_response);
		}
		else {
			http_body = http_response;
		}
	}
	else {
		http_body = http_response;
	}

	return http_body;
}
