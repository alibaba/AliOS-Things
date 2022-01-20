# Test characteristic read/write/notify from both GATTS and GATTC.

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


# Acting in peripheral role.
async def instance0_task():
    service = aioble.Service(SERVICE_UUID)
    characteristic = aioble.Characteristic(
        service, CHAR_UUID, read=True, write=True, notify=True, indicate=True
    )
    aioble.register_services(service)

    multitest.globals(BDADDR=aioble.config("mac"))
    multitest.next()

    # Write initial characteristic value.
    characteristic.write("periph0")

    # Wait for central to connect to us.
    print("advertise")
    connection = await aioble.advertise(
        20_000, adv_data=b"\x02\x01\x06\x04\xffMPY", timeout_ms=TIMEOUT_MS
    )
    print("connected")

    # A

    # Wait for a write to the characteristic from the central,
    # then reply with a notification.
    await characteristic.written(timeout_ms=TIMEOUT_MS)
    print("written", characteristic.read())
    print("write")
    characteristic.write("periph1")
    print("notify")
    characteristic.notify(connection)

    # B

    # Wait for a write to the characteristic from the central,
    # then reply with value-included notification.
    await characteristic.written(timeout_ms=TIMEOUT_MS)
    print("written", characteristic.read())
    print("notify")
    characteristic.notify(connection, "periph2")

    # C

    # Wait for a write to the characteristic from the central,
    # then reply with an indication.
    await characteristic.written(timeout_ms=TIMEOUT_MS)
    print("written", characteristic.read())
    print("write")
    characteristic.write("periph3")
    print("indicate")
    await characteristic.indicate(connection, timeout_ms=TIMEOUT_MS)

    # Wait for the central to disconnect.
    await connection.disconnected(timeout_ms=TIMEOUT_MS)
    print("disconnected")


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
    connection = await device.connect(timeout_ms=TIMEOUT_MS)

    # Discover characteristics.
    service = await connection.service(SERVICE_UUID)
    print("service", service.uuid)
    characteristic = await service.characteristic(CHAR_UUID)
    print("characteristic", characteristic.uuid)

    # Issue read of characteristic, should get initial value.
    print("read", await characteristic.read(timeout_ms=TIMEOUT_MS))

    # Write to the characteristic, which will trigger a notification.
    print("write")
    await characteristic.write("central0", response=True, timeout_ms=TIMEOUT_MS)
    # A
    print("notified", await characteristic.notified(timeout_ms=TIMEOUT_MS))
    # Read the new value set immediately before notification.
    print("read", await characteristic.read(timeout_ms=TIMEOUT_MS))

    # Write to the characteristic, which will trigger a value-included notification.
    print("write")
    await characteristic.write("central1", response=True, timeout_ms=TIMEOUT_MS)
    # B
    print("notified", await characteristic.notified(timeout_ms=TIMEOUT_MS))
    # Read value should be unchanged.
    print("read", await characteristic.read(timeout_ms=TIMEOUT_MS))

    # Write to the characteristic, which will trigger an indication.
    print("write")
    await characteristic.write("central2", response=True, timeout_ms=TIMEOUT_MS)
    # C
    print("indicated", await characteristic.indicated(timeout_ms=TIMEOUT_MS))
    # Read the new value set immediately before indication.
    print("read", await characteristic.read(timeout_ms=TIMEOUT_MS))

    # Disconnect from peripheral.
    print("disconnect")
    await connection.disconnect(timeout_ms=TIMEOUT_MS)
    print("disconnected")


def instance1():
    try:
        asyncio.run(instance1_task())
    finally:
        aioble.stop()
