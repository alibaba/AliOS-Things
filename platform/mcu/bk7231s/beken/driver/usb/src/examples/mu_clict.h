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

#ifndef __MUSB_CLI_CLIENTS_H__
#define __MUSB_CLI_CLIENTS_H__

/*
 * This is included only by the CLI.
 * Its purpose is to define which clients to build into the CLI.
 */

#include "mu_cli.h"

/* TODO: #include "demo_cnf.h"*/

#if defined(MUSB_DEMO_HID) || defined(MUSB_DEMO_ALL)
#include "mu_demhid.h"
#endif

#if defined(MUSB_DEMO_MSD) || defined(MUSB_DEMO_ALL)
#include "msd/mu_demmsd.h"
#endif

#if defined(MUSB_DEMO_AUDIO) || defined(MUSB_DEMO_ALL)
#include "mu_demaud.h"
#endif

#if defined(MUSB_DEMO_VIDEO) || defined(MUSB_DEMO_ALL)
#include "mu_demvid.h"
#endif

static MUSB_CliClient *MGC_apCliClient[] =
{
#if defined(MUSB_DEMO_HID) || defined(MUSB_DEMO_ALL)
    &MGC_DemoHidCliClient,
#endif

#if defined(MUSB_DEMO_MSD) || defined(MUSB_DEMO_ALL)
    &MGC_DemoMsdCliClient,
#endif

#if defined(MUSB_DEMO_AUDIO) || defined(MUSB_DEMO_ALL)
    &MGC_DemoAudioCliClient,
#endif

#if defined(MUSB_DEMO_VIDEO) || defined(MUSB_DEMO_ALL)
    &MGC_DemoVideoCliClient,
#endif

    0,
};

#endif	/* multiple inclusion protection */
