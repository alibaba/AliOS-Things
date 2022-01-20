# MicroPython aioble module
# MIT license; Copyright (c) 2021 Jim Mussared

from micropython import const
from collections import deque
import uasyncio as asyncio
import struct

import bluetooth

from .core import ble, GattError, register_irq_handler
from .device import DeviceConnection


_IRQ_GATTC_SERVICE_RESULT = const(9)
_IRQ_GATTC_SERVICE_DONE = const(10)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_DESCRIPTOR_RESULT = const(13)
_IRQ_GATTC_DESCRIPTOR_DONE = const(14)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_GATTC_NOTIFY = const(18)
_IRQ_GATTC_INDICATE = const(19)

_CCCD_UUID = const(0x2902)
_CCCD_NOTIFY = const(1)
_CCCD_INDICATE = const(2)

_FLAG_READ = const(0x0002)
_FLAG_WRITE_NO_RESPONSE = const(0x0004)
_FLAG_WRITE = const(0x0008)
_FLAG_NOTIFY = const(0x0010)
_FLAG_INDICATE = const(0x0020)

# Forward IRQs directly to static methods on the type that handles them and
# knows how to map handles to instances. Note: We copy all uuid and data
# params here for safety, but a future optimisation might be able to avoid
# these copies in a few places.
def _client_irq(event, data):
    if event == _IRQ_GATTC_SERVICE_RESULT:
        conn_handle, start_handle, end_handle, uuid = data
        ClientDiscover._discover_result(
            conn_handle, start_handle, end_handle, bluetooth.UUID(uuid)
        )
    elif event == _IRQ_GATTC_SERVICE_DONE:
        conn_handle, status = data
        ClientDiscover._discover_done(conn_handle, status)
    elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
        conn_handle, def_handle, value_handle, properties, uuid = data
        ClientDiscover._discover_result(
            conn_handle, def_handle, value_handle, properties, bluetooth.UUID(uuid)
        )
    elif event == _IRQ_GATTC_CHARACTERISTIC_DONE:
        conn_handle, status = data
        ClientDiscover._discover_done(conn_handle, status)
    elif event == _IRQ_GATTC_DESCRIPTOR_RESULT:
        conn_handle, dsc_handle, uuid = data
        ClientDiscover._discover_result(conn_handle, dsc_handle, bluetooth.UUID(uuid))
    elif event == _IRQ_GATTC_DESCRIPTOR_DONE:
        conn_handle, status = data
        ClientDiscover._discover_done(conn_handle, status)
    elif event == _IRQ_GATTC_READ_RESULT:
        conn_handle, value_handle, char_data = data
        ClientCharacteristic._read_result(conn_handle, value_handle, bytes(char_data))
    elif event == _IRQ_GATTC_READ_DONE:
        conn_handle, value_handle, status = data
        ClientCharacteristic._read_done(conn_handle, value_handle, status)
    elif event == _IRQ_GATTC_WRITE_DONE:
        conn_handle, value_handle, status = data
        ClientCharacteristic._write_done(conn_handle, value_handle, status)
    elif event == _IRQ_GATTC_NOTIFY:
        conn_handle, value_handle, notify_data = data
        ClientCharacteristic._on_notify(conn_handle, value_handle, bytes(notify_data))
    elif event == _IRQ_GATTC_INDICATE:
        conn_handle, value_handle, indicate_data = data
        ClientCharacteristic._on_indicate(conn_handle, value_handle, bytes(indicate_data))


register_irq_handler(_client_irq, None)


# Async generator for discovering services, characteristics, descriptors.
class ClientDiscover:
    def __init__(self, connection, disc_type, parent, timeout_ms, *args):
        self._connection = connection

        # Each result IRQ will append to this.
        self._queue = []
        # This will be set by the done IRQ.
        self._status = None

        # Tell the generator to process new events.
        self._event = asyncio.ThreadSafeFlag()

        # Must implement the _start_discovery static method. Instances of this
        # type are returned by __anext__.
        self._disc_type = disc_type

        # This will be the connection for a service discovery, and the service for a characteristic discovery.
        self._parent = parent

        # Timeout for the discovery process.
        # TODO: Not implemented.
        self._timeout_ms = timeout_ms

        # Additional arguments to pass to the _start_discovery method on disc_type.
        self._args = args

    async def _start(self):
        if self._connection._discover:
            # TODO: cancel existing? (e.g. perhaps they didn't let the loop run to completion)
            raise ValueError("Discovery in progress")

        # Tell the connection that we're the active discovery operation (the IRQ only gives us conn_handle).
        self._connection._discover = self
        # Call the appropriate ubluetooth.BLE method.
        self._disc_type._start_discovery(self._parent, *self._args)

    def __aiter__(self):
        return self

    async def __anext__(self):
        if self._connection._discover != self:
            # Start the discovery if necessary.
            await self._start()

        # Keep returning items from the queue until the status is set by the
        # done IRQ.
        while True:
            while self._queue:
                return self._disc_type(self._parent, *self._queue.pop())
            if self._status is not None:
                self._connection._discover = None
                raise StopAsyncIteration
            # Wait for more results to be added to the queue.
            await self._event.wait()

    # Tell the active discovery instance for this connection to add a new result
    # to the queue.
    def _discover_result(conn_handle, *args):
        if connection := DeviceConnection._connected.get(conn_handle, None):
            if discover := connection._discover:
                discover._queue.append(args)
                discover._event.set()

    # Tell the active discovery instance for this connection that it is complete.
    def _discover_done(conn_handle, status):
        if connection := DeviceConnection._connected.get(conn_handle, None):
            if discover := connection._discover:
                discover._status = status
                discover._event.set()


# Represents a single service supported by a connection. Do not construct this
# class directly, instead use `async for service in connection.services([uuid])` or
# `await connection.service(uuid)`.
class ClientService:
    def __init__(self, connection, start_handle, end_handle, uuid):
        self.connection = connection

        # Used for characteristic discovery.
        self._start_handle = start_handle
        self._end_handle = end_handle

        # Allows comparison to a known uuid.
        self.uuid = uuid

    def __str__(self):
        return "Service: {} {} {}".format(self._start_handle, self._end_handle, self.uuid)

    # Search for a specific characteristic by uuid.
    async def characteristic(self, uuid, timeout_ms=2000):
        result = None
        # Make sure loop runs to completion.
        async for characteristic in self.characteristics(uuid, timeout_ms):
            if not result and characteristic.uuid == uuid:
                # Keep first result.
                result = characteristic
        return result

    # Search for all services (optionally by uuid).
    # Use with `async for`, e.g.
    #     async for characteristic in service.characteristics():
    # Note: must allow the loop to run to completion.
    def characteristics(self, uuid=None, timeout_ms=2000):
        return ClientDiscover(self.connection, ClientCharacteristic, self, timeout_ms, uuid)

    # For ClientDiscover
    def _start_discovery(connection, uuid=None):
        ble.gattc_discover_services(connection._conn_handle, uuid)


class BaseClientCharacteristic:
    # Register this value handle so events can find us.
    def _register_with_connection(self):
        self._connection()._characteristics[self._value_handle] = self

    # Map an incoming IRQ to an registered characteristic.
    def _find(conn_handle, value_handle):
        if connection := DeviceConnection._connected.get(conn_handle, None):
            if characteristic := connection._characteristics.get(value_handle, None):
                return characteristic
            else:
                # IRQ for a characteristic that we weren't expecting. e.g.
                # notification when we're not waiting on notified().
                # TODO: This will happen on btstack, which doesn't give us
                # value handle for the done event.
                return None

    def _check(self, flag):
        if not (self.properties & flag):
            raise ValueError("Unsupported")

    # Issue a read to the characteristic.
    async def read(self, timeout_ms=1000):
        self._check(_FLAG_READ)
        # Make sure this conn_handle/value_handle is known.
        self._register_with_connection()
        # This will be set by the done IRQ.
        self._read_status = None
        # This will be set by the result and done IRQs. Re-use if possible.
        self._read_event = self._read_event or asyncio.ThreadSafeFlag()

        # Issue the read.
        ble.gattc_read(self._connection()._conn_handle, self._value_handle)

        with self._connection().timeout(timeout_ms):
            # The event will be set for each read result, then a final time for done.
            while self._read_status is None:
                await self._read_event.wait()
            if self._read_status != 0:
                raise GattError(self._read_status)
            return self._read_data

    # Map an incoming result IRQ to a registered characteristic.
    def _read_result(conn_handle, value_handle, data):
        if characteristic := ClientCharacteristic._find(conn_handle, value_handle):
            characteristic._read_data = data
            characteristic._read_event.set()

    # Map an incoming read done IRQ to a registered characteristic.
    def _read_done(conn_handle, value_handle, status):
        if characteristic := ClientCharacteristic._find(conn_handle, value_handle):
            characteristic._read_status = status
            characteristic._read_event.set()

    async def write(self, data, response=False, timeout_ms=1000):
        self._check(_FLAG_WRITE | _FLAG_WRITE_NO_RESPONSE)

        # If we only support write-with-response, then force sensible default.
        if (
            response is None
            and (self.properties & _FLAGS_WRITE)
            and not (self.properties & _FLAG_WRITE_NO_RESPONSE)
        ):
            response = True

        if response:
            # Same as read.
            self._register_with_connection()
            self._write_status = None
            self._write_event = self._write_event or asyncio.ThreadSafeFlag()

        # Issue the write.
        ble.gattc_write(self._connection()._conn_handle, self._value_handle, data, response)

        if response:
            with self._connection().timeout(timeout_ms):
                # The event will be set for the write done IRQ.
                await self._write_event.wait()
                if self._write_status != 0:
                    raise GattError(self._write_status)

    # Map an incoming write done IRQ to a registered characteristic.
    def _write_done(conn_handle, value_handle, status):
        if characteristic := ClientCharacteristic._find(conn_handle, value_handle):
            characteristic._write_status = status
            characteristic._write_event.set()


# Represents a single characteristic supported by a service. Do not construct
# this class directly, instead use `async for characteristic in
# service.characteristics([uuid])` or `await service.characteristic(uuid)`.
class ClientCharacteristic(BaseClientCharacteristic):
    def __init__(self, service, def_handle, value_handle, properties, uuid):
        self.service = service
        self.connection = service.connection

        # Used for read/write/notify ops.
        self._def_handle = def_handle
        self._value_handle = value_handle

        # Which operations are supported.
        self.properties = properties

        # Allows comparison to a known uuid.
        self.uuid = uuid

        if properties & _FLAG_READ:
            # Fired for each read result and read done IRQ.
            self._read_event = None
            self._read_data = None
            # Used to indicate that the read is complete.
            self._read_status = None

        if (properties & _FLAG_WRITE) or (properties & _FLAG_WRITE_NO_RESPONSE):
            # Fired for the write done IRQ.
            self._write_event = None
            # Used to indicate that the write is complete.
            self._write_status = None

        if properties & _FLAG_NOTIFY:
            # Fired when a notification arrives.
            self._notify_event = asyncio.ThreadSafeFlag()
            # Data for the most recent notification.
            self._notify_queue = deque((), 1)
        if properties & _FLAG_INDICATE:
            # Same for indications.
            self._indicate_event = asyncio.ThreadSafeFlag()
            self._indicate_queue = deque((), 1)

    def __str__(self):
        return "Characteristic: {} {} {} {}".format(
            self._def_handle, self._value_handle, self.properties, self.uuid
        )

    def _connection(self):
        return self.service.connection

    # Search for a specific descriptor by uuid.
    async def descriptor(self, uuid, timeout_ms=2000):
        result = None
        # Make sure loop runs to completion.
        async for descriptor in self.descriptors(timeout_ms):
            if not result and descriptor.uuid == uuid:
                # Keep first result.
                result = descriptor
        return result

    # Search for all services (optionally by uuid).
    # Use with `async for`, e.g.
    #     async for descriptor in characteristic.descriptors():
    # Note: must allow the loop to run to completion.
    def descriptors(self, timeout_ms=2000):
        return ClientDiscover(self.connection, ClientDescriptor, self, timeout_ms)

    # For ClientDiscover
    def _start_discovery(service, uuid=None):
        ble.gattc_discover_characteristics(
            service.connection._conn_handle,
            service._start_handle,
            service._end_handle,
            uuid,
        )

    # Helper for notified() and indicated().
    async def _notified_indicated(self, queue, event, timeout_ms):
        # Ensure that events for this connection can route to this characteristic.
        self._register_with_connection()

        # If the queue is empty, then we need to wait. However, if the queue
        # has a single item, we also need to do a no-op wait in order to
        # clear the event flag (because the queue will become empty and
        # therefore the event should be cleared).
        if len(queue) <= 1:
            with self._connection().timeout(timeout_ms):
                await event.wait()

        # Either we started > 1 item, or the wait completed successfully, return
        # the front of the queue.
        return queue.popleft()

    # Wait for the next notification.
    # Will return immediately if a notification has already been received.
    async def notified(self, timeout_ms=None):
        self._check(_FLAG_NOTIFY)
        return await self._notified_indicated(self._notify_queue, self._notify_event, timeout_ms)

    def _on_notify_indicate(self, queue, event, data):
        # If we've gone from empty to one item, then wake something
        # blocking on `await char.notified()` (or `await char.indicated()`).
        wake = len(queue) == 0
        # Append the data. By default this is a deque with max-length==1, so it
        # replaces. But if capture is enabled then it will append.
        queue.append(data)
        if wake:
            # Queue is now non-empty. If something is waiting, it will be
            # worken. If something isn't waiting right now, then a future
            # caller to `await char.written()` will see the queue is
            # non-empty, and wait on the event if it's going to empty the
            # queue.
            event.set()

    # Map an incoming notify IRQ to a registered characteristic.
    def _on_notify(conn_handle, value_handle, notify_data):
        if characteristic := ClientCharacteristic._find(conn_handle, value_handle):
            characteristic._on_notify_indicate(
                characteristic._notify_queue, characteristic._notify_event, notify_data
            )

    # Wait for the next indication.
    # Will return immediately if an indication has already been received.
    async def indicated(self, timeout_ms=None):
        self._check(_FLAG_INDICATE)
        return await self._notified_indicated(
            self._indicate_queue, self._indicate_event, timeout_ms
        )

    # Map an incoming indicate IRQ to a registered characteristic.
    def _on_indicate(conn_handle, value_handle, indicate_data):
        if characteristic := ClientCharacteristic._find(conn_handle, value_handle):
            characteristic._on_notify_indicate(
                characteristic._indicate_queue, characteristic._indicate_event, indicate_data
            )

    # Write to the Client Characteristic Configuration to subscribe to
    # notify/indications for this characteristic.
    async def subscribe(self, notify=True, indicate=False):
        # Ensure that the generated notifications are dispatched in case the app
        # hasn't awaited on notified/indicated yet.
        self._register_with_connection()
        if cccd := await self.descriptor(bluetooth.UUID(_CCCD_UUID)):
            await cccd.write(struct.pack("<H", _CCCD_NOTIFY * notify + _CCCD_INDICATE * indicate))
        else:
            raise ValueError("CCCD not found")


# Represents a single descriptor supported by a characteristic. Do not construct
# this class directly, instead use `async for descriptors in
# characteristic.descriptors([uuid])` or `await characteristic.descriptor(uuid)`.
class ClientDescriptor(BaseClientCharacteristic):
    def __init__(self, characteristic, dsc_handle, uuid):
        self.characteristic = characteristic

        # Allows comparison to a known uuid.
        self.uuid = uuid

        # Used for read/write.
        self._value_handle = dsc_handle

        # Default flags
        self.properties = _FLAG_READ | _FLAG_WRITE_NO_RESPONSE

    def __str__(self):
        return "Descriptor: {} {} {} {}".format(
            self._def_handle, self._value_handle, self.properties, self.uuid
        )

    def _connection(self):
        return self.characteristic.service.connection

    # For ClientDiscover
    def _start_discovery(characteristic, uuid=None):
        ble.gattc_discover_descriptors(
            characteristic._connection()._conn_handle,
            characteristic._value_handle,
            characteristic._value_handle + 5,
        )
