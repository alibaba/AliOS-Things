/************************************************************************/
/**	\file	i2c.h
 *	\brief	I2C bus functions
 *
 *	\date	2006/10/25
 *	\author	Stephane Lesage - ATEIS
 *
 */
/************************************************************************/
#ifndef _I2C_H_
#define _I2C_H_

/* Includes */
#include "ATypes.h"

/* Macros */
#if !NO_SYS
#include "AMutex.h"
#define I2C_EXTLOCKVAR		extern AMutexID kI2CLock
#define I2C_LOCKVAR			AMutexID kI2CLock
#define I2C_KEY(key)		AMUTEX_KEY(key)
#define I2C_Lock(key)		AMutex_Lock(kI2CLock, key)
#define I2C_Release(key)	AMutex_Release(kI2CLock, key)
#else
#define I2C_EXTLOCKVAR
#define I2C_LOCKVAR
#define I2C_KEY(key)
#define I2C_Lock(key)
#define I2C_Release(key)
#endif

/* Types */

/**	I2C clock speeds
*/
typedef enum
{
	I2C_FAST = 0,				//!< Fast mode (400 kHz)
	I2C_STANDARD = 1 			//!< Standard mode (100 kHz)
} I2CCLOCK;

/* Externals */

#ifdef __cplusplus
extern "C" {
#endif

I2C_EXTLOCKVAR;

/* Functions */

/**	Perform I2C bus initializations at start-up.

- Set the processor pins for I2C bus operation.
- Set bus frequency to #I2C_STANDARD.
- Create the synchronization object.

*/
void I2C_Init(void);

/**	Set I2C clock frequency
*/
void I2C_Clock(I2CCLOCK freq);

/**	Send a byte array to an I2C peripheral

\param[in]	addr	7 bits I2C address
\param[in]	buf		pointer to the data
\param[in]	n		number of bytes to send

\return success flag
*/
bool I2C_Write(uint8_t addr, const uint8_t* buf, size_t n);

/**	Get a byte array from an I2C peripheral

\param[in]	addr	7 bits I2C address
\param[out]	buf		pointer to the reception buffer
\param[in]	n		number of bytes to receive

\return success flag
*/
bool I2C_Read(uint8_t addr, uint8_t* buf, size_t n);

/**	Write registers of an I2C peripheral

\param[in]	addr	7 bits I2C address
\param[in]	reg		8 bit register
\param[in]	buf		pointer to the data
\param[in]	n		number of bytes to send

\return success flag
*/
bool I2C_Reg8Write(uint8_t addr, uint8_t reg, const uint8_t* buf, size_t n);

/**	Read registers of an I2C peripheral

\param[in]	addr	7 bits I2C address
\param[in]	reg		8 bit register
\param[out]	buf		pointer to the reception buffer
\param[in]	n		number of bytes to receive

\return success flag
*/
bool I2C_Reg8Read(uint8_t addr, uint8_t reg, uint8_t* buf, size_t n);

/**	Write registers of an I2C peripheral

\param[in]	addr	7 bits I2C address
\param[in]	reg		16 bits register address
\param[in]	buf		pointer to the data
\param[in]	n		number of bytes to send

\return success flag
*/
bool I2C_Reg16Write(uint8_t addr, uint16_t reg, const uint8_t* buf, size_t n);

/**	Read registers of an I2C peripheral

\param[in]	addr	7 bits I2C address
\param[in]	reg		16 bits register address
\param[out]	buf		pointer to the reception buffer
\param[in]	n		number of bytes to receive

\return success flag
*/
bool I2C_Reg16Read(uint8_t addr, uint16_t reg, uint8_t* buf, size_t n);

#ifdef __cplusplus
}
#endif

/* Include Guard */
#endif
