
#ifndef __2NDBOOT_FLASH_H__
#define __2NDBOOT_FLASH_H__

#define REG_READ(addr)                       *((volatile UINT32 *)(addr))
#define REG_WRITE(addr, _data) 	             (*((volatile UINT32 *)(addr)) = (_data))

#define MODE_STD                             0
#define MODE_DUAL                            1
#define MODE_QUAD                            2

#define FLASH_WRITE_UNIT                     32

#define FLASH_BASE                           (0x00803000)

#define REG_FLASH_OPERATE_SW                 (FLASH_BASE + 0 * 4)
#define ADDR_SW_REG_POSI                     (0)
#define ADDR_SW_REG_MASK                     (0xFFFFFF)
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
#define WRSR_DATA_MASK                       (0xFFFF)
#define CRC_EN                               (0x01UL << 26)

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
#endif // __2NDBOOT_FLASH_H__

