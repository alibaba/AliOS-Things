#ifndef _HTTPD_INT_H_
#define _HTTPD_INT_H_

#include "httpd.h"

int httpd_init(void);
void httpd_deinit(void);
void httpd_setup(uint16_t port, uint8_t max_conn, uint8_t secure);
int httpd_return_server_sock(void);
int httpd_accept(int server_sock, struct httpd_conn **client_conn);

int httpd_write(struct httpd_conn *conn, uint8_t *buf, size_t buf_len);
int httpd_read(struct httpd_conn *conn, uint8_t *buf, size_t buf_len);
int httpd_read_with_timeout(struct httpd_conn *conn, uint8_t *buf, uint16_t buf_len, int recv_timeout);

#endif /* _HTTPD_INT_H_ */
