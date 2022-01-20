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


_IRQ_SCAN_RESULT = const(5)
_IRQ_SCAN_DONE = const(6)

_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)

_ADV_IND = const(0)
_ADV_DIRECT_IND = const(1)
_ADV_SCAN_IND = const(2)
_ADV_NONCONN_IND = const(3)
_SCAN_RSP = const(4)

_ADV_TYPE_FLAGS = const(0x01)
_ADV_TYPE_NAME = const(0x09)
_ADV_TYPE_UUID16_INCOMPLETE = const(0x2)
_ADV_TYPE_UUID16_COMPLETE = const(0x3)
_ADV_TYPE_UUID32_INCOMPLETE = const(0x4)
_ADV_TYPE_UUID32_COMPLETE = const(0x5)
_ADV_TYPE_UUID128_INCOMPLETE = const(0x6)
_ADV_TYPE_UUID128_COMPLETE = const(0x7)
_ADV_TYPE_APPEARANCE = const(0x19)
_ADV_TYPE_MANUFACTURER = const(0xFF)


# Keep track of the active scanner so IRQs can be delivered to it.
_active_scanner = None


# Set of devices that are waiting for the peripheral connect IRQ.
_connecting = set()


def _central_irq(event, data):
    # Send results and done events to the active scanner instance.
    if event == _IRQ_SCAN_RESULT:
        addr_type, addr, adv_type, rssi, adv_data = data
        if not _active_scanner:
            return
        _active_scanner._queue.append((addr_type, bytes(addr), adv_type, rssi, bytes(adv_data)))
        _active_scanner._event.set()
    elif event == _IRQ_SCAN_DONE:
        if not _active_scanner:
            return
        _active_scanner._done = True
        _active_scanner._event.set()

    # Peripheral connect must be in response to a pending connection, so find
    # it in the pending connection set.
    elif event == _IRQ_PERIPHERAL_CONNECT:
        conn_handle, addr_type, addr = data

        for d in _connecting:
            if d.addr_type == addr_type and d.addr == addr:
                # Allow connect() to complete.
                connection = d._connection
                connection._conn_handle = conn_handle
                connection._event.set()
                break

    # Find the active device connection for this connection handle.
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        conn_handle, _, _ = data
        if connection := DeviceConnection._connected.get(conn_handle, None):
            # Tell the device_task that it should terminate.
            connection._event.set()


def _central_shutdown():
    global _active_scanner, _connecting
    _active_scanner = None
    _connecting = set()


register_irq_handler(_central_irq, _central_shutdown)


# Cancel an in-progress scan.
async def _cancel_pending():
    if _active_scanner:
        await _active_scanner.cancel()


# Start connecting to a peripheral.
# Call device.connect() rather than using method directly.
async def _connect(connection, timeout_ms):
    device = connection.device
    if device in _connecting:
        return

    # Enable BLE and cancel in-progress scans.
    ensure_active()
    await _cancel_pending()

    # Allow the connected IRQ to find the device by address.
    _connecting.add(device)

    # Event will be set in the connected IRQ, and then later
    # re-used to notify disconnection.
    connection._event = connection._event or asyncio.ThreadSafeFlag()

    try:
        with DeviceTimeout(None, timeout_ms):
            ble.gap_connect(device.addr_type, device.addr)

            # Wait for the connected IRQ.
            await connection._event.wait()
            assert connection._conn_handle is not None

            # Register connection handle -> device.
            DeviceConnection._connected[connection._conn_handle] = connection
    finally:
        # After timeout, don't hold a reference and ignore future events.
        _connecting.remove(device)


# Represents a single device that has been found during a scan. The scan
# iterator will return the same ScanResult instance multiple times as its data
# changes (i.e. changing RSSI or advertising data).
class ScanResult:
    def __init__(self, device):
        self.device = device
        self.adv_data = None
        self.resp_data = None
        self.rssi = None
        self.connectable = False

    # New scan result available, return true if it changes our state.
    def _update(self, adv_type, rssi, adv_data):
        updated = False

        if rssi != self.rssi:
            self.rssi = rssi
            updated = True

        if adv_type in (_ADV_IND, _ADV_NONCONN_IND):
            if adv_data != self.adv_data:
                self.adv_data = adv_data
                self.connectable = adv_type == _ADV_IND
                updated = True
        elif adv_type == _ADV_SCAN_IND:
            if adv_data != self.adv_data and self.resp_data:
                updated = True
            self.adv_data = adv_data
        elif adv_type == _SCAN_RSP and adv_data:
            if adv_data != self.resp_data:
                self.resp_data = adv_data
                updated = True

        return updated

    def __str__(self):
        return "Scan result: {} {}".format(self.device, self.rssi)

    # Gets all the fields for the specified types.
    def _decode_field(self, *adv_type):
        # Advertising payloads are repeated packets of the following form:
        #   1 byte data length (N + 1)
        #   1 byte type (see constants below)
        #   N bytes type-specific data
        for payload in (self.adv_data, self.resp_data):
            if not payload:
                continue
            i = 0
            while i + 1 < len(payload):
                if payload[i + 1] in adv_type:
                    yield payload[i + 2 : i + payload[i] + 1]
                i += 1 + payload[i]

    # Returns the value of the advertised name, otherwise empty string.
    def name(self):
        for n in self._decode_field(_ADV_TYPE_NAME):
            return str(n, "utf-8") if n else ""

    # Generator that enumerates the service UUIDs that are advertised.
    def services(self):
        for u in self._decode_field(_ADV_TYPE_UUID16_INCOMPLETE, _ADV_TYPE_UUID16_COMPLETE):
            yield bluetooth.UUID(struct.unpack("<H", u)[0])
        for u in self._decode_field(_ADV_TYPE_UUID32_INCOMPLETE, _ADV_TYPE_UUID32_COMPLETE):
            yield bluetooth.UUID(struct.unpack("<I", u)[0])
        for u in self._decode_field(_ADV_TYPE_UUID128_INCOMPLETE, _ADV_TYPE_UUID128_COMPLETE):
            yield bluetooth.UUID(u)

    # Generator that returns (manufacturer_id, data) tuples.
    def manufacturer(self, filter=None):
        for u in self._decode_field(_ADV_TYPE_MANUFACTURER):
            if len(u) < 2:
                continue
            m = struct.unpack("<H", u[0:2])[0]
            if filter is None or m == filter:
                yield (m, u[2:])


# Use with:
# async with aioble.scan(...) as scanner:
#   async for result in scanner:
#     ...
class scan:
    def __init__(self, duration_ms, interval_us=None, window_us=None, active=False):
        self._queue = []
        self._event = asyncio.ThreadSafeFlag()
        self._done = False

        # Keep track of what we've already seen.
        self._results = set()

        # Ideally we'd start the scan here and avoid having to save these
        # values, but we need to stop any previous scan first via awaiting
        # _cancel_pending(), but __init__ isn't async.
        self._duration_ms = duration_ms
        self._interval_us = interval_us or 1280000
        self._window_us = window_us or 11250
        self._active = active

    async def __aenter__(self):
        global _active_scanner
        ensure_active()
        await _cancel_pending()
        _active_scanner = self
        ble.gap_scan(self._duration_ms, self._interval_us, self._window_us, self._active)
        return self

    async def __aexit__(self, exc_type, exc_val, exc_traceback):
        # Cancel the current scan if we're still the active scanner. This will
        # happen if the loop breaks early before the scan duration completes.
        if _active_scanner == self:
            await self.cancel()

    def __aiter__(self):
        assert _active_scanner == self
        return self

    async def __anext__(self):
        global _active_scanner

        if _active_scanner != self:
            # The scan has been canceled (e.g. a connection was initiated).
            raise StopAsyncIteration

        while True:
            while self._queue:
                addr_type, addr, adv_type, rssi, adv_data = self._queue.pop()

                # Try to find an existing ScanResult for this device.
                for r in self._results:
                    if r.device.addr_type == addr_type and r.device.addr == addr:
                        result = r
                        break
                else:
                    # New device, create a new Device & ScanResult.
                    device = Device(addr_type, addr)
                    result = ScanResult(device)
                    self._results.add(result)

                # Add the new information from this event.
                if result._update(adv_type, rssi, adv_data):
                    # It's new information, so re-yield this result.
                    return result

            if self._done:
                # _IRQ_SCAN_DONE event was fired.
                _active_scanner = None
                raise StopAsyncIteration

            # Wait for either done or result IRQ.
            await self._event.wait()

    # Cancel any in-progress scan. We need to do this before starting any other operation.
    async def cancel(self):
        if self._done:
            return
        ble.gap_scan(None)
        while not self._done:
            await self._event.wait()
        global _active_scanner
        _active_scanner = None
