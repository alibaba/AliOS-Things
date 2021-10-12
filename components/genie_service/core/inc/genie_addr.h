#ifndef __GENIE_ADDR_H__
#define __GENIE_ADDR_H__

typedef enum _genie_addr
{
    GENIE_ADDR_MIN = 0xc000,
    GENIE_ADDR_LIGHT = 0xc000,
    GENIE_ADDR_SWITCH = 0xc001,
    GENIE_ADDR_SOCKET = 0xc002,
    GENIE_ADDR_CURTAIN = 0xc003,
    GENIE_ADDR_BODY_SCALE = 0xc004,
    GENIE_ADDR_BUTTON = 0xc005,

    GENIE_ADDR_MAX = 0xcfff
} genie_addr_e;

#endif
