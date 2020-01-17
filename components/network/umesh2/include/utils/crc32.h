#ifndef CRC32_H_
#define CRC32_H_

#include <stdint.h>

uint32_t crc32(const void *buf, unsigned long size);

void get_fcs(uint8_t *p_buffer, uint16_t length);
#endif /* CRC32_H_ */
