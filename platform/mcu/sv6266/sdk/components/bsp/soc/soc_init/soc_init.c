#include <nds32_intrinsic.h>
#include "soc_types.h"
#include "soc_defs.h"

#include "gpio/hal_gpio.h"
#include "pinmux/hal_pinmux.h"
#include "custom_io_hal.h"
#include "custom_io_chk.h"

void _soc_io_init(void)
{
    hal_pinmux_set_raw(_M_PIN_MODE, _M_FUNC_SEL);
}

