#ifndef _FLASH_H_
#define _FLASH_H_

#include "include.h"

#define FLASH_DEBUG

#ifdef FLASH_DEBUG
#define FLASH_PRT      os_printf
#define FLASH_WARN     warning_prf
#define FLASH_FATAL    fatal_prf
#else
#define FLASH_PRT      null_prf
#define FLASH_WARN     null_prf
#define FLASH_FATAL    null_prf
#endif

#define FLASH_BASE                           (0x00803000)

#define REG_FLASH_OPERATE_SW                 (FLASH_BASE + 0 * 4)
#define ADDR_SW_REG_POSI                     (0)
#define ADDR_SW_REG_MASK                     (0x00FFFFFF)
#define OP_TYPE_SW_POSI                      (24)
#define OP_TYPE_SW_MASK                      (0x1F)
#define OP_SW                                (0x01UL << 29)
#define WP_VALUE                             (0x01UL << 30)
#define BUSY_SW                              (0x01UL << 31)

#define REG_FLASH_DATA_SW_FLASH              (FLASH_BASE + 1 * 4)

#define REG_FLASH_DATA_FLASH_SW              (FLASH_BASE + 2 * 4)

#define REG_FLASH_RDID_DATA_FLASH            (FLASH_BASE + 4 * 4)

#define REG_FLASH_SR_DATA_CRC_CNT            (FLASH_BASE + 5 * 4)
#define SR_DATA_FLASH_POSI                   (0)
#define SR_DATA_FLASH_MASK                   (0xFF)
#define CRC_ERROR_COUNT_POSI                 (8)
#define CRC_ERROR_COUNT_MASK                 (0xFF)
#define DATA_FLASH_SW_SEL_POSI               (16)
#define DATA_FLASH_SW_SEL_MASK               (0x07)
#define DATA_SW_FLASH_SEL_POSI               (19)
#define DATA_SW_FLASH_SEL_MASK               (0x07)

#define REG_FLASH_CONF                       (FLASH_BASE + 7 * 4)
#define FLASH_CLK_CONF_POSI                  (0)
#define FLASH_CLK_CONF_MASK                  (0x0F)
#define MODEL_SEL_POSI                       (4)
#define MODEL_SEL_MASK                       (0x1F)
#define FWREN_FLASH_CPU                      (0x01UL << 9)
#define WRSR_DATA_POSI                       (10)
#define WRSR_DATA_MASK                       (0x00FFFF)
#define CRC_EN                               (0x01UL << 26)

#define M_VALUE_POST                         22
#define M_VALUE_MASK                         0xFF

#define LINE_MODE_TWO                        2
#define LINE_MODE_FOUR                       4

typedef enum
{
    FLASH_OPCODE_WREN    = 1,
    FLASH_OPCODE_WRDI    = 2,
    FLASH_OPCODE_RDSR    = 3,
    FLASH_OPCODE_WRSR    = 4,
    FLASH_OPCODE_READ    = 5,
    FLASH_OPCODE_RDSR2   = 6,
    FLASH_OPCODE_WRSR2   = 7,
    FLASH_OPCODE_PP      = 12,
    FLASH_OPCODE_SE      = 13,
    FLASH_OPCODE_BE1     = 14,
    FLASH_OPCODE_BE2     = 15,
    FLASH_OPCODE_CE      = 16,
    FLASH_OPCODE_DP      = 17,
    FLASH_OPCODE_RFDP    = 18,
    FLASH_OPCODE_RDID    = 20,
    FLASH_OPCODE_HPM     = 21,
    FLASH_OPCODE_CRMR    = 22,
    FLASH_OPCODE_CRMR2   = 23,
} FLASH_OPCODE;

typedef struct
{
    uint32_t flash_id;
    uint8_t  sr_size;
    uint8_t  line_mode;
    uint8_t  cmp_post;
    uint8_t  protect_post;
    uint8_t  protect_mask;
    uint16_t protect_all;
    uint16_t protect_none;
    uint16_t protect_half;
    uint16_t unprotect_last_block;
    uint8_t  qe_bit_post;
    uint8_t  qe_bit;
    uint8_t  m_value;
    uint8_t  mode_sel;
} flash_config_t;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
UINT32 flash_read(char *user_buf, UINT32 count, UINT32 address);
UINT32 flash_write(char *user_buf, UINT32 count, UINT32 address);
UINT32 flash_ctrl(UINT32 cmd, void *parm);

#endif //_FLASH_H_
