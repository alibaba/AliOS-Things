#include "driver/ledc.h"
#include <hal/soc/pwm.h>
#include "math.h"

#define DEFAULT_LEDC_SPEED_MODE LEDC_HIGH_SPEED_MODE
#define DEFAULT_LEDC_CHANNEL_DUTY_DEPTH LEDC_TIMER_10_BIT
#define LEDC_CHANNEL_MAX_DUTY (pow(2, DEFAULT_LEDC_CHANNEL_DUTY_DEPTH) - 1)

static int8_t slots[4] = {0, 0, 0, 0};

static int8_t get_available_slot()
{
    uint8_t a;
    for (a = 0; a < 4; a++)
    {
        if (slots[a] == 0)
        {
            return a;
        }
    }
    return -1;
}

static int8_t find_registered_slot_by_gpio_num(uint8_t gpio_num)
{
    uint8_t a;
    for (a = 0; a < 4; a++)
    {
        if (slots[a] == gpio_num)
        {
            return a;
        }
    }
    return -1;
}

/**
 * Initialises a PWM pin
 *
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_init(pwm_dev_t *pwm)
{

    pwm_config_t config = pwm->config;
    int8_t existing_slot_idx = find_registered_slot_by_gpio_num(pwm->port);
    int8_t slot_idx = existing_slot_idx > 0 ? existing_slot_idx : get_available_slot();
    if (slot_idx < 0)
    {
        return -1;
    }

    ledc_timer_config_t ledc_timer = {
        .bit_num = DEFAULT_LEDC_CHANNEL_DUTY_DEPTH, // resolution of PWM duty
        .freq_hz = config.freq,                     // frequency of PWM signal
        .speed_mode = DEFAULT_LEDC_SPEED_MODE,      // timer mode
        .timer_num = slot_idx                       // timer index
    };
    // Set configuration of timer0 for high speed channels
    int32_t ret = ledc_timer_config(&ledc_timer);
    if (ret != 0)
    {
        return ret;
    }

    /*
     * Prepare individual configuration
     * for each channel of LED Controller
     * by selecting:
     * - controller's channel number
     * - output duty cycle, set initially to 0
     * - GPIO number where LED is connected to
     * - speed mode, either high or low
     * - timer servicing selected channel
     *   Note: if different channels use one timer,
     *         then frequency and bit_num of these channels
     *         will be the same
     */
    ledc_channel_config_t ledc_channel =
        {
            .channel = slot_idx,                               // channel index
            .duty = LEDC_CHANNEL_MAX_DUTY * config.duty_cycle, // channel duty,
            .gpio_num = pwm->port,                             // gpio number,
            .speed_mode = DEFAULT_LEDC_SPEED_MODE,             // speed mode,
            .timer_sel = slot_idx                              // timer index
        };

    // Set LED Controller with previously prepared configuration
    ret = ledc_channel_config(&ledc_channel);

    slots[slot_idx] = ledc_channel.gpio_num;

    return ret;
}

/**
 * Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_start(pwm_dev_t *pwm)
{
    int8_t slot_idx = find_registered_slot_by_gpio_num(pwm->port);
    if (slot_idx < 0)
    {
        return -1;
    }
    return ledc_update_duty(DEFAULT_LEDC_SPEED_MODE, slot_idx);
}

/**
 * Stops output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_stop(pwm_dev_t *pwm)
{
    int8_t slot_idx = find_registered_slot_by_gpio_num(pwm->port);
    if (slot_idx < 0)
    {
        return -1;
    }
    return ledc_stop(DEFAULT_LEDC_SPEED_MODE, slot_idx, 0);
}

/**
 * De-initialises an PWM interface, Turns off an PWM hardware interface
 *
 * @param[in]  pwm  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_finalize(pwm_dev_t *pwm)
{
    int8_t slot_idx = find_registered_slot_by_gpio_num(pwm->port);
    if (slot_idx < 0)
    {
        return -1;
    }
    slots[slot_idx] = 0;
    return ledc_timer_rst(DEFAULT_LEDC_SPEED_MODE, slot_idx);
}
