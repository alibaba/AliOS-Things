/*******************************************************************************
**  Name:       userial.h
**
**  Description:
**
**  This file contains serial definitions from WIDCOMM's Universal Embedded
**  Drivers API.
**
*******************************************************************************/

#ifndef USERIAL_H
#define USERIAL_H

/*******************************************************************************
** Serial APIs
*******************************************************************************/

/**** port IDs ****/
#define USERIAL_PORT_1            0
#define USERIAL_PORT_2            1
#define USERIAL_PORT_3            2
#define USERIAL_PORT_4            3
#define USERIAL_PORT_5            4
#define USERIAL_PORT_6            5
#define USERIAL_PORT_7            6
#define USERIAL_PORT_8            7
#define USERIAL_PORT_9            8
#define USERIAL_PORT_10           9
#define USERIAL_PORT_11           10
#define USERIAL_PORT_12           11
#define USERIAL_PORT_13           12
#define USERIAL_PORT_14           13
#define USERIAL_PORT_15           14
#define USERIAL_PORT_16           15
#define USERIAL_PORT_17           16
#define USERIAL_PORT_18           17

typedef UINT8 tUSERIAL_PORT;

/**** baud rates ****/
#define USERIAL_BAUD_300          0
#define USERIAL_BAUD_600          1
#define USERIAL_BAUD_1200         2
#define USERIAL_BAUD_2400         3
#define USERIAL_BAUD_9600         4
#define USERIAL_BAUD_19200        5
#define USERIAL_BAUD_57600        6
#define USERIAL_BAUD_115200       7
#define USERIAL_BAUD_230400       8
#define USERIAL_BAUD_460800       9
#define USERIAL_BAUD_921600       10
#define USERIAL_BAUD_1M           11
#define USERIAL_BAUD_1_5M         12
#define USERIAL_BAUD_2M           13
#define USERIAL_BAUD_3M           14
#define USERIAL_BAUD_4M           15
#define USERIAL_BAUD_AUTO         16

/**** Data Format ****/

/* Stop Bits */
#define USERIAL_STOPBITS_1        1
#define USERIAL_STOPBITS_1_5      (1<<1)
#define USERIAL_STOPBITS_2        (1<<2)

/* Parity Bits */
#define USERIAL_PARITY_NONE       (1<<3)
#define USERIAL_PARITY_EVEN       (1<<4)
#define USERIAL_PARITY_ODD        (1<<5)

/* Data Bits */
#define USERIAL_DATABITS_5        (1<<6)
#define USERIAL_DATABITS_6        (1<<7)
#define USERIAL_DATABITS_7        (1<<8)
#define USERIAL_DATABITS_8        (1<<9)


/**** Flow Control ****/
#define USERIAL_FC_NONE           0
#define USERIAL_FC_HW             1
#define USERIAL_FC_SW             2

/**** Data Buffering Mechanism ****/
#define USERIAL_BUF_BYTE          0
#define USERIAL_BUF_GKI           1

/**** Signals ****/
#define USERIAL_SIG_RTSCTS        1
#define USERIAL_SIG_DSRDTR        (1<<1)
#define USERIAL_SIG_RI            (1<<2)
#define USERIAL_SIG_CD            (1<<3)
#define USERIAL_SIG_DTE_DEVICE    (1<<4)

/**** Errors *****/
#define USERIAL_ERR_OVERRUN       1
#define USERIAL_ERR_PARITY        (1<<1)
#define USERIAL_ERR_FRAMING       (1<<2)
#define USERIAL_ERR_BREAK         (1<<3)

/**** Serial Operations ****/
#define USERIAL_OP_FLUSH          0
#define USERIAL_OP_FLUSH_RX       1
#define USERIAL_OP_FLUSH_TX       2
#define USERIAL_OP_BREAK_OFF      3
#define USERIAL_OP_BREAK_ON       4
#define USERIAL_OP_BAUD_RD        5
#define USERIAL_OP_BAUD_WR        6
#define USERIAL_OP_FMT_RD         7
#define USERIAL_OP_FMT_WR         8
#define USERIAL_OP_SIG_RD         9
#define USERIAL_OP_SIG_WR         10
#define USERIAL_OP_FC_RD          11
#define USERIAL_OP_FC_WR          12
#define USERIAL_OP_CTS_AS_WAKEUP  13    /* H4IBSS */
#define USERIAL_OP_CTS_AS_FC      14    /* H4IBSS */

#if (defined LINUX_OS) && (LINUX_OS == TRUE)
#define USERIAL_OP_SCO_UP         20    /* LINUX SCO */
#define USERIAL_OP_SCO_DOWN       21    /* LINUX SCO */
#endif

typedef UINT8 tUSERIAL_OP;


/**** Serial feature types ****/
#define USERIAL_FEAT_PORT_1       0
#define USERIAL_FEAT_PORT_2       1
#define USERIAL_FEAT_PORT_3       2
#define USERIAL_FEAT_PORT_4       3
#define USERIAL_FEAT_BAUD_AUTO    4
#define USERIAL_FEAT_BAUD_300     5
#define USERIAL_FEAT_BAUD_600     6
#define USERIAL_FEAT_BAUD_1200    7
#define USERIAL_FEAT_BAUD_2400    8
#define USERIAL_FEAT_BAUD_9600    9
#define USERIAL_FEAT_BAUD_19200   10
#define USERIAL_FEAT_BAUD_57600   11
#define USERIAL_FEAT_BAUD_115200  12
#define USERIAL_FEAT_BAUD_230400  13
#define USERIAL_FEAT_BAUD_460800  14
#define USERIAL_FEAT_BAUD_921600  15
#define USERIAL_FEAT_STOPBITS_1   16
#define USERIAL_FEAT_STOPBITS_1_5 17
#define USERIAL_FEAT_STOPBITS_2   18
#define USERIAL_FEAT_PARITY_NONE  19
#define USERIAL_FEAT_PARITY_EVEN  20
#define USERIAL_FEAT_PARITY_ODD   21
#define USERIAL_FEAT_DATABITS_5   22
#define USERIAL_FEAT_DATABITS_6   23
#define USERIAL_FEAT_DATABITS_7   24
#define USERIAL_FEAT_DATABITS_8   25
#define USERIAL_FEAT_FC_NONE      26
#define USERIAL_FEAT_FC_HW        27
#define USERIAL_FEAT_FC_SW        28
#define USERIAL_FEAT_BUF_BYTE     29
#define USERIAL_FEAT_BUF_GKI      30
#define USERIAL_FEAT_SIG_RTS      31
#define USERIAL_FEAT_SIG_CTS      32
#define USERIAL_FEAT_SIG_DSR      33
#define USERIAL_FEAT_SIG_DTR      34
#define USERIAL_FEAT_SIG_RI       35
#define USERIAL_FEAT_SIG_CD       36
#define USERIAL_FEAT_OP_FLUSH     37
#define USERIAL_FEAT_OP_FLUSH_RX  38
#define USERIAL_FEAT_OP_FLUSH_TX  39
#define USERIAL_FEAT_OP_BREAK     40
#define USERIAL_FEAT_OP_BAUD_RD   41
#define USERIAL_FEAT_OP_BAUD_WR   42
#define USERIAL_FEAT_OP_FMT_RD    43
#define USERIAL_FEAT_OP_FMT_WR    44
#define USERIAL_FEAT_OP_SIG_RD    45
#define USERIAL_FEAT_OP_SIG_WR    46
#define USERIAL_FEAT_OP_FC_RD     47
#define USERIAL_FEAT_OP_FC_WR     48

typedef UINT8 tUSERIAL_FEATURE;


/**** Event types ****/
#define USERIAL_RX_READY_EVT      0
#define USERIAL_TX_DONE_EVT       1
#define USERIAL_SIG_EVT           2
#define USERIAL_ERR_EVT           3
#define USERIAL_WAKEUP_EVT        4 /* H4IBSS */

typedef UINT8 tUSERIAL_EVT;


/* Structure used to configure serial port during open        */
typedef struct {
    UINT16 fmt;          /* Data format                       */
    UINT8  baud;         /* Baud rate                         */
    UINT8  fc;           /* Flow control                      */
    UINT8  buf;          /* Data buffering mechanism          */
    UINT8  pool;         /* GKI buffer pool for received data */
    UINT16 size;         /* Size of GKI buffer pool           */
    UINT16 offset;       /* Offset in GKI buffer pool         */
} tUSERIAL_OPEN_CFG;

/* Union used to pass ioctl arguments */
typedef union {
    UINT16 fmt;
    UINT8  baud;
    UINT8  fc;
    UINT8  sigs;
#if (defined LINUX_OS) && (LINUX_OS == TRUE)
    UINT16 sco_handle;
#endif
} tUSERIAL_IOCTL_DATA;


/* Union to pass event data */
typedef union {
    UINT8 sigs;
    UINT8 error;
} tUSERIAL_EVT_DATA;

/* callback for events */
typedef void (tUSERIAL_CBACK)(tUSERIAL_PORT, tUSERIAL_EVT, tUSERIAL_EVT_DATA *);

/*******************************************************************************
** Function Prototypes
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

UDRV_API extern void    USERIAL_Init(void *);
UDRV_API extern void    USERIAL_Open(tUSERIAL_PORT, tUSERIAL_OPEN_CFG *, tUSERIAL_CBACK *);
UDRV_API extern void    USERIAL_ReadBuf(tUSERIAL_PORT, BT_HDR **);
UDRV_API extern UINT16  USERIAL_Read(tUSERIAL_PORT, UINT8 *, UINT16);
UDRV_API extern BOOLEAN USERIAL_WriteBuf(tUSERIAL_PORT, BT_HDR *);
UDRV_API extern UINT16  USERIAL_Write(tUSERIAL_PORT, UINT8 *, UINT16);
UDRV_API extern void    USERIAL_Ioctl(tUSERIAL_PORT, tUSERIAL_OP, tUSERIAL_IOCTL_DATA *);
UDRV_API extern void    USERIAL_Close(tUSERIAL_PORT);
UDRV_API extern BOOLEAN USERIAL_Feature(tUSERIAL_FEATURE);

/*******************************************************************************
 **
 ** Function           USERIAL_GetLineSpeed
 **
 ** Description        This function convert USERIAL baud to line speed.
 **
 ** Output Parameter   None
 **
 ** Returns            line speed
 **
 *******************************************************************************/
UDRV_API extern UINT32 USERIAL_GetLineSpeed(UINT8 baud);
/*******************************************************************************
 **
 ** Function           USERIAL_GetBaud
 **
 ** Description        This function convert line speed to USERIAL baud.
 **
 ** Output Parameter   None
 **
 ** Returns            line speed
 **
 *******************************************************************************/
UDRV_API extern UINT8 USERIAL_GetBaud(UINT32 line_speed);

#ifdef __cplusplus
}
#endif

#endif /* USERIAL_H */
