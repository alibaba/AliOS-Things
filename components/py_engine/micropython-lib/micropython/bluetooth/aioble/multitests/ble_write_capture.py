# Test characteristic write capture.

import sys

sys.path.append("")

from micropython import const
import time, machine

import uasyncio as asyncio
import aioble
import bluetooth

TIMEOUT_MS = 5000

SERVICE_UUID = bluetooth.UUID("A5A5A5A5-FFFF-9999-1111-5A5A5A5A5A5A")
CHAR_UUID = bluetooth.UUID("00000000-1111-2222-3333-444444444444")
CHAR_CAPTURE_UUID = bluetooth.UUID("00000000-1111-2222-3333-555555555555")


# Acting in peripheral role.
async def instance0_task():
    service = aioble.Service(SERVICE_UUID)
    characteristic = aioble.Characteristic(
        service,
        CHAR_UUID,
        write=True,
    )
    # Second characteristic enabled write capture.
    characteristic_capture = aioble.Characteristic(
        service,
        CHAR_CAPTURE_UUID,
        write=True,
        capture=True,
    )
    aioble.register_services(service)

    multitest.globals(BDADDR=aioble.config("mac"))
    multitest.next()

    # Wait for central to connect to us.
    print("advertise")
    async with await aioble.advertise(
        20_000, adv_data=b"\x02\x01\x06\x04\xffMPY", timeout_ms=TIMEOUT_MS
    ) as connection:
        print("connected")

        # We should miss writes while we're sleeping.
        for i in range(2):
            await characteristic.written(timeout_ms=TIMEOUT_MS)
            print("written", characteristic.read())
            await asyncio.sleep_ms(500)

        # Shouldn't miss any writes as they will be captured and queued.
        for i in range(5):
            write_connection, value = await characteristic_capture.written(timeout_ms=TIMEOUT_MS)
            print("written", value, write_connection == connection)
            await asyncio.sleep_ms(500)


def instance0():
    try:
        asyncio.run(instance0_task())
    finally:
        aioble.stop()


# Acting in central role.
async def instance1_task():
    multitest.next()

    # Connect to peripheral and then disconnect.
    print("connect")
    device = aioble.Device(*BDADDR)
    async with await device.connect(timeout_ms=TIMEOUT_MS) as connection:
        # Discover characteristics.
        service = await connection.service(SERVICE_UUID)
        print("service", service.uuid)
        characteristic = await service.characteristic(CHAR_UUID)
        characteristic_capture = await service.characteristic(CHAR_CAPTURE_UUID)
        print("characteristic", characteristic.uuid, characteristic_capture.uuid)

        # Write to the characteristic five times, but faster than the remote side is waiting.
        # Some writes will be lost.
        for i in range(5):
            print("write")
            await characteristic.write("central" + str(i), timeout_ms=TIMEOUT_MS)
            await asyncio.sleep_ms(200)

        # Write to the capture characteristic five times, but faster than the remote side is waiting.
        # The writes should be captured and queued.
        for i in range(5):
            print("write")
            await characteristic_capture.write("central" + str(i), timeout_ms=TIMEOUT_MS)
            await asyncio.sleep_ms(200)


def instance1():
    try:
        asyncio.run(instance1_task())
    finally:
        aioble.stop()
