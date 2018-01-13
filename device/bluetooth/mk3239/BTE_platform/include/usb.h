/*******************************************************************************
**  Name:       usb.h
**
**  Description:
**
**  This file contains serial definitions from WIDCOMM's Universal Embedded
**  Drivers API.
**
*******************************************************************************/

#ifndef USB_H
#define USB_H

/*******************************************************************************
** Serial APIs
*******************************************************************************/

/**** port IDs ****/
#define USB_PORT_1            0
#define USB_PORT_2            1
#define USB_PORT_3            2
#define USB_PORT_4            3

typedef UINT8 tUSB_PORT;

/**** baud rates ****/
#define USB_BAUD_300          0
#define USB_BAUD_600          1
#define USB_BAUD_1200         2
#define USB_BAUD_2400         3
#define USB_BAUD_9600         4
#define USB_BAUD_19200        5
#define USB_BAUD_57600        6
#define USB_BAUD_115200       7
#define USB_BAUD_230400       8
#define USB_BAUD_460800       9
#define USB_BAUD_921600       10
#define USB_BAUD_AUTO         11

/**** Data Format ****/

/* Stop Bits */
#define USB_STOPBITS_1        1
#define USB_STOPBITS_1_5      (1<<1)
#define USB_STOPBITS_2        (1<<2)

/* Parity Bits */
#define USB_PARITY_NONE       (1<<3)
#define USB_PARITY_EVEN       (1<<4)
#define USB_PARITY_ODD        (1<<5)

/* Data Bits */
#define USB_DATABITS_5        (1<<6)
#define USB_DATABITS_6        (1<<7)
#define USB_DATABITS_7        (1<<8)
#define USB_DATABITS_8        (1<<9)


/**** Flow Control ****/
#define USB_FC_NONE           0
#define USB_FC_HW             1
#define USB_FC_SW             2

/**** Data Buffering Mechanism ****/
#define USB_BUF_BYTE          0
#define USB_BUF_GKI           1

/**** Signals ****/
#define USB_SIG_RTSCTS        1
#define USB_SIG_DSRDTR        (1<<1)
#define USB_SIG_RI            (1<<2)
#define USB_SIG_CD            (1<<3)
#define USB_SIG_DTE_DEVICE    (1<<4)

/**** Errors *****/
#define USB_ERR_OVERRUN       1
#define USB_ERR_PARITY        (1<<1)
#define USB_ERR_FRAMING       (1<<2)
#define USB_ERR_BREAK         (1<<3)

/**** Serial Operations ****/
#define USB_OP_FLUSH          0
#define USB_OP_FLUSH_RX       1
#define USB_OP_FLUSH_TX       2
#define USB_OP_BREAK_OFF      3
#define USB_OP_BREAK_ON       4
#define USB_OP_BAUD_RD        5
#define USB_OP_BAUD_WR        6
#define USB_OP_FMT_RD         7
#define USB_OP_FMT_WR         8
#define USB_OP_SIG_RD         9
#define USB_OP_SIG_WR         10
#define USB_OP_FC_RD          11
#define USB_OP_FC_WR          12

typedef UINT8 tUSB_OP;


/**** Serial feature types ****/
#define USB_FEAT_PORT_1       0
#define USB_FEAT_PORT_2       1
#define USB_FEAT_PORT_3       2
#define USB_FEAT_PORT_4       3
#define USB_FEAT_BAUD_AUTO    4
#define USB_FEAT_BAUD_300     5
#define USB_FEAT_BAUD_600     6
#define USB_FEAT_BAUD_1200    7
#define USB_FEAT_BAUD_2400    8
#define USB_FEAT_BAUD_9600    9
#define USB_FEAT_BAUD_19200   10
#define USB_FEAT_BAUD_57600   11
#define USB_FEAT_BAUD_115200  12
#define USB_FEAT_BAUD_230400  13
#define USB_FEAT_BAUD_460800  14
#define USB_FEAT_BAUD_921600  15
#define USB_FEAT_STOPBITS_1   16
#define USB_FEAT_STOPBITS_1_5 17
#define USB_FEAT_STOPBITS_2   18
#define USB_FEAT_PARITY_NONE  19
#define USB_FEAT_PARITY_EVEN  20
#define USB_FEAT_PARITY_ODD   21
#define USB_FEAT_DATABITS_5   22
#define USB_FEAT_DATABITS_6   23
#define USB_FEAT_DATABITS_7   24
#define USB_FEAT_DATABITS_8   25
#define USB_FEAT_FC_NONE      26
#define USB_FEAT_FC_HW        27
#define USB_FEAT_FC_SW        28
#define USB_FEAT_BUF_BYTE     29
#define USB_FEAT_BUF_GKI      30
#define USB_FEAT_SIG_RTS      31
#define USB_FEAT_SIG_CTS      32
#define USB_FEAT_SIG_DSR      33
#define USB_FEAT_SIG_DTR      34
#define USB_FEAT_SIG_RI       35
#define USB_FEAT_SIG_CD       36
#define USB_FEAT_OP_FLUSH     37
#define USB_FEAT_OP_FLUSH_RX  38
#define USB_FEAT_OP_FLUSH_TX  39
#define USB_FEAT_OP_BREAK     40
#define USB_FEAT_OP_BAUD_RD   41
#define USB_FEAT_OP_BAUD_WR   42
#define USB_FEAT_OP_FMT_RD    43
#define USB_FEAT_OP_FMT_WR    44
#define USB_FEAT_OP_SIG_RD    45
#define USB_FEAT_OP_SIG_WR    46
#define USB_FEAT_OP_FC_RD     47
#define USB_FEAT_OP_FC_WR     48

typedef UINT8 tUSB_FEATURE;


/**** Event types ****/
#define USB_RX_READY_EVT      0
#define USB_TX_DONE_EVT       1
#define USB_SIG_EVT           2
#define USB_ERR_EVT           3

typedef UINT8 tUSB_EVT;


/* Structure used to configure serial port during open        */
typedef struct {
    UINT16 fmt;          /* Data format                       */
    UINT8  baud;         /* Baud rate                         */
    UINT8  fc;           /* Flow control                      */
    UINT8  buf;          /* Data buffering mechanism          */
    UINT8  pool;         /* GKI buffer pool for received data */
    UINT16 size;         /* Size of GKI buffer pool           */
    UINT16 offset;       /* Offset in GKI buffer pool         */
} tUSB_OPEN_CFG;

/* Union used to pass ioctl arguments */
typedef union {
    UINT16 fmt;
    UINT8  baud;
    UINT8  fc;
    UINT8  sigs;
} tUSB_IOCTL_DATA;


/* Union to pass event data */
typedef union {
    UINT8 sigs;
    UINT8 error;
} tUSB_EVT_DATA;

/* callback for events */
typedef void (tUSB_CBACK)(tUSB_PORT, tUSB_EVT, tUSB_EVT_DATA *);


/*******************************************************************************
** Function Prototypes
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

UDRV_API extern void    USB_Init(void *);
UDRV_API extern void    USB_Open(tUSB_PORT, tUSB_OPEN_CFG *, tUSB_CBACK *);
UDRV_API extern void    USB_ReadBuf(tUSB_PORT, BT_HDR **);
UDRV_API extern UINT16  USB_Read(tUSB_PORT, UINT8 *, UINT16);
UDRV_API extern BOOLEAN USB_WriteBuf(tUSB_PORT, BT_HDR *);
UDRV_API extern UINT16  USB_Write(tUSB_PORT, UINT8 *, UINT16);
UDRV_API extern void    USB_Ioctl(tUSB_PORT, tUSB_OP, tUSB_IOCTL_DATA *);
UDRV_API extern void    USB_Close(tUSB_PORT);
UDRV_API extern BOOLEAN USB_Feature(tUSB_FEATURE);

#ifdef __cplusplus
}
#endif

#endif /* USB_H */
