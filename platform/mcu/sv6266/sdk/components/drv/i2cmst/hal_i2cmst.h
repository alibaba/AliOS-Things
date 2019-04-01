#ifndef HAL_I2CMST
#define HAL_I2CMST
#if 0
#define REPEAT_ERROR_CNT (2000)
#else //rick ori
#define REPEAT_ERROR_CNT (0xffffff)
#endif

typedef enum {
    PROTOCOL_WITH_STOP   = 0x00,
    PROTOCOL_WITH_START  = 0x01
} ENUM_I2C_MST_PROTOCOL_T;


typedef enum {
    INT_R_RX_UNDER      = 0,
    INT_R_RX_OVER       = 1,
    INT_R_RX_FULL       = 2,
    INT_R_TX_OVER       = 3,
    INT_R_TX_EMPTY      = 4,
    INT_R_RD_REQ        = 5,
    INT_R_TX_ABRT       = 6,
    INT_R_RX_DONE       = 7,
    INT_R_ACTIVITY      = 8
} ENUM_I2C_MST_INT_STATUS_T;


typedef enum {
    ST_ACTIVITY        = 0,
    ST_TFNF            = 1,
    ST_TFE             = 2,
    ST_RFNE            = 3,
    ST_RFF             = 4,
    ST_MST_ACTIVITY    = 5
} ENUM_I2C_MST_STATUS_T;


typedef enum {
    ABRT_7B_ADDR_NOACK        = 0,
    ABRT_10B_ADDR_1_NOACK     = 1,
    ABRT_10B_ADDR_2_NOACK     = 2,
    ABRT_TXDATA_NOACK         = 3,
    ABRT_GCALL_NOACK          = 4
} ENUM_I2C_MST_ABRT_STATUS_T;


int8_t hal_i2c_mst_enable(void);
int8_t hal_i2c_mst_disable(void);
int8_t hal_i2c_mst_init(uint16_t clk_divider);
int8_t hal_i2c_mst_deinit(void);
int8_t hal_i2c_mst_write_byte(uint8_t i2c_slave_address, uint8_t *data, uint32_t data_length, uint8_t is_start, uint8_t is_stop);
int8_t hal_i2c_mst_read_byte(uint8_t i2c_slave_address, uint8_t *data, uint32_t data_length, uint8_t is_start, uint8_t is_stop);
int8_t hal_i2c_mst_get_interrupt_status(ENUM_I2C_MST_INT_STATUS_T bit);
int8_t hal_i2c_mst_get_status(ENUM_I2C_MST_STATUS_T bit);
int8_t hal_i2c_mst_get_noack_abort_status(ENUM_I2C_MST_ABRT_STATUS_T bit);

#endif /* end of include guard */

