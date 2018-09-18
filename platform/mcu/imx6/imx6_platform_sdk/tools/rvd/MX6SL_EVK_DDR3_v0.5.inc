//=============================================================================			
//init script for i.MX6SL DDR3			
//=============================================================================			
// Revision History			
// v01			
//=============================================================================			
			
wait = on			
//=============================================================================			
// Disable	WDOG		
//=============================================================================			
setmem /16	0x020bc000 =	0x30	
			
//=============================================================================			
// Enable all clocks (they are disabled by ROM code)			
//=============================================================================			
setmem /32	0x020c4068 =	0xffffffff	
setmem /32	0x020c406c =	0xffffffff	
setmem /32	0x020c4070 =	0xffffffff	
setmem /32	0x020c4074 =	0xffffffff	
setmem /32	0x020c4078 =	0xffffffff	
setmem /32	0x020c407c =	0xffffffff	
setmem /32	0x020c4080 =	0xffffffff	
setmem /32	0x020c4084 =	0xffffffff	
			
setmem /32	0x020c4018 =	0x00260324	//DDR clk to 400MHz
			
			
//=============================================================================			
// IOMUX			
//=============================================================================			
//DDR IO TYPE:			
setmem /32	0x020e05c0 =	0x00020000	// IOMUXC_SW_PAD_CTL_GRP_DDRMODE
setmem /32	0x020e05b4 =	0x00000000	// IOMUXC_SW_PAD_CTL_GRP_DDRPKE 
			
//CLOCK:			
setmem /32	0x020e0338 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_0
			
//Control:			
setmem /32	0x020e0300 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS
setmem /32	0x020e031c =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS
setmem /32	0x020e0320 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET
setmem /32	0x020e032c =	0x00000000	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2 - DSE can be configured using Group Control Register: IOMUXC_SW_PAD_CTL_GRP_CTLDS
//setmem /32	0x020e033c =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT0
//setmem /32	0x020e0340 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT1
setmem /32	0x020e05ac =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_ADDDS 
setmem /32	0x020e05c8 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_CTLDS 
			
//Data Strobes:			
setmem /32	0x020e05b0 =	0x00020000	// IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL 
setmem /32	0x020e0344 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0 
setmem /32	0x020e0348 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS1 
setmem /32	0x020e034c =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS2 
setmem /32	0x020e0350 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS3 
			
//Data:			
setmem /32	0x020e05d0 =	0x000C0000	// IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE 
setmem /32	0x020e05c4 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B0DS 
setmem /32	0x020e05cc =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B1DS 
setmem /32	0x020e05d4 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B2DS 
setmem /32	0x020e05d8 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B3DS 
			
setmem /32	0x020e030c =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0
setmem /32	0x020e0310 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM1
setmem /32	0x020e0314 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM2
setmem /32	0x020e0318 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM3
			
//=============================================================================			
// DDR Controller Registers			
//=============================================================================			
// Manufacturer:	Micron		
// Device Part Number:	MT41J128M16HA-187E		
// Clock Freq.: 	400MHz		
// Density per CS in Gb: 	4		
// Chip Selects used:	2		
// Total DRAM density (Gb)	8		
// Number of Banks:	8		
// Row address:    	14		
// Column address: 	10		
// Data bus width	32		
//=============================================================================			
setmem /32	0x021b0800 =	0xa1390003 	// DDR_PHY_P0_MPZQHWCTRL, enable both one-time & periodic HW ZQ calibration.
			
// write leveling, based on Freescale board layout and T topology			
// For target board, may need to run write leveling calibration 			
// to fine tune these settings			
// If target board does not use T topology, then these registers			
// should either be cleared or write leveling calibration can be run			
setmem /32	0x021b080c = 	0x001F001F	
setmem /32	0x021b0810 = 	0x001F001F	
			
//######################################################			
//calibration values based on calibration compare of 0x00ffff00:			
//Note, these calibration values are based on Freescale's board			
//May need to run calibration on target board to fine tune these 			
//######################################################			
			
setmem /32	0x021b083c =	0x407E007F	// MPDGCTRL0 PHY0
setmem /32	0x021b0840 =	0x00690069	// MPDGCTRL1 PHY0
setmem /32	0x021b0848 =	0x42424645	// MPRDDLCTL PHY0
setmem /32	0x021b0850 =	0x3B383630	// MPWRDLCTL PHY0
			
			
setmem /32	0x021b081c =	0x33333333	// DDR_PHY_P0_MPREDQBY0DL3
setmem /32	0x021b0820 =	0x33333333	// DDR_PHY_P0_MPREDQBY1DL3
setmem /32	0x021b0824 =	0x33333333	// DDR_PHY_P0_MPREDQBY2DL3
setmem /32	0x021b0828 =	0x33333333	// DDR_PHY_P0_MPREDQBY3DL3
			
// Complete calibration by forced measurement:			
setmem /32	0x021b08b8 =	0x00000800 	// DDR_PHY_P0_MPMUR0, frc_msr
			
setmem /32	0x021b0004 =	0x00020024	// MMDC0_MDPDC 
setmem /32	0x021b0008 =	0x00333040	// MMDC0_MDOTC
setmem /32	0x021b000c =	0x3F435313	// MMDC0_MDCFG0
setmem /32	0x021b0010 =	0xB68E8B63	// MMDC0_MDCFG1
setmem /32	0x021b0014 =	0x01FF00DB	// MMDC0_MDCFG2
setmem /32	0x021b0018 =	0x00081740	// MMDC0_MDMISC
			
setmem /32	0x021b001c =	0x00008000	// MMDC0_MDSCR, set the Configuration request bit during MMDC set up
setmem /32	0x021b002c =	0x000026d2	// MMDC0_MDRWD; recommend to maintain the default values
setmem /32	0x021b0030 =	0x00431023	// MMDC0_MDOR
setmem /32	0x021b0040 =	0x0000004F	// CS0_END 
setmem /32	0x021b0000 =	0xC3190000	// MMDC0_MDCTL
// Mode register writes			
setmem /32	0x021b001c =	0x04008032	// MMDC0_MDSCR, MR2 write, CS0
setmem /32	0x021b001c =	0x00008033	// MMDC0_MDSCR, MR3 write, CS0
setmem /32	0x021b001c =	0x00048031	// MMDC0_MDSCR, MR1 write, CS0
setmem /32	0x021b001c =	0x05208030	// MMDC0_MDSCR, MR0 write, CS0
setmem /32	0x021b001c =	0x04008040	// MMDC0_MDSCR, ZQ calibration command sent to device on CS0
			
setmem /32	0x021b001c =	0x0400803A	// MMDC0_MDSCR, MR2 write, CS1
setmem /32	0x021b001c =	0x0000803B	// MMDC0_MDSCR, MR3 write, CS1
setmem /32	0x021b001c =	0x00048039	// MMDC0_MDSCR, MR1 write, CS1
setmem /32	0x021b001c =	0x05208038	// MMDC0_MDSCR, MR0 write, CS1
setmem /32	0x021b001c =	0x04008048	// MMDC0_MDSCR, ZQ calibration command sent to device on CS1
			
			
setmem /32	0x021b0020 =	0x00005800	// MMDC0_MDREF
setmem /32	0x021b0818 =	0x00011117	// DDR_PHY_P0_MPODTCTRL
			
setmem /32	0x021b0004 =	0x00025564	// MMDC0_MDPDC now SDCTL power down enabled
setmem /32	0x021b0404 =	0x00011006 	//MMDC0_MAPSR ADOPT power down enabled
			
setmem /32	0x021b001c =	0x00000000	// MMDC0_MDSCR, clear this register (especially the configuration bit as initialization is complete)
