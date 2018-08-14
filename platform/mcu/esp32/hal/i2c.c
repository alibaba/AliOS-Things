/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <stdint.h>
#include <stdbool.h>
#define i2c_dev_t aos_i2c_dev_t
#define i2c_config_t aos_i2c_config_t
#include "hal/soc/i2c.h"
#undef i2c_dev_t
#undef i2c_config_t
#include "rom/ets_sys.h"
#include "driver/periph_ctrl.h"
#include "soc/i2c_reg.h"
#include "soc/i2c_struct.h"
#include "soc/dport_reg.h"
#include "driver/gpio.h"

#define	 I2C_CMD_MAX		(16u)

enum {
    I2C_CMD_RSTART,
    I2C_CMD_WRITE,
    I2C_CMD_READ,
    I2C_CMD_STOP,
    I2C_CMD_END
};

typedef enum {
    I2C_NUM_0 = 0,
    I2C_NUM_1 ,
    I2C_NUM_MAX
} i2c_port_m;


typedef struct i2c_resource {
    i2c_dev_t * dev;
    uint8_t scl_io;
    uint8_t sda_io;
} i2c_resource_t;


static i2c_resource_t g_dev[I2C_NUM_MAX] = {
    {(volatile i2c_dev_t *)0x60013000,23,22},
    {(volatile i2c_dev_t *)0x60027000,19,18}
};


static int8_t i2c_config_pin(int32_t port,uint32_t sda_num,uint32_t scl_num)
{
    int32_t sda_in_sig = 0;
    int32_t sda_out_sig = 0;
    int32_t scl_in_sig = 0;
    int32_t scl_out_sig = 0;

    if(sda_num >= GPIO_PIN_COUNT || scl_num >= GPIO_PIN_COUNT) {
        return(-1);
    }
    sda_in_sig 	= (I2C_NUM_0 == port) ? (I2CEXT0_SDA_IN_IDX)  : (I2CEXT1_SDA_IN_IDX);
    sda_out_sig = (I2C_NUM_0 == port) ? (I2CEXT0_SDA_OUT_IDX) : (I2CEXT1_SDA_OUT_IDX);
    scl_out_sig = (I2C_NUM_0 == port) ? (I2CEXT0_SCL_OUT_IDX) : (I2CEXT1_SCL_OUT_IDX);
    scl_in_sig 	= (I2C_NUM_0 == port) ? (I2CEXT0_SCL_IN_IDX)  : (I2CEXT1_SCL_IN_IDX);
    gpio_set_level(sda_num, 1);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[sda_num], PIN_FUNC_GPIO);
    gpio_set_direction(sda_num, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(sda_num, GPIO_PULLUP_ONLY);
    gpio_matrix_out(sda_num, sda_out_sig, 0, 0);
    gpio_matrix_in(sda_num, sda_in_sig, 0);
    gpio_set_level(scl_num, 1);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[scl_num], PIN_FUNC_GPIO);
    gpio_set_direction(scl_num, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_matrix_out(scl_num, scl_out_sig, 0, 0);
    gpio_set_pull_mode(scl_num, GPIO_PULLUP_ONLY);
    gpio_matrix_in(scl_num, scl_in_sig, 0);
    if(I2C_NUM_0 == port) {
        periph_module_enable(PERIPH_I2C0_MODULE);
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,DPORT_I2C_EXT0_RST);
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG,DPORT_I2C_EXT0_CLK_EN);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,DPORT_I2C_EXT0_RST);
    } else {
        periph_module_enable(PERIPH_I2C1_MODULE);
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,DPORT_I2C_EXT1_RST);
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG,DPORT_I2C_EXT1_CLK_EN);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,DPORT_I2C_EXT1_RST);
    }

    return (0);
}



static void i2c_config_ctr(i2c_dev_t * handle,uint32_t clk)
{
    int32_t cycle = (APB_CLK_FREQ / clk);
    int32_t half_cycle = cycle / 2;
    handle->ctr.rx_lsb_first = 0;
    handle->ctr.tx_lsb_first = 0;
    handle->ctr.ms_mode = 1; //master==1;0==slave
    handle->ctr.sda_force_out = 1;
    handle->ctr.scl_force_out = 1;
    handle->ctr.sample_scl_level = 0;
    handle->fifo_conf.nonfifo_en = 0;
    handle->timeout.tout = cycle * 8;
    handle->sda_hold.time = half_cycle / 2;
    handle->sda_sample.time = half_cycle / 2;
    handle->scl_low_period.period = half_cycle;
    handle->scl_high_period.period = half_cycle;
    handle->scl_start_hold.time = half_cycle;
    handle->scl_rstart_setup.time = half_cycle;
    handle->scl_stop_hold.time = half_cycle;
    handle->scl_stop_setup.time = half_cycle;

}


static int8_t i2c_config_cmd(i2c_dev_t * handle,uint8_t index,uint8_t op_code,uint8_t byte_num,bool ack_en,bool ack_exp,bool ack_val)
{
    if(index >= I2C_CMD_MAX || NULL == handle) {
        return -1;
    }

    handle->command[index].ack_en = ack_en;
    handle->command[index].ack_exp = ack_exp;
    handle->command[index].ack_val = ack_val;
    handle->command[index].byte_num = byte_num;
    handle->command[index].op_code = op_code;

    return (0);
}

static void i2c_start_prepare(i2c_dev_t * handle)
{
    if(NULL == handle) {
        return;
    }
    handle->fifo_conf.tx_fifo_rst = 1;
    handle->fifo_conf.tx_fifo_rst = 0;
    handle->fifo_conf.rx_fifo_rst = 1;
    handle->fifo_conf.rx_fifo_rst = 0;
    memset(handle->command,0x00,sizeof(handle->command));
    handle->int_clr.val = 0xFFFFFFFF;
}

static void i2c_send_addr(i2c_dev_t * handle,uint8_t is_write,uint16_t addr_value,int8_t addr_width)
{

    uint8_t low_mask = (is_write)?(0xF0):(0xF1);
    if(addr_width) {
        handle->fifo_data.data = (((addr_value >> 8) & 0x6) | low_mask);
        handle->fifo_data.data = ((addr_value >> 1) & 0xFF);
        return;
    }
    handle->fifo_data.data = addr_value & 0xFF;

}

static int8_t i2c_check_busy(i2c_dev_t * handle)
{
    int8_t is_busy = 0;
    unsigned long long start = krhino_sys_time_get();
    while(handle->status_reg.bus_busy) {
        if(krhino_sys_time_get() - start > 20) {
            return(1);
        }
    }
    return (1 == handle->status_reg.bus_busy);
}

static int8_t i2c_check_status(i2c_dev_t * handle)
{
    if(handle->int_raw.arbitration_lost) {
        return(-1);
    }
    if(handle->int_raw.time_out) {
        return (-1);
    }
    if(handle->int_raw.ack_err) {
        unsigned long long start = krhino_sys_time_get();
        while(handle->status_reg.bus_busy) {
            if(krhino_sys_time_get() - start > 2)break;
        }
        return (-1);
    }

    return(0);
}


static int32_t i2c_write_bytes(i2c_dev_t * handle,uint16_t addr,int8_t add_width,uint8_t * buff,uint32_t len,int8_t need_stop)
{
    if(NULL == handle || NULL == buff || len == 0) {
        return(-1);
    }
    if(i2c_check_busy(handle)) {
        return(-1);
    }
    uint16_t dev_addr = addr << 1;
    uint16_t total_size = len + (add_width ? 2 : 1);
    uint16_t send_counts = 0;
    uint16_t packet_size = 0;
    uint16_t send_len = 0;
    while(total_size) {
        packet_size = (total_size > 32) ? 32 : total_size;
        send_len = packet_size;
        i2c_start_prepare(handle);
        i2c_config_cmd(handle,0,I2C_CMD_RSTART,0,false,false,false);
        uint8_t index = 0;
        while(send_len) {
            if(0 == send_counts) {
                i2c_send_addr(handle,1,dev_addr,add_width);
                send_len -= (add_width)?(2):(1);
            }
            index += 1;
            handle->fifo_data.val = buff[send_counts];
            while(handle->status_reg.tx_fifo_cnt < index);
            send_counts += 1;
            send_len -= 1;
        }
        i2c_config_cmd(handle,1,I2C_CMD_WRITE,packet_size,true,false,false);
        total_size -= packet_size;

        if(total_size || !need_stop) {
            i2c_config_cmd(handle,2,I2C_CMD_END,0,false,false,false);
        } else if(need_stop) {
            i2c_config_cmd(handle,2,I2C_CMD_STOP,0,false,false,false);
        }
        handle->ctr.trans_start = 1;
        unsigned long long start = krhino_sys_time_get();
        do {
            if(0 != i2c_check_status(handle)) {
                break;
            }
            if((need_stop && handle->command[2].done) || !handle->status_reg.bus_busy) {
                break;
            }
        } while(krhino_sys_time_get()-start < 20);
    }

    return 0;

}


static int32_t i2c_read_bytes(i2c_dev_t * handle,uint16_t addr,int8_t add_width,uint8_t * buff,uint32_t len,int8_t need_stop)
{
    if(NULL == handle || NULL == buff || len == 0) {
        return(-1);
    }
    if(i2c_check_busy(handle)) {
        return(-1);
    }
    uint8_t is_run = 1;
    uint8_t index = 0;
    uint8_t nextCmdCount = 0;
    uint8_t currentCmdIdx = 0;
    uint8_t cmd_index = 0;
    uint16_t dev_addr = (addr << 1) | 1;
    i2c_start_prepare(handle);
    i2c_config_cmd(handle,cmd_index,I2C_CMD_RSTART,0,false,false,false);
    cmd_index = (cmd_index+1)%I2C_CMD_MAX;
    i2c_send_addr(handle,0,dev_addr,add_width);
    i2c_config_cmd(handle,cmd_index,I2C_CMD_WRITE,add_width?2:1,true,false,false);
    cmd_index = (cmd_index+1)%I2C_CMD_MAX;
    nextCmdCount = cmd_index;
    handle->ctr.trans_start = 1;
    while(is_run) {
        uint32_t startAt = krhino_sys_time_get();
        do {

            if(0 != i2c_check_status(handle)) {
                return (-1);
            }

            if(handle->command[currentCmdIdx].done) {
                nextCmdCount--;
                if (handle->command[currentCmdIdx].op_code == I2C_CMD_READ) {
                    if(currentCmdIdx >= 2) {
                        buff[index++] = handle->fifo_data.val & 0xFF;
                    }
                    handle->fifo_conf.tx_fifo_rst = 1;
                    handle->fifo_conf.tx_fifo_rst = 0;
                    handle->fifo_conf.rx_fifo_rst = 1;
                    handle->fifo_conf.rx_fifo_rst = 0;
                } else if (handle->command[currentCmdIdx].op_code == I2C_CMD_STOP) {
                    is_run = 0;
                    break;
                }
                currentCmdIdx = (currentCmdIdx+1)%I2C_CMD_MAX;
                if(nextCmdCount < 2) {
                    if(len > 0) {
                        i2c_config_cmd(handle, cmd_index, I2C_CMD_READ, 1, false, false, (len==1));
                        cmd_index = (cmd_index+1)%I2C_CMD_MAX;
                        nextCmdCount++;
                        len -= 1;
                    } else {
                        i2c_config_cmd(handle, cmd_index, I2C_CMD_STOP, 0, false, false, false);
                        cmd_index = (cmd_index+1)%I2C_CMD_MAX;
                        nextCmdCount++;
                    }
                }
                break;
            }
        } while(krhino_sys_time_get() - startAt < 20);
    }

    return (0);
}


int32_t hal_i2c_init(aos_i2c_dev_t *i2c)
{
    int32_t ret = 0;
    if(NULL == i2c || (I2C_NUM_0 != i2c->port)&&(I2C_NUM_1 != i2c->port)) {
        return (-1);
    }
    i2c_resource_t * resource = &g_dev[i2c->port];
    i2c_config_pin(i2c->port,resource->sda_io,resource->scl_io);
    i2c_config_ctr(resource->dev,i2c->config.freq);

    return ret;
}

int32_t hal_i2c_master_send(aos_i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if(NULL == i2c || (I2C_NUM_0 != i2c->port)&&(I2C_NUM_1 != i2c->port)) {
        return (-1);
    }
    i2c_resource_t * resource = &g_dev[i2c->port];
    uint16_t addr = dev_addr >> 1;
    i2c_write_bytes(resource->dev,addr,i2c->config.address_width == I2C_MEM_ADDR_SIZE_16BIT,data,size,0);

    return ret;
}

int32_t hal_i2c_master_recv(aos_i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if(NULL == i2c || (I2C_NUM_0 != i2c->port)&&(I2C_NUM_1 != i2c->port)) {
        return (-1);
    }
    i2c_resource_t * resource = &g_dev[i2c->port];
    uint16_t addr = dev_addr >> 1;
    i2c_read_bytes(resource->dev,addr,i2c->config.address_width == I2C_MEM_ADDR_SIZE_16BIT,data,size,0);

    return ret;
}

int32_t hal_i2c_slave_send(aos_i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;

    return ret;
}

int32_t hal_i2c_slave_recv(aos_i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;

    return ret;
}

int32_t hal_i2c_mem_write(aos_i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int32_t ret = 0;

    return ret;
}

int32_t hal_i2c_mem_read(aos_i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int32_t ret = 0;

    return ret;
}

int32_t hal_i2c_finalize(aos_i2c_dev_t *i2c)
{
    int32_t ret = 0;

    return ret;
}


