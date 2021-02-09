/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "ft6336.h"
#include "aos/hal/i2c.h"
#include "aos/hal/gpio.h"
#include "hal_iomux_haas1000.h"
#include "uinput.h"
#include "aos/kernel.h"

#define TOUCH_NONE_MAX 3

static i2c_dev_t i2c_dev;
static gpio_dev_t gpio_ft6336_reset;
static gpio_dev_t gpio_ft6336_irq;
static uint32_t ft6336_on_touch_count = 0;
static uint32_t ft6336_no_touch_count = TOUCH_NONE_MAX + 1;
static aos_hdl_t mutex;

event_notify_cb touch_notify;
static FT6336_TouchPointType tp;

// Private Function
uint8_t FT6336_readByte(uint8_t addr)
{
    uint8_t data;

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &addr, 1, 1000);
    aos_msleep(2);
    hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, &data, 1, 1000);
    return data;
}

void FT6336_writeByte(uint8_t addr, uint8_t data)
{
    DEBUG_PRINTLN("")
    DEBUG_PRINT("writeI2C reg 0x")
    DEBUG_PRINT(addr, HEX)
    DEBUG_PRINT(" -> 0x")
    DEBUG_PRINTLN(data, HEX)

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &data, 1, 1000);
}

void ft6336_irq_fuc()
{
    aos_mutex_lock(&mutex, 10);
    ft6336_on_touch_count = 1;
    aos_mutex_unlock(&mutex);
}

void ft6336_scan_task()
{
    while (1)
    {
        if (ft6336_on_touch_count) // irq done
        {
            uinput_event_t tevent;
            tp = FT6336_scan();
            if (tp.tp[0].status == 1)
            {                                  //最多支持两个触点
                if (ft6336_no_touch_count > 1) // keep no touching
                {
                    ft6336_no_touch_count = 0;
                    tevent.type = UINPUT_EVENT_TOUCH_DOWN;
                    tevent.abs.x = tp.tp[0].y;
                    tevent.abs.y = 240 - tp.tp[0].x;
                    printf("touch down x %d  y %d\n", tevent.abs.x, tevent.abs.y);
                    (*touch_notify)(tevent.type, &tevent, sizeof(uinput_event_t));
                }
                else if (ft6336_no_touch_count == 0 || ft6336_no_touch_count == 1)
                {
                    ft6336_no_touch_count = 0;
                    tevent.type = UINPUT_EVENT_TOUCH_MOTION;
                    tevent.abs.x = tp.tp[0].y;
                    tevent.abs.y = 240 - tp.tp[0].x;
                    printf("touch move x %d  y %d\n", tevent.abs.x, tevent.abs.y);
                    (*touch_notify)(tevent.type, &tevent, sizeof(uinput_event_t));
                }
            }
            aos_mutex_lock(&mutex, 10);
            ft6336_on_touch_count = 0;
            aos_mutex_unlock(&mutex);
        }

        if (ft6336_no_touch_count == TOUCH_NONE_MAX)
        {
            printf("touch up\n");
            uinput_event_t tevent;
            tevent.type = UINPUT_EVENT_TOUCH_UP;
            tevent.abs.x = tp.tp[0].y;
            tevent.abs.y = 240 - tp.tp[0].x;
            (*touch_notify)(tevent.type, &tevent, sizeof(uinput_event_t));
            ++ft6336_no_touch_count;
        }
        else if (ft6336_no_touch_count < TOUCH_NONE_MAX)
        {
            ++ft6336_no_touch_count;
        }

        aos_msleep(50);
    }
    aos_mutex_free(&mutex);
}

int uinput_service_init(event_notify_cb func)
{
    touch_notify = func;
    aos_task_new("ft6336_scan_task", ft6336_scan_task, NULL, 5120);
}

void FT6336_init(void)
{
    // Initialize I2C
    i2c_dev.port = 1;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq = I2C_BUS_BIT_RATES_400K;
    i2c_dev.config.mode = I2C_MODE_MASTER;
    i2c_dev.config.dev_addr = I2C_ADDR_FT6336;
    hal_i2c_init(&i2c_dev);
    // Int Pin Configuration

    aos_mutex_new(&mutex);

    gpio_ft6336_reset.port = HAL_IOMUX_PIN_P2_3;
    gpio_ft6336_reset.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&gpio_ft6336_reset);

    hal_gpio_output_low(&gpio_ft6336_reset);
    aos_msleep(10);
    hal_gpio_output_high(&gpio_ft6336_reset);
    aos_msleep(100);

    gpio_ft6336_irq.port = HAL_IOMUX_PIN_P2_2;
    gpio_ft6336_irq.config = IRQ_MODE;
    hal_gpio_enable_irq(&gpio_ft6336_irq, IRQ_TRIGGER_RISING_EDGE, &ft6336_irq_fuc, NULL);
}

uint8_t FT6336_read_device_mode(void)
{
    return (FT6336_readByte(FT6336_ADDR_DEVICE_MODE) & 0x70) >> 4;
}
void FT6336_write_device_mode(DEVICE_MODE_Enum mode)
{
    FT6336_writeByte(FT6336_ADDR_DEVICE_MODE, (mode & 0x07) << 4);
}
uint8_t FT6336_read_gesture_id(void)
{
    return FT6336_readByte(FT6336_ADDR_GESTURE_ID);
}
uint8_t FT6336_read_td_status(void)
{
    return FT6336_readByte(FT6336_ADDR_TD_STATUS);
}
uint8_t FT6336_read_touch_number(void)
{
    return FT6336_readByte(FT6336_ADDR_TD_STATUS) & 0x0F;
}
// Touch 1 functions
uint16_t FT6336_read_touch1_x(void)
{
    uint8_t read_buf[2];
    read_buf[0] = FT6336_readByte(FT6336_ADDR_TOUCH1_X);
    read_buf[1] = FT6336_readByte(FT6336_ADDR_TOUCH1_X + 1);
    return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}
uint16_t FT6336_read_touch1_y(void)
{
    uint8_t read_buf[2];
    read_buf[0] = FT6336_readByte(FT6336_ADDR_TOUCH1_Y);
    read_buf[1] = FT6336_readByte(FT6336_ADDR_TOUCH1_Y + 1);
    return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}
uint8_t FT6336_read_touch1_event(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH1_EVENT) >> 6;
}
uint8_t FT6336_read_touch1_id(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH1_ID) >> 4;
}
uint8_t FT6336_read_touch1_weight(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH1_WEIGHT);
}
uint8_t FT6336_read_touch1_misc(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH1_MISC) >> 4;
}
// Touch 2 functions
uint16_t FT6336_read_touch2_x(void)
{
    uint8_t read_buf[2];
    read_buf[0] = FT6336_readByte(FT6336_ADDR_TOUCH2_X);
    read_buf[1] = FT6336_readByte(FT6336_ADDR_TOUCH2_X + 1);
    return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}
uint16_t FT6336_read_touch2_y(void)
{
    uint8_t read_buf[2];
    read_buf[0] = FT6336_readByte(FT6336_ADDR_TOUCH2_Y);
    read_buf[1] = FT6336_readByte(FT6336_ADDR_TOUCH2_Y + 1);
    return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}
uint8_t FT6336_read_touch2_event(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH2_EVENT) >> 6;
}
uint8_t FT6336_read_touch2_id(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH2_ID) >> 4;
}
uint8_t FT6336_read_touch2_weight(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH2_WEIGHT);
}
uint8_t FT6336_read_touch2_misc(void)
{
    return FT6336_readByte(FT6336_ADDR_TOUCH2_MISC) >> 4;
}

// Mode Parameter Register
uint8_t FT6336_read_touch_threshold(void)
{
    return FT6336_readByte(FT6336_ADDR_THRESHOLD);
}
uint8_t FT6336_read_filter_coefficient(void)
{
    return FT6336_readByte(FT6336_ADDR_FILTER_COE);
}
uint8_t FT6336_read_ctrl_mode(void)
{
    return FT6336_readByte(FT6336_ADDR_CTRL);
}
void FT6336_write_ctrl_mode(CTRL_MODE_Enum mode)
{
    FT6336_writeByte(FT6336_ADDR_CTRL, mode);
}
uint8_t FT6336_read_time_period_enter_monitor(void)
{
    return FT6336_readByte(FT6336_ADDR_TIME_ENTER_MONITOR);
}
uint8_t FT6336_read_active_rate(void)
{
    return FT6336_readByte(FT6336_ADDR_ACTIVE_MODE_RATE);
}
uint8_t FT6336_read_monitor_rate(void)
{
    return FT6336_readByte(FT6336_ADDR_MONITOR_MODE_RATE);
}

// Gesture Parameters
uint8_t FT6336_read_radian_value(void)
{
    return FT6336_readByte(FT6336_ADDR_RADIAN_VALUE);
}
void FT6336_write_radian_value(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_RADIAN_VALUE, val);
}
uint8_t FT6336_read_offset_left_right(void)
{
    return FT6336_readByte(FT6336_ADDR_OFFSET_LEFT_RIGHT);
}
void FT6336_write_offset_left_right(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_OFFSET_LEFT_RIGHT, val);
}
uint8_t FT6336_read_offset_up_down(void)
{
    return FT6336_readByte(FT6336_ADDR_OFFSET_UP_DOWN);
}
void FT6336_write_offset_up_down(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_OFFSET_UP_DOWN, val);
}
uint8_t FT6336_read_distance_left_right(void)
{
    return FT6336_readByte(FT6336_ADDR_DISTANCE_LEFT_RIGHT);
}
void FT6336_write_distance_left_right(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_DISTANCE_LEFT_RIGHT, val);
}
uint8_t FT6336_read_distance_up_down(void)
{
    return FT6336_readByte(FT6336_ADDR_DISTANCE_UP_DOWN);
}
void FT6336_write_distance_up_down(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_DISTANCE_UP_DOWN, val);
}
uint8_t FT6336_read_distance_zoom(void)
{
    return FT6336_readByte(FT6336_ADDR_DISTANCE_ZOOM);
}
void FT6336_write_distance_zoom(uint8_t val)
{
    FT6336_writeByte(FT6336_ADDR_DISTANCE_ZOOM, val);
}

// System Information
uint16_t FT6336_read_library_version(void)
{
    uint8_t read_buf[2];
    read_buf[0] = FT6336_readByte(FT6336_ADDR_LIBRARY_VERSION_H);
    read_buf[1] = FT6336_readByte(FT6336_ADDR_LIBRARY_VERSION_L);
    return ((read_buf[0] & 0x0f) << 8) | read_buf[1];
}
uint8_t FT6336_read_chip_id(void)
{
    return FT6336_readByte(FT6336_ADDR_CHIP_ID);
}
uint8_t FT6336_read_g_mode(void)
{
    return FT6336_readByte(FT6336_ADDR_G_MODE);
}
void FT6336_write_g_mode(G_MODE_Enum mode)
{
    FT6336_writeByte(FT6336_ADDR_G_MODE, mode);
}
uint8_t FT6336_read_pwrmode(void)
{
    return FT6336_readByte(FT6336_ADDR_POWER_MODE);
}
uint8_t FT6336_read_firmware_id(void)
{
    return FT6336_readByte(FT6336_ADDR_FIRMARE_ID);
}
uint8_t FT6336_read_focaltech_id(void)
{
    return FT6336_readByte(FT6336_ADDR_FOCALTECH_ID);
}
uint8_t FT6336_read_release_code_id(void)
{
    return FT6336_readByte(FT6336_ADDR_RELEASE_CODE_ID);
}
uint8_t FT6336_read_state(void)
{
    return FT6336_readByte(FT6336_ADDR_STATE);
}

FT6336_TouchPointType FT6336_scan(void)
{
    FT6336_TouchPointType touchPoint;

    touchPoint.touch_count = FT6336_read_td_status();

    if (touchPoint.touch_count == 0)
    {
        touchPoint.tp[0].status = release;
        touchPoint.tp[1].status = release;
    }
    // else if (touchPoint.touch_count == 1)
    // {
    uint8_t id1 = FT6336_read_touch1_id(); // id1 = 0 or 1
    touchPoint.tp[id1].status = (touchPoint.tp[id1].status == release) ? touch : stream;
    touchPoint.tp[id1].x = FT6336_read_touch1_x();
    touchPoint.tp[id1].y = FT6336_read_touch1_y();
    touchPoint.tp[~id1 & 0x01].status = release;
    // printf("FT6336_read_touch1_event %d\n", FT6336_read_touch1_event());
    // printf("FT6336_read_touch2_event %d\n", FT6336_read_touch2_event());
    // }
    // else
    // {
    //     uint8_t id1 = FT6336_read_touch1_id(); // id1 = 0 or 1
    //     touchPoint.tp[id1].status = (touchPoint.tp[id1].status == release) ? touch : stream;
    //     touchPoint.tp[id1].x = FT6336_read_touch1_x();
    //     touchPoint.tp[id1].y = FT6336_read_touch1_y();
    //     uint8_t id2 = FT6336_read_touch2_id(); // id2 = 0 or 1(~id1 & 0x01)
    //     touchPoint.tp[id2].status = (touchPoint.tp[id2].status == release) ? touch : stream;
    //     touchPoint.tp[id2].x = FT6336_read_touch2_x();
    //     touchPoint.tp[id2].y = FT6336_read_touch2_y();
    // printf("FT6336_read_touch1_event %d\n", FT6336_read_touch1_event());
    // printf("FT6336_read_touch2_event %d\n", FT6336_read_touch2_event());
    // }

    return touchPoint;
}