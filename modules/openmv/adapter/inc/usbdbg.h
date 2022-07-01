/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * USB debug support.
 */
#ifndef __USBDBG_H__
#define __USBDBG_H__
/**
  * Firmware version (major, minor and patch numbers).
  *
  */
#define FIRMWARE_VERSION_MAJOR      (6)
#define FIRMWARE_VERSION_MINOR      (0)
#define FIRMWARE_VERSION_PATCH      (0)

/**
  * To add a new debugging command, increment the last command value used.
  * Set the MSB of the value if the request has a device-to-host data phase.
  * Add the command to usr/openmv.py using the same value.
  * Handle the command control and data in/out (if any) phases in usbdbg.c.
  *
  * See usbdbg.c for examples.
  */
enum usbdbg_cmd {
    USBDBG_NONE             =0x00,
    USBDBG_FW_VERSION       =0x80,
    USBDBG_FRAME_SIZE       =0x81,
    USBDBG_FRAME_DUMP       =0x82,
    USBDBG_ARCH_STR         =0x83,
    USBDBG_SCRIPT_EXEC      =0x05,
    USBDBG_SCRIPT_STOP      =0x06,
    USBDBG_SCRIPT_SAVE      =0x07,
    USBDBG_SCRIPT_RUNNING   =0x87,
    USBDBG_TEMPLATE_SAVE    =0x08,
    USBDBG_DESCRIPTOR_SAVE  =0x09,
    USBDBG_ATTR_READ        =0x8A,
    USBDBG_ATTR_WRITE       =0x0B,
    USBDBG_SYS_RESET        =0x0C,
    USBDBG_SYS_RESET_TO_BL  =0x0E,
    USBDBG_FB_ENABLE        =0x0D,
    USBDBG_TX_BUF_LEN       =0x8E,
    USBDBG_TX_BUF           =0x8F,
    USBDBG_SENSOR_ID        =0x90
};
void usbdbg_init();
void usbdbg_wait_for_command(uint32_t timeout);
bool usbdbg_script_ready();
vstr_t *usbdbg_get_script();
bool usbdbg_get_irq_enabled();
void usbdbg_set_irq_enabled(bool enabled);
void usbdbg_set_script_running(bool running);
void usbdbg_data_in(void *buffer, int length);
void usbdbg_data_out(void *buffer, int length);
void usbdbg_control(void *buffer, uint8_t brequest, uint32_t wlength);
bool usbdbg_handle_script_status(void);

#endif /* __USBDBG_H__ */
