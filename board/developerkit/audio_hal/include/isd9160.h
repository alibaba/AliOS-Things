#ifndef __ISD9160_H
#define __ISD9160_H

typedef void (*CB_SWVER)(const char *sw_ver);
typedef void (*CB_VRCMD)(const char *sw_ver, int cmd_id);

int handle_upgrade(const char *file_in_sd);
int handle_record(const char *file_in_sd, int *stop_flag);
int handle_playback(const char *file_in_sd);
int register_swver_callback(CB_SWVER pfunc);
int register_vrcmd_callback(CB_VRCMD pfunc);
int isd9160_loop_once(void);
void isd9160_reset(void);
int isd9160_i2c_init(void);

#endif /* __ISD9160_H */
