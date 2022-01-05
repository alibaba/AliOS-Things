HAL Directory - Purpose
===========================
This directory contains all the Hardware Abstraction Layer (HAL) files used to
adapt the upper levels of atca-ng and abstractions to physical hardware.

HAL contains physical implementations for I2C, SWI, SPI, UART and timers for
specific hardware platforms.

**Include just those HAL files you require based on platform type.**

CryptoAuthLib Supported HAL Layers
=============================================

HAL Layers files are combined into groups. Initial group is generic files that are typically included in a project.
Files are then broken out by uController Family and or Operating System Interface.


| Protocol Files | Interface  | Files                        | API         | Notes                              |
|----------------|------------|------------------------------|-------------|------------------------------------|
|atca            |            | atca_hal.c/h                 |             | For all projects                   |
|kit protocol    |            | kit_protocol.c/h             |             | For all Kit Protocol projects      |
|                |            | kit_phy.h                    |             |                                    |
|                |            | hal_i2c_bitbang.c/h          | ASF         | For all I2C Bitbang projects       |
|                |            | hal_swi_bitbang.c/h          | ASF         | For all SWI Bitbang projects       |


Most microcontrollers supported by [Atmel START](https://www.microchip.com/start)
have generic drivers depending on the interface.

| START Micros   | Interface  | Files                        | API         | Notes                              |
|----------------|------------|------------------------------|-------------|------------------------------------|
|                |            | hal_timer_start.c            | START       | Timer implementation               |
|                |   I2C      | hal_i2c_start.c/h            | START       |                                    |
|                |   SWI      | swi_uart_start.c/h           | START       | SWI using UART                     |


|AVR Micros      | Interface  | Files                        | API         | Notes                              |
|----------------|------------|------------------------------|-------------|------------------------------------|
|at90usb1287     |   I2C      | hal_at90usb1287_i2c_asf.c/h  | ASF         |                                    |
|                |            | hal_at90usb1287_timer_asf.c  | ASF         |                                    |
|                |   SWI      | swi_uart_at90usb1287_asf.c/h | ASF         |                                    |
|xmega_a3bu      |   I2C      | hal_xmega_a3bu_i2c_asf.c/h   | ASF         |                                    |
|                |            | hal_xmega_a3bu_timer_asf.c   | ASF         |                                    |
|                |   SWI      | swi_uart_xmaga_a3bu_asf.c/h  | ASF         |                                    |


|SAM Micros      | Interface  | Files                        | API         | Notes                              |
|----------------|------------|------------------------------|-------------|------------------------------------|
|sam4s           |   I2C      | hal_sam4s_i2c_asf.c/h        | ASF         |                                    |
|                |            | hal_sam4s_timer_asf.c        | ASF         |                                    |
|samb11          |   I2C      | hal_samb11_i2c_asf.c/h       | ASF         |                                    |
|                |            | hal_samb11_timer_asf.c       | ASF         |                                    |
|samd21          |   I2C      | hal_samd21_i2c_asf.c/h       | ASF         |                                    |
|                |            | hal_samd21_timer_asf.c       | ASF         | For all samd21 ASF projects        |
|samd21          |   I2C      | i2c_bitbang_samd21.c/h       | ASF         | For samd21 I2C bitbang projects    |
|samd21          |   SWI      | swi_bitbang_samd21.c/h       | ASF         | For samd21 SWI bitbang projects    |
|samd21          |   SWI      | swi_uart_samd21.c/h          | ASF         | For samd21 SWI uart projects       |
|samg55          |   I2C      | hal_samg55_i2c_asf.c/h       | ASF         |                                    |
|                |            | hal_samg55_timer_asf.c       | ASF         |                                    |
|samv71          |   I2C      | hal_samv71_i2c_asf.c/h       | ASF         |                                    |
|                |            | hal_samv71_timer_asf.c       | ASF         |                                    |


|PIC Micros      | Interface  | Files                        | API         | Notes                                           |
|----------------|------------|------------------------------|-------------|-------------------------------------------------|
|pic32mx695f512h |   I2C      | hal_pic32mx695f512h.c/h      | plib.h      |  For pic32mx695f512h Standalone Mplab projects  |
|                |            | hal_pic32mx695f512h_timer.c  | plib.h      |  For pic32mx695f512h Standalone Mplab projects  |
|PIC32MZ2048     |   I2C      | hal_pic32mz2048efm_i2c.c/h   |             |                                                 |
|                |            | hal_pic32mz2048efm_timer.c   |             |                                                 |



| OS             | Interface  | Files                            | API         | Notes                              |
|----------------|------------|----------------------------------|-------------|------------------------------------|
| MS Windows     |  kit-cdc   | hal_win_kit_cdc.c/h              | windows.h   | For all windows USB CDC projects   |
| MS Windows     |  kit-hid   | hal_win_kit_hid.c/h              | windows.h   | For all windows USB HID projects   |
|                |            |                                  | setupapi.h  |                                    |
| MS Windows     |            | hal_win_timer.c                  | windows.h   | For all windows projects           | 
| Linux          |    I2C     | hal_linux_i2c_userspace.c/h      | i2c-dev     |                                    |
| Linux          |  kit-cdc   | hal_linux_kit_cdc.c/h            | fopen       | For USB Linux CDC projects         |
| Linux          |  kit-hid   | hal_linux_kit_hid.c/h            | udev        | For USB Linux HID Projects         |
| Linux/Mac      |            | hal_linux_timer.c                |             | For all Linux/Mac projects         |
| All            |  kit-hid   | hal_all_platforms_kit_hidapi.c/h | hidapi      | Works for Windows, Linux, and Mac  |
| freeRTOS       |            | hal_freertos.c                   |             | freeRTOS common routines           |
