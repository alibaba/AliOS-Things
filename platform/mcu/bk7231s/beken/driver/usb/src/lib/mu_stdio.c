/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2004              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
* Micro-stdio library
* $Revision: 1.4 $
*/
#include "include.h"

#include "mu_dsi.h"
#include "mu_stdio.h"

#if CFG_USB
/*
* implementation
*/
uint16_t MUSB_GetLine(char *pBuffer, uint16_t wBufferSize)
{
    return 0;
}

/*
* implementation
*/
uint8_t MUSB_PrintString(const char *pBuffer)
{
    return 0;
}

/*
* implementation
*/
uint8_t MUSB_PrintLine(const char *pBuffer)
{
    return TRUE;
}
#endif /*CFG_USB*/
// eof

