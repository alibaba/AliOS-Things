#ifndef _OTA_BREEZE_TRANSPORT_H
#define _OTA_BREEZE_TRANSPORT_H

typedef enum {
    OTA_BREEZE_FLASH_ERASE_OK = 0,
    OTA_BREEZE_FLASH_ERASE_FAIL,
    OTA_BREEZE_FLASH_STORE_OK,
    OTA_BREEZE_FLASH_STORE_FAIL,
    OTA_BREEZE_FLASH_ERROR = 0xff
} ota_breeze_flash_evt_t;

void ota_breeze_send_error(void);
unsigned int ota_breeze_send_fw_version_rsp(unsigned char ota_cmd, unsigned char *buffer, unsigned int length);
int ota_breeze_split_sw_ver(char *data, unsigned int *v0, unsigned int *v1, unsigned int *v2);
void ota_breeze_on_auth(unsigned char is_authenticated);
void ota_breeze_on_tx_done(unsigned char cmd);
void ota_breeze_reset(void);
void ota_breeze_disconnect(void);
void ota_breeze_on_discontinuous_frame(void);
unsigned int ota_breeze_on_fw_upgrade_req(unsigned char *buffer, unsigned int length);
unsigned int ota_breeze_send_bytes_received(void);
unsigned int ota_breeze_is_in_check_status(void);
unsigned int ota_breeze_on_fw_data(unsigned char *buffer, unsigned int length, unsigned char num_frames);

#endif
