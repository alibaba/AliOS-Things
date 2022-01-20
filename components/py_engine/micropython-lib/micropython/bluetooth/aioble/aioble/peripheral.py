# MicroPython aioble module
# MIT license; Copyright (c) 2021 Jim Mussared

from micropython import const

import bluetooth
import struct

import uasyncio as asyncio

from .core import (
    ensure_active,
    ble,
    log_info,
    log_error,
    log_warn,
    register_irq_handler,
)
from .device import Device, DeviceConnection, DeviceTimeout


_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)


_ADV_TYPE_FLAGS = const(0x01)
_ADV_TYPE_NAME = const(0x09)
_ADV_TYPE_UUID16_COMPLETE = const(0x3)
_ADV_TYPE_UUID32_COMPLETE = const(0x5)
_ADV_TYPE_UUID128_COMPLETE = const(0x7)
_ADV_TYPE_UUID16_MORE = const(0x2)
_ADV_TYPE_UUID32_MORE = const(0x4)
_ADV_TYPE_UUID128_MORE = const(0x6)
_ADV_TYPE_APPEARANCE = const(0x19)
_ADV_TYPE_MANUFACTURER = const(0xFF)

_ADV_PAYLOAD_MAX_LEN = const(31)


_incoming_connection = None
_connect_event = None


def _peripheral_irq(event, data):
    global _incoming_connection

    if event == _IRQ_CENTRAL_CONNECT:
        conn_handle, addr_type, addr = data

        # Create, initialise, and register the device.
        device = Device(addr_type, bytes(addr))
        _incoming_connection = DeviceConnection(device)
        _incoming_connection._conn_handle = conn_handle
        DeviceConnection._connected[conn_handle] = _incoming_connection

        # Signal advertise() to return the connected device.
        _connect_event.set()

    elif event == _IRQ_CENTRAL_DISCONNECT:
        conn_handle, _, _ = data
        if connection := DeviceConnection._connected.get(conn_handle, None):
            # Tell the device_task that it should terminate.
            connection._event.set()


def _peripheral_shutdown():
    global _incoming_connection, _connect_event
    _incoming_connection = None
    _connect_event = None


register_irq_handler(_peripheral_irq, _peripheral_shutdown)


# Advertising payloads are repeated packets of the following form:
#   1 byte data length (N + 1)
#   1 byte type (see constants below)
#   N bytes type-specific data
def _append(adv_data, resp_data, adv_type, value):
    data = struct.pack("BB", len(value) + 1, adv_type) + value

    if len(data) + len(adv_data) < _ADV_PAYLOAD_MAX_LEN:
        adv_data += data
        return resp_data

    if len(data) + (len(resp_data) if resp_data else 0) < _ADV_PAYLOAD_MAX_LEN:
        if not resp_data:
            # Overflow into resp_data for the first time.
            resp_data = bytearray()
        resp_data += data
        return resp_data

    raise ValueError("Advertising payload too long")


async def advertise(
    interval_us,
    adv_data=None,
    resp_data=None,
    connectable=True,
    limited_disc=False,
    br_edr=False,
    name=None,
    services=None,
    appearance=0,
    manufacturer=None,
    timeout_ms=None,
):
    global _incoming_connection, _connect_event

    ensure_active()

    if not adv_data and not resp_data:
        # If the user didn't manually specify adv_data / resp_data then
        # construct them from the kwargs. Keep adding fields to adv_data,
        # overflowing to resp_data if necessary.
        # TODO: Try and do better bin-packing than just concatenating in
        # order?

        adv_data = bytearray()

        resp_data = _append(
            adv_data,
            resp_data,
            _ADV_TYPE_FLAGS,
            struct.pack("B", (0x01 if limited_disc else 0x02) + (0x18 if br_edr else 0x04)),
        )

        if name:
            resp_data = _append(adv_data, resp_data, _ADV_TYPE_NAME, name)

        if services:
            for uuid in services:
                b = bytes(uuid)
                if len(b) == 2:
                    resp_data = _append(adv_data, resp_data, _ADV_TYPE_UUID16_COMPLETE, b)
                elif len(b) == 4:
                    resp_data = _append(adv_data, resp_data, _ADV_TYPE_UUID32_COMPLETE, b)
                elif len(b) == 16:
                    resp_data = _append(adv_data, resp_data, _ADV_TYPE_UUID128_COMPLETE, b)

        if appearance:
            # See org.bluetooth.characteristic.gap.appearance.xml
            resp_data = _append(
                adv_data, resp_data, _ADV_TYPE_APPEARANCE, struct.pack("<H", appearance)
            )

        if manufacturer:
            resp_data = _append(
                adv_data,
                resp_data,
                _ADV_TYPE_MANUFACTURER,
                struct.pack("<H", manufacturer[0]) + manufacturer[1],
            )

    _connect_event = _connect_event or asyncio.ThreadSafeFlag()
    ble.gap_advertise(interval_us, adv_data=adv_data, resp_data=resp_data, connectable=connectable)

    try:
        # Allow optional timeout for a central to connect to us (or just to stop advertising).
        with DeviceTimeout(None, timeout_ms):
            await _connect_event.wait()

        # Get the newly connected connection to the central and start a task
        # to wait for disconnection.
        result = _incoming_connection
        _incoming_connection = None
        # This mirrors what connecting to a central does.
        result._run_task()
        return result
    except asyncio.CancelledError:
        # Something else cancelled this task (to manually stop advertising).
        ble.gap_advertise(None)
    except asyncio.TimeoutError:
        # DeviceTimeout waiting for connection.
        ble.gap_advertise(None)
        raise
