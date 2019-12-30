
extern void mp_hal_set_interrupt_char(int c);


// needed for machine.I2C
#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)
#define mp_hal_pin_od_low(pin)
#define mp_hal_pin_od_high(pin)
#define mp_hal_pin_open_drain(p)


// needed for soft machine.SPI
#define mp_hal_pin_output(p)
#define mp_hal_pin_input(p)
#define mp_hal_pin_name(p)
#define mp_hal_pin_high(p)

