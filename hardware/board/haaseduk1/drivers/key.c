#include "key.h"
#include "aos/hal/gpio.h"
#include "hal_iomux_haas1000.h"
#include "ulog/ulog.h"
#include <aos/kernel.h>
#include <stdio.h>
#include <vfsdev/gpio_dev.h>

#define TAG "key_poll"

#define EDK_BOARD_KEY1 HAL_IOMUX_PIN_P2_7
#define EDK_BOARD_KEY2 HAL_IOMUX_PIN_P2_4
#define EDK_BOARD_KEY3 HAL_IOMUX_PIN_P2_5
#define EDK_BOARD_KEY4 HAL_IOMUX_PIN_P3_2

#define KEY_NUM              4
#define TIMER_CHECK_INTERVAL 30
#define DI_STABLE_COUNT      4

#define USING_VFS_GPIO

static key_code_cb notify_key_code_cb = NULL;

static aos_timer_t key_poll_timer = {0};
static int32_t g_gpio_fd = -1;

typedef struct {
    uint8_t key_code;
    uint8_t check_count;
} key_value_t;

static key_value_t key_value;

typedef struct {
    uint8_t    installed;
    uint8_t    monitor_flag;
    uint8_t    check_count;
    gpio_dev_t gpio_dev;
} key_dev_input_t;

/*digital input gpio dev list , the default value is high*/
static key_dev_input_t key_dev_input[KEY_NUM] = {
    {0, 0, 0, {EDK_BOARD_KEY1, IRQ_MODE, NULL} },
    {0, 0, 0, {EDK_BOARD_KEY2, IRQ_MODE, NULL} },
    {0, 0, 0, {EDK_BOARD_KEY3, IRQ_MODE, NULL} },
    {0, 0, 0, {EDK_BOARD_KEY4, IRQ_MODE, NULL} },
};

static void key_rising_edge_handle(gpio_dev_t *gpio);
static void key_falling_edge_handle(gpio_dev_t *gpio);

int find_key_index(gpio_dev_t *gpio)
{
    int         index    = -1;
    gpio_dev_t *key_gpio = gpio;
    if (key_gpio == NULL) {
        return index;
    }

    switch (key_gpio->port) {
        case EDK_BOARD_KEY1:
            index = 0;
            break;
        case EDK_BOARD_KEY2:
            index = 1;
            break;
        case EDK_BOARD_KEY3:
            index = 2;
            break;
        case EDK_BOARD_KEY4:
            index = 3;
            break;
    }
    //printf("%s, index : %d\n", __func__, index);
    return index;
}

static void key_poll(void *timer, void *arg)
{
    uint32_t i;
    int32_t  ret        = 0;
    uint32_t gpio_value = 0;
    uint8_t  key_code   = 0;

    for (i = 0; i < KEY_NUM; i++) {
        if (key_dev_input[i].installed == 0 ||
            key_dev_input[i].monitor_flag == 0) {
            continue;
        }
#ifdef USING_VFS_GPIO
        int ret = 0;
        gpio_io_config_t config;
        config.id = key_dev_input[i].gpio_dev.port;
        config.config = GPIO_IO_INPUT | GPIO_IO_INPUT_PU;
        config.data = 0;

        ret = ioctl(g_gpio_fd, IOC_GPIO_GET, &config);
        if (ret) {
            LOGE(TAG, "get gpio:%d failed, ret %d", config.id, ret);
            return;
        }
#else
        ret = hal_gpio_input_get(&key_dev_input[i].gpio_dev, &gpio_value);
        if (ret) {
            LOGE(TAG, "Fail to get di %d port %d value at %s %d", i,
                 key_dev_input[i].gpio_dev.port, __FILE__, __LINE__);
            continue;
        }
#endif
        if (gpio_value == GPIO_PinState_Reset) {
            //printf(" %d key is pressing\n", i);
            key_code = key_code | (0x01 << i);
        }
    }

    if ((key_value.key_code == key_code) && (key_value.key_code != 0)) {
        key_value.check_count++;
    } else {
        key_value.key_code    = key_code;
        key_value.check_count = 0;
    }

    if (key_value.check_count >= DI_STABLE_COUNT) {
        key_value.check_count = 0;
        LOGI(TAG, "notify %d in key_poll\n", key_value.key_code);
        if (key_value.key_code) {
            notify_key_code_cb(key_value.key_code);
        }
    }
}

static void key_rising_edge_handle(gpio_dev_t *gpio)
{
    int         index    = -1;
    gpio_dev_t *key_gpio = gpio;

    index = find_key_index(key_gpio);
    if (index < 0 || index >= KEY_NUM) {
        printf("err in %s:%d\n", __func__, __LINE__);
        return;
    }

    //printf(" %d key release\n", index);
    key_dev_input[index].check_count  = 0;
    key_dev_input[index].monitor_flag = 0;


#ifdef USING_VFS_GPIO
    int ret = 0;
    gpio_irq_config_t irq_config;
    irq_config.id = key_gpio->port;
    irq_config.config = GPIO_IRQ_CLEAR;

    ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &irq_config);
    if (ret) {
        LOGE(TAG, "clear gpio irq:%d failed, ret %d", key_gpio->port, ret);
        return;
    }
    irq_config.config = GPIO_IRQ_DISABLE;
    ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &irq_config);
    if (ret) {
        LOGE(TAG, "clear gpio irq:%d failed, ret %d", key_gpio->port, ret);
        return;
    }

    irq_config.id = key_gpio->port;
    irq_config.config = GPIO_IRQ_ENABLE | GPIO_IRQ_EDGE_FALLING;
    irq_config.cb = key_falling_edge_handle;
    irq_config.arg = key_gpio;

    ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &irq_config);
    if (ret) {
        LOGE(TAG, "enable gpio irq:%d failed, ret %d", ret);
        return -1;
    }
#else

    hal_gpio_clear_irq(key_gpio);
    hal_gpio_disable_irq(key_gpio);

    hal_gpio_enable_irq(key_gpio, IRQ_TRIGGER_FALLING_EDGE,
                        key_falling_edge_handle, key_gpio);
#endif
}

static void key_falling_edge_handle(gpio_dev_t *gpio)
{
    int         index    = -1;
    gpio_dev_t *key_gpio = gpio;

    index = find_key_index(key_gpio);
    if (index < 0 || index >= KEY_NUM) {
        printf("err in %s:%d\n", __func__, __LINE__);
        return;
    }

    //printf(" %d key press\n", index);
#ifdef USING_VFS_GPIO
    int ret = -1;
    gpio_irq_config_t irq_config;
    irq_config.id = key_gpio->port;
    irq_config.config = GPIO_IRQ_CLEAR;

    ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &irq_config);
    if (ret) {
        LOGE(TAG, "clear gpio irq:%d failed, ret %d", key_gpio->port, ret);
        return;
    }
    irq_config.config = GPIO_IRQ_DISABLE;
    ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &irq_config);
    if (ret) {
        LOGE(TAG, "clear gpio irq:%d failed, ret %d", key_gpio->port, ret);
        return;
    }
    key_dev_input[index].check_count  = 0;
    key_dev_input[index].monitor_flag = 1;

    irq_config.id = key_gpio->port;
    irq_config.config = GPIO_IRQ_ENABLE | GPIO_IRQ_EDGE_RISING;
    irq_config.cb = key_rising_edge_handle;
    irq_config.arg = key_gpio;

    ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &irq_config);
    if (ret) {
        LOGE(TAG, "enable gpio irq:%d failed, ret %d", ret);
        return -1;
    }
#else
    hal_gpio_clear_irq(key_gpio);
    hal_gpio_disable_irq(key_gpio);

    key_dev_input[index].check_count  = 0;
    key_dev_input[index].monitor_flag = 1;

    hal_gpio_enable_irq(key_gpio, IRQ_TRIGGER_RISING_EDGE,
                        key_rising_edge_handle, key_gpio);
#endif
}

int key_init(key_code_cb key_func)
{
    int32_t  ret = 0;
    uint32_t i   = 0;
    printf("enter %s:%d\n", __func__, __LINE__);

    if (key_func) {
        notify_key_code_cb = key_func;
    } else {
        return -1;
    }
#ifdef USING_VFS_GPIO
    gpio_irq_config_t irq_config;
    g_gpio_fd = open("/dev/gpio", 0);
    if (g_gpio_fd < 0) {
        printf("open /dev/gpio failed\n");
        return -1;
    }
#endif

    for (i = 0; i < KEY_NUM; i++) {
#ifdef USING_VFS_GPIO
        irq_config.id = key_dev_input[i].gpio_dev.port;
        irq_config.config = GPIO_IRQ_ENABLE | GPIO_IRQ_EDGE_FALLING;
        irq_config.cb = key_falling_edge_handle;
        irq_config.arg = &key_dev_input[i].gpio_dev;

        ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &irq_config);
        if (ret) {
            LOGE(TAG, "di %d pin %d fail enable irq ret %d", i,
                 key_dev_input[i].gpio_dev.port, ret);
            return -1;
        }
#else
        ret = hal_gpio_init(&key_dev_input[i].gpio_dev);
        if (ret) {
            LOGE(TAG, "di %d pin %d init fail ret", i,
                 key_dev_input[i].gpio_dev.port, ret);
            return -1;
        }

        ret = hal_gpio_enable_irq(
            &key_dev_input[i].gpio_dev, IRQ_TRIGGER_FALLING_EDGE,
            key_falling_edge_handle, &key_dev_input[i].gpio_dev);
        if (ret) {
            LOGE(TAG, "di %d pin %d fail enable irq ret %d", i,
                 key_dev_input[i].gpio_dev.port, ret);
            return -1;
        }
#endif
        key_dev_input[i].installed = 1;
    }

    /*init the gpio check timer, check gpio value every 30ms */
    ret = aos_timer_new_ext(&key_poll_timer, key_poll, NULL,
                            TIMER_CHECK_INTERVAL, 1, 1);
    if (ret) {
        LOGE(TAG, "Fail to new gpio value check timer ret 0x%x", ret);
        for (i = 0; i < KEY_NUM; i++) {
#ifdef USING_VFS_GPIO
        irq_config.id = key_dev_input[i].gpio_dev.port;
        irq_config.config = GPIO_IRQ_DISABLE;
        irq_config.cb = key_falling_edge_handle;
        irq_config.arg = &key_dev_input[i].gpio_dev;

        ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &irq_config);
        if (ret) {
            LOGE(TAG, "di %d pin %d fail enable irq ret %d", i,
                 key_dev_input[i].gpio_dev.port, ret);
            return -1;
        }
#else
            hal_gpio_disable_irq(&key_dev_input[i].gpio_dev);
            hal_gpio_finalize(&key_dev_input[i].gpio_dev);
#endif
            key_dev_input[i].installed = 0;
        }
#ifdef USING_VFS_GPIO
        if (g_gpio_fd >= 0) {
            close(g_gpio_fd);
            g_gpio_fd = -1;
        }
#endif
        return -1;
    }

    return 0;
}
