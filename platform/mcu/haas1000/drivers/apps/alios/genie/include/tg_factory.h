#include "plat_types.h"

/**
 * @brief set wifi mac address
 * @param[in] mac: mac address，format: "01:23:45:67:89:ab"
 * @returns 0: succeed; -1: failed
 */
int tg_wifi_set_mac_address(const char * mac);

/**
 * @brief get wifi mac address
 * @param[out] mac: mac address，format: "01:23:45:67:89:ab"
 * @param[in] len: size of mac
 * @returns 0: succeed; -1: failed
 */
int tg_wifi_get_mac_address(char * mac, size_t len);

/**
 * @brief set bt mac address
 * @param[in] mac: mac address，format: "01:23:45:67:89:ab"
 * @returns 0: succeed; -1: failed
 */
int tg_bt_set_mac_address(const char *mac);

/**
 * @brief get bt mac address
 * @param[out] mac: mac address，format: "01:23:45:67:89:ab"
 * @param[in] len: size of mac
 * @returns 0: succeed; -1: failed
 */
int tg_bt_get_mac_address(char *mac, size_t len);


/**
 * @brief get flash size
 * @returns size of flash, unit:MB
 */
size_t tg_get_flash_size(void);

/**
 * @brief get flash model
 * @param[in] buffer: The buffer to store the flash model
 * @param[in] buffer_len: length of the buffer 
 * @returns 0: succeed; -1: failed
 */
int tg_get_flash_model(char *buffer, size_t len);

/**
 * @brief get memory size
 * @returns size of memory, unit:MB
 */
size_t tg_get_memory_size();

/**
 * @brief get chip temperature
 * @returns chip temperature, unit:degrees centigrade/1000
 */
int tg_get_chip_temperature(void);