# MicroPython aioble module
# MIT license; Copyright (c) 2021 Jim Mussared

from micropython import const

from .device import Device, DeviceDisconnectedError
from .core import log_info, log_warn, log_error, GattError, config, stop

try:
    from .peripheral import advertise
except:
    log_info("Peripheral support disabled")

try:
    from .central import scan
except:
    log_info("Central support disabled")

try:
    from .server import (
        Service,
        Characteristic,
        BufferedCharacteristic,
        Descriptor,
        register_services,
    )
except:
    log_info("GATT server support disabled")


ADDR_PUBLIC = const(0)
ADDR_RANDOM = const(1)
