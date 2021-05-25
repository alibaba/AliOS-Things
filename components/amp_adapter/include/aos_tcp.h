/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AOS_TCP_H_
#define _AOS_TCP_H_

/**
 * This function will establish a tcp socket.
 *
 * @param[in]  host  host name of tcp socket connect
 * @param[in]  port  port.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_tcp_establish(const char *host, unsigned int port);

/**
 * This function will destroy a tcp socket.
 *
 * @param[in]  fd  the file descriptor of the file or device.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_tcp_destroy(unsigned int fd);

/**
 * This function will send data with a tcp socket.
 *
 * @param[in]  fd         the file descriptor of the file or device.
 * @param[in]  buf        data buf.
 * @param[in]  len        data length.
 * @param[in]  timeout_ms ms of timeout.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_tcp_write(unsigned int fd, const char *buf, unsigned int len, unsigned int timeout_ms);

/**
 * This function will receive data from a tcp socket.
 *
 * @param[in]  fd         the file descriptor of the file or device.
 * @param[in]  buf        data buf.
 * @param[in]  len        data length.
 * @param[in]  timeout_ms ms of timeout.
 *
 * @return  0: success, otherwise: fail.
 */
int aos_tcp_read(unsigned int fd, char *buf, unsigned int len, unsigned int timeout_ms);


#endif /* _AOS_TCP_H_ */

