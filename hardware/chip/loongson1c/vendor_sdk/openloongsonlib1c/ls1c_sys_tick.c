// 系统滴答定时器相关接口

#include <k_api.h>
#include "ls1c_mipsregs.h"


#define CPU_HZ  (252 * 1000000)


static volatile unsigned long system_tick;


// 禁止滴答定时器
void sys_tick_disable(void)
{
    unsigned int status = 0;

    status = read_c0_status();
    status &= (~STATUSF_IP7);
    write_c0_status(status);

    return ;
}


// 使能滴答定时器
void sys_tick_enable(void)
{
    unsigned int status = 0;

    status = read_c0_status();
    status |= STATUSF_IP7;
    write_c0_status(status);

    return ;
}


// 滴答定时器初始化
void sys_tick_init(unsigned int tick)
{
    // 设置定时时间
    write_c0_compare(CPU_HZ/2/tick);
    write_c0_count(0);

    // 使能
    sys_tick_enable();

    return ;
}


void sys_tick_increase(void)
{
    ++system_tick;
}


// 滴答定时器中断处理函数
void sys_tick_handler(void)
{
    unsigned int count;

    count = read_c0_compare();
    write_c0_compare(count);
    write_c0_count(0);

    sys_tick_increase();
    krhino_tick_proc();
}


// 获取tick值
unsigned long sys_tick_get(void)
{
    return system_tick;
}
