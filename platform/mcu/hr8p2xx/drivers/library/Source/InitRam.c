/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/

extern unsigned long _sidata;
extern unsigned long __data_start__;
extern unsigned long __data_end__;
extern unsigned long __bss_start__;
extern unsigned long __bss_end__;

extern unsigned long _my_ram_end;
extern unsigned long _my_ram_end_d;
extern unsigned long _my_rom_end;

void  InitRam(void)
{
    unsigned long volatile *pulSrc, *pulDest;
    unsigned int volatile _my_ram_len;

    pulSrc = &_sidata;
    for(pulDest = (unsigned long volatile *)&__data_start__; pulDest <(unsigned long volatile *)&__data_end__; )
    {
        *(pulDest++) = *(pulSrc++);
    }

    for(pulDest = (unsigned long volatile *)&__bss_start__; pulDest < (unsigned long volatile *)&__bss_end__; )
    {
        *(pulDest++) = 0;
    }

    _my_ram_len = (unsigned int)&_my_ram_end_d - (unsigned int)&_my_ram_end;
    memcpy(&_my_ram_end, &_my_rom_end, _my_ram_len);
}
