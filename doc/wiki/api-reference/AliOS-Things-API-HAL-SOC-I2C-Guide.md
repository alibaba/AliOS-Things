# API INDEX

  * [1 hal_i2c_init](#1-hal_i2c_init)
  * [2 hal_i2c_master_send](#2-hal_i2c_master_send)
  * [3 hal_i2c_master_recv](#3-hal_i2c_master_recv)
  * [4 hal_i2C_slave_send](#4-hal_i2C_slave_send)
  * [5 hal_i2c_slave_recv](#5-hal_i2c_slave_recv)
  * [6 hal_i2c_mem_write](#6-hal_i2c_mem_write)
  * [7 hal_i2c_mem_read](#7-hal_i2c_mem_read)
  * [8 hal_i2c_finalize](#8-hal_i2c_finalize)

------

## 1-hal_i2c_init

```c
int32_t hal_i2c_init(i2c_dev_t *i2c)
```

- [x] **Description**

  Initialises an I2C interface
  Prepares an I2C hardware interface for communication as a master or slave

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | i2c | the device for which the i2c port should be initialised |

- [x] **Returns**

  0 : on success, EIO : if an error occurred during initialisation

## 2-hal_i2c_master_send

```c
int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
```

- [x] **Description**

  I2c master send

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | i2c |      the i2c device   |
  | [in] | dev_addr | device address   |
  | [in] | data |     i2c send data   |
  | [in] | size |     i2c send data size   |
  | [in] | timeout |  timeout in ms |

- [x] **Returns**

  0 : on success, EIO : if an error occurred during initialisation

## 3-hal_i2c_master_recv

```c
int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
```

- [x] **Description**

  I2c master recv

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  i2c |      the i2c device   |
  | [in] |  dev_addr | device address   |
  | [out] | data |     i2c receive data   |
  | [in] |  size |     i2c receive data size   |
  | [in] |  timeout |  timeout in ms |

- [x] **Returns**

  0 : on success, EIO : if an error occurred during initialisation

## 4-hal_i2C_slave_send

```c
int32_t hal_i2C_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
```

- [x] **Description**

  I2c slave send

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | i2c |     the i2c device   |
  | [in] | data |    i2c slave send data   |
  | [in] | size |    i2c slave send data size   |
  | [in] | timeout | timeout in ms |

- [x] **Returns**

  0 : on success, EIO : if an error occurred during initialisation

## 5-hal_i2c_slave_recv

```c
int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
```

- [x] **Description**

  I2c slave receive

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  i2c |     tthe i2c device   |
  | [out] | data |    i2c slave receive data   |
  | [in] |  size |    i2c slave receive data size   |
  | [in] |  timeout | timeout in ms |

- [x] **Returns**

  0 : on success, EIO : if an error occurred during initialisation

## 6-hal_i2c_mem_write

```c
int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, const uint8_t *data,
                          uint16_t size, uint32_t timeout)
```

- [x] **Description**

  I2c mem write

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | i2c the i2c device |  |
  | [in] | dev_addr |      device address   |
  | [in] | mem_addr |      mem address   |
  | [in] | mem_addr_size | mem address   |
  | [in] | datai2c master send data |  |
  | [in] | sizei2c master send data size |  |
  | [in] | timeout |       timeout in ms |

- [x] **Returns**

  0 : on success, EIO : if an error occurred during initialisation

## 7-hal_i2c_mem_read

```c
int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, uint8_t *data,
                         uint16_t size, uint32_t timeout)
```

- [x] **Description**

  I2c master mem read

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  i2c the i2c device |  |
  | [in] |  dev_addr |      device address   |
  | [in] |  mem_addr |      mem address   |
  | [in] |  mem_addr_size | mem address   |
  | [out] | datai2c master send data |  |
  | [in] |  sizei2c master send data size |  |
  | [in] |  timeout |       timeout in ms |

- [x] **Returns**

  0 : on success, EIO : if an error occurred during initialisation

## 8-hal_i2c_finalize

```c
int32_t hal_i2c_finalize(i2c_dev_t *i2c)
```

- [x] **Description**

  Deinitialises an I2C device

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | i2c | the i2c device |

- [x] **Returns**

  0 : on success, EIO : if an error occurred during deinitialisation