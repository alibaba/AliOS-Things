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
	CMD_FLASH_SET_HPM,
    CMD_FLASH_SET_PROTECT
};

typedef enum
{
    FLASH_PROTECT_NONE,
    FLASH_PROTECT_ALL,
    FLASH_PROTECT_HALF,
    FLASH_UNPROTECT_LAST_BLOCK
} PROTECT_TYPE;

typedef enum
{
    FLASH_XTX_16M_SR_WRITE_DISABLE,
    FLASH_XTX_16M_SR_WRITE_ENABLE
} XTX_FLASH_MODE;

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
extern UINT8 flash_get_line_mode(void);
extern void flash_set_line_mode(UINT8 );

#endif //_FLASH_PUB_H
