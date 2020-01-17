#ifndef _UMESH_UTILS_H_
#define _UMESH_UTILS_H_

#include <stdint.h>

void umesh_produce_random(uint8_t *random, uint32_t len);

int umesh_get_ucast_iv(uint8_t *r1, uint8_t *r2, uint8_t *out);

int umesh_get_bcast_iv(uint8_t *r1, uint8_t *r2, uint8_t *out);

/* host byte order to little endian */
uint16_t umesh_htole16(uint16_t data);
/* little endian to host byte order */
uint16_t umesh_le16toh(uint16_t data);
/* host byte order to little endian */
uint32_t umesh_htole32(uint32_t data);
/* little endian to host byte order*/
uint32_t umesh_le32toh(uint32_t data);


/* host byte order to little endian */
uint16_t umesh_htobe16(uint16_t data);
/* little endian to host byte order */
uint16_t umesh_be16toh(uint16_t data);
/* host byte order to little endian */
uint32_t umesh_htobe32(uint32_t data);
/* little endian to host byte order*/
uint32_t umesh_be32toh(uint32_t data);
#endif

