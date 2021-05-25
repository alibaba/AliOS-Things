#include <stdio.h>
#include "ameba_soc.h"

#if !defined (__ICCARM__)
extern u8 RAM_IMG1_VALID_PATTEN[];
void *tmp = RAM_IMG1_VALID_PATTEN;
#endif

#if defined ( __ICCARM__ )
size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize)
{
 	int nChars = 0;
 	/* Check for stdout and stderr 
 	(only necessary if file descriptors are enabled.) */
 	if (Handle != 1 && Handle != 2)
 	{
 		return -1;
 	}
 	for (/*Empty */; Bufsize > 0; --Bufsize)
 	{
 		DiagPutChar(*Buf++);
 		++nChars;
 	}
 	return nChars;
}

size_t __read(int Handle, unsigned char * Buf, size_t Bufsize)
{
	int nChars = 0;
	/* Check for stdin
	(only necessary if FILE descriptors are enabled) */
	if (Handle != 0)
	{
		return -1;
	}
	for (/*Empty*/; Bufsize > 0; --Bufsize)
	{
		int c = DiagGetChar(_FALSE);
		if (c < 0)
			break;
		*(Buf++) = c;
		++nChars;
	}
	return nChars;	
}
#else
int _write(int file, char * ptr, int len)
{
 	int nChars = 0;
 	/* Check for stdout and stderr 
 	(only necessary if file descriptors are enabled.) */
 	if (file != 1 && file != 2)
 	{
 		return -1;
 	}
 	for (/*Empty */; len > 0; --len)
 	{
 		DiagPutChar(*ptr++);
 		++nChars;
 	}
 	return nChars;
}

int _read(int file, char * ptr, int len)
{
	int nChars = 0;
	/* Check for stdin
	(only necessary if FILE descriptors are enabled) */
	if (file != 0)
	{
		return -1;
	}
	for (/*Empty*/; len > 0; --len)
	{
		int c = DiagGetChar(_TRUE);
		if ((c < 0) || (c == '\r')) {
			*ptr = '\0';
			break;
		}
		*(ptr++) = c;
		++nChars;
	}
	return nChars;	
}
#endif
