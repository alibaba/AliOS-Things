# API INDEX

  * [1 hal_rtc_init](#1-hal_rtc_init)
  * [2 hal_rtc_get_time](#2-hal_rtc_get_time)
  * [3 hal_rtc_set_time](#3-hal_rtc_set_time)
  * [4 hal_rtc_finalize](#4-hal_rtc_finalize)
------

## 1-hal_rtc_init

```c
void hal_rtc_init(rtc_dev_t *rtc)
```

- [x] **Description**

  This function will initialize the on board CPU real time clock
 

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | rtc | rtc device |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 2-hal_rtc_get_time

```c
int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
```

- [x] **Description**

  This function will return the value of time read from the on board CPU real time clock.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  rtc |  rtc device   |
  | [out] | time | pointer to a time structure |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 3-hal_rtc_set_time

```c
int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
```

- [x] **Description**

  This function will set MCU RTC time to a new value.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  rtc |  rtc device   |
  | [out] | time | pointer to a time structure |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 4-hal_rtc_finalize

```c
void hal_rtc_finalize(rtc_dev_t *rtc)
```

- [x] **Description**

  This function will finalize the on board CPU real time clock
 
- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | rtc | rtc device |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step