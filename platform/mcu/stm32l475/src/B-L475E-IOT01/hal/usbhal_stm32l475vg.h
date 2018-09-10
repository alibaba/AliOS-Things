/* Copyright (c) 2016 mbed.org, MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef USBHAL_STM32L475VG
#define USBHAL_STM32L475VG

#define USBHAL_IRQn  OTG_FS_IRQn


#define NB_ENDPOINT  4
/*  must be multiple of 4 bytes */
#define MAXTRANSFER_SIZE  0x200
#define FIFO_USB_RAM_SIZE (MAXTRANSFER_SIZE+MAX_PACKET_SIZE_EP0+MAX_PACKET_SIZE_EP1+MAX_PACKET_SIZE_EP2+MAX_PACKET_SIZE_EP3)
#if (FIFO_USB_RAM_SIZE > 0x500)
#error "FIFO dimensioning incorrect"
#endif

typedef struct
{
    void (*bus_reset)(void);
    void (*sof)(int frame);
    void (*connect_change)(unsigned int  connected);
    void (*suspend_change)(unsigned int suspended);
    void (*ep0_setup)(void);
    void (*ep0_in)(void);
    void (*ep0_out)(void);
    void (*ep0_read)(void);
    bool (*ep_realise)(uint8_t endpoint, uint32_t maxPacket, uint32_t flags);
    bool (*epCallback[2*NB_ENDPOINT-2])(void);
    uint8_t epComplete[8];
    /*  memorize dummy buffer used for reception */
    uint32_t pBufRx[MAXTRANSFER_SIZE>>2];
    uint32_t pBufRx0[MAX_PACKET_SIZE_EP0>>2];
}USBHAL_Private_t;


#endif

