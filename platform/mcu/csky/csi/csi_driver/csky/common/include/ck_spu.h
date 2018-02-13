#define getData(value,base,offset) value=*(volatile uint32_t *)(base+offset)
#define putData(value,base,offset) *(volatile uint32_t *)(base+offset)=value

#define GLOBAL_BASE 0x0
#define PROG_BASE   0x0100
#define INST_BASE   0x2100
#define PGPIO_BASE  0x6100

#define SPU_INST_RAM_SIZE 12288
#define SPU_PROG_RAM_SIZE 32768

#define OFFSET_PGPIO_CLK_EN      0x0
#define OFFSET_BLOCKS_CLK_EN     0x4
#define OFFSET_PGPIO_INTR_STATUS 0x8
#define OFFSET_PGPIO_INTR_SEL    0xC
#define OFFSET_PGPIO_START_EN    0x10

#define SPU_CLK_FREQUENCY  20000000
