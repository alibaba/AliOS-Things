/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AOS_SOCKET_H_
#define _AOS_SOCKET_H_

#include <sys/socket.h>
#include "netdb.h"

/**
 * @brief get socket errno.
 *
 * @param[in] NULL
 *
 * @return  socket errno
 */
int aos_socket_errno(void);

/**
 * @brief creates an endpoint for communication and returns a descriptor.
 *
 * @param[in] domain
 * @param[in] type
 * @param[in] protocol
 *
 * @return  On success, a file descriptor for the new socket is returned.
 *          On error, -1 is returned, and errno is set appropriately
 */
int aos_socket_open(int domain, int type, int protocol);

/**
 * @brief transmit data to socket.
 *
 * @param[in] sockfd
 * @param[in] data
 * @param[in] size
 * @param[in] flags
 *
 * @return  On success, these calls return the number of bytes sent.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_send(int sockfd, const void *data, size_t size, int flags);

/**
 * @brief receive data from a socket.
 *
 * @param[in] sockfd
 * @param[in] mem
 * @param[in] len
 * @param[in] flags
 *
 * @return  the number of bytes received, or -1 if an error occurred.
 */
int aos_socket_recv(int sockfd, void *mem, size_t len, int flags);

/**
 * @brief write data to socket.
 *
 * @param[in] sockfd
 * @param[in] data
 * @param[in] size
 *
 * @return  On success, these calls return the number of bytes sent.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_write(int sockfd, const void *data, size_t size);

/**
 * @brief read data from a socket.
 *
 * @param[in] sockfd
 * @param[in] data
 * @param[in] len
 *
 * @return  the number of bytes received, or -1 if an error occurred.
 */
int aos_socket_read(int sockfd, void *data, size_t len);

/**
 * @brief transmit data to socket.
 *
 * @param[in] sockfd
 * @param[in] data
 * @param[in] size
 * @param[in] flags
 * @param[in] to
 * @param[in] tolen
 *
 * @return  On success, these calls return the number of bytes sent.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_sendto(int sockfd, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);

/**
 * @brief receive data from a socket.
 *
 * @param[in] sockfd
 * @param[in] data
 * @param[in] len
 * @param[in] flags
 * @param[in] from
 * @param[in] fromlenq
 *
 * @return  the number of bytes received, or -1 if an error occurred.
 */
int aos_socket_recvfrom(int socket, void *data, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);

/**
 * @brief manipulate options for the socket referred to by the file descriptor sockfd.
 *
 * @param[in] sockfd
 * @param[in] level
 * @param[in] optname
 * @param[in] optval
 * @param[in] optlen
 *
 * @return  On success, zero is returned.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

/**
 * @brief manipulate options for the socket referred to by the file descriptor sockfd.
 *
 * @param[in] sockfd
 * @param[in] level
 * @param[in] optname
 * @param[in] optval
 * @param[in] optlen
 *
 * @return  On success, zero is returned.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);

/**
 * @brief  connect the socket referred to by the file descriptor sockfd to the address specified by addr.
 *
 * @param[in] sockfd
 * @param[in] addr
 * @param[in] addrlen
 *
 * @return  On success, zero is returned.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 * @brief  assigns the address specified by addr to the socket referred to by the file descriptor sockfd.
 *
 * @param[in] sockfd
 * @param[in] addr
 * @param[in] addrlen
 *
 * @return  On success, zero is returned.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 * @brief  marks the socket referred to by sockfd as a passive socket.
 *
 * @param[in] sockfd
 * @param[in] backlog
 *
 * @return  On success, zero is returned.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_listen(int sockfd, int backlog);

/**
 * @brief  accept the socket referred to by sockfd.
 *
 * @param[in] sockfd
 * @param[in] addr
 * @param[in] addrlen
 *
 * @return  On success, zero is returned.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/**
 * @brief  Monitor multiple file descriptors, waiting until one or more of the file descriptors become "ready" for some class of I/O operatio.
 *
 * @param[in] maxfdp1
 * @param[in] readset
 * @param[in] writeset
 * @param[in] exceptset
 * @param[in] timeout
 *
 * @return  On success, zero is returned.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);

/**
 * @brief  Close the socket referred to by sockfd.
 *
 * @param[in] sockfd
 *
 * @return  On success, zero is returned.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_close(int sockfd);

/**
 * @brief  Shutdown the socket referred to by sockfd.
 *
 * @param[in] sockfd
 * @param[in] how
 *
 * @return  On success, zero is returned.
 *          On error, -1 is returned, and errno is set appropriately.
 */
int aos_socket_shutdown(int socket, int how);

#endif /* _AOS_SOCKET_H_ */
