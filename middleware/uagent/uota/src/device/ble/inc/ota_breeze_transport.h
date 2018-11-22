#ifndef _OTA_BREEZE_TRANSPORT_H
#define _OTA_BREEZE_TRANSPORT_H

typedef enum {
    OTA_BREEZE_FLASH_ERASE_OK = 0,
    AOTA_BREEZE_FLASH_ERASE_FAIL,
    OTA_BREEZE_FLASH_STORE_OK,
    OTA_BREEZE_FLASH_STORE_FAIL,
    OTA_BREEZE_FLASH_ERROR = 0xff
} ota_breeze_flash_evt_t;

void ota_breeze_send_error(void);
uint32_t ota_breeze_send_fw_version_rsp(uint8_t ota_cmd, uint8_t *buffer, uint32_t length);
int ota_breeze_split_sw_ver(char *data, uint32_t *v0, uint32_t *v1, uint32_t *v2);
uint32_t ota_breeze_get_init_fw_size(void);
void ota_breeze_on_xfer_finished(uint8_t *buffer, uint16_t length);
void ota_breeze_on_auth(uint8_t is_authenticated);
void ota_breeze_on_tx_done(uint8_t cmd);
void ota_breeze_reset(void);
void ota_breeze_disconnect(void);
void ota_breeze_on_discontinuous_frame(void);

#endif
