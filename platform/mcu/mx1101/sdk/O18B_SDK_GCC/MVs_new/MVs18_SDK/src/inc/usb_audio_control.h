///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: usb_audio_control.h
//  maintainer: Orson
///////////////////////////////////////////////////////////////////////////////

#ifndef __USB_AUDIO_CONTROL_H__
#define __USB_AUDIO_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


bool UsbAudioControl(void);

void DeviceAudioSetUpData(void* Buf, uint16_t Len);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif


