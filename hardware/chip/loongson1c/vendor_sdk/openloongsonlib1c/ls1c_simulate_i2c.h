// 模拟i2c的头文件



#ifndef __OPENLOONGSON_SIMULATE_H
#define __OPENLOONGSON_SIMULATE_H


// 模拟i2c的接口信息
typedef struct
{
    unsigned int scl_gpio;          // SCL所在gpio引脚
    unsigned int sda_gpio;          // SDA所在gpio引脚
    int delay_time;                 // 周期的1/2，单位us
}simulate_i2c_t;



/*
 * 模拟i2c初始化
 * @i2c_info i2c的接口信息
 */
void simulate_i2c_init(simulate_i2c_t *i2c_info);


/*
 * 模拟I2C的开始
 * @i2c_info i2c接口信息
 */
void simulate_i2c_start(simulate_i2c_t *i2c_info);


/*
 * 模拟I2C的停止
 * @i2c_info i2c接口信息
 */
void simulate_i2c_stop(simulate_i2c_t *i2c_info);


/*
 * 给从设备发送一个ack应答信号
 * @i2c_info i2c接口信息
 */
void simulate_i2c_send_ack(simulate_i2c_t *i2c_info);


/*
 * 给从设备发送一个no ack非应答信号
 * @i2c_info i2c接口信息
 */
void simulate_i2c_send_no_ack(simulate_i2c_t *i2c_info);


/*
 * 读取从设备的ack应答信号
 * @i2c_info i2c接口信息
 * @ret 读取到的信号。0表示应答，1表示非应答
 */
unsigned int simulate_i2c_read_ack(simulate_i2c_t *i2c_info);


/*
 * 主设备从从设备那里读取一个8bit数据
 * @i2c_info i2c接口信息
 * @ret 读取的数据
 */
unsigned char simulate_i2c_read_byte(simulate_i2c_t *i2c_info);


/*
 * 主设备写8bit数据到从设备
 * @i2c_info i2c接口信息
 * @data 待写数据
 */
void simulate_i2c_write_byte(simulate_i2c_t *i2c_info, unsigned char data);



#endif

