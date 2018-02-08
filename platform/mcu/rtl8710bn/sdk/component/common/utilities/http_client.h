#ifndef _HTTP_H_
#define _HTTP_H_

char *http_post_header(char *address, char *resource, char *type, int data_len);
char *http_get_header(char *address, char *resource);
char *http_response_header(char *buf, int response_len);
char *http_response_body(char *buf, int response_len);
void http_free(void *buf);

#endif
