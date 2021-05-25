/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 * A modified version of termios in Haiku OS
 * Distributed under the terms of the MIT License.
 */

#ifndef _SYS_TERMIOS_H_
#define _SYS_TERMIOS_H_

typedef unsigned long tcflag_t;
typedef unsigned long speed_t;
typedef unsigned char cc_t;

#define NCCS    11          /* number of control characters */

struct termios {
    tcflag_t    c_iflag;    /* input modes */
    tcflag_t    c_oflag;    /* output modes */
    tcflag_t    c_cflag;    /* control modes */
    tcflag_t    c_lflag;    /* local modes */
    cc_t        c_cc[NCCS]; /* control characters */
};

/* control characters */
#define VINTR   0
#define VQUIT   1
#define VERASE  2
#define VKILL   3
#define VEOF    4
#define VEOL    5
#define VMIN    4
#define VTIME   5
#define VEOL2   6
#define VSWTCH  7
#define VSTART  8
#define VSTOP   9
#define VSUSP   10

/* c_iflag - input control modes */
#define IGNBRK      0x0001      /* ignore break condition */
#define BRKINT      0x0002      /* break sends interrupt */
#define IGNPAR      0x0004      /* ignore characters with parity errors */
#define PARMRK      0x0008      /* mark parity errors */
#define INPCK       0x0010      /* enable input parity checking */
#define ISTRIP      0x0020      /* strip high bit from characters */
#define INLCR       0x0040      /* maps newline to CR on input */
#define IGNCR       0x0080      /* ignore carriage returns */
#define ICRNL       0x0100      /* map CR to newline on input */
#define IUCLC       0x0200      /* map all upper case to lower */
#define IXON        0x0400      /* enable input SW flow control */
#define IXANY       0x0800      /* any character will restart input */
#define IXOFF       0x1000      /* enable output SW flow control */

/* c_oflag - output control modes */
#define OPOST       0x0001      /* enable postprocessing of output */
#define OLCUC       0x0002      /* map lowercase to uppercase */
#define ONLCR       0x0004      /* map NL to CR-NL on output */
#define OCRNL       0x0008      /* map CR to NL on output */
#define ONOCR       0x0010      /* no CR output when at column 0 */
#define ONLRET      0x0020      /* newline performs CR function */
#define OFILL       0x0040      /* use fill characters for delays */
#define OFDEL       0x0080      /* Fills are DEL, otherwise NUL */
#define NLDLY       0x0100      /* Newline delays: */
#define NL0         0x0000
#define NL1         0x0100
#define CRDLY       0x0600      /* Carriage return delays: */
#define CR0         0x0000
#define CR1         0x0200
#define CR2         0x0400
#define CR3         0x0600
#define TABDLY      0x1800      /* Tab delays: */
#define TAB0        0x0000
#define TAB1        0x0800
#define TAB2        0x1000
#define TAB3        0x1800
#define BSDLY       0x2000      /* Backspace delays: */
#define BS0         0x0000
#define BS1         0x2000
#define VTDLY       0x4000      /* Vertical tab delays: */
#define VT0         0x0000
#define VT1         0x4000
#define FFDLY       0x8000      /* Form feed delays: */
#define FF0         0x0000
#define FF1         0x8000

/* c_cflag - control modes */
#define CBAUD       0x1000F         /* line speed definitions */
#define B0          0x00            /* hang up */
#define B50         0x01            /* 50 baud */
#define B75         0x02
#define B110        0x03
#define B134        0x04
#define B150        0x05
#define B200        0x06
#define B300        0x07
#define B600        0x08
#define B1200       0x09
#define B1800       0x0A
#define B2400       0x0B
#define B4800       0x0C
#define B9600       0x0D
#define B19200      0x0E
#define B38400      0x0F
#define CBAUDEX     0x10000
#define B57600      0x10001
#define B115200     0x10002
#define B230400     0x10003
#define B460800     0x10004
#define B500000     0x10005
#define B576000     0x10006
#define B921600     0x10007
#define B1000000    0x10008
#define B1152000    0x10009
#define B1500000    0x1000A
#define B2000000    0x1000B
#define B2500000    0x1000C
#define B3000000    0x1000D
#define B3500000    0x1000E
#define B4000000    0x1000F
#define CSIZE       0x0030          /* character size */
#define CS5         0x0000
#define CS6         0x0010
#define CS7         0x0020
#define CS8         0x0030
#define CSTOPB      0x0040          /* send 2 stop bits, not 1 */
#define CREAD       0x0080          /* enable receiver */
#define PARENB      0x0100          /* parity enable */
#define PARODD      0x0200          /* odd parity, else even */
#define HUPCL       0x0400          /* hangs up on last close */
#define CLOCAL      0x0800          /* indicates local line */
#define XLOBLK      0x1000          /* block layer output? */
#define CTSFLOW     0x2000          /* enable CTS flow */
#define RTSFLOW     0x4000          /* enable RTS flow */
#define CRTSCTS     (RTSFLOW | CTSFLOW)

/* c_lflag - local modes */
#define ISIG        0x0001          /* enable signals */
#define ICANON      0x0002          /* Canonical input */
#define XCASE       0x0004          /* Canonical u/l case */
#define ECHO        0x0008          /* Enable echo */
#define ECHOE       0x0010          /* Echo erase as bs-sp-bs */
#define ECHOK       0x0020          /* Echo nl after kill */
#define ECHONL      0x0040          /* Echo nl */
#define NOFLSH      0x0080          /* Disable flush after int or quit */
#define TOSTOP      0x0100          /* stop bg processes that write to tty */
#define IEXTEN      0x0200          /* implementation defined extensions */
#define ECHOCTL     0x0400
#define ECHOPRT     0x0800
#define ECHOKE      0x1000
#define FLUSHO      0x2000
#define PENDIN      0x4000

/* options to tcsetattr() */
#define TCSANOW     0x01            /* make change immediate */
#define TCSADRAIN   0x02            /* drain output, then change */
#define TCSAFLUSH   0x04            /* drain output, flush input */

/* actions for tcflow() */
#define TCOOFF      0x01            /* suspend output */
#define TCOON       0x02            /* restart output */
#define TCIOFF      0x04            /* transmit STOP character, intended to stop input data */
#define TCION       0x08            /* transmit START character, intended to resume input data */

/* values for tcflush() */
#define TCIFLUSH    0x01            /* flush pending input */
#define TCOFLUSH    0x02            /* flush untransmitted output */
#define TCIOFLUSH   0x03            /* flush both */

#define cfmakeraw(termios) \
    do { \
        (termios)->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | \
                                INLCR | IGNCR | ICRNL | IXON); \
        (termios)->c_oflag &= ~OPOST; \
        (termios)->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN); \
        (termios)->c_cflag &= ~(CSIZE | PARENB); \
        (termios)->c_cflag |= CS8; \
    } while (0)

#define cfsetspeed(termios, speed) \
    ({ \
        (termios)->c_cflag &= ~CBAUD; \
        (termios)->c_cflag |= (speed); \
        0; \
    })

#endif /* _SYS_TERMIOS_H_ */
