#ifndef ESP_WPA2_H
#define ESP_WPA2_H

/**
  * @brief  Set wpa2 enterprise authentication.
  *
  * @attention wpa2 enterprise authentication can only be used when ESP8266 station is enabled.
  *            wpa2 enterprise authentication can only support PEAP-MSCHAPv2 and TTLS-MSCHAPv2 method.
  *
  * @param  enable
  *    - 1: enable wpa2 enterprise authentication;
  *    - 0: disable wpa2 enterprise authentication
  *
  * @return  0: succeed
            -1: fail
  */
int wifi_station_set_wpa2_enterprise_auth(int enable);

/**
  * @brief  Set username for PEAP/TTLS method.
  *
  * @param  username: point to address where stores the username;
  *         len: length of username, limited to 1~127
  *
  * @return  0: succeed
  *         -1: fail(len <= 0 or len >= 128)
  *         -2: fail(internal memory malloc fail)
  */
int wifi_station_set_enterprise_username(unsigned char *username, int len);

/**
  * @brief  Set password for PEAP/TTLS method..
  *
  * @param  password: point to address where stores the password;
  *         len: length of password
  *
  * @return  0: succeed
  *         -1: fail(len <= 0)
  *         -2: fail(internal memory malloc fail)
  */
int wifi_station_set_enterprise_password(unsigned char *password, int len);

/**
  * @brief  Set CA certificate for PEAP/TTLS method.
  *
  * @param  ca_cert: point to address where stores the CA certificate;
  *
  * @return 0: succeed
  */
int wifi_station_set_enterprise_ca_cert(unsigned char *ca_cert, int len);

#endif
