#ifndef _SDCARD_PUB_H_
#define _SDCARD_PUB_H_

#define SDCARD_SUCCESS                 (0)
#define SDCARD_FAILURE                 ((UINT32)-1)

#define SDCARD_DEV_NAME                ("sdcard")


#define SDCARD_CMD_MAGIC               (0x8709000)

enum
{
    CMD_SDCARD_SEND_BACKGROUND         = SDCARD_CMD_MAGIC + 0,
    CMD_SDCARD_RESET                   = SDCARD_CMD_MAGIC + 1,
};

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void sdcard_init(void);
extern void sdcard_exit(void);

#endif

