#ifndef _MB_UTILS_H
#define _MB_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif
/*! \defgroup modbus_utils Utilities
 *
 * This module contains some utility functions which can be used by
 * the application. It includes some special functions for working with
 * bitfields backed by a character array buffer.
 *
 */
/*! \addtogroup modbus_utils
 *  @{
 */
/*! \brief Function to set bits in a byte buffer.
 *
 * This function allows the efficient use of an array to implement bitfields.
 * The array used for storing the bits must always be a multiple of two
 * bytes. Up to eight bits can be set or cleared in one operation.
 *
 * \param ucByteBuf A buffer where the bit values are stored. Must be a
 *   multiple of 2 bytes. No length checking is performed and if
 *   usBitOffset / 8 is greater than the size of the buffer memory contents
 *   is overwritten.
 * \param usBitOffset The starting address of the bits to set. The first
 *   bit has the offset 0.
 * \param ucNBits Number of bits to modify. The value must always be smaller
 *   than 8.
 * \param ucValues Thew new values for the bits. The value for the first bit
 *   starting at <code>usBitOffset</code> is the LSB of the value
 *   <code>ucValues</code>
 *
 * \code
 * ucBits[2] = {0, 0};
 *
 * // Set bit 4 to 1 (read: set 1 bit starting at bit offset 4 to value 1)
 * xMBUtilSetBits( ucBits, 4, 1, 1 );
 *
 * // Set bit 7 to 1 and bit 8 to 0.
 * xMBUtilSetBits( ucBits, 7, 2, 0x01 );
 *
 * // Set bits 8 - 11 to 0x05 and bits 12 - 15 to 0x0A;
 * xMBUtilSetBits( ucBits, 8, 8, 0x5A);
 * \endcode
 */
void            xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset,
                                UCHAR ucNBits, UCHAR ucValues );

/*! \brief Function to read bits in a byte buffer.
 *
 * This function is used to extract up bit values from an array. Up to eight
 * bit values can be extracted in one step.
 *
 * \param ucByteBuf A buffer where the bit values are stored.
 * \param usBitOffset The starting address of the bits to set. The first
 *   bit has the offset 0.
 * \param ucNBits Number of bits to modify. The value must always be smaller
 *   than 8.
 *
 * \code
 * UCHAR ucBits[2] = {0, 0};
 * UCHAR ucResult;
 *
 * // Extract the bits 3 - 10.
 * ucResult = xMBUtilGetBits( ucBits, 3, 8 );
 * \endcode
 */
UCHAR           xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset,
                                UCHAR ucNBits );

/*! @} */

#ifdef __cplusplus
}
#endif
#endif
