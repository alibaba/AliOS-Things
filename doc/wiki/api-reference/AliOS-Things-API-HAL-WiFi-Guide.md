# API INDEX

  * [1 hal_wifi_get_default_module](#1-hal_wifi_get_default_module)
  * [2 hal_wifi_register_module](#2-hal_wifi_register_module)
  * [3 hal_wifi_init](#3-hal_wifi_init)
  * [4 hal_wifi_get_mac_addr](#4-hal_wifi_get_mac_addr)
  * [5 hal_wifi_start](#5-hal_wifi_start)
  * [6 hal_wifi_start_adv](#6-hal_wifi_start_adv)
  * [7 hal_wifi_get_ip_stat](#7-hal_wifi_get_ip_stat)
  * [8 hal_wifi_get_link_stat](#8-hal_wifi_get_link_stat)
  * [9 hal_wifi_start_scan](#9-hal_wifi_start_scan)
  * [10 hal_wifi_start_scan_adv](#10-hal_wifi_start_scan_adv)
  * [11 hal_wifi_power_off](#11-hal_wifi_power_off)
  * [12 hal_wifi_power_on](#12-hal_wifi_power_on)
  * [13 hal_wifi_suspend](#13-hal_wifi_suspend)
  * [14 hal_wifi_suspend_station](#14-hal_wifi_suspend_station)
  * [15 hal_wifi_suspend_soft_ap](#15-hal_wifi_suspend_soft_ap)
  * [16 hal_wifi_set_channel](#16-hal_wifi_set_channel)
  * [17 hal_wifi_start_wifi_monitor](#17-hal_wifi_start_wifi_monitor)
  * [18 hal_wifi_stop_wifi_monitor](#18-hal_wifi_stop_wifi_monitor)
  * [19 hal_wifi_register_monitor_cb](#19-hal_wifi_register_monitor_cb)
  * [20 hal_wifi_install_event](#20-hal_wifi_install_event)

------

## 1-hal_wifi_get_default_module

```c
hal_wifi_module_t *hal_wifi_get_default_module(void)
```

- [x] **Description**

  Get the default wifi instance.
  The system may have more than one wifi instance,
  this API returns the default one.

- [x] **Parameters**

  None.

- [x] **Returns**

  Instance pointer, or NULL if no instance registered.

## 2-hal_wifi_register_module

```c
void hal_wifi_register_module(hal_wifi_module_t *m)
```

- [x] **Description**

  Regster a wifi instance to the HAL framework.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | the wifi instance. |

- [x] **Returns**

  None.


## 3-hal_wifi_init

```c
int hal_wifi_init(void)
```

- [x] **Description**

  Initialize wifi instances.

- [x] **Parameters**

  None.

- [x] **Returns**

  0 on success, otherwise failure.

## 4-hal_wifi_get_mac_addr

```c
void hal_wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
```

- [x] **Description**

  Get the MAC address of the specified wifi instance.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  m |   the wifi instance, NULL if default.   |
  | [out] | mac | the place to hold the result. |

- [x] **Returns**

  None.


## 5-hal_wifi_start

```c
int hal_wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
```

- [x] **Description**

  Start the wifi instance.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m |         the wifi instance, NULL if default.   |
  | [in] | init_para | the config used to start the wifi. |

- [x] **Returns**

      0 on success, otherwise failure.

## 6-hal_wifi_start_adv

```c
int hal_wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
```

- [x] **Description**

  Start the wifi instance in anvanced way (more config specified).

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m |  the wifi instance, NULL if default.   |
  | [in] | init_para_adv | the advanced config used to start the wifi. |

- [x] **Returns**

      0 on success, otherwise failure.

## 7-hal_wifi_get_ip_stat

```c
int hal_wifi_get_ip_stat(hal_wifi_module_t *m,
                          hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
```

- [x] **Description**

  Get the status of the specified wifi instance, e.g. the IP, mask, dhcp mode, etc.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  m | the wifi instance, NULL if default.   |
  | [out] | out_net_para | the place to hold the results.   |
  | [in] |  wifi_type |    SOFT_AP or STATION. |

- [x] **Returns**

      0 on success, otherwise failure.

## 8-hal_wifi_get_link_stat

```c
int hal_wifi_get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
```

- [x] **Description**

  Get the link status of the wifi instance ,e.g. ssid, bssid, channel, rssi, etc.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  m |        the wifi instance, NULL if default.   |
  | [out] | out_stat | the place to hold the results. |

- [x] **Returns**

      0 on success, otherwise failure.

## 9-hal_wifi_start_scan

```c
void hal_wifi_start_scan(hal_wifi_module_t *m)
```

- [x] **Description**

  Start the scanning of the specified wifi instance.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | the wifi instance, NULL if default. |

- [x] **Returns**

  None.


## 10-hal_wifi_start_scan_adv

```c
void hal_wifi_start_scan_adv(hal_wifi_module_t *m)
```

- [x] **Description**

  Start the scanning of the specified wifi instance in advanced way.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | the wifi instance, NULL if default. |

- [x] **Returns**

  None.


## 11-hal_wifi_power_off

```c
int hal_wifi_power_off(hal_wifi_module_t *m)
```

- [x] **Description**

  Power off the wifi instance.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | the wifi instance, NULL if default. |

- [x] **Returns**

     0 on success, otherwise failure.

## 12-hal_wifi_power_on

```c
int hal_wifi_power_on(hal_wifi_module_t *m)
```

- [x] **Description**

  Power on the wifi instance.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | the wifi instance, NULL if default. |

- [x] **Returns**

     0 on success, otherwise failure.

## 13-hal_wifi_suspend

```c
int hal_wifi_suspend(hal_wifi_module_t *m)
```

- [x] **Description**

  Suspend the wifi instance.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | the wifi instance, NULL if default. |

- [x] **Returns**

     0 on success, otherwise failure.

## 14-hal_wifi_suspend_station

```c
int hal_wifi_suspend_station(hal_wifi_module_t *m)
```

- [x] **Description**

  Suspend the wifi instance in station mode.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | the wifi instance, NULL if default. |

- [x] **Returns**

     0 on success, otherwise failure.

## 15-hal_wifi_suspend_soft_ap

```c
int hal_wifi_suspend_soft_ap(hal_wifi_module_t *m)
```

- [x] **Description**

  Suspend the wifi instance in soft_ap mode.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | the wifi instance, NULL if default. |

- [x] **Returns**

     0 on success, otherwise failure.

## 16-hal_wifi_set_channel

```c
int hal_wifi_set_channel(hal_wifi_module_t *m, int ch)
```

- [x] **Description**

  Set the channel of the wifi instance.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | the wifi instance, NULL if default. |

- [x] **Returns**

     0 on success, otherwise failure.

## 17-hal_wifi_start_wifi_monitor

```c
void hal_wifi_start_wifi_monitor(hal_wifi_module_t *m)
```

- [x] **Description**

  Start the monitor mode of the wifi instance.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | the wifi instance, NULL if default. |

- [x] **Returns**

  None.


## 18-hal_wifi_stop_wifi_monitor

```c
void hal_wifi_stop_wifi_monitor(hal_wifi_module_t *m)
```

- [x] **Description**

  Stop the monitor mode of the wifi instance.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m | The wifi instance, NULL if default. |

- [x] **Returns**

  None.


## 19-hal_wifi_register_monitor_cb

```c
void hal_wifi_register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
```

- [x] **Description**

  Register the montior callback on the wifi instance.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m |  the wifi instance, NULL if default.   |
  | [in] | fn | the callback function. |

- [x] **Returns**

  None.


## 20-hal_wifi_install_event

```c
void hal_wifi_install_event(hal_wifi_module_t *m, const hal_wifi_event_cb_t *cb)
```

- [x] **Description**

  Set the event callback function array for the wifi.
  Please don't do time consuming work in these callbacks.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | m |  the wifi instance, NULL for default.   |
  | [in] | cb | the event callback function info. |

- [x] **Returns**

  None.

