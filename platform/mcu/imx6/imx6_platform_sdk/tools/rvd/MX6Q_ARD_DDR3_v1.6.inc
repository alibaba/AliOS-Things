//=============================================================================			
//init script for i.MX6Q DDR3 ARD			
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
			
//=============================================================================			
// IOMUX			
//=============================================================================			
//DDR IO TYPE:			
setmem /32	0x020e0798 =	0x000C0000	// IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE 
setmem /32	0x020e0758 =	0x00000000	// IOMUXC_SW_PAD_CTL_GRP_DDRPKE 
			
//CLOCK:			
setmem /32	0x020e0588 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_0
setmem /32	0x020e0594 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_1
			
//ADDRESS:			
setmem /32	0x020e056c =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS
setmem /32	0x020e0578 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS
setmem /32	0x020e074c =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_ADDDS 
			
//CONTROL:			
setmem /32	0x020e057c =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET
			
setmem /32	0x020e058c =	0x00000000	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2 - DSE can be configured using Group Control Register: IOMUXC_SW_PAD_CTL_GRP_CTLDS
setmem /32	0x020e059c =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT0
setmem /32	0x020e05a0 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT1
setmem /32	0x020e078c =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_CTLDS 
			
//DATA STROBE:			
setmem /32	0x020e0750 =	0x00020000	// IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL 
			
setmem /32	0x020e05a8 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0 
setmem /32	0x020e05b0 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS1 
setmem /32	0x020e0524 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS2 
setmem /32	0x020e051c =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS3 
setmem /32	0x020e0518 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS4 
setmem /32	0x020e050c =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS5 
setmem /32	0x020e05b8 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS6 
setmem /32	0x020e05c0 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS7 
			
//DATA:			
setmem /32	0x020e0774 =	0x00020000	// IOMUXC_SW_PAD_CTL_GRP_DDRMODE
			
setmem /32	0x020e0784 =	0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B0DS 
setmem /32	0x020e0788 =	0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B1DS 
setmem /32	0x020e0794 =	0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B2DS 
setmem /32	0x020e079c =	0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B3DS 
setmem /32	0x020e07a0 =	0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B4DS 
setmem /32	0x020e07a4 =	0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B5DS 
setmem /32	0x020e07a8 =	0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B6DS 
setmem /32	0x020e0748 =	0x00000028	// IOMUXC_SW_PAD_CTL_GRP_B7DS 
			
setmem /32	0x020e05ac =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0
setmem /32	0x020e05b4 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM1
setmem /32	0x020e0528 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM2
setmem /32	0x020e0520 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM3
setmem /32	0x020e0514 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM4
setmem /32	0x020e0510 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM5
setmem /32	0x020e05bc =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM6
setmem /32	0x020e05c4 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM7
			
//=============================================================================			
// DDR Controller Registers			
//=============================================================================			
// Manufacturer:	Micron		
// Device Part Number:	MT41K256M16-125 D9PXV DDR3-1600		
// Clock Freq.: 	533MHz		
// Density per CS in Gb: 	16		
// Chip Selects used:	1		
// Number of Banks:	8		
// Row address:    	15		
// Column address: 	10		
// Data bus width	64		
//=============================================================================			
setmem /32	0x021b0800 =	0xa1390003 	// DDR_PHY_P0_MPZQHWCTRL, enable both one-time & periodic HW ZQ calibration.
			
// write leveling, based on Freescale board layout and T topology			
// For target board, may need to run write leveling calibration 			
// to fine tune these settings			
// If target board does not use T topology, then these registers			
// should either be cleared or write leveling calibration can be run			
setmem /32	0x021b080c = 	0x001F001F	
setmem /32	0x021b0810 = 	0x001F001F	
setmem /32	0x021b480c = 	0x001F001F	
setmem /32	0x021b4810 = 	0x001F001F	
			
//######################################################			
//calibration values based on calibration compare of 0x00ffff00:			
//Note, these calibration values are based on Freescale's board			
//May need to run calibration on target board to fine tune these 			
//######################################################			
			
//Read DQS Gating calibration			
setmem /32	0x021b083c =	0x43260335	// MPDGCTRL0 PHY0
setmem /32	0x021b0840 =	0x031A030B	// MPDGCTRL1 PHY0
setmem /32	0x021b483c =	0x4323033B	// MPDGCTRL0 PHY1
setmem /32	0x021b4840 =	0x0323026F	// MPDGCTRL1 PHY1
			
//Read calibration			
setmem /32	0x021b0848 =	0x483D4545	// MPRDDLCTL PHY0
setmem /32	0x021b4848 =	0x44433E48	// MPRDDLCTL PHY1
			
//Write calibration			
setmem /32	0x021b0850 =	0x41444840	// MPWRDLCTL PHY0
setmem /32	0x021b4850 =	0x4835483E	// MPWRDLCTL PHY1
			
//read data bit delay: (3 is the reccommended default value, although out of reset value is 0):			
setmem /32	0x021b081c =	0x33333333	// DDR_PHY_P0_MPREDQBY0DL3
setmem /32	0x021b0820 =	0x33333333	// DDR_PHY_P0_MPREDQBY1DL3
setmem /32	0x021b0824 =	0x33333333	// DDR_PHY_P0_MPREDQBY2DL3
setmem /32	0x021b0828 =	0x33333333	// DDR_PHY_P0_MPREDQBY3DL3
setmem /32	0x021b481c =	0x33333333	// DDR_PHY_P1_MPREDQBY0DL3
setmem /32	0x021b4820 =	0x33333333	// DDR_PHY_P1_MPREDQBY1DL3
setmem /32	0x021b4824 =	0x33333333	// DDR_PHY_P1_MPREDQBY2DL3
setmem /32	0x021b4828 =	0x33333333	// DDR_PHY_P1_MPREDQBY3DL3
			
//For i.mx6qd parts of versions A & B (v1.0, v1.1), uncomment the following lines. For version C (v1.2), keep commented			
//setmem /32	0x021b08c0 =	0x24911492	// fine tune SDCLK duty cyc to low - seen to improve measured duty cycle of i.mx6
//setmem /32	0x021b48c0 =	0x24911492	
			
// Complete calibration by forced measurement:			
setmem /32	0x021b08b8 =	0x00000800 	// DDR_PHY_P0_MPMUR0, frc_msr
setmem /32	0x021b48b8 =	0x00000800 	// DDR_PHY_P0_MPMUR0, frc_msr
			
//MMDC init:			
setmem /32	0x021b0004 =	0x00020036	// MMDC0_MDPDC 
setmem /32	0x021b0008 =	0x09444040	// MMDC0_MDOTC
setmem /32	0x021b000c =	0x8A8F7955	// MMDC0_MDCFG0
setmem /32	0x021b0010 =	0xFF328F64	// MMDC0_MDCFG1
setmem /32	0x021b0014 =	0x01FF00DB	// MMDC0_MDCFG2
setmem /32	0x021b0018 =	0x00001740	// MMDC0_MDMISC
//NOTE about MDMISC RALAT:			
//MDMISC: RALAT kept to the high level of 5 to ensure stable operation at 528MHz. 			
//MDMISC: consider reducing RALAT if your 528MHz board design allow that. Lower RALAT benefits: 			
//a. better operation at low frequency			
//b. Small performence improvment			
			
setmem /32	0x021b001c =	0x00008000	// MMDC0_MDSCR, set the Configuration request bit during MMDC set up
setmem /32	0x021b002c =	0x000026d2	// MMDC0_MDRWD; recommend to maintain the default values
setmem /32	0x021b0030 =	0x008F1023	// MMDC0_MDOR
setmem /32	0x021b0040 =	0x00000047	// CS0_END 
			
setmem /32	0x021b0000 =	0x841A0000	// MMDC0_MDCTL
			
// Mode register writes			
setmem /32	0x021b001c =	0x04088032	// MMDC0_MDSCR, MR2 write, CS0
setmem /32	0x021b001c =	0x00008033	// MMDC0_MDSCR, MR3 write, CS0
setmem /32	0x021b001c =	0x00048031	// MMDC0_MDSCR, MR1 write, CS0
setmem /32	0x021b001c =	0x09408030	// MMDC0_MDSCR, MR0 write, CS0
setmem /32	0x021b001c =	0x04008040	// MMDC0_MDSCR, ZQ calibration command sent to device on CS0
			
//setmem /32	0x021b001c =	0x0408803A	// MMDC0_MDSCR, MR2 write, CS1
//setmem /32	0x021b001c =	0x0000803B	// MMDC0_MDSCR, MR3 write, CS1
//setmem /32	0x021b001c =	0x00048039	// MMDC0_MDSCR, MR1 write, CS1
//setmem /32	0x021b001c =	0x09408038	// MMDC0_MDSCR, MR0 write, CS1
//setmem /32	0x021b001c =	0x04008048	// MMDC0_MDSCR, ZQ calibration command sent to device on CS1
			
			
setmem /32	0x021b0020 =	0x00005800	// MMDC0_MDREF
setmem /32	0x021b0818 =	0x00011117	// DDR_PHY_P0_MPODTCTRL
setmem /32	0x021b4818 =	0x00011117	// DDR_PHY_P1_MPODTCTRL
			
			
setmem /32	0x021b0004 =	0x00025576	// MMDC0_MDPDC with PWDT bits set
setmem /32	0x021b0404 = 	0x00011006	// MMDC0_MAPSR ADOPT power down enabled, MMDC will enter automatically to self-refresh while the number of idle cycle reached.
			
setmem /32	0x021b001c =	0x00000000	// MMDC0_MDSCR, clear this register (especially the configuration bit as initialization is complete)
			
