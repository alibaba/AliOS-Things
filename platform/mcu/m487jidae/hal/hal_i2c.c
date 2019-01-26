/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>

#include "aos/hal/i2c.h"
#include "board.h"

struct nu_i2c_var {
    struct i2c_s *obj;
    uint32_t      ref_cnt;
};

static struct nu_i2c_var i2c0_var = {
    .obj     = NULL,
    .ref_cnt = 0,  
};
static struct nu_i2c_var i2c1_var = {
    .obj     = NULL,
    .ref_cnt = 0,  
};
static struct nu_i2c_var i2c2_var = {
    .obj     = NULL,
    .ref_cnt = 0,  
};

static const struct nu_modinit_s i2c_modinit_tab[] = {
    {I2C_0, I2C0_MODULE, 0, 0, I2C0_RST, I2C0_IRQn, &i2c0_var},
    {I2C_1, I2C1_MODULE, 0, 0, I2C1_RST, I2C1_IRQn, &i2c1_var},
    {I2C_2, I2C2_MODULE, 0, 0, I2C2_RST, I2C2_IRQn, &i2c2_var},

    {NC, 0, 0, 0, 0, (IRQn_Type) 0, NULL}
};

static uint32_t i2c_modinit_mask = 0;


#define NU_I2C_TIMEOUT_STAT_INT     500000
#define NU_I2C_TIMEOUT_STOP         500000
#define TRANCTRL_STARTED        (1)
#define TRANCTRL_NAKLASTDATA    (1 << 1)
#define TRANCTRL_LASTDATANAKED  (1 << 2)

#define NoData         0    // the slave has not been addressed
#define ReadAddressed  1    // the master has requested a read from this slave (slave = transmitter)
#define WriteGeneral   2    // the master is writing to all slave
#define WriteAddressed 3    // the master is writing to this slave (slave = receiver)

static void platform_i2c_enable_int(struct i2c_s *obj)
{
    struct nu_modinit_s *modinit = get_modinit(obj->i2c, i2c_modinit_tab);

    // Enable I2C interrupt
    NVIC_EnableIRQ(modinit->irq_n);
    obj->inten = 1;
}

static void platform_i2c_disable_int(struct i2c_s *obj)
{
    struct nu_modinit_s *modinit = get_modinit(obj->i2c, i2c_modinit_tab);
    
    // Disable I2C interrupt
    NVIC_DisableIRQ(modinit->irq_n);
    obj->inten = 0;
}

static int platform_i2c_set_int(struct i2c_s *obj, int inten)
{
    int inten_back;

    inten_back = obj->inten;

    if (inten) {
        platform_i2c_enable_int(obj);
    } else {
        platform_i2c_disable_int(obj);
    }

    return inten_back;
}

static void platform_i2c_fsm_reset(struct i2c_s *obj, uint32_t i2c_ctl)
{
    I2C_T *i2c_base = (I2C_T *) NU_MODBASE(obj->i2c);

    obj->stop = 0;

    obj->tran_ctrl = 0;

    I2C_SET_CONTROL_REG(i2c_base, i2c_ctl);
    obj->slaveaddr_state = NoData;
}

static void platform_i2c_fsm_tranfini(struct i2c_s *obj, int lastdatanaked)
{
    if (lastdatanaked) {
        obj->tran_ctrl |= TRANCTRL_LASTDATANAKED;
    }

    obj->tran_ctrl &= ~TRANCTRL_STARTED;
    platform_i2c_disable_int(obj);
}

static void i2c_irq(struct nu_i2c_var* psNuI2CVar)
{
        struct i2c_s *obj = psNuI2CVar->obj;    
    I2C_T *i2c_base = (I2C_T *) NU_MODBASE(obj->i2c);
    uint32_t status;

    if (I2C_GET_TIMEOUT_FLAG(i2c_base)) {
        I2C_ClearTimeoutFlag(i2c_base);
        return;
    }

    status = I2C_GET_STATUS(i2c_base);

    switch (status) {
    // Master Transmit
    case 0x28:  // Master Transmit Data ACK
    case 0x18:  // Master Transmit Address ACK
    case 0x08:  // Start
    case 0x10:  // Master Repeat Start
        if ((obj->tran_ctrl & TRANCTRL_STARTED) && obj->tran_pos) {
            if (obj->tran_pos < obj->tran_end) {
                I2C_SET_DATA(i2c_base, *obj->tran_pos ++);
                I2C_SET_CONTROL_REG(i2c_base, I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
            } else {
                platform_i2c_fsm_tranfini(obj, 0);
            }
        } else {
            platform_i2c_disable_int(obj);
        }
        break;

    case 0x30:  // Master Transmit Data NACK
        platform_i2c_fsm_tranfini(obj, 1);
        break;

    case 0x20:  // Master Transmit Address NACK
        platform_i2c_fsm_tranfini(obj, 1);
        break;

    case 0x38:  // Master Arbitration Lost
        platform_i2c_fsm_reset(obj, I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
        break;

    case 0x48:  // Master Receive Address NACK
        platform_i2c_fsm_tranfini(obj, 1);
        break;

    case 0x40:  // Master Receive Address ACK
    case 0x50:  // Master Receive Data ACK
    case 0x58:  // Master Receive Data NACK
        if ((obj->tran_ctrl & TRANCTRL_STARTED) && obj->tran_pos) {
            if (obj->tran_pos < obj->tran_end) {
                if (status == 0x50 || status == 0x58) {
                    *obj->tran_pos ++ = I2C_GET_DATA(i2c_base);
                }

                if (status == 0x58) {
                    platform_i2c_fsm_tranfini(obj, 1);
                } else {
                    uint32_t i2c_ctl = I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk;
                    if ((obj->tran_end - obj->tran_pos) == 1 &&
                            obj->tran_ctrl & TRANCTRL_NAKLASTDATA) {
                        // Last data
                        i2c_ctl &= ~I2C_CTL0_AA_Msk;
                    }
                    I2C_SET_CONTROL_REG(i2c_base, i2c_ctl);
                }
            } else {
                obj->tran_ctrl &= ~TRANCTRL_STARTED;
                platform_i2c_disable_int(obj);
                break;
            }
        } else {
            platform_i2c_disable_int(obj);
        }
        break;

    //case 0x00:  // Bus error

    // Slave Transmit
    case 0xB8:  // Slave Transmit Data ACK
    case 0xA8:  // Slave Transmit Address ACK
    case 0xB0:  // Slave Transmit Arbitration Lost
        if ((obj->tran_ctrl & TRANCTRL_STARTED) && obj->tran_pos) {
            if (obj->tran_pos < obj->tran_end) {
                uint32_t i2c_ctl = I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk;

                I2C_SET_DATA(i2c_base, *obj->tran_pos ++);
                if (obj->tran_pos == obj->tran_end &&
                        obj->tran_ctrl & TRANCTRL_NAKLASTDATA) {
                    // Last data
                    i2c_ctl &= ~I2C_CTL0_AA_Msk;
                }
                I2C_SET_CONTROL_REG(i2c_base, i2c_ctl);
            } else {
                obj->tran_ctrl &= ~TRANCTRL_STARTED;
                platform_i2c_disable_int(obj);
                break;
            }
        } else {
            platform_i2c_disable_int(obj);
        }
        obj->slaveaddr_state = ReadAddressed;
        break;
    //case 0xA0:  // Slave Transmit Repeat Start or Stop
    case 0xC0:  // Slave Transmit Data NACK
    case 0xC8:  // Slave Transmit Last Data ACK
        obj->slaveaddr_state = NoData;
        platform_i2c_fsm_reset(obj, I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
        break;

    // Slave Receive
    case 0x80:  // Slave Receive Data ACK
    case 0x88:  // Slave Receive Data NACK
    case 0x60:  // Slave Receive Address ACK
    case 0x68:  // Slave Receive Arbitration Lost
        obj->slaveaddr_state = WriteAddressed;
        if ((obj->tran_ctrl & TRANCTRL_STARTED) && obj->tran_pos) {
            if (obj->tran_pos < obj->tran_end) {
                if (status == 0x80 || status == 0x88) {
                    *obj->tran_pos ++ = I2C_GET_DATA(i2c_base);
                }

                if (status == 0x88) {
                    obj->slaveaddr_state = NoData;
                    platform_i2c_fsm_reset(obj, I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
                } else {
                    uint32_t i2c_ctl = I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk;
                    if ((obj->tran_end - obj->tran_pos) == 1 &&
                            obj->tran_ctrl & TRANCTRL_NAKLASTDATA) {
                        // Last data
                        i2c_ctl &= ~I2C_CTL0_AA_Msk;
                    }
                    I2C_SET_CONTROL_REG(i2c_base, i2c_ctl);
                }
            } else {
                obj->tran_ctrl &= ~TRANCTRL_STARTED;
                platform_i2c_disable_int(obj);
                break;
            }
        } else {
            platform_i2c_disable_int(obj);
        }
        break;
    //case 0xA0:  // Slave Receive Repeat Start or Stop

    // GC mode
    //case 0xA0:  // GC mode Repeat Start or Stop
    case 0x90:  // GC mode Data ACK
    case 0x98:  // GC mode Data NACK
    case 0x70:  // GC mode Address ACK
    case 0x78:  // GC mode Arbitration Lost
        obj->slaveaddr_state = WriteAddressed;
        if ((obj->tran_ctrl & TRANCTRL_STARTED) && obj->tran_pos) {
            if (obj->tran_pos < obj->tran_end) {
                if (status == 0x90 || status == 0x98) {
                    *obj->tran_pos ++ = I2C_GET_DATA(i2c_base);
                }

                if (status == 0x98) {
                    obj->slaveaddr_state = NoData;
                    platform_i2c_fsm_reset(obj, I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
                } else {
                    uint32_t i2c_ctl = I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk;
                    if ((obj->tran_end - obj->tran_pos) == 1 &&
                            obj->tran_ctrl & TRANCTRL_NAKLASTDATA) {
                        // Last data
                        i2c_ctl &= ~I2C_CTL0_AA_Msk;
                    }
                    I2C_SET_CONTROL_REG(i2c_base, i2c_ctl);
                }
            } else {
                obj->tran_ctrl &= ~TRANCTRL_STARTED;
                platform_i2c_disable_int(obj);
                break;
            }
        } else {
            platform_i2c_disable_int(obj);
        }
        break;

    case 0xF8:  // Bus Released
        break;

    default:
        platform_i2c_fsm_reset(obj, I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
    }
    
}

void I2C0_IRQHandler (void) { i2c_irq(&i2c0_var); }
void I2C1_IRQHandler (void) { i2c_irq(&i2c1_var); }
void I2C2_IRQHandler (void) { i2c_irq(&i2c2_var); }



void platform_i2c_frequency(I2CName s_I2cName, int hz)
{
    I2C_SetBusClockFreq((I2C_T *) NU_MODBASE(s_I2cName), hz);
}

void platform_i2c_mode(I2CName s_I2cName, int mode)
{
    if ( mode == I2C_MODE_SLAVE )
    {
        //Switch to slave mode
    I2C_T *i2c_base = (I2C_T *) NU_MODBASE(s_I2cName);

    // Switch to not addressed mode
    I2C_SET_CONTROL_REG(i2c_base, I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk);
    }
}

static int platform_i2c_addr2data(int address, int read)
{
    return read ? (address | 1) : (address & 0xFE);
}

static int platform_i2c_is_trsn_done(struct i2c_s *obj)
{
    I2C_T *i2c_base = (I2C_T *) NU_MODBASE(obj->i2c);
    int i2c_int;
    uint32_t status;
    int inten_back;

    inten_back = platform_i2c_set_int(obj, 0);
    i2c_int = !! (i2c_base->CTL0 & I2C_CTL0_SI_Msk);
    status = I2C_GET_STATUS(i2c_base);
    platform_i2c_set_int(obj, inten_back);

    return (i2c_int || status == 0xF8);
}

static int platform_i2c_poll_status_timeout(struct i2c_s *obj, int (*platform_is_status)(struct i2c_s *obj), uint32_t timeout)
{
    long long t1, t2, elapsed = 0;
    int status_assert = 0;

    t1 = aos_now_ms();
    while (1) {
        status_assert = platform_is_status(obj);
        if (status_assert) {
            break;
        }

        t2 = aos_now_ms();
        elapsed = (t2 > t1) ? (t2 - t1) : ((uint64_t) t2 + 0xFFFFFFFF - t1 + 1);
        if (elapsed >= timeout) {
            break;
        }
    }

    return (elapsed >= timeout);
}

static int i2c_is_trsn_done(struct i2c_s *obj)
{
    I2C_T *i2c_base = (I2C_T *) NU_MODBASE(obj->i2c);
    int i2c_int;
    uint32_t status;
    int inten_back;

    inten_back = platform_i2c_set_int(obj, 0);
    i2c_int = !! (i2c_base->CTL0 & I2C_CTL0_SI_Msk);
    status = I2C_GET_STATUS(i2c_base);
    platform_i2c_set_int(obj, inten_back);

    return (i2c_int || status == 0xF8);
}

static int platform_i2c_is_tran_started(struct i2c_s *obj)
{
    int started;
    int inten_back;

    inten_back = platform_i2c_set_int(obj, 0);
    started = !! (obj->tran_ctrl & TRANCTRL_STARTED);
    platform_i2c_set_int(obj, inten_back);

    return started;
}

static int platform_i2c_poll_tran_heatbeat_timeout(struct i2c_s *obj, uint32_t timeout)
{
    long long t1, t2, elapsed = 0;
    int tran_started;
    char *tran_pos = NULL;
    char *tran_pos2 = NULL;

    platform_i2c_disable_int(obj);
    tran_pos = obj->tran_pos;
    platform_i2c_enable_int(obj);
    t1 = aos_now_ms();
    while (1) {
        platform_i2c_disable_int(obj);
        tran_started = platform_i2c_is_tran_started(obj);
        platform_i2c_enable_int(obj);
        if (! tran_started) {    // Transfer completed or stopped
            break;
        }

        platform_i2c_disable_int(obj);
        tran_pos2 = obj->tran_pos;
        platform_i2c_enable_int(obj);
        t2 = aos_now_ms();
        if (tran_pos2 != tran_pos) {    // Transfer on-going
            t1 = t2;
            tran_pos = tran_pos2;
            continue;
        }

        elapsed = (t2 > t1) ? (t2 - t1) : ((uint64_t) t2 + 0xFFFFFFFF - t1 + 1);
        if (elapsed >= timeout) {   // Transfer idle
            break;
        }
    }

    return (elapsed >= timeout);
}

static int platform_i2c_do_tran(struct i2c_s *obj, char *buf, int length, int read, int naklastdata)
{
    if (! buf || ! length) {
        return 0;
    }

    int tran_len = 0;

    platform_i2c_disable_int(obj);
    obj->tran_ctrl = naklastdata ? (TRANCTRL_STARTED | TRANCTRL_NAKLASTDATA) : TRANCTRL_STARTED;
    obj->tran_beg = buf;
    obj->tran_pos = buf;
    obj->tran_end = buf + length;
    platform_i2c_enable_int(obj);

    if ( platform_i2c_poll_tran_heatbeat_timeout(obj, NU_I2C_TIMEOUT_STAT_INT)) {
        // N/A
    } else {
        platform_i2c_disable_int(obj);
        tran_len = obj->tran_pos - obj->tran_beg;
        obj->tran_beg = NULL;
        obj->tran_pos = NULL;
        obj->tran_end = NULL;
        platform_i2c_enable_int(obj);
    }

    return tran_len;
}

static int platform_i2c_byte_write(struct i2c_s *obj, int data)
{
    char data_[1];
    data_[0] = data & 0xFF;

    if ( platform_i2c_do_tran(obj, data_, 1, 0, 0) == 1 &&
            ! (obj->tran_ctrl & TRANCTRL_LASTDATANAKED)) {
        return 1;
    } else {
        return 0;
    }
}

static int platform_i2c_do_trsn(struct i2c_s *obj, uint32_t i2c_ctl, int sync)
{
    I2C_T *i2c_base = (I2C_T *) NU_MODBASE(obj->i2c);
    int err = 0;

    platform_i2c_disable_int(obj);

    if (platform_i2c_poll_status_timeout(obj, platform_i2c_is_trsn_done, NU_I2C_TIMEOUT_STAT_INT)) {
        err = HAL_BUSY;
    } else {
        // NOTE: Avoid duplicate Start/Stop. Otherwise, we may meet strange error.
        uint32_t status = I2C_GET_STATUS(i2c_base);

        switch (status) {
        case 0x08:  // Start
        case 0x10:  // Master Repeat Start
            if (i2c_ctl & I2C_CTL0_STA_Msk) {
                return 0;
            } else {
                break;
            }
        case 0xF8:  // Bus Released
            if ((i2c_ctl & (I2C_CTL0_STA_Msk | I2C_CTL0_STO_Msk)) == I2C_CTL0_STO_Msk) {
                return 0;
            } else {
                break;
            }
        }
        I2C_SET_CONTROL_REG(i2c_base, i2c_ctl);
        if (sync && platform_i2c_poll_status_timeout(obj, i2c_is_trsn_done, NU_I2C_TIMEOUT_STAT_INT)) {
            err = HAL_BUSY;
        }
    }

    platform_i2c_enable_int(obj);

    return err;
}

static int platform_i2c_start(struct i2c_s *obj)
{
    return platform_i2c_do_trsn(obj, I2C_CTL0_STA_Msk | I2C_CTL0_SI_Msk, 1);
}

static int platform_i2c_stop(struct i2c_s *obj)
{
    return platform_i2c_do_trsn(obj, I2C_CTL0_STO_Msk | I2C_CTL0_SI_Msk, 1);
}


static int platform_i2c_master_read(struct i2c_s *obj, int address, char *data, int length, int stop)
{
    if (platform_i2c_start(obj)) {
        platform_i2c_stop(obj);
        return HAL_BUSY;
    }

    if (platform_i2c_byte_write(obj, platform_i2c_addr2data(address, 1)) != 1) {
        platform_i2c_stop(obj);
        return HAL_I2C_ERROR_NO_SLAVE;
    }

    // Read in bytes
    length = platform_i2c_do_tran(obj, data, length, 1, 1);

    // If not repeated start, send stop.
    if (stop) {
        platform_i2c_stop(obj);
    }

    return length;
}

static int platform_i2c_master_write(struct i2c_s *obj, int address, const char *data, int length, int stop)
{
    if (platform_i2c_start(obj)) {
        platform_i2c_stop(obj);
        return HAL_BUSY;
    }

    if (platform_i2c_byte_write(obj, platform_i2c_addr2data(address, 0)) != 1) {
        platform_i2c_stop(obj);
        return HAL_I2C_ERROR_NO_SLAVE;
    }

    // Write out bytes
    length = platform_i2c_do_tran(obj, (char *) data, length, 0, 1);

    if (stop) {
        platform_i2c_stop(obj);
    }

    return length;
}

int platform_i2c_slave_receive(struct i2c_s *obj)
{
    int slaveaddr_state;

    platform_i2c_disable_int(obj);
    slaveaddr_state = obj->slaveaddr_state;
    platform_i2c_enable_int(obj);

    return slaveaddr_state;
}

int platform_i2c_slave_read(struct i2c_s *obj, char *data, int length)
{
    return platform_i2c_do_tran(obj, data, length, 1, 1);
}

int platform_i2c_slave_write(struct i2c_s *obj, const char *data, int length)
{
    return platform_i2c_do_tran(obj, (char *) data, length, 0, 1);
}

static int platform_i2c_addr2bspaddr(int address)
{
    return (address >> 1);
}

void platform_i2c_slave_address(struct i2c_s *obj, int idx, uint32_t address, uint32_t mask)
{
    I2C_T *i2c_base = (I2C_T *) NU_MODBASE(obj->i2c);

    platform_i2c_disable_int(obj);

    // NOTE: On NUC472/M451, non-zero slave address can still work as GC mode is enabled.
    //       On M480, non-zero slave address won't work as GC mode is enabled.
    I2C_SetSlaveAddr(i2c_base, 0, platform_i2c_addr2bspaddr(address), I2C_GCMODE_DISABLE);

    platform_i2c_enable_int(obj);
}


static struct i2c_s* hal_get_i2c_s ( i2c_dev_t *i2c )
{
    if ( !(i2c) || (i2c->port >= i32BoardMaxI2CNum) )
        goto exit_hal_get_i2c_s;

    // Get UART Private configuration, these setting are defined in board/xxxxx.
    return (struct i2c_s*)&board_i2c[i2c->port];

exit_hal_get_i2c_s:
    return NULL;
}
/**
 * Initialises an I2C interface
 * Prepares an I2C hardware interface for communication as a master or slave
 *
 * @param[in]  i2c  the device for which the i2c port should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    struct nu_modinit_s *modinit;
    struct i2c_s* pi2c_s;
  struct nu_i2c_var *var;
    I2CName s_I2cName;
    I2C_T* pI2c;

    if ( !(pi2c_s = hal_get_i2c_s ( i2c )) )
        goto exit_hal_i2c_init; 

  uint32_t i2c_sda = pinmap_peripheral(pi2c_s->pin_sda, PinMap_I2C_SDA);
  uint32_t i2c_scl = pinmap_peripheral(pi2c_s->pin_scl, PinMap_I2C_SCL);
  s_I2cName = (I2CName) pinmap_merge(i2c_sda, i2c_scl);
    
  if ( (int)s_I2cName == NC)  
        goto exit_hal_i2c_init;

    modinit = get_modinit(s_I2cName, i2c_modinit_tab);
  if ( modinit == NULL || modinit->modname != s_I2cName )  
        goto exit_hal_i2c_init;

    var = (struct nu_uart_var *) modinit->var;
    if (! var->ref_cnt) {
        
        // Reset this module
        SYS_ResetModule(modinit->rsetidx);

        // Enable IP clock
        CLK_EnableModuleClock(modinit->clkidx);

        pinmap_pinout(pi2c_s->pin_sda, PinMap_I2C_SDA);
        pinmap_pinout(pi2c_s->pin_scl, PinMap_I2C_SCL);

        // NOTE: Setting I2C bus clock to 100 KHz is required.
        I2C_Open((I2C_T *) NU_MODBASE(s_I2cName), 100000);
        
        // NOTE: INTEN bit and FSM control bits (STA, STO, SI, AA) are packed in one register CTL0. We cannot control interrupt through
        //       INTEN bit without impacting FSM control bits. Use NVIC_EnableIRQ/NVIC_DisableIRQ instead for interrupt control.
        I2C_T *i2c_base = (I2C_T *) NU_MODBASE(s_I2cName);
        i2c_base->CTL0 |= (I2C_CTL0_INTEN_Msk | I2C_CTL0_I2CEN_Msk);
        
        // Slave mode?
        platform_i2c_mode(s_I2cName, i2c->config.mode);     

        var->obj = pi2c_s;
    } // if (! var->ref_cnt)
    
    var->ref_cnt ++;
    
    if ( var->ref_cnt ) {
        // Mark this module to be inited.
        int i = modinit - i2c_modinit_tab;
        i2c_modinit_mask |= 1 << i;
    }
    
    return HAL_OK;
    
exit_hal_i2c_init:
    return HAL_ERROR;
}



/**
 * I2c master send
 *
 * @param[in]  i2c       the i2c device
 * @param[in]  dev_addr  device address
 * @param[in]  data      i2c send data
 * @param[in]  size      i2c send data size
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    struct nu_modinit_s *modinit;
    struct i2c_s* pi2c_s;
  struct nu_i2c_var *var;

    if ( !(pi2c_s = hal_get_i2c_s ( i2c )) )
        goto exit_hal_i2c_master_send;
    
    modinit = get_modinit(pi2c_s->i2c, i2c_modinit_tab);
    /* Valid? */
    if ( !modinit ) goto exit_hal_i2c_master_send;
    
    var = modinit->var; 
    /* Initialized? */
    if ( !var->ref_cnt ) goto exit_hal_i2c_master_send;
    
    if ( size != platform_i2c_master_write(pi2c_s, dev_addr, data, size, 1) )
        goto exit_hal_i2c_master_send;
    
    return HAL_OK;

    
exit_hal_i2c_master_send:
    
    return HAL_ERROR;
}

/**
 * I2c master recv
 *
 * @param[in]   i2c       the i2c device
 * @param[in]   dev_addr  device address
 * @param[out]  data      i2c receive data
 * @param[in]   size      i2c receive data size
 * @param[in]   timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                        if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    struct nu_modinit_s *modinit;
    struct i2c_s* pi2c_s;
  struct nu_i2c_var *var;

    if ( !(pi2c_s = hal_get_i2c_s ( i2c )) )
        goto exit_hal_i2c_master_recv;
    
    modinit = get_modinit(pi2c_s->i2c, i2c_modinit_tab);
    /* Valid? */
    if ( !modinit ) goto exit_hal_i2c_master_recv;
    
    var = modinit->var; 
    /* Initialized? */
    if ( !var->ref_cnt ) goto exit_hal_i2c_master_recv;
    
    if ( size != platform_i2c_master_read(pi2c_s, dev_addr, data, size, 1) )
        goto exit_hal_i2c_master_recv;
    
    return HAL_OK;
    
exit_hal_i2c_master_recv:
    
    return HAL_ERROR;}

/**
 * I2c slave send
 *
 * @param[in]  i2c      the i2c device
 * @param[in]  data     i2c slave send data
 * @param[in]  size     i2c slave send data size
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    struct nu_modinit_s *modinit;
    struct i2c_s* pi2c_s;
  struct nu_i2c_var *var;

    if ( !(pi2c_s = hal_get_i2c_s ( i2c )) )
        goto exit_hal_i2c_slave_send;
    
    modinit = get_modinit(pi2c_s->i2c, i2c_modinit_tab);
    /* Valid? */
    if ( !modinit ) goto exit_hal_i2c_slave_send;
    
    var = modinit->var; 
    /* Initialized? */
    if ( !var->ref_cnt ) goto exit_hal_i2c_slave_send;
    
    if ( size != platform_i2c_slave_write(pi2c_s, data, size) )
        goto exit_hal_i2c_slave_send;
    
    return HAL_OK;
    
exit_hal_i2c_slave_send:
    return HAL_ERROR;
}

/**
 * I2c slave receive
 *
 * @param[in]   i2c      tthe i2c device
 * @param[out]  data     i2c slave receive data
 * @param[in]   size     i2c slave receive data size
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    struct nu_modinit_s *modinit;
    struct i2c_s* pi2c_s;
  struct nu_i2c_var *var;

    if ( !(pi2c_s = hal_get_i2c_s ( i2c )) )
        goto exit_hal_i2c_slave_recv;
    
    modinit = get_modinit(pi2c_s->i2c, i2c_modinit_tab);
    /* Valid? */
    if ( !modinit ) goto exit_hal_i2c_slave_recv;
    
    var = modinit->var; 
    /* Initialized? */
    if ( !var->ref_cnt ) goto exit_hal_i2c_slave_recv;
    
    if ( size != platform_i2c_slave_read(pi2c_s, data, size) )
        goto exit_hal_i2c_slave_recv;
    
    return HAL_OK;
    
exit_hal_i2c_slave_recv:
    return HAL_ERROR;

}

/**
 * I2c mem write
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  mem_addr       mem address
 * @param[in]  mem_addr_size  mem address
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                            if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    return HAL_OK;
}

/**
 * I2c master mem read
 *
 * @param[in]   i2c            the i2c device
 * @param[in]   dev_addr       device address
 * @param[in]   mem_addr       mem address
 * @param[in]   mem_addr_size  mem address
 * @param[out]  data           i2c master send data
 * @param[in]   size           i2c master send data size
 * @param[in]  timeout         timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                             if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    return HAL_OK;
}

/**
 * Deinitialises an I2C device
 *
 * @param[in]  i2c  the i2c device
 *
 * @return  0 : on success, EIO : if an error occurred during deinitialisation
 */
int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    struct i2c_s* pi2c_s;
    struct nu_modinit_s *modinit;
    struct nu_i2c_var *var;
    I2C_T *pI2c;

    pi2c_s = hal_get_i2c_s ( i2c );
    if ( !pi2c_s )
        goto exit_hal_i2c_finalize;

    modinit = get_modinit(pi2c_s->i2c, i2c_modinit_tab);
    /* Valid? */
    if ( !modinit ) goto exit_hal_i2c_finalize;

    var = (struct nu_i2c_var *)modinit->var;
    /* Initialized? */
    if ( !var->ref_cnt ) goto exit_hal_i2c_finalize;

    pI2c = (UART_T *) NU_MODBASE(pi2c_s->i2c);
    
    var->ref_cnt --;
    
    if (! var->ref_cnt) {
        
        do {
                    I2C_Close(pI2c);
            
                    platform_i2c_disable_int(pi2c_s);

                    // Disable IP clock
                    CLK_DisableModuleClock(modinit->clkidx);
            
        } while (0);
        
        /* Unlink parent and children. */
        var->obj = NULL ;
    }

    if (! var->ref_cnt) {
        // Mark this module to be deinited.
        int i = modinit - i2c_modinit_tab;
        i2c_modinit_mask &= ~(1 << i);
    }

    return HAL_OK;
    
exit_hal_i2c_finalize:

    return HAL_ERROR;
}
