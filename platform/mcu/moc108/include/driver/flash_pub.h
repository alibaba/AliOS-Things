#ifndef _FLASH_PUB_H
#define _FLASH_PUB_H

#define FLASH_DEV_NAME                ("flash")

#define FLASH_FAILURE                (1)
#define FLASH_SUCCESS                (0)

#define FLASH_CMD_MAGIC              (0xe240000)
enum
{
    CMD_FLASH_SET_CLK = FLASH_CMD_MAGIC + 1,
    CMD_FLASH_SET_DCO,
    CMD_FLASH_SET_DPLL,

    CMD_FLASH_WRITE_ENABLE,
    CMD_FLASH_WRITE_DISABLE,

    CMD_FLASH_READ_SR,
    CMD_FLASH_WRITE_SR,

    CMD_FLASH_READ_QE,
    CMD_FLASH_SET_QE,

    CMD_FLASH_SET_QWFR,
    CMD_FLASH_CLR_QWFR,

    CMD_FLASH_SET_WSR,
    CMD_FLASH_GET_ID,
    CMD_FLASH_READ_MID,
    CMD_FLASH_ERASE_SECTOR,
    CMD_FLASH_SET_HPM
};

typedef struct
{
    UINT8 byte;
    UINT16 value;
} flash_sr_t;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void flash_init(void);
extern void flash_exit(void);
#endif //_FLASH_PUB_H
