#ifndef __serial_h__
#define __serial_h__ 1
/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: serial.h$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the serial I/O functions.
*
*END************************************************************************/

//#include "ioctl.h"

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/* Incoming and outgoing data not processed */
#define IO_SERIAL_RAW_IO             (0)

/* Perform xon/xoff processing */
#define IO_SERIAL_XON_XOFF           (1)

/*
** Perform translation :
**    outgoing \n to CR\LF
**    incoming CR to \n
**    incoming backspace erases previous character
*/
#define IO_SERIAL_TRANSLATION        (2)

/* echo incoming characters */
#define IO_SERIAL_ECHO               (4)

/* Perform hardware flow control processing */
#define IO_SERIAL_HW_FLOW_CONTROL    (8)

/* Serial I/O IOCTL commands */
#define IO_IOCTL_SERIAL_GET_FLAGS        _IO(IO_TYPE_SERIAL,0x01)
#define IO_IOCTL_SERIAL_SET_FLAGS        _IO(IO_TYPE_SERIAL,0x02)
#define IO_IOCTL_SERIAL_GET_BAUD         _IO(IO_TYPE_SERIAL,0x03)
#define IO_IOCTL_SERIAL_SET_BAUD         _IO(IO_TYPE_SERIAL,0x04)
#define IO_IOCTL_SERIAL_GET_STATS        _IO(IO_TYPE_SERIAL,0x05)
#define IO_IOCTL_SERIAL_CLEAR_STATS      _IO(IO_TYPE_SERIAL,0x06)
#define IO_IOCTL_SERIAL_TRANSMIT_DONE    _IO(IO_TYPE_SERIAL,0x07)
#define IO_IOCTL_SERIAL_GET_CONFIG       _IO(IO_TYPE_SERIAL,0x08)

#define IO_IOCTL_SERIAL_GET_HW_SIGNAL    _IO(IO_TYPE_SERIAL,0x09)
#define IO_IOCTL_SERIAL_SET_HW_SIGNAL    _IO(IO_TYPE_SERIAL,0x0A)
#define IO_IOCTL_SERIAL_CLEAR_HW_SIGNAL  _IO(IO_TYPE_SERIAL,0x0B)
/* Standard HW signal names used with GET/SET/CLEAR HW SIGNAL */
#define IO_SERIAL_CTS                    (1)
#define IO_SERIAL_RTS                    (2)
#define IO_SERIAL_DTR                    (4)
#define IO_SERIAL_DSR                    (8)
#define IO_SERIAL_DCD                    (0x10)
#define IO_SERIAL_RI                     (0x20)

#define IO_IOCTL_SERIAL_SET_DATA_BITS    _IO(IO_TYPE_SERIAL,0x0C)
#define IO_IOCTL_SERIAL_GET_DATA_BITS    _IO(IO_TYPE_SERIAL,0x0D)
/* Value used with SET DATA BITS is just the integer number of bits */

#define IO_IOCTL_SERIAL_SET_STOP_BITS    _IO(IO_TYPE_SERIAL,0x0E)
#define IO_IOCTL_SERIAL_GET_STOP_BITS    _IO(IO_TYPE_SERIAL,0x0F)
/* Standard names used with SET STOP BITS */
#define IO_SERIAL_STOP_BITS_1            (1)
#define IO_SERIAL_STOP_BITS_1_5          (2)
#define IO_SERIAL_STOP_BITS_2            (3)

#define IO_IOCTL_SERIAL_SET_PARITY       _IO(IO_TYPE_SERIAL,0x10)
#define IO_IOCTL_SERIAL_GET_PARITY       _IO(IO_TYPE_SERIAL,0x11)
/* Standard parity names used with SET PARITY */
#define IO_SERIAL_PARITY_NONE            (1)
#define IO_SERIAL_PARITY_ODD             (2)
#define IO_SERIAL_PARITY_EVEN            (3)
#define IO_SERIAL_PARITY_FORCE           (4)
#define IO_SERIAL_PARITY_MARK            (5)
#define IO_SERIAL_PARITY_SPACE           (6)

#define IO_IOCTL_SERIAL_START_BREAK      _IO(IO_TYPE_SERIAL,0x12)
#define IO_IOCTL_SERIAL_STOP_BREAK       _IO(IO_TYPE_SERIAL,0x13)
#define IO_IOCTL_SERIAL_TX_DRAINED       _IO(IO_TYPE_SERIAL,0x14)


/*--------------------------------------------------------------------------*/
/*
**                      FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern void    _io_serial_default_init(void);

extern uint_32 _io_serial_polled_install(
      char_ptr, 
      uint_32 (_CODE_PTR_)(pointer, pointer _PTR_, char _PTR_),
      uint_32 (_CODE_PTR_)(pointer, pointer),
      char    (_CODE_PTR_)(pointer),
      void    (_CODE_PTR_)(pointer, char),
      boolean (_CODE_PTR_)(pointer),
      uint_32 (_CODE_PTR_)(pointer, uint_32, pointer),
      pointer, uint_32);

extern uint_32 _io_serial_int_install(
      char_ptr, 
      uint_32 (_CODE_PTR_)(pointer, char _PTR_),
      uint_32 (_CODE_PTR_)(pointer),
      uint_32 (_CODE_PTR_)(pointer, pointer),
      void     (_CODE_PTR_)(pointer, char),
      uint_32 (_CODE_PTR_)(pointer, uint_32, pointer),
      pointer, uint_32);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
