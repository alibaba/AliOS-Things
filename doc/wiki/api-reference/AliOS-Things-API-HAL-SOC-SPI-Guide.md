# API INDEX

  * [1 hal_spi_init](#1-hal_spi_init)
  * [2 hal_spi_send](#2-hal_spi_send)
  * [3 hal_spi_recv](#3-hal_spi_recv)
  * [4 hal_spi_send_recv](#4-hal_spi_send_recv)
  * [5 hal_spi_finalize](#5-hal_spi_finalize)

------

## 1-hal_spi_init

```c
int32_t hal_spi_init(spi_dev_t *spi)
```

- [x] **Description**

  Initialises the SPI interface for a given SPI device

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | spi | the spi device |

- [x] **Returns**

  0 : on success, EIO : if the SPI device could not be initialised

## 2-hal_spi_send

```c
int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
```

- [x] **Description**

  Spi send

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | spi |     the spi device   |
  | [in] | data |    spi send data   |
  | [in] | size |    spi send data size   |
  | [in] | timeout | timeout in ms |

- [x] **Returns**

  0 : on success, EIO : if the SPI device could not be initialised

## 3-hal_spi_recv

```c
int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
```

- [x] **Description**

  spi_recv

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  spi |     the spi device   |
  | [out] | data |    spi recv data   |
  | [in] |  size |    spi recv data size   |
  | [in] |  timeout | timeout in ms |

- [x] **Returns**

  0 : on success, EIO : if the SPI device could not be initialised

## 4-hal_spi_send_recv

```c
int32_t hal_spi_send_recv(spi_dev_t *spi, const uint8_t *tx_data, uint16_t tx_size,
                          uint8_t *rx_data, uint16_t rx_size, uint32_t timeout)
```

- [x] **Description**

  spi send data and recv

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | spi |     the spi device   |
  | [in] | tx_data | spi send data   |
  | [in] | rx_data | spi recv data   |
  | [in] | tx_size | spi data to be sent   |
  | [in] | rx_size | spi data to be recv   |
  | [in] | timeout | timeout in ms |

- [x] **Returns**

  0, on success;  EIO : if the SPI device could not be initialised

## 5-hal_spi_finalize

```c
int32_t hal_spi_finalize(spi_dev_t *spi)
```

- [x] **Description**

  De-initialises a SPI interface
 

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | spi the SPI device to be de-initialised |

- [x] **Returns**

  0 : on success, EIO : if an error occurred