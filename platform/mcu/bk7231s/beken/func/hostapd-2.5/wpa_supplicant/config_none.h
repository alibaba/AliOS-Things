#ifndef _CONFIG_NONE_H_
#define _CONFIG_NONE_H_

#include "config.h"

extern int wpa_config_set_none(struct wpa_ssid *ssid);
extern int wpa_config_set_wep(struct wpa_ssid *ssid);
extern int wpa_config_set_wpa(struct wpa_ssid *ssid, struct wpa_ie_data *ie);

extern struct wpa_config * wpa_config_read(const char *name, struct wpa_config *cfgp);
extern int wpa_config_write(const char *name, struct wpa_config *config);

#endif // _CONFIG_NONE_H_
// eof

