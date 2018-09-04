


#ifndef ZX29_EFC_H
#define ZX29_EFC_H

#include <tee_types.h>

/* bit operate */
/* set one value to 1,other bit don't change*/
#define HAL_BIT_SET(reg, bit) ((reg) = ((reg) | (1u << (bit))))

/* set one value to 0,other bit don't change*/
#define HAL_BIT_CLR(reg, bit) ((reg) = ((reg) & (~(1u << (bit)))))

#define BIT(n) (1UL << (n))
#define BIT_MASK(n) (BIT(n) - 1)
/****************************************************************************
 *                         Macros
 ****************************************************************************/
/*64K, soft only use 8k*/
#define IROM_BASEADDR (0x0)
/*64K*/
#define IRAM_AP1_BASEADDR (0x10000)
/*16K*/
#define IRAM_AP2_BASEADDR (0x20000)
/*4K*/
#define EFLASH1_INFO_BASEADDR (0xF000000)
/*1216K*/
#define EFLASH1_BASEADDR (0xFED0000)
/*256K*/
#define EFLASH2_BASEADDR (0x10000000)
/*4K*/
#define EFLASH2_CTRL_BASEADDR (0x10080000)
/*1K*/
#define EFLASH2_INFO_BASEADDR (0x10081000)
/*4K*/
#define CK_TIMER0_BASEADDR (0x22000000)
/*4K*/
#define CK_TIMER1_BASEADDR (0x22001000)
/*4K*/
#define AP_WDT_BASEADDR (0x22004000)
/*4K*/
#define AP_CRPM_BASEADDR (0x22005000)
/*4K*/
#define ICU_CK_BASEADDR (0x22008000)
/*4K*/
#define AP_DMACFG_BASEADDR (0x22010000)
/*4K*/
#define GPIO_AO_BASEADDR (0x20130000)
/*4K*/
#define PAD_CTRL_AO_BASEADDR (0x20131000)
/*4K*/
#define PCU_AO_BASEADDR (0x20132000)
/*4K*/
#define LP_TIMER_BASEADDR (0x20133000)
/*4K*/
#define LP_UART_BASEADDR (0x20134000)
/*4K*/
#define A0_CRPM_BASEADDR (0x20136000)
/*4K*/
#define SBY_CRPM_BASEADDR (0x20140000)
/*4K*/
#define PCU_SBY_BASEADDR (0x20141000)
/*4K*/
#define IWDT_BASEADDR (0x20143000)
/*4K*/
#define RTC_BASEADDR (0x20144000)
/*4K*/
#define LPM_BASEADDR (0x20145000)
/*4K*/
#define PMU_CTRL_BASEADDR (0x20146000)
/*4K*/
#define EFLASH1_CTRL_BASEADDR (0x21301000)
/*4K*/
#define ICP_BASEADDR (0x21302000)
/*4K*/
#define PAD_CTRL_PD_BASEADDR (0x21303000)
/*4K*/
#define TOP_CRPM_BASEADDR (0x21306000)
/*4K*/
#define SOC_SYS_BASEADDR (0x21307000)
/*4K*/
#define LSP_CRPM_BASEADDR (0x21400000)
/*4K*/
#define UART0_BASEADDR (0x21401000)
/*4K*/
#define UART1_BASEADDR (0x21402000)
/*4K*/
#define SSP0_BASEADDR (0x21403000)
/*4K*/
#define SSP1_BASEADDR (0x21404000)
/*4K*/
#define I2C0_BASEADDR (0x21405000)
/*4K*/
#define I2S_BASEADDR (0x21406000)
/*4K*/
#define DAC_BASEADDR (0x21407000)
/*4K*/
#define GPIO_PD_BASEADDR (0x21408000)
/*4K*/
#define ADC_BASEADDR (0x21409000)
/*4K*/
#define I2C1_BASEADDR (0x2140A000)
/*4K*/
#define AD_TIMER0_BASEADDR (0x2140B000)
/*4K*/
#define AD_TIMER1_BASEADDR (0x2140C000)
/*4K*/
#define SSP2_BASEADDR (0x2140D000)
/*4K*/
#define UART2_BASEADDR (0x2140E000)
/*4K*/
#define SECURE_CFG_BASEADDR (0x21420000)
/*4K*/
#define AES_BASEADDR (0x21421000)
/*4K*/
#define HASH_BASEADDR (0x21422000)
/*4K*/
#define RSA_BASEADDR (0x21423000)
/*4K*/
#define TRNG_BASEADDR (0x21424000)
/*64M*/
#define SPIFLASH_BASEADDR (0x1C000000)
/*4K*/
#define SPIFLASH_CTRL_BASEADDR (0x20000000)
/*128K*/
#define DTCM_BASEADDR (0x81000000)
/*8K*/
#define CEVA_REG_BASEADDR (0x81040000)
/*4K*/
#define CP_DMACFG_BASEADDR (0x83000000)
/*4K*/
#define DSP_WDT_BASEADDR (0x83002000)
/*4K*/
#define USIM0_BASEADDR (0x83003000)
/*4K*/
#define CP_CRPM_BASEADDR (0x83004000)
/*4K*/
#define DSP_TIMER2_BASEADDR (0x83005000)
/*4K*/
#define DSP_TIMER3_BASEADDR (0x83006000)
/****************************************************************************
 *                              Macros
 ****************************************************************************/
/***** CRPM *****/
#define SYS_TOP_CRPM_ADDR (TOP_CRPM_BASEADDR)
#define TOP_CRPM_EFLASH_CLK (SYS_TOP_CRPM_ADDR + 0x6c)
/***** PMU ******/
#define SYS_PMU_SBY_ADDR (PMU_CTRL_BASEADDR)
/*****TOP_CRM+0x70******/
#define EFLASH_RESET_ADDR (SYS_TOP_CRPM_ADDR + 0x70)

#define EFC1_BASE_ADDR (EFLASH1_CTRL_BASEADDR)
#define EFC2_BASE_ADDR (EFLASH2_CTRL_BASEADDR)
#define EFLASH1_MAIN_START (EFLASH1_BASEADDR)
#define EFLASH2_MAIN_START (EFLASH2_BASEADDR)
#define EFLASH1_INFO_START (EFLASH1_INFO_BASEADDR)
#define EFLASH2_INFO_START (EFLASH2_INFO_BASEADDR)

#define EFLASH1_MAIN_SIZE (1216 * 1024)
#define EFLASH2_MAIN_SIZE (256 * 1024)
#define EFLASH1_PAGE_SIZE (2048)
#define EFLASH2_PAGE_SIZE (512)
#define EFLASH1_INFO_SIZE (2 * 2048)
#define EFLASH2_INFO_SIZE (2 * 512)
#define EFLASH1_FAST_SIZE (1024)
#define EFLASH2_FAST_SIZE (256)
#define EFLASH1_ALLIGN_SIZE (16)
#define EFLASH2_ALLIGN_SIZE (4)
/****reg seting*****/
#define EFLASH_RESET_SHIFT (0)
/*START/STATUS [0X28]*/
#define EFC_START_BUSY_FLAG BIT(0)
#define EFC_WRITE_ENABLE BIT(1)
#define EFC_ERR_ADDR BIT(3)
#define EFC_OTP_LOCKED BIT(6)
#define EFC_DATA_REQ BIT(8)
#define EFC_DATA_ACK BIT(30)
#define EFC_TIMING_UPT (31)
/*TIMING2  [0X30]*/
#define EFC_TIMING_ACR (16)
#define EFC_TIMING_AC (20)
#define EFC_TIMING_FASTPS (24)
/*LOCK  [0X34]*/
#define EFC_RECALL_PIN BIT(0)
/*INT_STATUS  [0X3C]*/
#define EFC_INT_TRANS_DONE BIT(0)
#define EFC_INT_DATA_REQ BIT(1)
/*BUF_CTRL    [0X80]*/
#define EFC_CLR_DSP_IBUF (0)
#define EFC_CLR_DSP_DBUF (1)
#define EFC_CLR_CK802_IBUF BIT(2)
#define EFC_CLR_CK802_DBUF BIT(3)
#define EFC_CLR_M0_BUF (4)
/*EFC_REGION_CFG  [0X108]*/
#define EFC_AP_ACCESS_PRIORITY (0)
#define EFC_SECURE_CTRL (2)
#define EFC_SECURE_ENABLE (5)
/*EFLASH1_MPU_SEC_ENABLE  [0X154]*/
#define EFC1_MPU_SEC_ENABLE (0)
/*EFLASH2_CFG  [0X184]*/
#define EFC2_MPU_SEC_ENABLE (0)
/*EFLASH_CLKEN    [0X06C]*/
#define EFC1_CFG_SW_PCLKEN 0
#define EFC1_CTRL_SW_CLKEN 2
#define EFC2_CTRL_SW_CLKEN 4
#define EFC1_CFG_CLK_REQEN 1
#define EFC1_CTRL_CLK_REQEN 3
#define EFC2_CLK_REQEN 5

#define OP_EN_PASSWORD (0x12345AA5)
#define EFC_DEV_NUM (2)

/*trim position */
#define EFLASH1_TRIM_ADDR \
    (EFLASH1_INFO_START + \
     EFLASH1_PAGE_SIZE) // eflash1 information_region 2nd page start
#define EFLASH2_TRIM_ADDR \
    (EFLASH2_INFO_START + \
     EFLASH2_PAGE_SIZE) // eflash2 information_region 2nd page start
#define EFLASH_TRIM_NUM 3

/*otp position*/
#define EFLASH1_PAGE0_OTP                     \
    (EFLASH1_INFO_START + EFLASH1_PAGE_SIZE - \
     1) // eflash1 information_region 1st page last byte
#define EFLASH1_PAGE1_OTP                         \
    (EFLASH1_INFO_START + EFLASH1_PAGE_SIZE * 2 - \
     1) // eflash1 information_region 2nd page last byte
#define EFLASH2_PAGE0_OTP                     \
    (EFLASH2_INFO_START + EFLASH2_PAGE_SIZE - \
     1) // eflash2 information_region 1st page last byte
#define EFLASH2_PAGE1_OTP                         \
    (EFLASH2_INFO_START + EFLASH2_PAGE_SIZE * 2 - \
     1) // eflash2 information_region 2nd page last byte
#define OTP_LOCK_FLAG (0)

#define ROUNDDOWN(a, b) ((a) & ~((b)-1))

#define IS_EFC1_MAIN_ADDR(addr)      \
    ((addr >= EFLASH1_MAIN_START) && \
     (addr < EFLASH1_MAIN_START + EFLASH1_MAIN_SIZE))
#define IS_EFC1_INFO_ADDR(addr)      \
    ((addr >= EFLASH1_INFO_START) && \
     (addr < EFLASH1_INFO_START + EFLASH1_INFO_SIZE))
#define IS_EFC2_MAIN_ADDR(addr)      \
    ((addr >= EFLASH2_MAIN_START) && \
     (addr < EFLASH2_MAIN_START + EFLASH2_MAIN_SIZE))
#define IS_EFC2_INFO_ADDR(addr)      \
    ((addr >= EFLASH2_INFO_START) && \
     (addr < EFLASH2_INFO_START + EFLASH2_INFO_SIZE))

#define IS_EFC_ADDR(addr)                                  \
    (IS_EFC1_MAIN_ADDR(addr) || IS_EFC1_INFO_ADDR(addr) || \
     IS_EFC2_MAIN_ADDR(addr) || IS_EFC2_INFO_ADDR(addr))
/****************************************************************************
 *                                             Types
 ****************************************************************************/

enum efc_si_dev_num
{
    EFLASH_DEV1       = 0,
    EFLASH_DEV2       = 1,
    MAX_EFLASHDEV_NUM = 2
};

enum efc_si_infopage_num
{
    INFO_PAGE0       = 0,
    INFO_PAGE1       = 1,
    MAX_INFOPAGE_NUM = 2
};

enum efc_si_core_type
{
    CORE_DSP     = 8,
    CORE_CK802   = 9,
    CORE_M0      = 10,
    MAX_EFC_CORE = 11
};

enum efc_si_opera_code
{
    OP_CODE_SET_CONFIG  = 0,
    OP_CODE_SETUP       = 1,
    OP_CODE_PROGRAM     = 2,
    OP_CODE_PAGE_ERASE  = 3,
    OP_CODE_MACRO_ERASE = 4,
    OP_CODE_STANDBY     = 5
};

#define __IOM volatile

typedef struct
{
    __IOM uint32_t version;  /* 0x00 */
    __IOM uint32_t sec_flag; /* 0x04        [4:0]region0-4 flag ,[31]default
                                flag,0:non-sec  1:sec*/
    __IOM uint32_t password; /* 0x08 */
    __IOM          uint32_t
                   op_code; /* 0x0c         [2:0]   000:set config  001:setup  010:program
                               011:page erase  100:macro erase    101:standby*/
    __IOM uint32_t op_addr;      /* 0x10 */
    __IOM uint32_t op_len;       /* 0x14 */
    __IOM uint32_t data[4];      /* 0x18-0x24 */
    __IOM uint32_t start_status; /* 0x28         [0]read as busy flag, when not
busy write 1 will start operation, write 0 has no affection;
__IOM                                                     [1]write enable
[2]command not support  [3]addr out of range  [4]master non-secure
[5]unwriteable zone
__IOM                                                     [6]otp locked [7]error
standby req,already in standby mode  [8]data request in fast program
__IOM                                                     [31]write 1 will
refresh work clock domain timing parameter.write 0 has no affection.*/
    __IOM uint32_t timing1;      /* 0x2c */
    __IOM uint32_t timing2;      /* 0x30 */
    __IOM          uint32_t
                   lock; /* 0x34        [0]this bit controll recall pin status  [1]ctrl opera
                            priority  [3:2]page0-1 lock reg,0 not lock,can be programmed*/
    __IOM uint32_t int_mask; /* 0x38        [0]transfer done int mask    [1]data
                                req int mask,  write 1 to mask*/
    __IOM uint32_t int_status; /* 0x3c        [0]transfer done int  [1]data req
                                  int,  write 1 to clear*/
    __IOM uint32_t debug;      /* 0x40        [7:0]Operation state machine
                                  [9:8]Power state machine*/
    __IOM uint32_t reserved0[15];
    __IOM uint32_t buf_ctrl; /* 0x80         [5:0]buf clear reg, bit0:DSP_I,
bit1:DSP_D, bit2:CK_I, bit3:CK_D, bit4:M0, bit5:DMA   write 1 to clear.
__IOM                                             [10:8]buffer prefetch reg,
bit8:DSP_I, bit9:CK_I, bit10:M0     write 1 to enable*/
    __IOM uint32_t
          pri_cfg[7]; /* 0x84-0x9c     [3:0]default priority [7:4]highest priority
                         [8]priority take effect flag*/
    __IOM uint32_t reserved1[24]; /* 0xA0-0xFC */
    __IOM uint32_t region0_cfg; /* 0x100      [1:0]access priority  [2]1-secure
                                   aera, 0-nonsecure aera  [3]1-none executive
                                   [5]read lock status, write 1 to lock*/
    __IOM uint32_t reserved2;
    __IOM          uint32_t
                   region1_cfg0; /* 0x108      [1:0]access priority    [2]1-secure aera,
             0-nonsecure aera    [3]1-none executive  [5]read lock status, write 1 to lock
__IOM                                             [31:10]start address of
             region1*/
    __IOM uint32_t region1_cfg1; /* 0x10c      [31:10]end address of region1*/
    __IOM uint32_t region2_cfg0; /* 0x110 */
    __IOM uint32_t region2_cfg1; /* 0x114 */
    __IOM uint32_t region3_cfg0; /* 0x118 */
    __IOM uint32_t region3_cfg1; /* 0x11c */
    __IOM uint32_t region4_cfg0; /* 0x120 */
    __IOM uint32_t region4_cfg1; /* 0x124 */
} efc_si_regs;

typedef struct
{
    uint32_t idx;
    uint32_t fast_write_size;
    uint32_t write_allign_size;
    uint32_t page_size;
    uint32_t main_size;
    uint32_t main_base;
    uint32_t info_size;
    uint32_t info_base;
    uint32_t irq_line;
    uint32_t copy_flag;
    uint32_t poll_mode;
    //   eflash_event_cb_t cb;
    //  eflash_status_t status;
    efc_si_regs *reg;
    uint8_t *    page_buffer;
} efc_si_dev_data;


#define EPERM 1         /* Operation not permitted */
#define ENOENT 2        /* No such file or directory */
#define ESRCH 3         /* No such process */
#define EINTR 4         /* Interrupted system call */
#define EIO 5           /* I/O error */
#define ENXIO 6         /* No such device or address */
#define E2BIG 7         /* Argument list too long */
#define ENOEXEC 8       /* Exec format error */
#define EBADF 9         /* Bad file number */
#define ECHILD 10       /* No child processes */
#define EAGAIN 11       /* Try again */
#define ENOMEM 12       /* Out of memory */
#define EACCES 13       /* Permission denied */
#define EFAULT 14       /* Bad address */
#define ENOTBLK 15      /* Block device required */
#define EBUSY 16        /* Device or resource busy */
#define EEXIST 17       /* File exists */
#define EXDEV 18        /* Cross-device link */
#define ENODEV 19       /* No such device */
#define ENOTDIR 20      /* Not a directory */
#define EISDIR 21       /* Is a directory */
#define EINVAL 22       /* Invalid argument */
#define ENFILE 23       /* File table overflow */
#define EMFILE 24       /* Too many open files */
#define ENOTTY 25       /* Not a typewriter */
#define ETXTBSY 26      /* Text file busy */
#define EFBIG 27        /* File too large */
#define ENOSPC 28       /* No space left on device */
#define ESPIPE 29       /* Illegal seek */
#define EROFS 30        /* Read-only file system */
#define EMLINK 31       /* Too many links */
#define EPIPE 32        /* Broken pipe */
#define EDOM 33         /* Math argument out of domain of func */
#define ERANGE 34       /* Math result not representable */
#define EDEADLK 35      /* Resource deadlock would occur */
#define ENAMETOOLONG 36 /* File name too long */
#define ENOLCK 37       /* No record locks available */

/*
 * This error code is special: arch syscall entry code will return
 * -ENOSYS if users try to call a syscall that doesn't exist.  To keep
 * failures of syscalls that really do exist distinguishable from
 * failures due to attempts to use a nonexistent syscall, syscall
 * implementations should refrain from returning -ENOSYS.
 */
#define ENOSYS 38 /* Invalid system call number */

#define ENOTEMPTY 39       /* Directory not empty */
#define ELOOP 40           /* Too many symbolic links encountered */
#define EWOULDBLOCK EAGAIN /* Operation would block */
#define ENOMSG 42          /* No message of desired type */
#define EIDRM 43           /* Identifier removed */
#define ECHRNG 44          /* Channel number out of range */
#define EL2NSYNC 45        /* Level 2 not synchronized */
#define EL3HLT 46          /* Level 3 halted */
#define EL3RST 47          /* Level 3 reset */
#define ELNRNG 48          /* Link number out of range */
#define EUNATCH 49         /* Protocol driver not attached */
#define ENOCSI 50          /* No CSI structure available */
#define EL2HLT 51          /* Level 2 halted */
#define EBADE 52           /* Invalid exchange */
#define EBADR 53           /* Invalid request descriptor */
#define EXFULL 54          /* Exchange full */
#define ENOANO 55          /* No anode */
#define EBADRQC 56         /* Invalid request code */
#define EBADSLT 57         /* Invalid slot */

#define EDEADLOCK EDEADLK

#define EBFONT 59          /* Bad font file format */
#define ENOSTR 60          /* Device not a stream */
#define ENODATA 61         /* No data available */
#define ETIME 62           /* Timer expired */
#define ENOSR 63           /* Out of streams resources */
#define ENONET 64          /* Machine is not on the network */
#define ENOPKG 65          /* Package not installed */
#define EREMOTE 66         /* Object is remote */
#define ENOLINK 67         /* Link has been severed */
#define EADV 68            /* Advertise error */
#define ESRMNT 69          /* Srmount error */
#define ECOMM 70           /* Communication error on send */
#define EPROTO 71          /* Protocol error */
#define EMULTIHOP 72       /* Multihop attempted */
#define EDOTDOT 73         /* RFS specific error */
#define EBADMSG 74         /* Not a data message */
#define EOVERFLOW 75       /* Value too large for defined data type */
#define ENOTUNIQ 76        /* Name not unique on network */
#define EBADFD 77          /* File descriptor in bad state */
#define EREMCHG 78         /* Remote address changed */
#define ELIBACC 79         /* Can not access a needed shared library */
#define ELIBBAD 80         /* Accessing a corrupted shared library */
#define ELIBSCN 81         /* .lib section in a.out corrupted */
#define ELIBMAX 82         /* Attempting to link in too many shared libraries */
#define ELIBEXEC 83        /* Cannot exec a shared library directly */
#define EILSEQ 84          /* Illegal byte sequence */
#define ERESTART 85        /* Interrupted system call should be restarted */
#define ESTRPIPE 86        /* Streams pipe error */
#define EUSERS 87          /* Too many users */
#define ENOTSOCK 88        /* Socket operation on non-socket */
#define EDESTADDRREQ 89    /* Destination address required */
#define EMSGSIZE 90        /* Message too long */
#define EPROTOTYPE 91      /* Protocol wrong type for socket */
#define ENOPROTOOPT 92     /* Protocol not available */
#define EPROTONOSUPPORT 93 /* Protocol not supported */
#define ESOCKTNOSUPPORT 94 /* Socket type not supported */
#define EOPNOTSUPP 95      /* Operation not supported on transport endpoint */
#define EPFNOSUPPORT 96    /* Protocol family not supported */
#define EAFNOSUPPORT 97    /* Address family not supported by protocol */
#define EADDRINUSE 98      /* Address already in use */
#define EADDRNOTAVAIL 99   /* Cannot assign requested address */
#define ENETDOWN 100       /* Network is down */
#define ENETUNREACH 101    /* Network is unreachable */
#define ENETRESET 102      /* Network dropped connection because of reset */
#define ECONNABORTED 103   /* Software caused connection abort */
#define ECONNRESET 104     /* Connection reset by peer */
#define ENOBUFS 105        /* No buffer space available */
#define EISCONN 106        /* Transport endpoint is already connected */
#define ENOTCONN 107       /* Transport endpoint is not connected */
#define ESHUTDOWN 108      /* Cannot send after transport endpoint shutdown */
#define ETOOMANYREFS 109   /* Too many references: cannot splice */
#define ETIMEDOUT 110      /* Connection timed out */
#define ECONNREFUSED 111   /* Connection refused */
#define EHOSTDOWN 112      /* Host is down */
#define EHOSTUNREACH 113   /* No route to host */
#define EALREADY 114       /* Operation already in progress */
#define EINPROGRESS 115    /* Operation now in progress */
#define ESTALE 116         /* Stale file handle */
#define EUCLEAN 117        /* Structure needs cleaning */
#define ENOTNAM 118        /* Not a XENIX named type file */
#define ENAVAIL 119        /* No XENIX semaphores available */
#define EISNAM 120         /* Is a named type file */
#define EREMOTEIO 121      /* Remote I/O error */
#define EDQUOT 122         /* Quota exceeded */

#define ENOMEDIUM 123    /* No medium found */
#define EMEDIUMTYPE 124  /* Wrong medium type */
#define ECANCELED 125    /* Operation Canceled */
#define ENOKEY 126       /* Required key not available */
#define EKEYEXPIRED 127  /* Key has expired */
#define EKEYREVOKED 128  /* Key has been revoked */
#define EKEYREJECTED 129 /* Key was rejected by service */

/* for robust mutexes */
#define EOWNERDEAD 130      /* Owner died */
#define ENOTRECOVERABLE 131 /* State not recoverable */

#define ERFKILL 132 /* Operation not possible due to RF-kill */

#define EHWPOISON 133 /* Memory page has hardware error */

#define ENOTSUP 134 /* Not supported */


int eflash_write_poll(uint32_t addroffset, uint8_t *buff, uint32_t bytesize);
int eflash_read(uint32_t addroffset, uint8_t *buff, uint32_t bytesize);
int eflash_erase(uint32_t addroffset, uint32_t nums);
int eflash_init(void);
#endif
