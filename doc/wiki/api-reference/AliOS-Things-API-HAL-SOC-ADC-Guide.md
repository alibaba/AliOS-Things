# API INDEX

  * [1 hal_adc_init](#1-hal_adc_init)
  * [2 hal_adc_value_get](#2-hal_adc_value_get)
  * [3 hal_adc_finalize](#3-hal_adc_finalize)

------

## 1-hal_adc_init

```c
int32_t hal_adc_init(adc_dev_t *adc)
```

- [x] **Description**

  Initialises an ADC interface, Prepares an ADC hardware interface for sampling

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | adc | the interface which should be initialised |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 2-hal_adc_value_get

```c
int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
```

- [x] **Description**

  Takes a single sample from an ADC interface

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  adc |     the interface which should be sampled   |
  | [out] | output |  pointer to a variable which will receive the sample   |
  | [in] |  timeout | ms timeout |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step

## 3-hal_adc_finalize

```c
int32_t hal_adc_finalize(adc_dev_t *adc)
```

- [x] **Description**

  De-initialises an ADC interface, Turns off an ADC hardware interface

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | adc | the interface which should be de-initialised |

- [x] **Returns**

  0 : on success, EIO : if an error occurred with any step
