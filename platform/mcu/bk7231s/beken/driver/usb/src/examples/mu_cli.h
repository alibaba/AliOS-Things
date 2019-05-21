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

#ifndef __MUSB_CLI_H__
#define __MUSB_CLI_H__

#include "mu_cdi.h"

/**
 * Demo-specific command handler.
 * The CLI demo calls this for the current client when
 * it does not recognize a command.
 * This is the client's opportunity to do something special,
 * (e.g. MSD host demo could list files, HID demo could use terminal
 * input to send keyboard events, etc.).
 * @param pLine NUL-terminated ASCII command line
 * @return TRUE if command known (not necessarily successful;
 * appropriate error messages should be printed by client)
 * @return FALSE if command unknown (CLI will print error saying this)
 */
typedef uint8_t (*MUSB_pfCliClientCommand)(const char *pLine);

/**
* MUSB_CliClient (CLI client).
* An instance of a client capable of working with the CLI.
* @field pszName client name for user reference
* @field pfCliClientCommand function to process commands
* not known by CLI; NULL if not implemented
* @field pFunctionClient CDI function client pointer
* @field pHostClient CDI host client pointer
* @field pOtgClient CDI OTG client pointer
*/
typedef struct
{
    const char *pszName;
    MUSB_pfCliClientCommand pfCliClientCommand;
    MUSB_FunctionClient *pFunctionClient;
    MUSB_HostClient *pHostClient;
    MUSB_OtgClient *pOtgClient;
} MUSB_CliClient;

#endif	/* multiple inclusion protection */
