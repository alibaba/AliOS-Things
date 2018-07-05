# API INDEX

  * [1 hal_uart_init](#1-hal_uart_init)
  * [2 hal_uart_send](#2-hal_uart_send)
  * [3 hal_uart_recv](#3-hal_uart_recv)
  * [4 hal_uart_finalize](#4-hal_uart_finalize)

------

## 1-hal_uart_init

```c
int32_t hal_uart_init(uart_dev_t *uart)
```

- [x] **Description**

  Initialises a UART interface
 

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | uart | the interface which should be initialised |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 2-hal_uart_send

```c
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
```

- [x] **Description**

  Transmit data on a UART interface

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | uart | the UART interface   |
  | [in] | data | pointer to the start of data   |
  | [in] | size | number of bytes to transmit |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 3-hal_uart_recv

```c
int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
```

- [x] **Description**

  Receive data on a UART interface

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  uart |        the UART interface   |
  | [out] | data |        pointer to the buffer which will store incoming data   |
  | [in] |  expect_size | number of bytes to receive   |
  | [out] | recv_size |   number of bytes received   |
  | [in] |  timeout |     timeout in milisecond |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 4-hal_uart_finalize

```c
int32_t hal_uart_finalize(uart_dev_t *uart)
```

- [x] **Description**

  Deinitialises a UART interface

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | uart | the interface which should be deinitialised |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step