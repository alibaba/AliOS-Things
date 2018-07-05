# API INDEX

  * [1 hal_wdg_init](#1-hal_wdg_init)
  * [2 hal_wdg_reload](#2-hal_wdg_reload)
  * [3 hal_wdg_finalize](#3-hal_wdg_finalize)

------

## 1-hal_wdg_init

```c
int32_t hal_wdg_init(wdg_dev_t *wdg)
```

- [x] **Description**

  This function will initialize the on board CPU hardware watch dog

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | wdg | the watch dog device |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 2-hal_wdg_reload

```c
void hal_wdg_reload(wdg_dev_t *wdg)
```

- [x] **Description**

  Reload watchdog counter.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | wdg | the watch dog device |

- [x] **Returns**

  None.


## 3-hal_wdg_finalize

```c
int32_t hal_wdg_finalize(wdg_dev_t *wdg)
```

- [x] **Description**

  This function performs any platform-specific cleanup needed for hardware watch dog.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | wdg | the watch dog device |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step
