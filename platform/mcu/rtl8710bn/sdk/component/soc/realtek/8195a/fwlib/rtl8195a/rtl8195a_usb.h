/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTL8195A_USB_H_
#define _RTL8195A_USB_H_


// common command for USB
#define USB_CMD_TX_ETH         0x83    // request to TX a 802.3 packet
#define USB_CMD_TX_WLN         0x81    // request to TX a 802.11 packet
#define USB_CMD_H2C            0x11    // H2C(host to device) command packet
#define USB_CMD_MEMRD          0x51    // request to read a block of memory data
#define USB_CMD_MEMWR          0x53    // request to write a block of memory
#define USB_CMD_MEMST          0x55    // request to set a block of memory with a value
#define USB_CMD_STARTUP        0x61    // request to jump to the start up function

#define USB_CMD_RX_ETH         0x82    // indicate a RX 802.3 packet
#define USB_CMD_RX_WLN         0x80    // indicate a RX 802.11 packet
#define USB_CMD_C2H            0x10    // C2H(device to host) command packet
#define USB_CMD_MEMRD_RSP      0x50    // response to memory block read command
#define USB_CMD_MEMWR_RSP      0x52    // response to memory write command
#define USB_CMD_MEMST_RSP      0x54    // response to memory set command
#define USB_CMD_STARTED        0x60    // indicate the program has jumped to the given function


// TODO: This data structer just for test, we should modify it for the normal driver
typedef struct _USB_TX_DESC{
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
} USB_TX_DESC, *PUSB_TX_DESC;

#define SIZE_USB_TX_DESC	sizeof(USB_TX_DESC)

// TX Desc for Memory Write command
typedef struct _USB_TX_DESC_MW{
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
} USB_TX_DESC_MW, *PUSB_TX_DESC_MW;

// TX Desc for Memory Read command
typedef struct _USB_TX_DESC_MR{
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
	u32	start_addr;         // memory write start address
	
	// u4Byte 3
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
    u32 read_len:16;        // bit[15:0], the length to read
    u32 rsvd2:16;           // bit[31:16]
#else
    u32 rsvd2:16;           // bit[31:16]
    u32 read_len:16;        // bit[15:0], the length to read
#endif
	
	// u4Byte 4
	u32	rsvd3;

	// u4Byte 5
	u32	rsvd4;
} USB_TX_DESC_MR, *PUSB_TX_DESC_MR;

// TX Desc for Memory Set command
typedef struct _USB_TX_DESC_MS{
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
} USB_TX_DESC_MS, *PUSB_TX_DESC_MS;

// TX Desc for Jump to Start command
typedef struct _USB_TX_DESC_JS{
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
	u32	start_fun;         // the pointer of the startup function 
	
	// u4Byte 3
	u32	rsvd2;
	
	// u4Byte 4
	u32	rsvd3;

	// u4Byte 5
	u32	rsvd4;
} USB_TX_DESC_JS, *PUSB_TX_DESC_JS;

typedef struct _USB_RX_DESC{
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
} USB_RX_DESC, *PUSB_RX_DESC;


// For memory read command
typedef struct _USB_RX_DESC_MR{
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
} USB_RX_DESC_MR, *PUSB_RX_DESC_MR;

// For memory write reply command
typedef struct _USB_RX_DESC_MW{
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
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
    u32 write_len:16;   // bit[15:0], the type of this packet
    u32 result:8;      // bit[23:16], the result of memory write command
    u32 rsvd2:8;       // bit[31:24]
#else
    u32 rsvd2:8;       // bit[31:24]
    u32 result:8;      // bit[23:16], the result of memory write command
    u32 write_len:16;   // bit[15:0], the type of this packet
#endif
	
	// u4Byte 4
	u32	rsvd3;

	// u4Byte 5
	u32	rsvd4;
} USB_RX_DESC_MW, *PUSB_RX_DESC_MW;

// For memory set reply command
typedef struct _USB_RX_DESC_MS{
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
#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)
    u32 write_len:16;   // bit[15:0], the type of this packet
    u32 result:8;      // bit[23:16], the result of memory write command
    u32 rsvd2:8;       // bit[31:24]
#else
    u32 rsvd2:8;       // bit[31:24]
    u32 result:8;      // bit[23:16], the result of memory write command
    u32 write_len:16;   // bit[15:0], the type of this packet
#endif
	
	// u4Byte 4
	u32	rsvd3;

	// u4Byte 5
	u32	rsvd4;
} USB_RX_DESC_MS, *PUSB_RX_DESC_MS;

// For firmware ready reply command
typedef struct _USB_RX_DESC_FS{
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
	u32	rsvd2;
	
	// u4Byte 3
	u32	rsvd3;
	
	// u4Byte 4
	u32	rsvd4;

	// u4Byte 5
	u32	rsvd5;
} USB_RX_DESC_FS, *PUSB_RX_DESC_FS;


#define SIZE_USB_RX_DESC	sizeof(USB_RX_DESC)

#endif	// #ifndef _RTL8195A_USB_H_

