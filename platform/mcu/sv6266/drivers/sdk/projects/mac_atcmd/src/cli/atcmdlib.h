#ifndef _ATCMDLIB_H_
#define _ATCMDLIB_H_


#define ATCMD_UARDID SSV6XXX_UART0
#define ATCMD_UARDBAUDRATE BAUD_115200

typedef struct t_ATCMD_CTX
{
    uint8_t                init;
    uint8_t                irqunm;
    EVENTMSG_DATA     data;
} ATCMD_CTX ;

int atcmd_init(void);
int atcmdprintf(const char *fmt, ...);

#endif /* _ATCMDLIB_H_ */
