# API INDEX

  * [1 mbedtls_ssl_connect](#1-mbedtls_ssl_connect)
  * [2 mbedtls_ssl_send](#2-mbedtls_ssl_send)
  * [3 mbedtls_ssl_recv](#3-mbedtls_ssl_recv)
  * [4 mbedtls_ssl_close](#4-mbedtls_ssl_close)

------

## 1-mbedtls_ssl_connect

```c
void *mbedtls_ssl_connect(void *tcp_fd, const char *ca_cert, int ca_cert_len)
```

- [x] **Description**

  Create a ssl connect.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | tcp_fd |      handle of the tcp socket.   |
  | [in] | ca_cert |     CA.   |
  | [in] | ca_cert_len | length of the CA. |

- [x] **Returns**

  NULL: error; ssl_param: success.

## 2-mbedtls_ssl_send

```c
int mbedtls_ssl_send(void *ssl, const char *buffer, int length)
```

- [x] **Description**

  Send data through ssl.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | ssl |    handle of the ssl.   |
  | [in] | buffer | data to send.   |
  | [in] | length | length of the data. |

- [x] **Returns**

  -1: error; others: length of the data be sended.

## 3-mbedtls_ssl_recv

```c
int mbedtls_ssl_recv(void *ssl, char *buffer, int length)
```

- [x] **Description**

  Recv data through ssl.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | ssl |    handle of the ssl.   |
  | [in] | buffer | buffer to recv data.   |
  | [in] | length | the max size of the buffer. |

- [x] **Returns**

  -1: error; 0: EOF; others: length of the data be sended.

## 4-mbedtls_ssl_close

```c
int mbedtls_ssl_close(void *ssl)
```

- [x] **Description**

  Close the ssl.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | ssl | handle to be closed. |

- [x] **Returns**

  0: success.
