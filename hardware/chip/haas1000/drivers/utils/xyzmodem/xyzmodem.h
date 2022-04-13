#ifndef __XYZ_MODEM__
#define __XYZ_MODEM__

/*
 * start xmodem transfer
 * param: wait, connect timeout in seconds
 * return, 0 is ok
 *         other value is error;
 */
int xmodem_start_xfer(uint32_t wait);
int xmodem_stop_xfer(void);
int xmodem_send_stream(const uint8_t *data, size_t size, int ascii);

#endif /*__XYZ_MODEM__*/

