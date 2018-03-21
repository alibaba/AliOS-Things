#ifndef USBD_MSC_H
#define USBD_MSC_H

#include "usb.h"
#include "usb_gadget.h"
#include "core/inc/usb_composite.h"
#include "msc/inc/usbd_msc_config.h"

/* config usb msc device debug inforation */
#define USBD_MSC_DEBUG          0

#if	USBD_MSC_DEBUG
#define USBD_PRINTF(fmt, args...)		            DBG_8195A("\n\r%s: " fmt, __FUNCTION__, ## args)
#define USBD_ERROR(fmt, args...)		            DBG_8195A("\n\r%s: " fmt, __FUNCTION__, ## args)
#define USBD_WARN(fmt, args...)		            	DBG_8195A("\n\r%s: " fmt, __FUNCTION__, ## args)
#define FUN_ENTER                                   DBG_8195A("\n\r%s ==>\n", __func__)
#define FUN_EXIT                                    DBG_8195A("\n\r%s <==\n", __func__)
#define FUN_TRACE                                   DBG_8195A("\n\r%s:%d \n", __func__, __LINE__)
#else
#define USBD_PRINTF(fmt, args...)
#define USBD_ERROR(fmt, args...)		            DBG_8195A("\n\r%s: " fmt, __FUNCTION__, ## args)
#define USBD_WARN(fmt, args...)
#define FUN_ENTER
#define FUN_EXIT
#define FUN_TRACE
#endif

/* MSC Request Codes */
#define MSC_REQUEST_RESET               0xFF
#define MSC_REQUEST_GET_MAX_LUN         0xFE

/* MSC LUN */
#define MSC_MAX_LOGIC_UNIT_NUMBER	1

enum data_direction{
	DATA_DIR_UNKNOWN = 0,
	DATA_DIR_FROM_HOST,
	DATA_DIR_TO_HOST,
	DATA_DIR_NONE
};

typedef enum _disk_type{
	DISK_SDCARD,
	DISK_FLASH
}disk_type;

//structure predefine
struct msc_dev;
struct msc_bufhd;

struct msc_opts{
	int (*disk_init)(void);
	int (*disk_deinit)(void);
	int (*disk_getcapacity)(u32* sectors);
	int (*disk_read)(u32 sector,u8 *buffer,u32 count);
	int (*disk_write)(u32 sector,const u8 *buffer,u32 count);
};

struct msc_lun {
	unsigned int	initially_ro:1;
	unsigned int	ro:1;
	unsigned int	removable:1;
	unsigned int	cdrom:1;
	unsigned int	prevent_medium_removal:1;
	unsigned int	registered:1;
	unsigned int	info_valid:1;
	unsigned int	nofua:1;

	u32		sense_data;
	u32		sense_data_info;
	u32		unit_attention_data;
	
	u64	file_length;
	unsigned int    num_sectors; /*  */
	unsigned int	blkbits; /* Bits of logical block size
						       of bound block device */
	unsigned int	blksize; /* logical block size of bound block device */
	const char	*name;		/* "lun.name" */

	unsigned int	lba; // the current read and write logical block address
	u8 is_open;
	_mutex	lun_mutex;
	struct msc_opts *lun_opts;
};


struct msc_common{
	struct msc_dev	*mscdev;
	
	struct msc_lun	**luns;
	struct msc_lun	*curlun;

	struct usb_gadget	*gadget;
	struct usb_ep		*ep0;		
	struct usb_request	*req0;		/* for control responses */

	/* scsi cbw relevant */
	enum data_direction	data_dir;
	u32			data_size;
	u32			data_size_from_cmnd;
	u32			tag;
	u32			residue;
	u32			usb_amount_left;
	u8			scsi_cmnd[16]; // max command
	u8			cmnd_size;
	
	u8		lun;	/* current lun*/
	u8		nluns;

	u8	nbufhd;
	u8	nbufhd_a;
	_list	bufhd_pool;
	_mutex  bufhd_mutex;
	/* bulk out cmd*/
	_list	boc_list;
	_mutex  boc_mutex;

	/* bolk out data*/
	_mutex  bod_mutex;
	_list	bod_list;
	/**/
	struct msc_bufhd* curbh; // current buffer header
	struct msc_bufhd* cbw_bh; // buffer header for CBW
	struct msc_bufhd* csw_bh; // buffer header for CSW
	
	unsigned int		can_stall:1;
	unsigned int		phase_error:1;
	unsigned int		short_packet_received:1;
	unsigned int		bad_lun_okay:1;
	unsigned int 		running:1;
};

typedef enum _bufhd_type{
	BUFHD_CBW = 0,
	BUFHD_CSW,
	BUFHD_DATA,
}bufhd_type;

struct msc_bufhd{
	u8* buf;
	int buf_size;
	bufhd_type type;
	_list  list;	
	struct usb_request	*reqin; 	/* for bulkin responses */
	struct usb_request	*reqout;
};

struct msc_dev{
	struct msc_common 	*common;
	
	u16 		interface_number;
	u8			config;

	struct usb_ep	*in_ep;
	struct usb_ep	*out_ep;
	unsigned int	bulk_in_enabled:1;
	unsigned int	bulk_out_enabled:1;

    const struct usb_endpoint_descriptor
				*in, *out, *status;
    // lock is held when accessing usb
	struct task_struct msc_outCmdTask;
	struct task_struct msc_outDataTask;
    struct usb_function	func;
};

u32 min(u32 value1,u32 value2);

int usbd_msc_halt_bulk_in_endpoint(struct msc_dev *mscdev);
void usbd_msc_put_bufhd(struct msc_common *common, struct msc_bufhd* bufhd);
struct msc_bufhd* usbd_msc_get_bufhd(struct msc_common *common);
int usbd_msc_bulk_in_transfer(struct msc_dev *mscdev, struct usb_request *req);
int usbd_msc_bulk_out_transfer(struct msc_dev *mscdev, struct usb_request *req);

/*
* N_bh : number of buffer header
* Size_bh: buffer size per buffer
* type:msc physical disk type
*/
int usbd_msc_init(int N_bh, int Size_bh, disk_type type);
void usbd_msc_deinit(void);

#endif
