#include <stdint.h>
#include <mt_printf.h>
#include <mt_reg_base.h>

#define CFG_CLK_PM_SUPPORT 1

#ifdef CFG_CLK_PM_SUPPORT
#include "clk.h"
#include "mt8512_clk.h"
#endif
#include "mt_pwm.h"
#include <mt_gpio.h>
#include "aos/cli.h"

#define BIT(nr)             (1 << (nr))
static uint32_t readl(void *addr)
{
   return *((volatile uint32_t*)addr);
}
static void writel(void *addr, uint32_t val)
{
    *((volatile uint32_t*)addr) = val;
}

#define PWM_EN_REG              0x0000
#define PWMCON                  0x00
#define PWMGDUR                 0x0c
#define PWMWAVENUM              0x28
#define PWMDWIDTH               0x2c
#define PWMTHRES                0x30
#define PWM_SEND_WAVENUM        0x34

#define PWM_CLK_DIV_MAX         7
#define PWM_NUM_MAX             7

#ifdef CFG_CLK_PM_SUPPORT
struct clk *clk_top;
struct clk *clk_main;
struct clk *clk_pwms[PWM_NUM_MAX];
#endif

#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")
#define isb() __asm__ __volatile__ ("isb" : : : "memory")

#define mb()    \
    do {    \
        dsb();    \
    } while (0)

/*==========================================*/
static const unsigned long pwm_com_register[] = {
    0x0010, 0x0050, 0x0090, 0x00d0, 0x0110, 0x0150, 0x0190
};
/*==========================================*/

static int pwm_duties[PWM_NUM_MAX];
static int pwm_periods[PWM_NUM_MAX];

static void pwm_clk_enable(int pwm_no)
{
#ifdef CFG_CLK_PM_SUPPORT
    clk_enable(clk_top);
    clk_enable(clk_main);
    clk_enable(clk_pwms[pwm_no]);
#endif
}

static void pwm_clk_disable(int pwm_no)
{
#ifdef CFG_CLK_PM_SUPPORT
    clk_disable(clk_pwms[pwm_no]);
    clk_disable(clk_main);
    clk_disable(clk_top);
#endif
}

static inline uint32_t pwm_readl(int pwm_no, unsigned long offset)
{
    void  *reg = (void *)(PWM_BASE + pwm_com_register[pwm_no] + offset);

    return readl(reg);
}

static inline void pwm_writel(int pwm_no, unsigned long offset, unsigned int val)
{
    void  *reg = (void *)(PWM_BASE + pwm_com_register[pwm_no] + offset);

    writel(reg, val);
}

int pwm_config(int pwm_no, uint32_t clkdiv, uint32_t data_width, uint32_t thresh)
{
    uint32_t value;

    pwm_periods[pwm_no] = data_width;
    pwm_duties[pwm_no] = thresh;

    value = pwm_readl(pwm_no, PWMCON);
    value = value | BIT(15) | clkdiv;
    pwm_writel(pwm_no, PWMCON, value);
    mb();
    pwm_writel(pwm_no, PWMDWIDTH, data_width);
    pwm_writel(pwm_no, PWMTHRES, thresh);
    pwm_enable(pwm_no);
    return 0;
}

int pwm_enable(int pwm_no)
{
    uint32_t val;

    if(pwm_no < 0 || pwm_no >= PWM_NUM_MAX) {
        PRINTF_E("pwm_no: %d is too big!\n", pwm_no);
        return -1;
    }

    pwm_clk_enable(pwm_no);

    val = readl(PWM_BASE + PWM_EN_REG);
    val |= BIT(pwm_no);
    writel(PWM_BASE + PWM_EN_REG, val);

    return 0;
}

int pwm_disable(int pwm_no)
{
    uint32_t val;

    if(pwm_no < 0 || pwm_no >= PWM_NUM_MAX) {
        PRINTF_E("pwm_no: %d is too big!\n", pwm_no);
        return -1;
    }

    val = readl(PWM_BASE + PWM_EN_REG);
    val &= ~ BIT(pwm_no);
    writel(PWM_BASE + PWM_EN_REG, val);
    pwm_clk_disable(pwm_no);
    return 0;
}

static void pwm_dump(int pwm_no)
{
    uint32_t value;

    if(pwm_no < 0 || pwm_no >= PWM_NUM_MAX) {
        PRINTF_E("pwm_no: %d is too big!\n", pwm_no);
        return;
    }

    value = pwm_readl(pwm_no, PWM_SEND_WAVENUM);

    PRINTF_I("pwm%d: send wavenum:%u, duty/period:%d%%\n", pwm_no, value,
        (pwm_readl(pwm_no, PWMTHRES) + 1) * 100 / (pwm_readl(pwm_no, PWMDWIDTH) + 1));
    PRINTF_I("\tDATA_WIDTH:%u, THRESH:%u, CON:0x%x, EN:0x%x\n",
        pwm_readl(pwm_no, PWMDWIDTH),
        pwm_readl(pwm_no, PWMTHRES),
        pwm_readl(pwm_no, PWMCON),
        readl(PWM_BASE + PWM_EN_REG));
}

void pwm_dump_all(void)
{
    int i;

    for(i = 0; i < PWM_NUM_MAX; ++i)
        pwm_dump(i);

}
void pwm_test(char* buf, int len, int argc, char **argv)
{
	if (argc == 1)
	{
		pwm_dump_all();
	}
	else if (argc == 3)
	{
		int pwm_no = atoi(argv[1]);
		int type = atoi(argv[2]);
		if (type == 1) {
			pwm_disable(pwm_no);
		} else {
			pwm_enable(pwm_no);
		}
	}
	else
	{
		int pwm_no = atoi(argv[1]);
		int clkdiv = atoi(argv[2]);
		int data_width = atoi(argv[3]);
		int thresh = atoi(argv[4]);
		pwm_config(pwm_no, clkdiv, data_width, thresh);

	}
}

static struct cli_command cli_cmd_pwm_selftest[] =
{
    {"pwm","get/set pwm",pwm_test},
};
void pwm_init (void)
{
    PRINTF_I("[pwm_init] Init Start..................\n");
#ifdef CFG_CLK_PM_SUPPORT
    clk_top = clk_get(CLK_INFRA_PWM_HCLK);
    if (is_err_or_null_clk(clk_top)) {
        PRINTF_E("failed to get clock CLK_INFRA_PWM_HCLK!\n");
    }
    clk_main = clk_get(CLK_INFRA_PWM);
    if (is_err_or_null_clk(clk_main)) {
        PRINTF_E("failed to get clock CLK_INFRA_PWM!\n");
    }
    clk_pwms[0] = clk_get(CLK_INFRA_PWM1);
    if (is_err_or_null_clk(clk_pwms[0])) {
        PRINTF_E("failed to get clock CLK_INFRA_PWM1!\n");
    }
    clk_pwms[1] = clk_get(CLK_INFRA_PWM2);
    if (is_err_or_null_clk(clk_pwms[1])) {
        PRINTF_E("failed to get clock CLK_INFRA_PWM2!\n");
    }
    clk_pwms[2] = clk_get(CLK_INFRA_PWM3);
    if (is_err_or_null_clk(clk_pwms[2])) {
        PRINTF_E("failed to get clock CLK_INFRA_PWM3!\n");
    }
    clk_pwms[3] = clk_get(CLK_INFRA_PWM4);
    if (is_err_or_null_clk(clk_pwms[3])) {
        PRINTF_E("failed to get clock CLK_INFRA_PWM4!\n");
    }
    clk_pwms[4] = clk_get(CLK_INFRA_PWM5);
    if (is_err_or_null_clk(clk_pwms[4])) {
        PRINTF_E("failed to get clock CLK_INFRA_PWM5!\n");
    }
    clk_pwms[5] = clk_get(CLK_INFRA_PWM_FB6);
    if (is_err_or_null_clk(clk_pwms[5])) {
        PRINTF_E("failed to get clock CLK_INFRA_PWM_FB6!\n");
    }
    clk_pwms[6] = clk_get(CLK_INFRA_PWM_FB7);
    if (is_err_or_null_clk(clk_pwms[6])) {
        PRINTF_E("failed to get clock CLK_INFRA_PWM_FB7!\n");
    }
#endif

	gpio_set_mode(GPIO44, GPIO_MODE_01);
	gpio_set_mode(GPIO45, GPIO_MODE_01);
	gpio_set_mode(GPIO46, GPIO_MODE_01);

}

void pwm_cli_init(void)
{
    aos_cli_register_commands(&cli_cmd_pwm_selftest[0],
		sizeof(cli_cmd_pwm_selftest)/sizeof(struct cli_command));
}
