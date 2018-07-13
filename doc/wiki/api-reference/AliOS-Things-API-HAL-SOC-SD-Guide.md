# API INDEX

  * [1 hal_sd_init](#1-hal_sd_init)
  * [2 hal_sd_blks_read](#2-hal_sd_blks_read)
  * [3 hal_sd_blks_write](#3-hal_sd_blks_write)
  * [4 hal_sd_erase](#4-hal_sd_erase)
  * [5 hal_sd_stat_get](#5-hal_sd_stat_get)
  * [6 hal_sd_info_get](#6-hal_sd_info_get)
  * [7 hal_sd_finalize](#7-hal_sd_finalize)

------

## 1-hal_sd_init

```c
int32_t hal_sd_init(sd_dev_t *sd)
```

- [x] **Description**

  Initialises a sd interface

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | sd | the interface which should be initialised |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 2-hal_sd_blks_read

```c
int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                         uint32_t blks, uint32_t timeout)
```

- [x] **Description**

  Read sd blocks

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  sd |       the interface which should be initialised   |
  | [out] | data |     pointer to the buffer which will store incoming data   |
  | [in] |  blk_addr | sd blk addr   |
  | [in] |  blks |     sd blks   |
  | [in] |  timeout |  timeout in milisecond |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 3-hal_sd_blks_write

```c
int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                          uint32_t blks, uint32_t timeout)
```

- [x] **Description**

  Write sd blocks

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | sd |       the interface which should be initialised   |
  | [in] | data |     pointer to the buffer which will store incoming data   |
  | [in] | blk_addr | sd blk addr   |
  | [in] | blks |     sd blks   |
  | [in] | timeout |  timeout in milisecond |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 4-hal_sd_erase

```c
int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
```

- [x] **Description**

  Erase sd blocks

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | sd |  the interface which should be initialised   |
  | [in] | blk_start_addr | sd blocks start addr   |
  | [in] | blk_end_addr |   sd blocks end add |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 5-hal_sd_stat_get

```c
int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
```

- [x] **Description**

  Get sd state

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  sd |   the interface which should be initialised   |
  | [out] | stat | pointer to the buffer which will store incoming dat |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 6-hal_sd_info_get

```c
int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
```

- [x] **Description**

  Get sd info

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  sd |   the interface which should be initialised   |
  | [out] | stat | pointer to the buffer which will store incoming dat |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 7-hal_sd_finalize

```c
int32_t hal_sd_finalize(sd_dev_t *sd)
```

- [x] **Description**

  Deinitialises a sd interface

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | sd | the interface which should be initialised |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step
