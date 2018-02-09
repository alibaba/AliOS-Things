#ifndef _USBD_MSC_CONFIG_H
#define _USBD_MSC_CONFIG_H

/* config usb MSC device buffer resource */
#define MSC_NBR_BUFHD	2 /* number of buffer header for bulk in/out data*/
#define MSC_BUFLEN	(20*512)/* Default size of buffer length. Minmun of 512 byte*/

#endif
