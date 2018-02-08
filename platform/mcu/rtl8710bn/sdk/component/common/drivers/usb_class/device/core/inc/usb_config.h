#ifndef _USB_CONFIG_H_
#define _USB_CONFIG_H_

#include "core/inc/usb_composite.h"

extern _LONG_CALL_ int usb_assign_descriptors(struct usb_function *f,
		struct usb_descriptor_header **fs,
		struct usb_descriptor_header **hs,
		struct usb_descriptor_header **ss);

extern _LONG_CALL_ void usb_free_all_descriptors(struct usb_function *f);
#endif
