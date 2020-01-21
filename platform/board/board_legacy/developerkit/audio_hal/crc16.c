/*
 *From: andrew cooke <andrew@...>
 *Date: Sat, 3 Aug 2013 15:13:39 -0400
 *Title: CRC16
 */

#include "crc16.h"

#define CRC16CCITT

/* available algorithms (select one):
#define	CRC16CCITT
#define	CRC16CCITT_XMODEM
#define	CRC16CCITT_KERMIT
#define	CRC16CCITT_1D0F
#define	CRC16IBM
#define	CRC16X25
*/

typedef uint16_t bit_order_16(uint16_t value);
typedef uint8_t  bit_order_8(uint8_t value);

uint16_t straight_16(uint16_t value)
{
    return value;
}

uint16_t reverse_16(uint16_t value)
{
    uint16_t reversed = 0;
    int      i;
    for (i = 0; i < 16; ++i) {
        reversed <<= 1;
        reversed |= value & 0x1;
        value >>= 1;
    }
    return reversed;
}

uint8_t straight_8(uint8_t value)
{
    return value;
}

uint8_t reverse_8(uint8_t value)
{
    uint8_t reversed = 0;
    int     i;
    for (i = 0; i < 8; ++i) {
        reversed <<= 1;
        reversed |= value & 0x1;
        value >>= 1;
    }
    return reversed;
}

uint16_t crc16common(uint8_t const *message, int nBytes, bit_order_8 data_order,
                     bit_order_16 remainder_order, uint16_t remainder,
                     uint16_t polynomial)
{
    int     byte;
    uint8_t bit;
    for (byte = 0; byte < nBytes; ++byte) {
        remainder ^= (data_order(message[byte]) << 8);
        for (bit = 8; bit > 0; --bit) {
            if (remainder & 0x8000) {
                remainder = (remainder << 1) ^ polynomial;
            } else {
                remainder = (remainder << 1);
            }
        }
    }
    return remainder_order(remainder);
}


uint16_t crc16ccitt(uint8_t const *message, int nBytes)
{
    return crc16common(message, nBytes, straight_8, straight_16, 0xffff,
                       0x1021);
}

uint16_t crc16ccitt_xmodem(uint8_t const *message, int nBytes)
{
    return crc16common(message, nBytes, straight_8, straight_16, 0x0000,
                       0x1021);
}

uint16_t crc16ccitt_kermit(uint8_t const *message, int nBytes)
{
    uint16_t swap =
      crc16common(message, nBytes, reverse_8, reverse_16, 0x0000, 0x1021);
    return swap << 8 | swap >> 8;
}

uint16_t crc16x25(uint8_t const *message, int nBytes)
{
    uint16_t swap =
      crc16common(message, nBytes, reverse_8, reverse_16, 0x0FFFF, 0x1021);
    //    return ~((swap >> 8) | ((swap & 0xFF) << 8));
    return ~(swap);
}

uint16_t crc16ccitt_1d0f(uint8_t const *message, int nBytes)
{
    return crc16common(message, nBytes, straight_8, straight_16, 0x1d0f,
                       0x1021);
}

uint16_t crc16ibm(uint8_t const *message, int nBytes)
{
    return crc16common(message, nBytes, reverse_8, reverse_16, 0x0000, 0x8005);
}

uint16_t checksum_crc16(const uint8_t *data, uint16_t size)
{
    uint16_t out = 0;

    /* Sanity check: */
    if (data == 0)
        return 0;

#ifdef CRC16CCITT
    out = crc16ccitt(data, size);
#endif
#ifdef CRC16CCITT_XMODEM
    out = crc16ccitt_xmodem(data, size);
#endif
#ifdef CRC16CCITT_KERMIT
    out = crc16ccitt_kermit(data, size);
#endif
#ifdef CRC16CCITT_1D0F
    out = crc16ccitt_1d0f(data, size);
#endif
#ifdef CRC16IBM
    out = crc16ibm(data, size);
#endif
#ifdef CRC16X25
    out = crc16x25(data, size);
#endif

    return out;
}
