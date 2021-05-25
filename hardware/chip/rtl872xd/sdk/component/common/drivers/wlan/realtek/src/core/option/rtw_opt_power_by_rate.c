/******************************************************************************
*                           PHY_REG_PG.TXT
******************************************************************************/

// Note: power index value = power*2
// => ex: 16dBm => 0x32 , 13dBm => 0x26, 0x26262830 => 13 13 14 15 dBm

unsigned int Array_MP_8195A_PHY_REG_PG[] = { 
	0, 0, 0, 0x00000e08, 0x0000ff00, 0x00003200,// 1M					(16 dBm)
	0, 0, 0, 0x0000086c, 0xffffff00, 0x32323200,// 11M 5.5M 2M			(16 16 16 dBm)
	0, 0, 0, 0x00000e00, 0xffffffff, 0x34363636,// 18M 12M 9M 6M		(17 18 18 18 dBm)
	0, 0, 0, 0x00000e04, 0xffffffff, 0x28303234,// 54M 48M 36M 24M		(14 15 16 17 dBm)
	0, 0, 0, 0x00000e10, 0xffffffff, 0x30343434,// MCS3 MCS2 MCS1 MCS0	(15 17 17 17 dBm)
	0, 0, 0, 0x00000e14, 0xffffffff, 0x26262830	// MCS7 MCS6 MCS5 MCS4	(13 13 14 15 dBm)
};

const unsigned int Array_MP_8711B_PHY_REG_PG[] = { 
	0, 0, 0, 0x00000e08, 0x0000ff00, 0x00003200,// 1M					(16 dBm)
	0, 0, 0, 0x0000086c, 0xffffff00, 0x32323200,// 11M 5.5M 2M			(16 16 16 dBm)
	0, 0, 0, 0x00000e00, 0xffffffff, 0x34363636,// 18M 12M 9M 6M		(17 18 18 18 dBm)
	0, 0, 0, 0x00000e04, 0xffffffff, 0x28303234,// 54M 48M 36M 24M		(14 15 16 17 dBm)
	0, 0, 0, 0x00000e10, 0xffffffff, 0x30343434,// MCS3 MCS2 MCS1 MCS0	(15 17 17 17 dBm)
	0, 0, 0, 0x00000e14, 0xffffffff, 0x26262830// MCS7 MCS6 MCS5 MCS4	(13 13 14 15 dBm)
};