#ifndef _ALI_DFU_PORT_H
#define _ALI_DFU_PORT_H
#include <stdbool.h>

#define DFU_IMAGE_SIZE_MAX 240*1024

void dfu_reboot(void);
unsigned char dfu_check_checksum(short image_id, unsigned short *crc16_output);
int ali_dfu_image_update(short signature, int offset, int length, int *p_void);

/**
 * @brief get the current runing partition.
 * @return the current runing partition.
 */
uint8_t get_program_image(void);

/**
 * @brief switch the running partition, without reboot.
 * @param[in] the partition which switch to.
 * @return the runing partition when next boot.
 */
uint8_t change_program_image(uint8_t dfu_image);

int erase_dfu_flash(void);

#endif
