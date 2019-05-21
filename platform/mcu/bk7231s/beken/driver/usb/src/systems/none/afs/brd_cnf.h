/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2005              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

#ifndef __MUSB_NONE_BOARD_CONFIG_H__
#define __MUSB_NONE_BOARD_CONFIG_H__

extern void bk_printf(const char *fmt, ...);

#define MUSB_VBUS_ERROR_RETRIES             4
#define MUSB_MAX_CONTROLLERS	            1
#define	MUSB_TS_SESSREQ		                8	/* SRP, WAIT_A_BCON, WAIT_B_ACON */
#define	MUSB_MAX_TRANSITION_TM	            250	/* 250 msec for transition coalesce */
#define MUSB_TB_SRP_FAIL	                5
#define MUSB_FORCE_FULLSPEED
#define MUSB_ISO
#define	MUSB_FDRC
#define	MUSB_FDRC_AHB                       1

#endif	/* multiple inclusion protection */
// eof

