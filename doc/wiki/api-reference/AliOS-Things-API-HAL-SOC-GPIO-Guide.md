# API INDEX

  * [1 hal_gpio_init](#1-hal_gpio_init)
  * [2 hal_gpio_output_high](#2-hal_gpio_output_high)
  * [3 hal_gpio_output_low](#3-hal_gpio_output_low)
  * [4 hal_gpio_output_toggle](#4-hal_gpio_output_toggle)
  * [5 hal_gpio_input_get](#5-hal_gpio_input_get)
  * [6 hal_gpio_enable_irq](#6-hal_gpio_enable_irq)
  * [7 hal_gpio_disable_irq](#7-hal_gpio_disable_irq)
  * [8 hal_gpio_clear_irq](#8-hal_gpio_clear_irq)
  * [9 hal_gpio_finalize](#9-hal_gpio_finalize)

------

## 1-hal_gpio_init

```c
int32_t hal_gpio_init(gpio_dev_t *gpio)
```

- [x] **Description**

  Initialises a GPIO pin

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | gpio | the gpio device which should be initialised |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 2-hal_gpio_output_high

```c
int32_t hal_gpio_output_high(gpio_dev_t *gpio)
```

- [x] **Description**

  Sets an output GPIO pin high

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | gpio | the gpio pin which should be set high |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 3-hal_gpio_output_low

```c
int32_t hal_gpio_output_low(gpio_dev_t *gpio)
```

- [x] **Description**

  Sets an output GPIO pin low

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | gpio | the gpio pin which should be set low |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 4-hal_gpio_output_toggle

```c
int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
```

- [x] **Description**

  Trigger an output GPIO pin's output. Using this function on a
  gpio pin which is set to input mode is undefined.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | gpio | the gpio pin which should be set low |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 5-hal_gpio_input_get

```c
int8_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
```

- [x] **Description**

  Get the state of an input GPIO pin. Using this function on a
  gpio pin which is set to output mode will return an undefined value.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | gpio |  the gpio pin which should be read   |
  | [in] | value | gpio value |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 6-hal_gpio_enable_irq

```c
int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
```

- [x] **Description**

  Enables an interrupt trigger for an input GPIO pin.
  Using this function on a gpio pin which is set to
  output mode is undefined.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | gpio |    the gpio pin which will provide the interrupt trigger   |
  | [in] | trigger | the type of trigger (rising/falling edge)   |
  | [in] | handler | a function pointer to the interrupt handler   |
  | [in] | arg |     an argument that will be passed to the interrupt handle |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 7-hal_gpio_disable_irq

```c
int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
```

- [x] **Description**

  Disables an interrupt trigger for an input GPIO pin.
  Using this function on a gpio pin which has not been set up
  using @ref hal_gpio_input_irq_enable is undefined.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | gpio | the gpio pin which provided the interrupt trigge |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 8-hal_gpio_clear_irq

```c
int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
```

- [x] **Description**

  Disables an interrupt trigger for an input GPIO pin.
  Using this function on a gpio pin which has not been set up
  using @ref hal_gpio_input_irq_enable is undefined.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | gpio | the gpio pin which provided the interrupt trigge |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 9-hal_gpio_finalize

```c
int32_t hal_gpio_finalize(gpio_dev_t *gpio)
```

- [x] **Description**

  Set a GPIO pin in default state.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | gpio | the gpio pin which should be deinitialised |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step
