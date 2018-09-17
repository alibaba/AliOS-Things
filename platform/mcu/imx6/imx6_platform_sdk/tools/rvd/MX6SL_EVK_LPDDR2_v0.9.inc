//=============================================================================			
//init script for i.MX6SL LPDDR2			
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
setmem /32	0x020e0338 =	0x00000028	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_0
			
//Control:			
setmem /32	0x020e0300 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS
setmem /32	0x020e031c =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS
setmem /32	0x020e0320 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET
setmem /32	0x020e032c =	0x00000000	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2 - DSE can be configured using Group Control Register: IOMUXC_SW_PAD_CTL_GRP_CTLDS
//setmem /32	0x020e033c =	0x00000008	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT0
//setmem /32	0x020e0340 =	0x00000008	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT1
setmem /32	0x020e05ac =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_ADDDS 
setmem /32	0x020e05c8 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_CTLDS 
			
//Data Strobes:			
setmem /32	0x020e05b0 =	0x00020000	// IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL 
setmem /32	0x020e0344 =	0x00003030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0 
setmem /32	0x020e0348 =	0x00003030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS1 
setmem /32	0x020e034c =	0x00003030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS2 
setmem /32	0x020e0350 =	0x00003030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS3 
			
//Data:			
setmem /32	0x020e05d0 =	0x00080000	// IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE 
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
// Manufacturer:	Samsung		
// Device Part Number:	K4P8G304EB-AGC1		
// Clock Freq.: 	400MHz		
// Density per CS in Gb: 	4		
// Chip Selects used:	2		
// Total DRAM density (Gb)	8		
// Number of Banks:	8		
// Row address:    	14		
// Column address: 	10		
// Data bus width	32		
//=============================================================================			
			
// MMDC0_MDSCR, set the Configuration request bit during MMDC set up			
setmem /32	0x021b001c =	0x00008000	// Chan 0
			
setmem /32	0x021b085c = 	0x1b4700c7	//LPDDR2 ZQ params
			
//=============================================================================			
// Calibration setup.			
// 			
//=============================================================================			
setmem /32	0x021b0800 =	0xa1390003	// DDR_PHY_P0_MPZQHWCTRL, enable one time ZQ calibration
			
setmem /32 0x021b0890 = 0x00300000 //ca bus abs delay 			
			
setmem /32	0x021b08b8 = 	0x00000800	//frc_msr.
			
// read delays, settings recommended by design to remain constant			
setmem /32	0x021b081c =	0x33333333	// DDR_PHY_P0_MPREDQBY0DL3
setmem /32	0x021b0820 =	0x33333333	// DDR_PHY_P0_MPREDQBY1DL3
setmem /32	0x021b0824 =	0x33333333	// DDR_PHY_P0_MPREDQBY2DL3
setmem /32	0x021b0828 =	0x33333333	// DDR_PHY_P0_MPREDQBY3DL3
			
// write delays, settings recommended by design to remain constant			
setmem /32	0x021b082c =	0xf3333333	//DDR_PHY_P0 all byte 0 data & dm delayed by 3
setmem /32	0x021b0830 =	0xf3333333	//DDR_PHY_P0 all byte 0 data & dm delayed by 3
setmem /32	0x021b0834 =	0xf3333333	//DDR_PHY_P0 all byte 0 data & dm delayed by 3
setmem /32	0x021b0838 =	0xf3333333	//DDR_PHY_P0 all byte 0 data & dm delayed by 3
			
// Read and write data delay, per byte. 			
// For optimized DDR operation it is recommended to run mmdc_calibration on your board, and replace 4 delay register assigns with resulted values 			
// Note:			
// a. DQS gating is not relevant for LPDDR2. DSQ gating calibration section should be skipped, or the following write/read calibration will stall			
// b. The calibration code that runs for both MMDC0 & MMDC1 should be used.			
setmem /32	0x021b0848 =	0x4241444a	// MPRDDLCTL PHY0
			
setmem /32	0x021b0850 =	0x3030312b	// MPWRDLCTL PHY0
			
setmem /32	0x021b083c =	0x20000000	//PHY0 dqs gating dis
setmem /32	0x021b0840 =	0x0	
			
setmem /32	0x021b08b8 = 	0x00000800	//frc_msr.
//=============================================================================			
// Calibration setup end			
//=============================================================================			
			
// Channel0 - starting address 0x80000000			
setmem /32	0x021b000c =	0x33374133	// MMDC0_MDCFG0
setmem /32	0x021b0004 =	0x00020024	// MMDC0_MDPDC
setmem /32	0x021b0010 =	0x00100A82	// MMDC0_MDCFG1
setmem /32	0x021b0014 =	0x00000093	// MMDC0_MDCFG2
setmem /32	0x021b0018 =	0x00001688	// MMDC0_MDMISC
setmem /32	0x021b002c =	0x0F9F26D2	// MMDC0_MDRWD
setmem /32	0x021b0030 =	0x0000020E	// MMDC0_MDOR
setmem /32	0x021b0038 =	0x00190778	// MMDC0_MDCFG3LP
setmem /32	0x021b0008 =	0x00000000	// MMDC0_MDOTC
setmem /32	0x021b0040 =	0x0000004F	// Chan0 CS0_END 
setmem /32	0x021b0000 =	0xC3110000	// MMDC0_MDCTL
			
//=============================================================================			
// LPDDR2 Mode Register Writes			
//=============================================================================			
// Channel 0 CS0			
setmem /32	0x021b001c =	0x003F8030	// MRW: BA=0 CS=0 MR_ADDR=63 MR_OP=0 (Reset)
setmem /32	0x021b001c =	0xFF0A8030	// MRW: BA=0 CS=0 MR_ADDR=10 MR_OP=0xff (IO calibration, calibration code)
setmem /32	0x021b001c =	0x82018030	// MRW: BA=0 CS=0 MR_ADDR=1  MR_OP=see Register Configuration
setmem /32	0x021b001c =	0x04028030	// MRW: BA=0 CS=0 MR_ADDR=2  MR_OP=see Register Configuration
setmem /32	0x021b001c =	0x02038030	// MRW: BA=0 CS=0 MR_ADDR=3  MR_OP=see Register Configuration
// Channel 0 CS1			
//setmem /32	0x021b001c =	0x003F8038	// MRW: BA=0 CS=1 MR_ADDR=63 MR_OP=0 (Reset)
setmem /32	0x021b001c =	0xFF0A8038	// MRW: BA=0 CS=1 MR_ADDR=10 MR_OP=0xff (IO calibration, calibration code)
setmem /32	0x021b001c =	0x82018038	// MRW: BA=0 CS=1 MR_ADDR=1  MR_OP=see Register Configuration
setmem /32	0x021b001c =	0x04028038	// MRW: BA=0 CS=1 MR_ADDR=2  MR_OP=see Register Configuration
setmem /32	0x021b001c =	0x02038038	// MRW: BA=0 CS=1 MR_ADDR=3  MR_OP=see Register Configuration
			
			
//######################################################			
//final DDR setup, before operation start:			
setmem /32	0x021b0800 =	0xa1310003	// DDR_PHY_P0_MPZQHWCTRL, enable automatic ZQ calibration
			
setmem /32	0x021b0020 =	0x00001800	// MMDC0_MDREF
			
setmem /32	0x021b0818 =	0x0	// DDR_PHY_P0_MPODTCTRL
			
setmem /32	0x021b08b8 =	0x00000800	// DDR_PHY_P0_MPMUR0, frc_msr
			
setmem /32	0x021b0004 =	0x00025564	// MMDC0_MDPDC now SDCTL power down enabled
			
setmem /32	0x021b0404 =	0x00011006 	//MMDC0_MAPSR ADOPT power down enabled
			
setmem /32	0x021b001c =	0x00000000	// MMDC0_MDSCR, clear this register (especially the configuration bit as initialization is complete)
