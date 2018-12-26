//font.c
//font related code, table based implementation strategy!!
//
#include "type.h"
#include "app_config.h"

#if (defined(FUNC_FONT_EN))

extern const uint8_t gFontAsci[];
extern const uint8_t gFontHzk16[];


//return 1 means OK
//return 0 means error found
bool GetOneAsci(uint8_t Asc, uint8_t* DataOut/*16B*/)
{
	register uint32_t i;
	uint8_t* FontPos;

	FontPos = (uint8_t*)gFontAsci + Asc * 16;
	for(i = 0; i < 16; i++)
	{
		*DataOut++ = *FontPos++;
	}

	return 1;
}

//
//return 1 means OK
//return 0 means error found
bool GetOneHz16(uint8_t* Hz, uint8_t* DataOut/*32B*/)
{
	register uint32_t i;
	uint8_t* FontPos;
	uint32_t q, w;

	q = Hz[0];
	w = Hz[1];

	if((q < 0xa1) || (w < 0xa1))
	{
		return 0;//not in the arry
	}

	q -= 0xa1;
	w -= 0xa1;

	FontPos = (uint8_t*)gFontHzk16 + (q * 94 + w) * 32;
	for(i = 0; i < 32; i++)
	{
		*DataOut++ = *FontPos++;
	}

	return 1;
}

//
//return 1 means OK
//return 0 means error found
bool GetOneCharacter(uint8_t* String, uint8_t* DataOut, uint32_t* FilledDataSize, uint32_t* CostStringSize)
{
	bool Rv;

	if(String[0] >= 0x80)//hz
	{
		*FilledDataSize = 32;
		*CostStringSize =  2;

		Rv = GetOneHz16(String, DataOut);
	}
	else
	{
		*FilledDataSize = 16;
		*CostStringSize =  1;

		Rv = GetOneAsci(*String, DataOut);
	}

	return Rv;
}
#endif


