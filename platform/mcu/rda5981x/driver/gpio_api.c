/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
#include "mbed_assert.h"
#include "gpio_api.h"
#include "pinmap.h"
#include "rda_ccfg_api.h"
#include "stddef.h"

#define NONE                    (uint32_t)NC
#define GPIO_INT_CTRL_REG       (RDA_GPIO->INTCTRL)
#define GPIO_INT_SEL_REG        (RDA_GPIO->INTSEL)
#define GPIO_DATA_IN_REG        (RDA_GPIO->DIN)

#if defined(GPIO_IRQ_DEBUG)
static uint32_t exception_cntr;
#endif /* GPIO_IRQ_DEBUG */

static uint32_t channel_ids[CHANNEL_NUM] = {0};
static uint32_t channel_arg[CHANNEL_NUM] = {0};
static uint32_t channel_pinidxs[CHANNEL_NUM] = {0};
static uint8_t  channel_bothedge_flag[CHANNEL_NUM] = {0};
static gpio_irq_handler irq_handler[CHANNEL_NUM] = {NULL};

static const PinMap PinMap_GPIO[] = {
    {PB_0, (GPIO_0 |  0), 0},
    {PB_1, (GPIO_0 |  1), 0},
    {PB_2, (GPIO_0 |  2), 0},
    {PB_3, (GPIO_0 |  3), 0},
    {PB_4, (GPIO_0 |  4), 0},
    {PB_5, (GPIO_0 |  5), 0},
    {PB_6, (GPIO_0 |  6), 0},
    {PB_7, (GPIO_0 |  7), 0},
    {PB_8, (GPIO_0 |  8), 0},
    {PB_9, (GPIO_0 |  9), 0},
    {PA_8, (GPIO_0 | 10), 0},
    {PA_9, (GPIO_0 | 11), 0},
    {PC_0, (GPIO_0 | 12), 1},
    {PC_1, (GPIO_0 | 13), 1},
    {PC_2, (GPIO_0 | 14), 0},
    {PC_3, (GPIO_0 | 15), 0},
    {PC_4, (GPIO_0 | 16), 0},
    {PC_5, (GPIO_0 | 17), 0},
    {PC_6, (GPIO_0 | 18), 0},
    {PC_7, (GPIO_0 | 19), 0},
    {PC_8, (GPIO_0 | 20), 0},
    {PC_9, (GPIO_0 | 21), 0},
    {PD_0, (GPIO_0 | 22), 0},
    {PD_1, (GPIO_0 | 23), 0},
    {PD_2, (GPIO_0 | 24), 0},
    {PD_3, (GPIO_0 | 25), 0},
    {PA_0, (GPIO_0 | 26), 1},
    {PA_1, (GPIO_0 | 27), 1},
    {PA_2, (GPIO_0 | 14), 1},
    {PA_3, (GPIO_0 | 15), 1},
    {PA_4, (GPIO_0 | 16), 1},
    {PA_5, (GPIO_0 | 17), 1},
    {PA_6, (GPIO_0 | 18), 1},
    {PA_7, (GPIO_0 | 19), 1},
    {NC,   NC,            0}
};

#define PER_BITBAND_ADDR(reg, bit)      (uint32_t *)(RDA_PERBTBND_BASE + (((uint32_t)(reg)-RDA_PER_BASE)<<5U) + (((uint32_t)(bit))<<2U))

PinName gpio_pinname(int pin_n) {
    MBED_ASSERT(pin_n < GPIO_NUM);
    return PinMap_GPIO[pin_n].pin;
}

uint32_t gpio_set(PinName pin) {
    MBED_ASSERT(pin != (PinName)NC);
    uint32_t func = 0;
    uint32_t idx  = 0;

    func = pinmap_function(pin, PinMap_GPIO);
    idx  = pinmap_peripheral(pin, PinMap_GPIO) & 0x001F;
    pin_function(pin, func);

    return idx;
}

void gpio_init(gpio_t *obj, PinName pin) {
    uint32_t gpio_idx   = 0;

    obj->pin = pin;
    if (pin == (PinName)NC)
        return;

    gpio_idx = gpio_set(pin);

    if((6U <= gpio_idx) && (9U >= gpio_idx)) {
        rda_ccfg_gp((unsigned char)gpio_idx, 0x01U);
    }

    obj->reg_out = PER_BITBAND_ADDR(&RDA_GPIO->DOUT, gpio_idx);
    obj->reg_in  = PER_BITBAND_ADDR(&RDA_GPIO->DIN, gpio_idx);
    obj->reg_dir = PER_BITBAND_ADDR(&RDA_GPIO->DIR, gpio_idx);
}

void gpio_mode(gpio_t *obj, PinMode mode) {
    pin_mode(obj->pin, mode);
}

void gpio_dir(gpio_t *obj, PinDirection direction) {
    uint32_t dir = 0x00UL;
    MBED_ASSERT(obj->pin != (PinName)NC);
    if(PIN_INPUT == direction) {
        dir = 0x01UL;
    }
    if(rda_ccfg_hwver() >= 5) {
        uint32_t gpio_idx  = pinmap_peripheral(obj->pin, PinMap_GPIO) & 0x001F;
        /* Since U05, for gpio 2/3/8/14/15/16/17/20/21, 1'b1 means output */
        if(0x00UL != (0x0033C10CUL & (0x01UL << gpio_idx))) {
            dir ^= 0x01UL;
        }
    }
    *obj->reg_dir = dir;
}

static GPIO_IRQ_IDX_T gpio_irq_ava_chidx(void)
{
    GPIO_IRQ_IDX_T ret;
    for(ret = GPIO_IRQ_CH0; ret < CHANNEL_NUM; ret++) {
        if(0 == channel_ids[ret])
            break;
    }
    return ret;
}

static uint32_t gpio_irq_pinidx(PinName pin)
{
    uint8_t idx;
    const uint32_t pinmap_gpio_irq[GPIO_NUM] = {
        /* GPIO 0 ~ 13 */
        PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7, PB_8, PB_9, PA_8, PA_9, PC_0, PC_1,
        /* GPIO 14 ~ 21, Not support interrupt */
        NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
        /* GPIO 22 ~ 27 */
        PD_0, PD_1, PD_2, PD_3, PA_0, PA_1
    };

    for(idx = 0; idx < GPIO_NUM; idx++) {
        if(pinmap_gpio_irq[idx] ==  NONE) {
            continue;
        } else if(pinmap_gpio_irq[idx] == (uint32_t)pin) {
            break;
        }
    }

    if(GPIO_NUM == idx) {
        error("The pin cannot generate interrupt");
    }
    return idx;
}

static void handle_interrupt_in(void) {
    /* Read current interrupt register */
    uint32_t int_ctrl = GPIO_INT_CTRL_REG;
    uint32_t din_val  = GPIO_DATA_IN_REG;
    uint32_t idx;

    if(int_ctrl & (0x01UL << 16)) {
        GPIO_INT_CTRL_REG |= (0x01UL << 16);
        while(GPIO_INT_CTRL_REG & (0x01UL << 16));
    }
    for(idx = GPIO_IRQ_CH0; idx < CHANNEL_NUM; idx++) {
        if(int_ctrl & (0x01UL << (21 + idx))) {
            gpio_irq_event flagRiseFall_1, flagRiseFall_2;
            GPIO_INT_CTRL_REG |= (0x01UL << (17 + idx)); // clear int flag
            flagRiseFall_1 = (int_ctrl & (0x01UL << (2 + idx))) ? IRQ_RISE : IRQ_FALL;
            flagRiseFall_2 = (din_val & (0x01UL << channel_pinidxs[idx])) ? IRQ_RISE : IRQ_FALL;
            if(channel_bothedge_flag[idx]){
                if(0 != channel_ids[idx]){
                    irq_handler[idx](channel_ids[idx], flagRiseFall_2, channel_arg[idx]);
                }
            }else if(flagRiseFall_1 == flagRiseFall_2) {
               // if(channel_bothedge_flag[idx]) {
                   // GPIO_INT_CTRL_REG ^= (0x01UL << (2 + idx));
               // }
                if (0 != channel_ids[idx]) {
                    irq_handler[idx](channel_ids[idx], flagRiseFall_1, channel_arg[idx]);
                }
#if defined(GPIO_IRQ_DEBUG)
                exception_cntr = 0;
#endif /* GPIO_IRQ_DEBUG */
            }
#if defined(GPIO_IRQ_DEBUG)
            else {
                exception_cntr++;
                if(exception_cntr >= 2) {
                    exception_cntr = 0;
                    mbed_error_printf("invalid gpio irq: %d,%d\r\n", (int)flagRiseFall_1, (int)flagRiseFall_2);
                }
            }
#endif /* GPIO_IRQ_DEBUG */
        }
    }
}

static void irq_handler_entry(uint32_t id, gpio_irq_event event, uint32_t arg) {
    void(*handler)(void*) = (void(*)(void*))id;
    switch (event) {
        case IRQ_RISE: handler((void *)arg); break;
        case IRQ_FALL: handler((void *)arg);
        case IRQ_NONE: break;
    }
}

int gpio_irq_init(gpio_t *obj, uint32_t handler, uint32_t arg) {
    uint32_t regval;

    if (obj->pin == NC) return -1;

    obj->ch = (uint16_t)gpio_irq_ava_chidx();
    MBED_ASSERT(CHANNEL_NUM != obj->ch);

    irq_handler[obj->ch] = irq_handler_entry;

    channel_ids[obj->ch] = handler;
    channel_pinidxs[obj->ch] = gpio_irq_pinidx(obj->pin);
    channel_arg[obj->ch] = arg;
    regval = RDA_GPIO->INTSEL & ~(0x3FFUL << 10);
    RDA_GPIO->INTSEL = regval |  (0x3FFUL << 10);

    NVIC_SetVector(GPIO_IRQn, (uint32_t)handle_interrupt_in);
    NVIC_SetPriority(GPIO_IRQn, 0x1FUL);
    return 0;
}

void gpio_irq_free(gpio_t *obj) {
    channel_ids[obj->ch] = 0;
}

void gpio_irq_set(gpio_t *obj, gpio_irq_event event, uint32_t enable) {
    uint32_t reg_val;
    uint16_t intEn;
    MBED_ASSERT(1 >= obj->ch);

    if(IRQ_RISE == event) {
        obj->flagR = (uint8_t)enable;
    } else {
        obj->flagF = (uint8_t)enable;
    }
    if(obj->flagR && obj->flagF)
        channel_bothedge_flag[obj->ch] = 1U;
    else
        channel_bothedge_flag[obj->ch] = 0U;
    if(obj->flagR || obj->flagF)
        intEn = 1;
    else
        intEn = 0;

    if(0 == intEn) {
        GPIO_INT_CTRL_REG &= ~(0x01UL << (6 + obj->ch));
    } else {
        /* Set interrupt select reg */
        NVIC_DisableIRQ(GPIO_IRQn);
        if(!(obj->flagF && obj->flagR)){
            reg_val = GPIO_INT_SEL_REG & ~(0x1FUL << (5 * obj->ch));
            GPIO_INT_SEL_REG = reg_val | ((0x1FUL & ((channel_pinidxs[obj->ch] >= 22) ? (channel_pinidxs[obj->ch] - 8) : channel_pinidxs[obj->ch])) << (5 * obj->ch));
        }

        /* Set interrupt control reg */
        reg_val = GPIO_INT_CTRL_REG & ~(0x01UL << (2 + obj->ch));
        GPIO_INT_CTRL_REG = reg_val | (((channel_bothedge_flag[obj->ch]) ? (reg_val | (0x01U << obj->ch)) : (((1U == obj->flagR)) ? ((0x01UL << (2 + obj->ch)) | (0x01UL << (17+obj->ch))) : (0x00UL)))
                                    | (0x01UL << (6 + obj->ch)));
        NVIC_EnableIRQ(GPIO_IRQn);
    }
}

void gpio_irq_enable() {
    NVIC_EnableIRQ(GPIO_IRQn);
}

void gpio_irq_disable() {
    NVIC_DisableIRQ(GPIO_IRQn);
}
