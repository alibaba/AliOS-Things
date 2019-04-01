#ifndef EFUSE_API_H
#define EFUSE_API_H
//#include "efuse/hal_efuse.h"

typedef union USB_VPID {
  uint8_t  u8[2];			
  uint16_t u16;
} usb_pvid;

//export API
int efuse_dump_data();
int efuse_get_free_bits();

int efuse_write_mac(uint8_t *mac_addr);
int efuse_write_chip_id(uint32_t chip_id);
int efuse_write_usb_vid(uint16_t usb_vid);
int efuse_write_usb_pid(uint16_t usb_pid);
int efuse_write_tx_power1(uint8_t value);
int efuse_write_tx_power2(uint8_t value);
int efuse_write_xtal(uint8_t xtal);

int efuse_read_mac(uint8_t *mac_addr);
int efuse_read_mac_inc(uint8_t *mac_addr);
int efuse_read_chip_id(uint32_t *p_chip_id);
int efuse_read_usb_vid(uint16_t *pvid);
int efuse_read_usb_pid(uint16_t *ppid);
int efuse_read_tx_power1(uint8_t* pvalue);
int efuse_read_tx_power2(uint8_t* pvalue);
int efuse_read_xtal(uint8_t *pxtal);

#endif 