#include <stdint.h>
#include <stdio.h>
#include <aos/errno.h>
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include <vfsdev/pwm_dev.h>

int vfs_pwm_test(int channel, int freq, unsigned int period_s)
{
    int ret = -1;
    int fd = 0;
    char name[16] = {0};
    float duty_cycle = 0.5;

    if (period_s < 1)
        period_s = 2;

    snprintf(name, sizeof(name), "/dev/pwm%d", channel);
    fd = open(name, 0);
    printf("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {

        ret = ioctl(fd, IOC_PWM_FREQ, (unsigned long)freq);
        printf("set pwm's freq to %d %s, ret:%d\r\n", freq, ret ? "fail" : "succeed", ret);

        ret = ioctl(fd, IOC_PWM_DUTY_CYCLE, (unsigned long)&duty_cycle);
        printf("set pwm's duty cycle to %f %s, ret:%d\r\n", duty_cycle, ret ? "fail" : "succeed", ret);

        ret = ioctl(fd, IOC_PWM_ON, (unsigned long)0);
        printf("set pwm on %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);

        aos_msleep(period_s * 1000);
        ret = ioctl(fd, IOC_PWM_OFF, (unsigned long)0);
        printf("set pwm off %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);
        close(fd);
        ret = 0;
    } else
        ret = -EIO;

    return ret;
}

static void pwm_output_test(int32_t argc, char **argv)
{
    int32_t ret;
    int channel;
    int freq;
    unsigned int period_s;

    channel = argc > 1 ? atoi(argv[1]) : 0;
    freq = argc > 2 ? atoi(argv[2]) : 100;
    period_s = argc > 3 ? atoi(argv[3]) : 3;
    
    printf("pwm comp output test start!");

    ret = vfs_pwm_test(channel, freq, period_s);
    if (!ret) {
        printf("pwm comp output test success!");
    } else {
         printf("pwm comp output test failed, ret:%d\r\n", ret);
    }
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(pwm_output_test, pwm_output, pwm output operation example)
#endif