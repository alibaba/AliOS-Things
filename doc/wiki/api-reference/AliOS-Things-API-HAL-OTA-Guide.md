# API INDEX

  * [1 hal_ota_register_module](#1-hal_ota_register_module)
  * [2 hal_ota_init](#2-hal_ota_init)
  * [3 hal_ota_write](#3-hal_ota_write)
  * [4 hal_ota_read](#4-hal_ota_read)
  * [5 hal_ota_set_boot](#5-hal_ota_set_boot)
  * [6 hal_ota_get_default_module](#6-hal_ota_get_default_module)

------

## 1-hal_ota_register_module

```c
void hal_ota_register_module(hal_ota_module_t *module)
```

- [x] **Description**

  Arch register a new module before HAL startup

- [x] **Parameters**

  None.

- [x] **Returns**

  None.


## 2-hal_ota_init

```c
hal_stat_t hal_ota_init(void *something)
```

- [x] **Description**

  init ota partition

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  |  | something | extra info for ota init |

- [x] **Returns**

  0 : On success, 1 : If an error occurred with any step

## 3-hal_ota_write

```c
hal_stat_t hal_ota_write(hal_ota_module_t *m, volatile uint32_t *off_set,
                         uint8_t *in_buf , uint32_t in_buf_len)
```

- [x] **Description**

  Write data to an area on ota partition

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  |  | mRefer the ota module which will be used,default module will be used if value is NULL |  |
  |  | off_set |    Point to the start address that the data is written to, andpoint to the last unwritten address after this function isreturned, so you can call this function serval times withoutupdate this start address.   |
  |  | inbuf |      point to the data buffer that will be written to flash   |
  |  | in_buf_len | The length of the buffe |

- [x] **Returns**

  0 : On success, 1 : If an error occurred with any step

## 4-hal_ota_read

```c
hal_stat_t hal_ota_read(hal_ota_module_t *m, volatile uint32_t *off_set,
                        uint8_t *out_buf, uint32_t out_buf_len)
```

- [x] **Description**

  Read data from an area on ota Flash to data buffer in RAM

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  |  | m Refer the ota module which will be used,default module will be used if value is NULL |  |
  |  | off_set |     Point to the start address that the data is read, and point to the last unread address after this function is returned, so you can call this function serval times without update this start address.   |
  |  | out_buf |     Point to the data buffer that stores the data read from flash   |
  |  | out_buf_len | The length of the buffe |

- [x] **Returns**

  0 : On success, 1 : If an error occurred with any step

## 5-hal_ota_set_boot

```c
hal_stat_t hal_ota_set_boot(hal_ota_module_t *m, void *something)
```

- [x] **Description**

  Set boot options when ota reboot

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  |  | m |         Refer the ota module which will be used,default module will be used if value is NULL   |
  |  | something | boot parms |

- [x] **Returns**

  kNoErr : On success. kGeneralErr : If an error occurred with any step

## 6-hal_ota_get_default_module

```c
hal_ota_module_t *hal_ota_get_default_module(void)
```

- [x] **Description**

  Get the default ota module

- [x] **Parameters**

  None.

- [x] **Returns**

  the first registered ota module ,which is the head of module list
