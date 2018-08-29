#ifndef __ISD9160_H
#define __ISD9160_H

int handle_upgrade(const char *file_in_sd);
int handle_record(const char *file_in_sd, int *stop_flag);
int handle_playback(const char *file_in_sd);
int isd9160_loop_once(void);
void isd9160_reset(void);
int isd9160_i2c_init(void);

#endif /* __ISD9160_H */
