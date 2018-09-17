/************************************************************************/
/**	\file	i2c_c674.c
 *	\brief	I2C bus functions for C674x architecture
 *
 *	\date	2011/04/08
 *	\author	Pierre Audenard - ATEIS France
 *
 */
/************************************************************************/

/* Includes */
#include "i2c.h"
#include "cpu.h"
#include <stdlib.h>
#include <string.h>

/* Macros */
#if !NO_SYS
#define I2C_LOCKCREATE()	AMutex_Create(kI2CLock)
#else
#define I2C_LOCKCREATE()
#endif

#include <ti/pspiom/cslr/cslr_i2c.h>
#include <ti/pspiom/cslr/soc_OMAPL137.h>

/* Types */

/* Externals */

/* Variables */
I2C_LOCKVAR;

/* Functions */

void I2C_Init()
{
	I2C_LOCKCREATE();

	CSL_I2cRegsOvly I2C0_Regs = (CSL_I2cRegsOvly) CSL_I2C_0_REGS;

	/* No Interrupt */
	I2C0_Regs->ICIMR = 0;

	/* Prescaler = 1 */
	I2C0_Regs->ICPSC = CSL_FMK(I2C_ICPSC_IPSC, 1);

	/* Bring I2C module out of reset */
	CSL_FINST(I2C0_Regs->ICMDR, I2C_ICMDR_IRS, ENABLE);

	/* Setup clock */
	I2C_Clock(I2C_STANDARD);
}

void I2C_Clock(I2CCLOCK freq)
{
	/* Reset I2C status */
	CSL_I2cRegsOvly I2C0_Regs = (CSL_I2cRegsOvly) CSL_I2C_0_REGS;
	CSL_FINST(I2C0_Regs->ICMDR, I2C_ICMDR_IRS, DISABLE);

	/* Setup periods */
	switch (freq)
	{
		case I2C_FAST:
			I2C0_Regs->ICCLKH = (CPU_AUXCLK/400000)/2;
			I2C0_Regs->ICCLKL = (CPU_AUXCLK/400000) - I2C0_Regs->ICCLKH;
			break;

		default:
			I2C0_Regs->ICCLKH = (CPU_AUXCLK/100000)/2;
			I2C0_Regs->ICCLKL = (CPU_AUXCLK/100000) - I2C0_Regs->ICCLKH;
			break;
	}
}

bool I2C_InternalWrite(uint8_t addr, uint8_t* regbuf, uint8_t regsize, const uint8_t* buf, size_t n)
{
    int i;
    bool status=false;
    int i2cstatus;

	/* Check */
	if (n==0) goto i2cret;

	/* Reset I2C status */
	CSL_I2cRegsOvly I2C0_Regs = (CSL_I2cRegsOvly) CSL_I2C_0_REGS;
	CSL_FINST(I2C0_Regs->ICMDR, I2C_ICMDR_IRS, DISABLE);

	/* Setup transfer */
	I2C0_Regs->ICSAR = addr;									// Slave address
	I2C0_Regs->ICCNT = regsize+n;								// Byte count
	I2C0_Regs->ICMDR = CSL_FMKT(I2C_ICMDR_MST, MASTER_MODE)		// Master mode
					 | CSL_FMKT(I2C_ICMDR_FREE, TRUE)			// Free mode
					 | CSL_FMKT(I2C_ICMDR_TRX, TX_MODE)			// Receiver mode
					 | CSL_FMKT(I2C_ICMDR_STT, SET)				// START Condition
					 | CSL_FMKT(I2C_ICMDR_STP, SET)				// STOP Condition
					 | CSL_FMKT(I2C_ICMDR_IRS, ENABLE);

	/* Write register address */
	for (i=0; i<regsize; i++)
	{
		do
		{
			i2cstatus = I2C0_Regs->ICSTR;
			if (i2cstatus & (CSL_I2C_ICSTR_AL_MASK|CSL_I2C_ICSTR_NACK_MASK))	goto i2cret;
		} while ((i2cstatus & CSL_I2C_ICSTR_ICXRDY_MASK) == 0);
		I2C0_Regs->ICDXR = regbuf[i];
	}

	/* Write data */
	for (i=0; i<n; i++)
	{
		do
		{
			i2cstatus = I2C0_Regs->ICSTR;
			if (i2cstatus & (CSL_I2C_ICSTR_AL_MASK|CSL_I2C_ICSTR_NACK_MASK))	goto i2cret;
		} while ((i2cstatus & CSL_I2C_ICSTR_ICXRDY_MASK) == 0);
		I2C0_Regs->ICDXR = buf[i];
	}

	/* Wait for transfer complete */
	do
	{
		i2cstatus = I2C0_Regs->ICSTR;
		if (i2cstatus & (CSL_I2C_ICSTR_AL_MASK|CSL_I2C_ICSTR_NACK_MASK))	goto i2cret;
	} while (CSL_FEXT(I2C0_Regs->ICMDR, I2C_ICMDR_STP)==1);

	status = true;

i2cret:
	/* Reset I2C status */
	CSL_FINST(I2C0_Regs->ICMDR, I2C_ICMDR_IRS, DISABLE);
	return status;
}


bool I2C_InternalRead(uint8_t addr, uint8_t* regbuf, uint8_t regsize, uint8_t* buf, size_t n)
{
	int i;
    bool status = false;
    int i2cstatus;

	/* Reset I2C status */
	CSL_I2cRegsOvly I2C0_Regs = (CSL_I2cRegsOvly) CSL_I2C_0_REGS;
	CSL_FINST(I2C0_Regs->ICMDR, I2C_ICMDR_IRS, DISABLE);

	/* Write Register address before */
 	if (regsize)
 	{
		/* Setup transfer */
		I2C0_Regs->ICSAR = addr;									// Slave address
		I2C0_Regs->ICCNT = regsize;									// Byte count
		I2C0_Regs->ICMDR = CSL_FMKT(I2C_ICMDR_MST, MASTER_MODE)		// Master mode
						 | CSL_FMKT(I2C_ICMDR_FREE, TRUE)			// Free mode
						 | CSL_FMKT(I2C_ICMDR_TRX, TX_MODE)			// Receiver mode
						 | CSL_FMKT(I2C_ICMDR_STT, SET)				// START Condition
						 | CSL_FMKT(I2C_ICMDR_IRS, ENABLE);

		/* Write register address */
		for (i=0; i<regsize; i++)
		{
			do
			{
				i2cstatus = I2C0_Regs->ICSTR;
				if (i2cstatus & (CSL_I2C_ICSTR_AL_MASK|CSL_I2C_ICSTR_NACK_MASK))	goto i2cret;
			} while ((i2cstatus & CSL_I2C_ICSTR_ICXRDY_MASK) == 0);
			I2C0_Regs->ICDXR = regbuf[i];
		}

		/* Wait for transfer complete */
		do
		{
			i2cstatus = I2C0_Regs->ICSTR;
			if (i2cstatus & (CSL_I2C_ICSTR_AL_MASK|CSL_I2C_ICSTR_NACK_MASK))	goto i2cret;
		} while ((i2cstatus & CSL_I2C_ICSTR_ARDY_MASK) == 0);
 	}

	/* Setup transfer */
	I2C0_Regs->ICCNT = n;										// Byte count
	I2C0_Regs->ICMDR = CSL_FMKT(I2C_ICMDR_MST, MASTER_MODE)		// Master mode
					 | CSL_FMKT(I2C_ICMDR_FREE, TRUE)			// Free mode
					 | CSL_FMKT(I2C_ICMDR_TRX, RX_MODE)			// Receiver mode
					 | CSL_FMKT(I2C_ICMDR_STT, SET)				// REPEAT START Condition
					 | CSL_FMKT(I2C_ICMDR_STP, SET)				// STOP Condition
					 | CSL_FMKT(I2C_ICMDR_IRS, ENABLE);

	/* Read data */
	for (i=0; i<n; i++)
	{
		do
		{
			i2cstatus = I2C0_Regs->ICSTR;
			if (i2cstatus & (CSL_I2C_ICSTR_AL_MASK|CSL_I2C_ICSTR_NACK_MASK))	goto i2cret;
		} while ((i2cstatus & CSL_I2C_ICSTR_ICRRDY_MASK) == 0);

		buf[i] = I2C0_Regs->ICDRR;
	}
 	status = true;

i2cret:
	/* Reset I2C status */
 	CSL_FINST(I2C0_Regs->ICMDR, I2C_ICMDR_IRS, DISABLE);
 	return status;
}

bool I2C_Write(uint8_t addr, const uint8_t* buf, size_t n)
{
	return I2C_InternalWrite(addr, NULL, 0, buf, n);
}

bool I2C_Reg8Write(uint8_t addr, uint8_t reg, const uint8_t* buf, size_t n)
{
	return I2C_InternalWrite(addr, &reg, 1, buf, n);
}

bool I2C_Reg16Write(uint8_t addr, uint16_t reg, const uint8_t* buf, size_t n)
{
	uint8_t regbuf[2];
	regbuf[0] = reg >> 8;
	regbuf[1] = reg & 0xff;
	return I2C_InternalWrite(addr, regbuf, 2, buf, n);
}

bool I2C_Read(uint8_t addr, uint8_t* buf, size_t n)
{
	return I2C_InternalRead(addr, NULL, 0, buf, n);
}

bool I2C_Reg8Read(uint8_t addr, uint8_t reg, uint8_t* buf, size_t n)
{
	return I2C_InternalRead(addr, &reg, 1, buf, n);
}

bool I2C_Reg16Read(uint8_t addr, uint16_t reg, uint8_t* buf, size_t n)
{
	uint8_t regbuf[2];
	regbuf[0] = reg>>8;
	regbuf[1] = reg&0xFF;
	return I2C_InternalRead(addr, &regbuf[0], 2, buf, n);
}
