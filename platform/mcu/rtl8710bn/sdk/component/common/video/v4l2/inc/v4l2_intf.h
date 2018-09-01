#ifndef _V4L2_INTF_H_
#define _V4L2_INTF_H_

#define VIDEO_NUM_DEVICES	4//should be aligned with value in v4l2-dev.c

typedef enum _streaming_state streaming_state;
enum _streaming_state {
    STREAMING_OFF = 0,
    STREAMING_ON = 1,
    STREAMING_PAUSED = 2,
    STREAMING_READY = 3,
};

long v4l_usr_ioctl(int fd, unsigned int cmd, void *arg);
int v4l_dev_open();
void v4l_dev_release();
void stop_capturing();
int start_capturing();
void uninit_v4l2_device ();
int init_v4l2_device ();
int v4l_set_param(u32 format_type, int *width, int *height, int *frame_rate, int *compression_ratio);
int v4l_getset_usb_ctrl(u8 bRequestType, u8 bRequest, u16 wValue, u16 wIndex, u16 wLength, void *data, int timeout, int set);
extern void spec_v4l2_probe(); 
#endif
