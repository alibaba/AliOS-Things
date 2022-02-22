#ifndef _CRC64_H_
#define _CRC64_H_

#include <stdint.h>
#include <stddef.h>

typedef uint64_t crc64_t;

crc64_t upload_crc64_update(crc64_t crc, const unsigned char *s, uint64_t l);

#endif
