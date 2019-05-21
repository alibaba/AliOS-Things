#ifndef _DDRV_H_
#define _DDRV_H_

/* These 32 ioctl are wireless device private, for 16 commands.
 * Each driver is free to use them for whatever purpose it chooses,
 * however the driver *must* export the description of those ioctls
 * with SIOCGIWPRIV and *must* use arguments as defined below.
 * If you don't follow those rules, DaveM is going to hate you (reason :
 * it make mixed 32/64bit operation impossible).
 */
#define SIOCIWFIRSTPRIV	                 0x8BE0
#define SIOCIWLASTPRIV	                 0x8BFF

extern int ioctl_inet(int dev, u8 vif_index, unsigned int cmd, unsigned long arg);
extern void exit(int seconds);

#endif
// eof

