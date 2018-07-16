# API INDEX

  * [1 hal_pwm_init](#1-hal_pwm_init)
  * [2 hal_pwm_start](#2-hal_pwm_start)
  * [3 hal_pwm_stop](#3-hal_pwm_stop)
  * [4 hal_pwm_finalize](#4-hal_pwm_finalize)

------

## 1-hal_pwm_init

```c
int32_t hal_pwm_init(pwm_dev_t *pwm)
```

- [x] **Description**

  Initialises a PWM pin
 

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | pwm | the PWM device |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 2-hal_pwm_start

```c
int32_t hal_pwm_start(pwm_dev_t *pwm)
```

- [x] **Description**

  Starts Pulse-Width Modulation signal output on a PWM pin

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | pwm | the PWM device |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 3-hal_pwm_stop

```c
int32_t hal_pwm_stop(pwm_dev_t *pwm)
```

- [x] **Description**

  Stops output on a PWM pin

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | pwm | the PWM device |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 4-hal_pwm_finalize

```c
int32_t hal_pwm_finalize(pwm_dev_t *pwm)
```

- [x] **Description**

  finalize output on a PWM pin

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | pwm | the PWM device |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step
