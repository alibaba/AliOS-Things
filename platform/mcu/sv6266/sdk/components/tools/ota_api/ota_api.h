#ifndef _OTA_API_H_
#define _OTA_API_H_

/**
 * Start OTA upadte procedure.
 * @retval   0        Upadte success.
 * @retval  -1        Upadte fail.
 */
int32_t ota_update(void);

/**
 * Set OTA server parameter.
 * @param  nProtocol        OTA protocal mode, 0 : HTTP, 1 : TFTP.
 * @param  pIP                OTA Server addrass.
 * @param  port               OTA Server port.
 * @retval   0                   Set server success.
 * @retval  -1                  Set server fail.
 */
int32_t ota_set_server(uint8_t nProtocol, char *pIP, uint16_t port);

/**
 * Set OTA file parameter.
 * @param  filename                    OTA file name.
 * @param  filepath                     OTA file path.
 * @retval   0                              Set parameter success.
 * @retval  -1                             Set parameter fail.
 */
int32_t ota_set_parameter(char* filename, char* filepath);

#endif //_OTA_API_H_

