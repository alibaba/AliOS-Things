/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *******************************************************************************/

#ifndef __RTL8195A_GSPI_H__
#define __RTL8195A_GSPI_H__

#define SPI_LOCAL_DOMAIN 				0x0
#define SPI_TXFIFO_DOMAIN 				0xc
#define SPI_RXFIFO_DOMAIN 				0x1f

//IO Bus domain address mapping
#define DEFUALT_OFFSET				0x0
#define SPI_LOCAL_OFFSET    				0x10250000
#define SPI_TX_FIFO_OFFSET	    			0x10310000
#define SPI_RX_FIFO_OFFSET	    			0x10340000

#define SPI_LOCAL_DEVICE_ID				0
#define SPI_TXQ_FIFO_DEVICE_ID			3
#define SPI_RXQ_FIFO_DEVICE_ID			7
#define SPI_UNDEFINED_DEVICE_ID			(-1)


//SPI Local registers
#define SPI_REG_INT_CTRL				0x0004 // 4 bytes, SPI INT Control
#define SPI_REG_INT_TIMEOUT		   		0x0006  // 2 bytes, SPI 32us INT timout
#define SPI_REG_HIMR					0x0014 // 4 bytes, SPI Host Interrupt Mask
#define SPI_REG_HISR					0x0018 // 4 bytes, SPI Host Interrupt Service Routine
#define SPI_REG_RX0_REQ_LEN				0x001C // 4 bytes, RXDMA Request Length
#define SPI_REG_FREE_TX_SPACE				0x0020 // 4 bytes, Free Tx Buffer Page
#define SPI_REG_TX_SEQNUM				0x0024 // 1 byte, TX Sequence Number Definition
#define SPI_REG_HCPWM					0x0038 // 1 byte, HCI Current Power Mode
#define SPI_REG_HCPWM2			 		0x003A // 2 bytes, HCI Current Power Mode 2
#define SPI_REG_AVAI_PATH_L				0x0040 // 4 bytes, SPI TX Available Low Size reg
#define SPI_REG_AVAI_PATH_H				0x0044 // 4 bytes, SPI TX Available High Size reg
#define SPI_REG_RX_AGG_CTL				0x0048 // 4 bytes, SPI RX AGG control
#define SPI_REG_H2C_MSG					0x004C // 4 bytes, SPI_REG_H2C_MSG
#define SPI_REG_C2H_MSG					0x0050  // 4 bytes, SPI_REG_C2H_MSG
#define SPI_REG_HRPWM					0x0080  // 1 byte, SPI_REG_HRPWM
#define SPI_REG_HPS_CLKR				0x0084 // 1 byte, not uesd
#define SPI_REG_CPU_IND					0x0087 // 1 byte, firmware indication to host
#define SPI_REG_32K_TRANS_CTL				0x0088 // 1 byte, 32K transparent control, BIT0 EN32K_TRANS
#define SPI_REG_32K_IDLE_TIME				0x008B // 1 byte, 32K idle time, 
#define SPI_REG_DELY_LINE_SEL				0x008C // 1 byte, Delay line selection, 
#define SPI_REG_SPI_CFG					0x00F0 // 1 byte, SPI configuration, 

#define LOCAL_REG_FREE_TX_SPACE				(SPI_LOCAL_OFFSET | SPI_REG_FREE_TX_SPACE)

// Register SPI_REG_CPU_IND
#define SPI_CPU_RDY_IND		(BIT0)


/************************************************/
// SPI_REG_HISR: SDIO Host Interrupt Service Routine
#define SPI_HISR_RX_REQUEST				(BIT0)
#define SPI_HISR_AVAL_INT				(BIT1)
#define SPI_HISR_TXPKT_OVER_BUFF			(BIT2)
#define SPI_HISR_TX_AGG_SIZE_MISMATCH			(BIT3)
#define SPI_HISR_TXBD_OVF				(BIT4)
//BIT5~16 not used
#define SPI_HISR_C2H_MSG_INT				(BIT17)
#define SPI_HISR_CPWM1_INT				(BIT18)
#define SPI_HISR_CPWM2_INT				(BIT19)
//BIT20~31 not used
#define SPI_HISR_CPU_NOT_RDY				(BIT22)


#define MASK_SPI_HISR_CLEAR		(SPI_HISR_RX_REQUEST |\
						SPI_HISR_AVAL_INT |\
						SPI_HISR_TXPKT_OVER_BUFF |\
						SPI_HISR_TX_AGG_SIZE_MISMATCH |\
						SPI_HISR_TXBD_OVF |\
						SPI_HISR_C2H_MSG_INT |\
						SPI_HISR_CPWM1_INT |\
						SPI_HISR_CPWM2_INT)

// RTL8195A SPI Host Interrupt Mask Register
#define SPI_HIMR_RX_REQUEST_MSK				(BIT0)
#define SPI_HIMR_AVAL_MSK					(BIT1)
#define SPI_HIMR_TXPKT_SIZE_OVER_BUFF_MSK			(BIT2)
#define SPI_HIMR_AGG_SIZE_MISMATCH_MSK			(BIT3)
#define SPI_HIMR_TXBD_OVF_MSK				(BIT4)
//BIT5~16 not used
#define SPI_HIMR_C2H_MSG_INT_MSK				(BIT17)
#define SPI_HIMR_CPWM1_INT_MSK				(BIT18)
#define SPI_HIMR_CPWM2_INT_MSK				(BIT19)
//BIT20~31 not used
#define SPI_HIMR_DISABLED			0

// Register SPI_REG_HCPWM
#define SPI_HCPWM_WLAN_TRX			(BIT1)


enum{
	SPI_LITTLE_ENDIAN = 2,
	SPI_BIG_ENDIAN = 0
};
enum{
	SPI_WORD_LEN_16 = 0,
	SPI_WORD_LEN_32 = 1
};

typedef enum{
	SPI_LITTLE_ENDIAN_16 = SPI_LITTLE_ENDIAN|SPI_WORD_LEN_16,
	SPI_LITTLE_ENDIAN_32 = SPI_LITTLE_ENDIAN|SPI_WORD_LEN_32, // default configure
	SPI_BIG_ENDIAN_16 = SPI_BIG_ENDIAN|SPI_WORD_LEN_16,
	SPI_BIG_ENDIAN_32 = SPI_BIG_ENDIAN|SPI_WORD_LEN_32
}_gspi_conf_t;

#define GSPI_CMD_LEN		4
#define GSPI_STATUS_LEN		8


#define FILL_SPI_CMD(byte_en, addr, domain_id, fun, write_flag) 	((byte_en & 0xff) | ((addr & 0xffff) << 8) \
									| ((domain_id & 0x1f) << 24) | ((fun & 0x3) << 29) | ((write_flag & 0x1) << 31))


#define GET_STATUS_HISR(status)				((((*(u32*)status)) & 0x3) |((((*(u32*)status) >> 2) & 0x7) << 17))
#define GET_STATUS_FREE_TX(status)		((((*(u32*)status) >> 5) & 0x7ffffff) << 2)
#define GET_STATUS_RXQ_REQ_LEN(status)	(((*(u32*)((u8 *)status + 4))) & 0xffffff)
#define GET_STATUS_TX_SEQ(status)			(((*(u32*)((u8 *)status + 4)) >> 24) & 0xff)

#define GSPI_CMD_TX         0x83    // 
#define GSPI_CMD_RX			0X82

// define transmit packat type
#define GPSI_TX_PACKET_802_3	(0x83)
#define GSPI_TX_PACKET_802_11	(0x81)
#define GSPI_TX_H2C_CMD			(0x11)
#define GSPI_TX_MEM_READ		(0x51)
#define GSPI_TX_MEM_WRITE		(0x53)
#define GSPI_TX_MEM_SET			(0x55)
#define GSPI_TX_FM_FREETOGO		(0x61)

//define receive packet type
#define GSPI_RX_PACKET_802_3	(0x82)
#define GSPI_RX_PACKET_802_11	(0x80)
#define GSPI_RX_C2H_CMD			(0x10)
#define GSPI_RX_MEM_READ		(0x50)
#define GSPI_RX_MEM_WRITE		(0x52)
#define GSPI_RX_MEM_SET			(0x54)
#define GSPI_RX_FM_FREETOGO		(0x60)


typedef struct _GSPI_TX_DESC{
	// u4Byte 0
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
	u32	txpktsize:16;       // bit[15:0]
	u32	offset:8;    		// bit[23:16], store the sizeof(SDIO_TX_DESC)
	u32	bus_agg_num:8;		// bit[31:24], the bus aggregation number
#else
    u32 bus_agg_num:8;      // bit[31:24], the bus aggregation number
    u32 offset:8;           // bit[23:16], store the sizeof(SDIO_TX_DESC)
    u32 txpktsize:16;       // bit[15:0]
#endif

	// u4Byte 1
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
    u32 type:8;             // bit[7:0], the packet type
    u32 rsvd0:24;
#else
    u32 rsvd0:24;
    u32 type:8;             // bit[7:0], the packet type
#endif

	// u4Byte 2
	u32	rsvd1;
	
	// u4Byte 3
	u32	rsvd2;
	
	// u4Byte 4
	u32	rsvd3;

	// u4Byte 5
	u32	rsvd4;
} GSPI_TX_DESC, *PGSPI_TX_DESC;

typedef struct _GSPI_RX_DESC{
	// u4Byte 0
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
	u32	pkt_len:16;     // bit[15:0], the packet size
	u32	offset:8;    	// bit[23:16], the offset from the packet start to the buf start, also means the size of RX Desc
	u32	rsvd0:6;        // bit[29:24]
	u32	icv:1;          // bit[30], ICV error
	u32	crc:1;          // bit[31], CRC error
#else
	u32	crc:1;          // bit[31], CRC error
	u32	icv:1;          // bit[30], ICV error
	u32	rsvd0:6;        // bit[29:24]
	u32	offset:8;    	// bit[23:16], the offset from the packet start to the buf start, also means the size of RX Desc
	u32	pkt_len:16;     // bit[15:0], the packet size
#endif

	// u4Byte 1
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
	u32	type:8;         // bit[7:0], the type of this packet
	u32	rsvd1:24;       // bit[31:8]
#else
    u32 rsvd1:24;       // bit[31:8]
    u32 type:8;         // bit[7:0], the type of this packet
#endif

	// u4Byte 2
	u32	rsvd2;
	
	// u4Byte 3
	u32	rsvd3;
	
	// u4Byte 4
	u32	rsvd4;

	// u4Byte 5
	u32	rsvd5;
} GSPI_RX_DESC, *PGSPI_RX_DESC;

#define SIZE_TX_DESC	(sizeof(GSPI_TX_DESC))
#define SIZE_RX_DESC	(sizeof(GSPI_RX_DESC))

// For memory read command
typedef struct _GSPI_DESC_MR{
	// u4Byte 0
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
	u32	pkt_len:16;     // bit[15:0], the packet size
	u32	offset:8;    	// bit[23:16], the offset from the packet start to the buf start, also means the size of RX Desc
	u32	rsvd0:8;        // bit[31:24]
#else
	u32	rsvd0:8;        // bit[31:24]
	u32	offset:8;    	// bit[23:16], the offset from the packet start to the buf start, also means the size of RX Desc
	u32	pkt_len:16;     // bit[15:0], the packet size
#endif

	// u4Byte 1
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
	u32	type:8;         // bit[7:0], the type of this packet
	u32	rsvd1:24;       // bit[31:8]
#else
    u32 rsvd1:24;       // bit[31:8]
    u32 type:8;         // bit[7:0], the type of this packet
#endif

	// u4Byte 2
	u32	start_addr;
	
	// u4Byte 3
	u32	rsvd2;
	
	// u4Byte 4
	u32	rsvd3;

	// u4Byte 5
	u32	rsvd4;
} GSPI_DESC_MR, *PGSPI_DESC_MR;

// For memory write reply command
typedef struct _GSPI_DESC_MW{
	// u4Byte 0
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
	u32	txpktsize:16;       // bit[15:0]
	u32	offset:8;    		// bit[23:16], store the sizeof(TX_DESC)
	u32	bus_agg_num:8;		// bit[31:24], the bus aggregation number
#else
    u32 bus_agg_num:8;      // bit[31:24], the bus aggregation number
    u32 offset:8;           // bit[23:16], store the sizeof(TX_DESC)
    u32 txpktsize:16;       // bit[15:0]
#endif

	// u4Byte 1
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
    u32 type:8;             // bit[7:0], the packet type
    u32 reply:1;            // bit[8], request to send a reply message
    u32 rsvd0:23;
#else
    u32 rsvd0:23;
    u32 reply:1;            // bit[8], request to send a reply message
    u32 type:8;             // bit[7:0], the packet type
#endif

	// u4Byte 2
	u32	start_addr;         // memory write start address
	
	// u4Byte 3
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
    u32 write_len:16;       // bit[15:0], the length to write
    u32 rsvd2:16;           // bit[31:16]
#else
    u32 rsvd2:16;           // bit[31:16]
    u32 write_len:16;       // bit[15:0], the length to write
#endif
	
	// u4Byte 4
	u32	rsvd3;

	// u4Byte 5
	u32	rsvd4;
}  GSPI_DESC_MW, *PGSPI_DESC_MW;

// For memory set command
typedef struct _GSPI_DESC_MS{
	// u4Byte 0
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
	u32	txpktsize:16;       // bit[15:0]
	u32	offset:8;    		// bit[23:16], store the sizeof(TX_DESC)
	u32	bus_agg_num:8;		// bit[31:24], the bus aggregation number
#else
    u32 bus_agg_num:8;      // bit[31:24], the bus aggregation number
    u32 offset:8;           // bit[23:16], store the sizeof(TX_DESC)
    u32 txpktsize:16;       // bit[15:0]
#endif

	// u4Byte 1
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
    u32 type:8;             // bit[7:0], the packet type
    u32 data:8;             // bit[8:15], the value to be written to the memory
    u32 reply:1;            // bit[16], request to send a reply message
    u32 rsvd0:15;
#else
    u32 rsvd0:15;
    u32 reply:1;            // bit[16], request to send a reply message
    u32 data:8;             // bit[8:15], the value to be written to the memory
    u32 type:8;             // bit[7:0], the packet type
#endif

	// u4Byte 2
	u32	start_addr;         // memory write start address
	
	// u4Byte 3
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
    u32 write_len:16;       // bit[15:0], the length to write
    u32 rsvd2:16;           // bit[31:16]
#else
    u32 rsvd2:16;           // bit[31:16]
    u32 write_len:16;       // bit[15:0], the length to write
#endif
	
	// u4Byte 4
	u32	rsvd3;

	// u4Byte 5
	u32	rsvd4;
} GSPI_DESC_MS, *PGSPI_DESC_MS;
// TX Desc for Jump to Start command
typedef struct _GSPI_DESC_JS{
	// u4Byte 0
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
	u32	txpktsize:16;       // bit[15:0]
	u32	offset:8;    		// bit[23:16], store the sizeof(TX_DESC)
	u32	bus_agg_num:8;		// bit[31:24], the bus aggregation number
#else
    u32 bus_agg_num:8;      // bit[31:24], the bus aggregation number
    u32 offset:8;           // bit[23:16], store the sizeof(TX_DESC)
    u32 txpktsize:16;       // bit[15:0]
#endif

	// u4Byte 1
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
    u32 type:8;             // bit[7:0], the packet type
    u32 rsvd0:24;
#else
    u32 rsvd0:24;
    u32 type:8;             // bit[7:0], the packet type
#endif

	// u4Byte 2
	u32	start_fun;         // the pointer of the startup function 
	
	// u4Byte 3
	u32	rsvd2;
	
	// u4Byte 4
	u32	rsvd3;

	// u4Byte 5
	u32	rsvd4;
} GSPI_DESC_JS, *PGSPI_DESC_JS;


// CCPWM2 bit map definition for Firmware download
#define GSPI_INIT_DONE					(BIT0)
#define GSPI_MEM_WR_DONE				(BIT1)
#define GSPI_MEM_RD_DONE				(BIT2)
#define GSPI_MEM_ST_DONE				(BIT3)
#define GSPI_CPWM2_TOGGLE				(BIT15)

// Register REG_SPDIO_CPU_IND
#define GPSI_SYSTEM_TRX_RDY_IND		(BIT0)

#endif //__GSPI_REG_H__

