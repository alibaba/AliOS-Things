#include <section_config.h>
#include <basic_types.h>

LIBC_ROM_TEXT_SECTION
_LONG_CALL_
u8 _char2num(u8 ch)
{
    if((ch>='0')&&(ch<='9'))
        return ch - '0';
    else if ((ch>='a')&&(ch<='f'))
        return ch - 'a' + 10;
    else if ((ch>='A')&&(ch<='F'))
        return ch - 'A' + 10;
    else
	 return 0xff;
}

LIBC_ROM_TEXT_SECTION
_LONG_CALL_
u8 _2char2dec(u8 hch, u8 lch)
{
	return ((_char2num(hch) * 10 ) + _char2num(lch));
}

LIBC_ROM_TEXT_SECTION
_LONG_CALL_
u8 _2char2hex(u8 hch, u8 lch)
{
	return ((_char2num(hch) << 4) | _char2num(lch));
}