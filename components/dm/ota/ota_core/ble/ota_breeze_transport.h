#ifndef OTA_BREEZE_TRANSPORT_H
#define OTA_BREEZE_TRANSPORT_H

typedef enum {
    OTA_BREEZE_FLASH_ERASE_OK = 0,
    OTA_BREEZE_FLASH_ERASE_FAIL,
    OTA_BREEZE_FLASH_STORE_OK,
    OTA_BREEZE_FLASH_STORE_FAIL,
    OTA_BREEZE_FLASH_ERROR = 0xff
} ota_breeze_flash_evt_t;

void ota_breeze_reset(void);
void ota_breeze_disconnect(void);
void ota_breeze_send_error(void);
void ota_breeze_on_discontinuous_frame(void);
void ota_breeze_on_tx_done(unsigned char cmd);
unsigned int ota_breeze_is_in_check_status(void);
unsigned int ota_breeze_send_bytes_received(void);
void ota_breeze_on_auth(unsigned char is_authenticated);
unsigned int ota_breeze_send_crc_result(unsigned char crc_ok);
unsigned int ota_breeze_send_fw_upgrade_rsp(unsigned char allow_upgrade);
unsigned int ota_breeze_on_fw_upgrade_req(unsigned char *buffer, unsigned int length);
int ota_breeze_split_sw_ver(char *data, unsigned int *v0, unsigned int *v1, unsigned int *v2);
unsigned int ota_breeze_on_fw_data(unsigned char *buffer, unsigned int length, unsigned char num_frames);
unsigned int ota_breeze_send_fw_version_rsp(unsigned char ota_cmd, unsigned char *buffer, unsigned int length);
#endif /*OTA_BREEZE_TRANSPORT_H*/
