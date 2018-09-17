//*================================================================================================
//* Copyright (C) 2011, Freescale Semiconductor, Inc. All Rights Reserved
//* THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
//* BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
//* Freescale Semiconductor, Inc.
//*================================================================================================


// DDR init script, written in ARM RVDS syntax
// Target the CPUDDR3 board
// Timing optimized to 400MHz. 64-bit data bus 


// Initialization script for i.MX6dl CPU Board (DDR3)
// Version 1.0 (JAN 30 ,2012)

// - based on Boaz's MX61 DDR3 script(528M)

// Version 1.1 (Feb 7 ,2012)
// - update DDR CFGs for 400M

// Version 1.2 (Feb 17 ,2012)
// - update IOMUX issue
// 
//*================================================================================================



wait = on

//*================================================================================================
// Disable WDOG
//*================================================================================================
setmem /16 0x020bc000 = 0x30


//*================================================================================================
// Enable all clocks (they are disabled by ROM code)
//*================================================================================================
setmem /32 0x020c4068 = 0xffffffff
setmem /32 0x020c406c = 0xffffffff
setmem /32 0x020c4070 = 0xffffffff
setmem /32 0x020c4074 = 0xffffffff
setmem /32 0x020c4078 = 0xffffffff
setmem /32 0x020c407c = 0xffffffff
setmem /32 0x020c4080 = 0xffffffff
setmem /32 0x020c4084 = 0xffffffff


//*================================================================================================
// Initialize 64-bit DDR3 
//*================================================================================================

//######################################################
// IOMUX 
//######################################################

//DDR IO TYPE:
setmem /32 0x020e0774 = 0x000c0000 // IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE - DDR_SEL=11 
setmem /32 0x020e0754 = 0x00000000 // IOMUXC_SW_PAD_CTL_GRP_DDRPKE - PKE=0 , Pull disabled for all, except DQS.

//CLOCK:
setmem /32 0x020e04ac = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_0 - DSE=101, DDR_INPUT=0, HYS=0
setmem /32 0x020e04b0 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_1 - DSE=101, DDR_INPUT=0, HYS=0

//ADDRESS:
setmem /32 0x020e0464 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0490 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e074c = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_ADDDS - DSE=110

//CONTROL:
setmem /32 0x020e0494 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET - DSE=110, DDR_INPUT=1, HYS=0, DDR_SEL=00
setmem /32 0x020e04a4 = 0x00003000 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCKE0
setmem /32 0x020e04a8 = 0x00003000 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCKE1
setmem /32 0x020e04a0 = 0x00000000 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2
setmem /32 0x020e04b4 = 0x00003030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT0
setmem /32 0x020e04b8 = 0x00003030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT1
setmem /32 0x020e076c = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_CTLDS - DSE=110


//DATA STROBE:
setmem /32 0x020e0750 = 0x00020000 // IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL - DDR_INPUT=1

setmem /32 0x020e04bc = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0 - DSE=110
setmem /32 0x020e04c0 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS1 - DSE=110
setmem /32 0x020e04c4 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS2 - DSE=110
setmem /32 0x020e04c8 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS3 - DSE=110
setmem /32 0x020e04cc = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS4 - DSE=110
setmem /32 0x020e04d0 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS5 - DSE=110
setmem /32 0x020e04d4 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS6 - DSE=110
setmem /32 0x020e04d8 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS7 - DSE=110

//DATA:
setmem /32 0x020e0760 = 0x00020000 // IOMUXC_SW_PAD_CTL_GRP_DDRMODE- DDR_INPUT 1,diff

setmem /32 0x020e0764 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B0DS - DSE=110
setmem /32 0x020e0770 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B1DS - DSE=110
setmem /32 0x020e0778 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B2DS - DSE=110
setmem /32 0x020e077c = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B3DS - DSE=110
setmem /32 0x020e0780 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B4DS - DSE=110
setmem /32 0x020e0784 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B5DS - DSE=110
setmem /32 0x020e078c = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B6DS - DSE=110
setmem /32 0x020e0748 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B7DS - DSE=110

setmem /32 0x020e0470 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0474 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM1 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0478 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM2 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e047c = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM3 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0480 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM4 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0484 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM5 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0488 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM6 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e048c = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM7 - DSE=110, DDR_INPUT=1, HYS=0



//######################################################
//Calibrations:
//######################################################
// ZQ:
setmem /32 0x021b0800 = 0xa1390003      // DDR_PHY_P0_MPZQHWCTRL, enable both one-time & periodic HW ZQ calibration.
setmem /32 0x021b4800 = 0xa1390003      // DDR_PHY_P0_MPZQHWCTRL

// write leveling
setmem /32 0x021b080c = 0x00340038
setmem /32 0x021b0810 = 0x003b0040

setmem /32 0x021b480c = 0x0048004c
setmem /32 0x021b4810 = 0x0052006d

//DQS gating, read delay, write delay calibration values based on calibration compare of 0x00ffff00:

setmem /32 0x021b083c = 0x42070201
setmem /32 0x021b0840 = 0x0177017d
setmem /32 0x021b483c = 0x4203021d
setmem /32 0x021b4840 = 0x02180225
setmem /32 0x021b0848 = 0x474a4a49
setmem /32 0x021b4848 = 0x4a4d4b48
setmem /32 0x021b0850 = 0x403f3533
setmem /32 0x021b4850 = 0x3136372f


//read data bit delay: (3 is the reccommended default value, although out of reset value is 0):
setmem /32 0x021b081c = 0x33333333      // DDR_PHY_P0_MPREDQBY0DL3
setmem /32 0x021b0820 = 0x33333333      // DDR_PHY_P0_MPREDQBY1DL3
setmem /32 0x021b0824 = 0x33333333      // DDR_PHY_P0_MPREDQBY2DL3
setmem /32 0x021b0828 = 0x33333333      // DDR_PHY_P0_MPREDQBY3DL3
setmem /32 0x021b481c = 0x33333333      // DDR_PHY_P1_MPREDQBY0DL3
setmem /32 0x021b4820 = 0x33333333      // DDR_PHY_P1_MPREDQBY1DL3
setmem /32 0x021b4824 = 0x33333333      // DDR_PHY_P1_MPREDQBY2DL3
setmem /32 0x021b4828 = 0x33333333      // DDR_PHY_P1_MPREDQBY3DL3

//######################################################
// Complete calibration by forced measurment:
//######################################################
setmem /32 0x021b08b8 = 0x00000800      // DDR_PHY_P0_MPMUR0, frc_msr
setmem /32 0x021b48b8 = 0x00000800      // DDR_PHY_P0_MPMUR0, frc_msr

//######################################################
//MMDC init:

//in DDR3, 64-bit mode, only MMDC0 is initiated:
setmem /32 0x021b0004 = 0x0002002d		// MMDC0_MDPDC see spread sheet for timings
setmem /32 0x021b0008 = 0x00333030		// MMDC0_MDOTC see spread sheet for timings
setmem /32 0x021b000c = 0x40445323		// MMDC0_MDCFG0 see spread sheet for timings. CL=6
setmem /32 0x021b0010 = 0xb66e8c63		// MMDC0_MDCFG1 see spread sheet for timings
setmem /32 0x021b0014 = 0x01ff00db      // MMDC0_MDCFG2 - tRRD - 4ck; tWTR - 4ck; tRTP - 4ck; tDLLK - 512ck
setmem /32 0x021b0018 = 0x00081740      // MMDC0_MDMISC, RALAT=0x5

setmem /32 0x021b001c = 0x00008000      // MMDC0_MDSCR

setmem /32 0x021b002c = 0x000026d2      // MMDC0_MDRWD
setmem /32 0x021b0030 = 0x00440e21      // MMDC0_MDOR - tXPR - 69ck; SDE_to_RST - 13ck; RST_to_CKE - 32ck
setmem /32 0x021b0040 = 0x00000027      // CS0_END - 0x4fffffff 

setmem /32 0x021b0000 = 0xc31a0000      // MMDC0_MDCTL - row - 14bits; col = 10bits; burst length 8; 64-bit data bus



//######################################################
// Initialize 2GB DDR3 - Micron MT41J128M 
//MR2:
setmem /32 0x021b001c = 0x04008032      // MMDC0_MDSCR
setmem /32 0x021b001c = 0x0400803a      // MMDC0_MDSCR
//MR3:
setmem /32 0x021b001c = 0x00008033      // MMDC0_MDSCR
setmem /32 0x021b001c = 0x0000803b      // MMDC0_MDSCR
//MR1:
setmem /32 0x021b001c = 0x00428031      // MMDC0_MDSCR
setmem /32 0x021b001c = 0x00428039      // MMDC0_MDSCR
//MR0:
setmem /32 0x021b001c = 0x07208030      // MMDC0_MDSCR
setmem /32 0x021b001c = 0x07208038      // MMDC0_MDSCR

//DDR device ZQ calibration:
setmem /32 0x021b001c = 0x04008040      // MMDC0_MDSCR, 
setmem /32 0x021b001c = 0x04008048      // MMDC0_MDSCR
//######################################################
//final DDR setup, before operation start:

setmem /32 0x021b0020 = 0x00005800      // MMDC0_MDREF, enable auto refresh, set refresh rate.

setmem /32 0x021b0818 = 0x00022227      // DDR_PHY_P0_MPODTCTRL, ODT enable
setmem /32 0x021b4818 = 0x00022227      // DDR_PHY_P1_MPODTCTRL

setmem /32 0x021b0004 = 0x0002556d		// MMDC0_MDPDC see spread sheet for timings, SDCTL power down enabled

setmem /32 0x021b0404 = 0x00011006      //MMDC0_MAPSR ADOPT power down enabled

setmem /32 0x021b001c = 0x00000000      // MMDC0_MDSCR

