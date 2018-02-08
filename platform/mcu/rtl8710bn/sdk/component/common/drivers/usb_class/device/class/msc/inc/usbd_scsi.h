#ifndef USBD_SCSI_H
#define USBD_SCSI_H
#include "basic_types.h"
#include "msc/inc/usbd_msc.h"

#define MAX_COMMAND_SIZE 16
#define MSC_MAX_LUNS	8

/* SCSI Commands */
#define SCSI_FORMAT_UNIT                            0x04
#define SCSI_INQUIRY                                0x12
#define SCSI_MODE_SELECT6                           0x15
#define SCSI_MODE_SELECT10                          0x55
#define SCSI_MODE_SENSE6                            0x1A
#define SCSI_MODE_SENSE10                           0x5A
#define SCSI_ALLOW_MEDIUM_REMOVAL                   0x1E
#define SCSI_READ6                                  0x08
#define SCSI_READ10                                 0x28
#define SCSI_READ12                                 0xA8
#define SCSI_READ16                                 0x88

#define SCSI_READ_CAPACITY10                        0x25
#define SCSI_READ_CAPACITY16                        0x9E

#define SCSI_SYNCHRONIZE_CACHE						0x35
#define SCSI_REQUEST_SENSE                          0x03
#define SCSI_START_STOP_UNIT                        0x1B
#define SCSI_TEST_UNIT_READY                        0x00
#define SCSI_WRITE6                                 0x0A
#define SCSI_WRITE10                                0x2A
#define SCSI_WRITE12                                0xAA
#define SCSI_WRITE16                                0x8A

#define SCSI_VERIFY10                               0x2F
#define SCSI_VERIFY12                               0xAF
#define SCSI_VERIFY16                               0x8F

#define SCSI_SEND_DIAGNOSTIC                        0x1D
#define SCSI_READ_FORMAT_CAPACITIES                 0x23

#define READ_FORMAT_CAPACITY_DATA_LEN               0x0C
#define READ_CAPACITY10_DATA_LEN                    0x08
#define MODE_SENSE10_DATA_LEN                       0x08
#define MODE_SENSE6_DATA_LEN                        0x04
#define REQUEST_SENSE_DATA_LEN                      0x12
#define STANDARD_INQUIRY_DATA_LEN                   0x24

/* SCSI Sense Key/Additional Sense Code/ASC Qualifier values */
#define SS_NO_SENSE				0
#define SS_COMMUNICATION_FAILURE		0x040800
#define SS_INVALID_COMMAND			0x052000
#define SS_INVALID_FIELD_IN_CDB			0x052400
#define SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE	0x052100
#define SS_LOGICAL_UNIT_NOT_SUPPORTED		0x052500
#define SS_MEDIUM_NOT_PRESENT			0x023a00
#define SS_MEDIUM_REMOVAL_PREVENTED		0x055302
#define SS_NOT_READY_TO_READY_TRANSITION	0x062800
#define SS_RESET_OCCURRED			0x062900
#define SS_SAVING_PARAMETERS_NOT_SUPPORTED	0x053900
#define SS_UNRECOVERED_READ_ERROR		0x031100
#define SS_WRITE_ERROR				0x030c02
#define SS_WRITE_PROTECTED			0x072700



#define SK(x)		((u8) ((x) >> 16))	/* Sense Key byte, etc. */
#define ASC(x)		((u8) ((x) >> 8))
#define ASCQ(x)		((u8) (x))

/*
* Bulk only data structures
*/

/* command block wrapper */
struct bulk_cb_wrap {
	unsigned int	Signature;		/* contains 'USBC', denote bulk_cb_wrap */
	unsigned int	Tag;			/* unique per command id */
	unsigned int	DataTransferLength;	/* size of data for transfer */
	unsigned char	Flags;			/* data transfer direction */
	unsigned char	Lun;			/* LUN normally 0, (which command block is sent) */
	unsigned char	Length;			/* length of the CDB */
	unsigned char	CDB[16];		/* max command */
};

#define US_BULK_CB_WRAP_LEN	31
#define US_BULK_CB_SIGN		0x43425355	/*spells out USBC */
#define US_BULK_FLAG_IN		(1 << 7)
#define US_BULK_FLAG_OUT	0

/* command status wrapper */
struct bulk_cs_wrap {
	unsigned int	Signature;	/* should = 'USBS' */
	unsigned int	Tag;		/* same as original command, echoed by the device as received */
	unsigned int	Residue;	/* amount not transferred */
	unsigned char	Status;		/* execute command status */
};

#define US_BULK_CS_WRAP_LEN	13
#define US_BULK_CS_SIGN		0x53425355      /* spells out 'USBS' */
// execute command status
#define US_BULK_STAT_OK		0
#define US_BULK_STAT_FAIL	1
#define US_BULK_STAT_PHASE	2

/* bulk-only class specific requests */
#define US_BULK_RESET_REQUEST   0xff
#define US_BULK_GET_MAX_LUN     0xfe

extern int usbd_msc_receive_cbw(struct msc_dev *mscdev, struct usb_request *req);
#endif