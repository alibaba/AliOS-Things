//#define __SFILE__ "cli_cmd.c"
#include <stdio.h>
#include <string.h>
#include "soc_types.h"
#include "soc_defs.h"
#include "cli.h"

#include "drv/i2cmst/drv_i2cmst.h"
#include "drv/pinmux/drv_pinmux.h"
#include "drv/pinmux/hal_pinmux.h"


typedef enum
{
    i2c_w = 0,
    i2c_r = 1
} i2c_control;

#define AH_DEVICE_VERSION                         0x00  // R
#define AH_FIRMWARE_VERSON                        0x01  // R
#define AH_AUTHENTICATION_PROTOCOL_MAJOR_VERSION  0x02  // R
#define AH_AUTHENTICATION_PROTOCOL_MINOR_VERSION  0x03  // R 
#define AH_DEVICE_ID                              0x04  // R
#define AH_IPOD_CERTIFICATE_DATA_LENGTH           0x50  // RW


#define AH_DEVICE_VERSION_LEN                         0x01  //
#define AH_FIRMWARE_VERSON_LEN                        0x01  // 
#define AH_AUTHENTICATION_PROTOCOL_MAJOR_VERSION_LEN  0x01  // 
#define AH_AUTHENTICATION_PROTOCOL_MINOR_VERSION_LEN  0x01  // 
#define AH_DEVICE_ID_LEN                              0x04  // 
#define AH_IPOD_CERTIFICATE_DATA_LENGTH_LEN           0x02  // 


#define I2C_SLAVE_RW_SIZE       (16)
#define I2C_SLAVE_ADDR          (0x50)
#define I2C_SLAVE_ADDR_WAC      (0x10)

void At_WriteReg (s32 argc, char *argv[])
{
    uint32_t address = strtoul(argv[1], NULL, 16);
    uint32_t data = strtoul(argv[2], NULL, 16);

	printf("reg : write 0x%lx  0x%lx \n", address, data);

	REG32(address)  = data;
}
/*---------------------------------------------------------------------------*/
void At_ReadReg (s32 argc, char *argv[])
{
	char *pkey = 0;
    char *pnum = 0;
    int num = atoi(pnum);
    int i = 0;
    
    uint32_t address = strtoul(argv[1], NULL, 16);
    
    printf("\n");

    if(num == 0) {
        printf("reg : read 0x%x  0x%x \n", address, REG32(address));
    }
    else {
        for(i=0;i<num;i++) {
            printf("0x%08X = 0x%08x\n", (address+i*0x04), REG32(address+i*0x04));
        }
    }
}

void Cmd_i2c_master_rw_wac(s32 argc, char *argv[])
{
    printf("run i2C Master example with wac start...%s %s\n" , __DATE__ , __TIME__);

    uint8_t write_buf[I2C_SLAVE_RW_SIZE] ={0};
    uint8_t read_buf[I2C_SLAVE_RW_SIZE]  ={0};
    uint8_t read_addr[1] ;

    uint16_t i2cmaster_clk = 0;
    uint16_t i2cmaster_id = 0;

    i2cmaster_id = strtoul(argv[1], NULL, 10);
    i2cmaster_clk = strtoul(argv[2], NULL, 10);

    if ((argc != 3) || (i2cmaster_id < 1) || (i2cmaster_id > 2) || (i2cmaster_clk < 1) || (i2cmaster_clk > 1000)) {
        printf("Usage           : i2c_master_rw_wac <id> <clk_KHz>\n");
        printf("<id>            : i2c master id. 1:I2C_01 ; 2:I2C_02\n");
        printf("<clk_Hz>        : clk speed(1K-1000K)\n");
        return;
    }

    if(1 == i2cmaster_id) {
        // GPIO_10 : I2C_M_SDA, GPIO_12 : I2C_M_SCL
        drv_pinmux_manual_function_select_disable(SEL_I2C_MST_II);
        drv_pinmux_manual_function_select_enable(SEL_I2C_MST_I);
    }
    else {
        // GPIO_21 : I2C_M_SCL, GPIO_22 : I2C_M_SDA
        drv_pinmux_manual_function_select_disable(SEL_I2C_MST_I);
        drv_pinmux_manual_function_select_enable(SEL_I2C_MST_II);
    }

    if(0 != drv_i2c_mst_init(i2cmaster_clk)){
        printf("init fail!\n");
        return ;
    }

    printf("===== READ MFi info ===== \n");
    read_addr[0] = AH_DEVICE_VERSION ;
    if(0 != drv_i2c_mst_write(I2C_SLAVE_ADDR_WAC, read_addr, 1, 1, 1)){
        printf("write fail!\n");
        return ;
    }
        
    if(0 != drv_i2c_mst_read(I2C_SLAVE_ADDR_WAC, read_buf, AH_DEVICE_VERSION_LEN, 1, 1)){
        printf("read fail!\n");
        return ;
    }
        
    printf("MFi device version: %d\n", read_buf[0]);

    read_addr[0] = AH_FIRMWARE_VERSON ;
    if(0 != drv_i2c_mst_write(I2C_SLAVE_ADDR_WAC, read_addr, 1, 1, 1)){
        printf("write fail!\n");
        return ;
    }
    if(0 != drv_i2c_mst_read(I2C_SLAVE_ADDR_WAC, read_buf, AH_FIRMWARE_VERSON_LEN, 1, 1)){
        printf("read fail!\n");
        return ;
    }
    printf("MFi fw version: %d\n", read_buf[0]);

    read_addr[0] = AH_AUTHENTICATION_PROTOCOL_MAJOR_VERSION ;
    if(0 != drv_i2c_mst_write(I2C_SLAVE_ADDR_WAC, read_addr, 1, 1, 1)){
        printf("write fail!\n");
        return ;
    }
    if(0 != drv_i2c_mst_read(I2C_SLAVE_ADDR_WAC, read_buf, AH_AUTHENTICATION_PROTOCOL_MAJOR_VERSION_LEN, 1, 1)){
        printf("read fail!\n");
        return ;
    }
    printf("MFi major version: %d\n", read_buf[0]);

    read_addr[0] = AH_AUTHENTICATION_PROTOCOL_MINOR_VERSION ;
    if(0 != drv_i2c_mst_write(I2C_SLAVE_ADDR_WAC, read_addr, 1, 1, 1)){
        printf("write fail!\n");
        return ;
    }
    if(0 != drv_i2c_mst_read(I2C_SLAVE_ADDR_WAC, read_buf, AH_AUTHENTICATION_PROTOCOL_MINOR_VERSION_LEN, 1, 1)){
        printf("read fail!\n");
        return ;
    }
    printf("MFi minor version: %d\n", read_buf[0]);

    read_addr[0] = AH_DEVICE_ID;
    if(0 != drv_i2c_mst_write(I2C_SLAVE_ADDR_WAC, read_addr, 1, 1, 1)){
        printf("write fail!\n");
        return ;
    }
    if(0 != drv_i2c_mst_read(I2C_SLAVE_ADDR_WAC, read_buf, AH_DEVICE_ID_LEN, 1, 1)){
        printf("read fail!\n");
        return ;
    }
    printf("MFi device id: %02d%02d%02d%02d\n", read_buf[0], read_buf[1], read_buf[2], read_buf[3]);

    printf("===== R/W MFi info ===== \n");
    write_buf[0] = AH_IPOD_CERTIFICATE_DATA_LENGTH;

    uint16_t certificate_data_len = 777;
    write_buf[1] = certificate_data_len >> 8;
    write_buf[2] = certificate_data_len & 0xFF;

    printf("write MFi certificate data len: %d\n", certificate_data_len);    
    if(0 != drv_i2c_mst_write(I2C_SLAVE_ADDR_WAC, write_buf, AH_IPOD_CERTIFICATE_DATA_LENGTH_LEN + 1, 1, 1)){
        printf("write fail!\n");
        return ;
    }
    
    read_addr[0] = AH_IPOD_CERTIFICATE_DATA_LENGTH;
    if(0 != drv_i2c_mst_write(I2C_SLAVE_ADDR_WAC, read_addr, 1, 1, 1)){
        printf("write fail!\n");
        return ;
    }
    if(0 != drv_i2c_mst_read(I2C_SLAVE_ADDR_WAC, read_buf, AH_IPOD_CERTIFICATE_DATA_LENGTH_LEN, 1, 1)){
        printf("read fail!\n");
        return ;
    }

    printf("read  MFi certificate data len: %d\n", (uint16_t)(read_buf[0] << 8) + read_buf[1]);

}

void Cmd_i2c_master_rw(s32 argc, char *argv[])
{
    printf("run i2C Master example start...%s %s\n" , __DATE__ , __TIME__);

    uint8_t write_buf[I2C_SLAVE_RW_SIZE];
    uint8_t read_buf[I2C_SLAVE_RW_SIZE];
    uint8_t data_start_addr = 0;

    uint8_t i = 0;
    for(i = 0; i < I2C_SLAVE_RW_SIZE; i ++) {
        write_buf[i] = 0x20 + i;
        read_buf[i] = 0x0;
    }

    write_buf[0] = data_start_addr ;

    uint16_t i2cmaster_clk = 0;
    uint16_t i2cmaster_id = 0;

    i2cmaster_id = strtoul(argv[1], NULL, 10);
    i2cmaster_clk = strtoul(argv[2], NULL, 10);

    if ((argc != 3) || (i2cmaster_id < 1) || (i2cmaster_id > 2) || (i2cmaster_clk < 1) || (i2cmaster_clk > 1000)) {
        printf("Usage           : i2c_master_rw <id> <clk_KHz>\n");
        printf("<id>            : i2c master id. 1:I2C_01 ; 2:I2C_02\n");
        printf("<clk_Hz>        : clk speed(1K-1000K)\n");
        return;
    }

    if(1 == i2cmaster_id) {
        // GPIO_10 : I2C_M_SDA, GPIO_12 : I2C_M_SCL
        drv_pinmux_manual_function_select_disable(SEL_I2C_MST_II);
        drv_pinmux_manual_function_select_enable(SEL_I2C_MST_I);
    }
    else {
        // GPIO_21 : I2C_M_SCL, GPIO_22 : I2C_M_SDA
        drv_pinmux_manual_function_select_disable(SEL_I2C_MST_I);
        drv_pinmux_manual_function_select_enable(SEL_I2C_MST_II);
    }
    
    printf("set clk => %d\n" , i2cmaster_clk);

    if(0 != drv_i2c_mst_init(i2cmaster_clk)){
        printf("init fail!\n");
        return ;
    }
    
    if(0 != drv_i2c_mst_write(I2C_SLAVE_ADDR, write_buf, I2C_SLAVE_RW_SIZE, 1, 1)){
        printf("write fail!\n");
        return ;
    }

    printf("data read from addr[%d]\n", data_start_addr);
    
    if(0 != drv_i2c_mst_write(I2C_SLAVE_ADDR, &data_start_addr, 1, 1, 1)){
        printf("write fail!\n");
        return ;
    }

    
    if(0 != drv_i2c_mst_read(I2C_SLAVE_ADDR, read_buf, I2C_SLAVE_RW_SIZE, 1, 1)){
        printf("read fail!\n");
        return ;
    }
    
    
    for(i = 0; i < I2C_SLAVE_RW_SIZE-1 ; i ++) {
        printf("read_buf[%d] = 0x%x\n",i , read_buf[i]);
    }

    if(0 != memcmp(write_buf+1, read_buf, I2C_SLAVE_RW_SIZE-1)){
        printf("\n[compare r/w data fail!]\n");
        printf("\n[I2C Master Test : FAIL!]\n");
    }else{
        printf("\n[compare r/w data success!]\n");
        printf("\n[I2C Master Test : PASS!]\n");
    }
    
    drv_i2c_mst_deinit();
}

static void Cmd_help (int32_t argc, char *argv[])
{
    printf ("\n*************************************************************************\n");
    printf ("Test I2C Target : \n");
    printf ("               I2C_01 :\n");
    printf ("                   GPIO_10 : I2C_M_SDA\n");
    printf ("                   GPIO_12 : I2C_M_SCL\n");
    printf ("               I2C_02 :\n");
    printf ("                   GPIO_21 : I2C_M_SCL\n");
    printf ("                   GPIO_22 : I2C_M_SDA\n");
    
    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       i2c_master_rw <i2c_id> <clk_Hz>\n");
    printf ("Brief:\n");
    printf ("       This function can test the I2C master to write & read EEPROM data.\n");
    printf ("Note:\n");
    printf ("       None\n");
    printf ("Options:\n");
    printf ("<i2c_id>               The I2C master id. 1:I2C_01 ; 2:I2C_02\n");
    printf ("<clk_Hz>               The I2C master clk speed(1-1000)kHz.\n");
    /*
    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       i2c_master_rw_wac  <clk_Hz>\n");
    printf ("Brief:\n");
    printf ("       This function can test the I2C master to get Mfi inf.\n");
    printf ("Note:\n");
    printf ("       None\n");
    printf ("Options:\n");
    printf ("<clk_Hz>                       The I2C master clk speed(1-1000)kHz.\n");
    */
}

/* ---------------------- Registered CMDs to CMD Table ---------------------- */
const CLICmds gCliCmdTable[] =
{
    { "help",               Cmd_help,                   "i2C Master test help"          	},
    { "i2c_master_rw",      Cmd_i2c_master_rw,          "i2c_master_rw <i2c_id> <clk_Hz>"   },
    //{ "regr",               At_ReadReg,                 "At_WriteReg"                   	},
    //{ "regw",               At_WriteReg,                "At_ReadReg"                    	},
    //{ "i2c_master_rw_wac",  Cmd_i2c_master_rw_wac,      "i2c_master_rw_wac <clk_Hz>"},
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};


