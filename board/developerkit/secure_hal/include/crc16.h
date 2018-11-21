/*
 * crc.h
 *
 *  Created on: 06 ????. 2015 ?.
*/

#ifndef INC_CRC_H_
#define INC_CRC_H_

#include <stdlib.h>

#define	CRC16CCITT

/* available algorithms (select one):
#define	CRC16CCITT
#define	CRC16CCITT_XMODEM
#define	CRC16CCITT_KERMIT
#define	CRC16CCITT_1D0F
#define	CRC16IBM
#define	CRC16X25
*/

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
//typedef long int32_t;
//typedef long long int64_t;
//typedef unsigned long uint32_t;
//typedef unsigned long long uint64_t;

typedef uint16_t bit_order_16(uint16_t value);
typedef uint8_t bit_order_8(uint8_t value);

uint16_t crc16(const uint8_t *data, uint16_t size);


#endif /* INC_CRC_H_ */