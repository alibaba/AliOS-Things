EN | [中文](AliOS-Things-Environment-Setup.zh)

# Content

- [1 System environment setup](#1-system-environment-setup)
- [2 Hardware environment setup](#2-hardware-environment-setup)

# 1 System environment setup

- [Windows](AliOS-Things-Windows-Environment-Setup)
- [Linux](AliOS-Things-Linux-Environment-Setup)
- [MAC](AliOS-Things-MAC-Environment-Setup)

# 2 Hardware environment setup

## MXCHIP MK3060 WiFi Module

- [Hardware Setup](MK3060-Hardware-Setup)

## ESP32 DevKitC

- [Hardware Information](http://esp-idf.readthedocs.io/en/latest/get-started/get-started-devkitc.html)
- Image download example<br>
  python platform/mcu/esp32/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyUSBx --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB  0x10000 /path/to/image
