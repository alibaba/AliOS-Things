#ifndef _USB_SLAVE_DEFAULT_CONFIG_H
#define _USB_SLAVE_DEFAULT_CONFIG_H

#ifndef USB_MAX_SLAVE_INTERFACES  
#define USB_MAX_SLAVE_INTERFACES                             16
#endif

#ifndef USB_MAX_SLAVE_CLASS_DRIVER
#define USB_MAX_SLAVE_CLASS_DRIVER                           1
#endif

#ifndef USB_SLAVE_REQUEST_CONTROL_MAX_LENGTH
#define USB_SLAVE_REQUEST_CONTROL_MAX_LENGTH                 256
#endif

#ifndef USB_SLAVE_REQUEST_DATA_MAX_LENGTH
#define USB_SLAVE_REQUEST_DATA_MAX_LENGTH                    2048
#endif

#endif
