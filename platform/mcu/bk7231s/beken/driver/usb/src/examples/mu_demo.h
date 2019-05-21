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

#ifndef __MUSB_DEMO_H__
#define __MUSB_DEMO_H__

/**
 * Demo entrypoint.
 * Demo applications define this entrypoint so they are
 * after the system is initialized.
 * It is not expected to return.
 *
 * @param argc ANSI argc, reserved for possible future use
 * @param argv ANSI argv, reserved for possible future use
 */
extern void MUSB_DemoEntry(int argc, char *argv[]);

#endif	/* multiple inclusion protection */
