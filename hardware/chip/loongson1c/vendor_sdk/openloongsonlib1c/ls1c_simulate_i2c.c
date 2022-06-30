// 模拟i2c的源文件


#include "ls1c_public.h"
#include "ls1c_gpio.h"
#include "ls1c_delay.h"
#include "ls1c_simulate_i2c.h"


/*
 * 配置SCL所在gpio引脚为输出模式
 * @i2c_info i2c接口信息
 */
void simulate_i2c_config_scl_out(simulate_i2c_t *i2c_info)
{
    gpio_init(i2c_info->scl_gpio, gpio_mode_output);
    return ;
}


/*
 * 配置SDA所在gpio引脚为输出模式
 * @i2c_info i2c接口信息
 */
void simulate_i2c_config_sda_out(simulate_i2c_t *i2c_info)
{
    gpio_init(i2c_info->sda_gpio, gpio_mode_output);
    return ;
}


/*
 * 配置SDA所在gpio引脚为输入模式
 * @i2c_info i2c接口信息
 */
void simulate_i2c_config_sda_in(simulate_i2c_t *i2c_info)
{
    gpio_init(i2c_info->sda_gpio, gpio_mode_input);
    return ;
}


/*
 * SCL引脚输出高电平
 * @i2c_info i2c接口信息
 */
void simulate_i2c_scl_out_high(simulate_i2c_t *i2c_info)
{
    gpio_set(i2c_info->scl_gpio, gpio_level_high);
    return ;
}


/*
 * SCL引脚输出低电平
 * @i2c_info i2c接口信息
 */
void simulate_i2c_scl_out_low(simulate_i2c_t *i2c_info)
{
    gpio_set(i2c_info->scl_gpio, gpio_level_low);
    return ;
}


/*
 * SDA引脚输出高电平
 * @i2c_info i2c接口信息
 */
void simulate_i2c_sda_out_high(simulate_i2c_t *i2c_info)
{
    gpio_set(i2c_info->sda_gpio, gpio_level_high);
    return ;
}


/*
 * SDA引脚输出低电平
 * @i2c_info i2c接口信息
 */
void simulate_i2c_sda_out_low(simulate_i2c_t *i2c_info)
{
    gpio_set(i2c_info->sda_gpio, gpio_level_low);
    return ;
}


/*
 * 读取SDA引脚
 * @i2c_info i2c接口信息
 * @ret SDA引脚的电平值
 */
unsigned int simulate_i2c_sda_in(simulate_i2c_t *i2c_info)
{
    return gpio_get(i2c_info->sda_gpio);
}




/*
 * 模拟i2c初始化
 * @i2c_info i2c的接口信息
 */
void simulate_i2c_init(simulate_i2c_t *i2c_info)
{
    // SCL输出高电平
    simulate_i2c_config_scl_out(i2c_info);
    simulate_i2c_scl_out_high(i2c_info);
    
    return ;
}


/*
 * 模拟I2C的开始
 * @i2c_info i2c接口信息
 */
void simulate_i2c_start(simulate_i2c_t *i2c_info)
{
    // SDA输出模式
    simulate_i2c_config_sda_out(i2c_info);

    // 这里可能需要一个stop
    simulate_i2c_scl_out_high(i2c_info);
    delay_us(i2c_info->delay_time);
    simulate_i2c_sda_out_high(i2c_info);
    delay_us(2 * i2c_info->delay_time);

    // start
    simulate_i2c_sda_out_low(i2c_info);
    delay_us(i2c_info->delay_time);
    simulate_i2c_scl_out_low(i2c_info);
    delay_us(i2c_info->delay_time);

    return ;
}


/*
 * 模拟I2C的停止
 * @i2c_info i2c接口信息
 */
void simulate_i2c_stop(simulate_i2c_t *i2c_info)
{
    // SDA输出模式
    simulate_i2c_config_sda_out(i2c_info);

    // 先把SCL和SDA拉低
    simulate_i2c_scl_out_low(i2c_info);
    delay_us(i2c_info->delay_time);
    simulate_i2c_sda_out_low(i2c_info);
    delay_us(i2c_info->delay_time);

    // stop
    simulate_i2c_scl_out_high(i2c_info);
    delay_us(i2c_info->delay_time);
    simulate_i2c_sda_out_high(i2c_info);
    delay_us(2 * i2c_info->delay_time);

    return ;
}


/*
 * 给从设备发送一个ack应答信号
 * @i2c_info i2c接口信息
 */
void simulate_i2c_send_ack(simulate_i2c_t *i2c_info)
{
    // SDA输出模式
    simulate_i2c_config_sda_out(i2c_info);

    // SDA=0
    simulate_i2c_sda_out_low(i2c_info);
    delay_us(i2c_info->delay_time);

    // SCL发送一个脉冲
    simulate_i2c_scl_out_high(i2c_info);
    delay_us(i2c_info->delay_time);
    simulate_i2c_scl_out_low(i2c_info);
    delay_us(i2c_info->delay_time);

    return ;
}


/*
 * 给从设备发送一个no ack非应答信号
 * @i2c_info i2c接口信息
 */
void simulate_i2c_send_no_ack(simulate_i2c_t *i2c_info)
{
    // SDA输出模式
    simulate_i2c_config_sda_out(i2c_info);

    // SDA=1
    simulate_i2c_sda_out_high(i2c_info);
    delay_us(i2c_info->delay_time);

    // SCL发送一个脉冲
    simulate_i2c_scl_out_high(i2c_info);
    delay_us(i2c_info->delay_time);
    simulate_i2c_scl_out_low(i2c_info);
    delay_us(i2c_info->delay_time);

    return ;
}


/*
 * 读取从设备的ack应答信号
 * @i2c_info i2c接口信息
 * @ret 读取到的信号。0表示应答，1表示非应答
 */
unsigned int simulate_i2c_read_ack(simulate_i2c_t *i2c_info)
{
    unsigned int ack = 1;
    
    // SDA输入模式，释放SDA
    simulate_i2c_config_sda_in(i2c_info);

    delay_us(i2c_info->delay_time);
    simulate_i2c_scl_out_high(i2c_info);
    delay_us(i2c_info->delay_time);
    ack = simulate_i2c_sda_in(i2c_info);
    simulate_i2c_scl_out_low(i2c_info);
    delay_us(i2c_info->delay_time);

    return ack;
}


/*
 * 主设备从从设备那里读取一个8bit数据
 * @i2c_info i2c接口信息
 * @ret 读取的数据
 */
unsigned char simulate_i2c_read_byte(simulate_i2c_t *i2c_info)
{
    int i;
    unsigned char data = 0;

    // SDA输入模式
    simulate_i2c_config_sda_in(i2c_info);

    for (i=0; i<8; i++)
    {
        delay_us(i2c_info->delay_time);
        simulate_i2c_scl_out_high(i2c_info);
        delay_us(i2c_info->delay_time);

        // 读取一个bit
        data <<= 1;
        if (gpio_level_high == simulate_i2c_sda_in(i2c_info))
            data |= 0x01;

        simulate_i2c_scl_out_low(i2c_info);
    }

    return data;
}


/*
 * 主设备写8bit数据到从设备
 * @i2c_info i2c接口信息
 * @data 待写数据
 */
void simulate_i2c_write_byte(simulate_i2c_t *i2c_info, unsigned char data)
{
    int i;

    // SDA输出模式
    simulate_i2c_config_sda_out(i2c_info);

    for (i=0; i<8; i++)
    {
        delay_us(i2c_info->delay_time);

        // 写一个bit
        if (data & 0x80)
            simulate_i2c_sda_out_high(i2c_info);
        else
            simulate_i2c_sda_out_low(i2c_info);

        delay_us(i2c_info->delay_time);
        simulate_i2c_scl_out_high(i2c_info);
        delay_us(i2c_info->delay_time);
        simulate_i2c_scl_out_low(i2c_info);

        data <<= 1;
    }
    
    delay_us(i2c_info->delay_time);

    return ;
}



