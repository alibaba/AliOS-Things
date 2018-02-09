#include <platform_opts.h>
#include "serial_api.h"
#include "serial_ex_api.h"
#include "PinNames.h"
#include "i2c_api.h"
#include "pinmap.h"
#include "ex_api.h"

#define MBED_I2C_MTR_SDA    PB_3 //i2c3
#define MBED_I2C_MTR_SCL    PB_2

#define UART_BAUDRATE 115200

#define MBED_I2C_SLAVE_ADDR0    0x4D  // 0x9A  //
#define MBED_I2C_BUS_CLK        500000  //hz *Remind that in baud rate 9600 or 19200, 100000hz is suitable*

static i2c_t   i2cmaster;

#define I2C_DATA_LENGTH         2
static char	i2cdatardsrc[I2C_DATA_LENGTH];
static char	i2cdatarddst[I2C_DATA_LENGTH];

const u8 DLL = 921600/UART_BAUDRATE;

char    ctrl_initial_1[2] = {0x03 << 3,0x80};
char    ctrl_initial_2[2] = {0x00 << 3,921600/UART_BAUDRATE};
char    ctrl_initial_3[2] = {0x01 << 3,0x00};
char    ctrl_initial_4[2] = {0x03 << 3,0xbf};
char    ctrl_initial_5[2] = {0x02 << 3,0x10};
char    ctrl_initial_6[2] = {0x03 << 3,0x03};
char    ctrl_initial_7[2] = {0x02 << 3,0x06};
char    ctrl_initial_8[2] = {0x02 << 3,0x01};
//end i2c


// Master// Tx
#define CLEAR_MST_TXC_FLAG	(masterTXC = 0)
#define SET_MST_TXC_FLAG	(masterTXC = 1)
#define WAIT_MST_TXC		while(masterTXC == 0){;}

volatile int     masterTXC;
static char i2c_ready = 0;


static void i2c_master_rxc_callback(void *userdata)
{

	int     i2clocalcnt;
	int     result = 0;

	// verify result
	result = 1;
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		if (i2cdatarddst[i2clocalcnt] != i2cdatardsrc[i2clocalcnt]) {
			result = 0;
			break;
		}
	}
}

static void i2c_master_txc_callback(void *userdata)
{
    SET_MST_TXC_FLAG;
}

static void i2c_master_write(void)
{

    //DBG_8195A("Mst-W\n");
    CLEAR_MST_TXC_FLAG;

        i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &ctrl_initial_1[0], 2, 1);
        i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &ctrl_initial_2[0], 2, 1);
        i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &ctrl_initial_3[0], 2, 1);
        i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &ctrl_initial_4[0], 2, 1);
        i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &ctrl_initial_5[0], 2, 1);
        i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &ctrl_initial_6[0], 2, 1);
        i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &ctrl_initial_7[0], 2, 1);
        i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &ctrl_initial_8[0], 2, 1);

	WAIT_MST_TXC;

}

static void i2c_master_enable(void)
{
	_memset(&i2cmaster, 0x00, sizeof(i2c_t));
	i2c_init(&i2cmaster, MBED_I2C_MTR_SDA ,MBED_I2C_MTR_SCL);
        i2c_frequency(&i2cmaster,MBED_I2C_BUS_CLK);
        i2c_set_user_callback(&i2cmaster, I2C_RX_COMPLETE, i2c_master_rxc_callback);
        i2c_set_user_callback(&i2cmaster, I2C_TX_COMPLETE, i2c_master_txc_callback);
        //i2c_set_user_callback(&i2cmaster, I2C_ERR_OCCURRED, i2c_master_err_callback);

}

void i2c_redirect_init(void)
{

    // prepare for transmission

    _memset(&i2cdatardsrc[0], 0x00, I2C_DATA_LENGTH);
    _memset(&i2cdatarddst[0], 0x00, I2C_DATA_LENGTH);

    i2c_ready = 1;

    i2c_master_enable();
    i2c_master_write();

}

static u8    tx_data_i2c[2];
static u8    rx_data_i2c[2];

void i2c_put_char(u8 c){

     _memset(&tx_data_i2c[0],0x00,2);
     _memset(&rx_data_i2c[0],0x00,2);
     tx_data_i2c[0] = 0x00 << 3;
     tx_data_i2c[1] = c;
     i2c_write(&i2cmaster, 0x4D, &tx_data_i2c[0], 2, 1);
     i2c_read (&i2cmaster, 0x4D, &rx_data_i2c[0], 2, 1);
}

int use_mode;

void console_init(void)
{      
    i2c_redirect_init();

    if(HalCheckSDramExist()){
        //DiagPrintf("It's 8195_AM\n");
        redirect_rom_init();
    }

#if !TASK_SCHEDULER_DISABLED    
    RtlConsolInitRam((u32)RAM_STAGE,(u32)0,(VOID*)NULL);
#else
    RtlConsolInitRam((u32)ROM_STAGE,(u32)0,(VOID*)NULL);
#endif

#if BUFFERED_PRINTF
    rtl_printf_init();
#endif
}



VOID HalSerialPutcRtl8195a(IN  u8 c){
  
  	u32 CounterIndex = 0;
    
    extern char i2c_ready;
    if(i2c_ready)
        i2c_put_char(c);
    
}










